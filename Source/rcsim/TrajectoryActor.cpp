// Fill out your copyright notice in the Description page of Project Settings.


#include "TrajectoryActor.h"
#include "DrawDebugHelpers.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Components/SkeletalMeshComponent.h"
#include <cmath>

// Sets default values
ATrajectoryActor::ATrajectoryActor()
{
 // Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
 PrimaryActorTick.bCanEverTick = true;

 LatLonScale =1.0f;
}

// Called when the game starts or when spawned
void ATrajectoryActor::BeginPlay()
{
 Super::BeginPlay();

 if (Positions.Num() ==0)
 {
 UE_LOG(LogTemp, Warning, TEXT("TrajectoryActor: No GNSS data loaded. Use the GNSS Loader UI to import a CSV file."));
 }
 else
 {
 UE_LOG(LogTemp, Warning, TEXT("TrajectoryActor: Loaded %d GNSS points"), Positions.Num());
 }
}

// Called every frame
void ATrajectoryActor::Tick(float DeltaTime)
{
 Super::Tick(DeltaTime);

 // Draw trajectory for debugging
 const int32 Num = Positions.Num();
 
 // Debug log (only once per second to avoid spam)
 static float TimeSinceLastLog =0.0f;
 TimeSinceLastLog += DeltaTime;
 if (TimeSinceLastLog >1.0f && Num >0)
 {
 UE_LOG(LogTemp, Log, TEXT("TrajectoryActor Tick: Drawing %d points"), Num);
 TimeSinceLastLog =0.0f;
 }
 
 for (int32 i =0; i < Num; ++i)
 {
 const FVector& P = Positions[i];
 
 // Large spheres for Open World visualization (1 meter radius)
 DrawDebugSphere(GetWorld(), P,100.0f,16, FColor::Green, false, -1.0f,0,2.0f);
 
 // Add text to identify each point
 DrawDebugString(GetWorld(), P + FVector(0,0,600.0f), 
 FString::Printf(TEXT("GNSS Point %d"), i), 
 nullptr, FColor::White, -1.0f, true,2.0f);
 
 if (i >0)
 {
 // Thick yellow lines between points
 DrawDebugLine(GetWorld(), Positions[i -1], P, FColor::Yellow, false, -1.0f,0,10.0f);
 }
 }
}

bool ATrajectoryActor::LoadGNSSFromCSV(const FString& FilePath)
{
 FString FileContent;
 if (!FFileHelper::LoadFileToString(FileContent, *FilePath))
 {
 UE_LOG(LogTemp, Error, TEXT("TrajectoryActor: Failed to load file: %s"), *FilePath);
 return false;
 }

 // Clear existing data
 ClearTrajectory();

 TArray<FString> Lines;
 FileContent.ParseIntoArrayLines(Lines);

 if (Lines.Num() <=1)
 {
 UE_LOG(LogTemp, Error, TEXT("TrajectoryActor: CSV file is empty or contains only header"));
 return false;
 }

 bool bFirstLine = true;
 double originLat =0.0, originLon =0.0, originAlt =0.0;

 int32 ValidPoints =0;
 for (const FString& Line : Lines)
 {
 // Skip header line
 if (bFirstLine)
 {
 bFirstLine = false;
 // Verify header format
 if (!Line.Contains(TEXT("timestamp")) || !Line.Contains(TEXT("lat")) || !Line.Contains(TEXT("lon")))
 {
 UE_LOG(LogTemp, Warning, TEXT("TrajectoryActor: CSV header doesn't match expected format. Expected: timestamp,lat,lon,alt,heading,speed"));
 }
 continue;
 }

 // Skip empty lines
 if (Line.TrimStartAndEnd().IsEmpty())
 {
 continue;
 }

 TArray<FString> Tokens;
 Line.ParseIntoArray(Tokens, TEXT(","), true);

 if (Tokens.Num() >=6)
 {
 // Parse CSV fields
 double timestamp = FCString::Atod(*Tokens[0]);
 double lat = FCString::Atod(*Tokens[1]);
 double lon = FCString::Atod(*Tokens[2]);
 double alt = FCString::Atod(*Tokens[3]);
 double heading = FCString::Atod(*Tokens[4]);
 double speed = FCString::Atod(*Tokens[5]);

 // Use first point as origin for coordinate conversion
 if (ValidPoints ==0)
 {
 originLat = lat;
 originLon = lon;
 originAlt = alt;
 UE_LOG(LogTemp, Warning, TEXT("TrajectoryActor: Origin set to Lat=%.6f, Lon=%.6f, Alt=%.2f"), 
 originLat, originLon, originAlt);
 }

 // Convert GNSS coordinates to Unreal world coordinates
 FVector pos = LatLonAltToUnreal(lat, lon, alt, originLat, originLon, originAlt, (double)LatLonScale);
 Positions.Add(pos);
 Rotations.Add(FRotator(0.0f, heading,0.0f));
 Timestamps.Add(timestamp);
 Speeds.Add(speed);

 ValidPoints++;
 }
 else
 {
 UE_LOG(LogTemp, Warning, TEXT("TrajectoryActor: Skipping invalid line (expected6 fields, got %d): %s"), 
 Tokens.Num(), *Line);
 }
 }

 if (ValidPoints ==0)
 {
 UE_LOG(LogTemp, Error, TEXT("TrajectoryActor: No valid GNSS points found in CSV file"));
 return false;
 }

 UE_LOG(LogTemp, Warning, TEXT("TrajectoryActor: Successfully loaded %d GNSS points"), ValidPoints);
 
 if (Timestamps.Num() >1)
 {
 double duration = Timestamps.Last() - Timestamps[0];
 UE_LOG(LogTemp, Warning, TEXT("TrajectoryActor: Trajectory duration: %.2f seconds"), duration);
 }

 // just after loading in LoadGNSSFromCSV
 for (int i =1; i < Positions.Num(); ++i) {
 float distCm = FVector::Dist(Positions[i -1], Positions[i]);
 UE_LOG(LogTemp, Warning, TEXT("Spacing %d-%d: %.1f cm (%.2f m)"), i -1, i, distCm, distCm /100.0f);
 }

 return true;
}

void ATrajectoryActor::ClearTrajectory()
{
 Positions.Empty();
 Rotations.Empty();
 Timestamps.Empty();
 Speeds.Empty();
 UE_LOG(LogTemp, Log, TEXT("TrajectoryActor: Trajectory data cleared"));
}

FVector ATrajectoryActor::LatLonAltToUnreal(double lat, double lon, double alt,
 double originLat, double originLon, double originAlt, double Scale)
{
 // WGS-84 approx for meters per degree:
 const double deg2rad =3.14159265358979323846 /180.0;
 double latRad = originLat * deg2rad;
 // more accurate meters/deg formulas
 double metersPerDegLat =111132.954 -559.822 * std::cos(2.0 * latRad) +1.175 * std::cos(4.0 * latRad);
 double metersPerDegLon = (111412.84 * std::cos(latRad) -93.5 * std::cos(3.0 * latRad));

 double dLat = lat - originLat;
 double dLon = lon - originLon;

 double x_m = dLon * metersPerDegLon * Scale; // East (meters)
 double y_m = dLat * metersPerDegLat * Scale; // North (meters)
 double z_m = alt - originAlt; // meters

 return FVector(x_m *100.0, y_m *100.0, z_m *100.0); // Unreal = cm
}

void ATrajectoryActor::ScaleTrajectory(float ScaleFactor)
{
 for (int32 i =0; i < Positions.Num(); ++i)
 {
 Positions[i] *= ScaleFactor;
 }
}

void ATrajectoryActor::ApplyAutoScalingToFit(float TargetExtentMeters)
{
 if (Positions.Num() ==0) return;

 // compute bounding box extent (max distance between min & max)
 FVector Min = Positions[0], Max = Positions[0];
 for (const FVector& P : Positions)
 {
 Min.X = FMath::Min(Min.X, P.X);
 Min.Y = FMath::Min(Min.Y, P.Y);
 Min.Z = FMath::Min(Min.Z, P.Z);
 Max.X = FMath::Max(Max.X, P.X);
 Max.Y = FMath::Max(Max.Y, P.Y);
 Max.Z = FMath::Max(Max.Z, P.Z);
 }
 float diagCm = FVector::Dist(Min, Max); // cm
 float currentExtentMeters = diagCm /100.0f;
 if (currentExtentMeters <= KINDA_SMALL_NUMBER) return;

 float targetCm = TargetExtentMeters *100.0f;
 float scale = targetCm / diagCm;
 ScaleTrajectory(scale);

 UE_LOG(LogTemp, Warning, TEXT("TrajectoryActor: Auto-scaled trajectory by %.3f (from %.2fm to %.2fm)"),
 scale, currentExtentMeters, TargetExtentMeters);
}

void ATrajectoryActor::ScaleToMatchVehicleLength(AActor* VehicleActor, float DesiredVehicleLengthMeters)
{
 if (!VehicleActor) return;
 // estimer longueur du véhicule en m
 float vehicleLengthMeters =1.0f;
 if (USkeletalMeshComponent* Mesh = VehicleActor->FindComponentByClass<USkeletalMeshComponent>())
 {
 FVector BoxExtent = Mesh->Bounds.BoxExtent; // cm
 float meshLengthMeters = (BoxExtent.X *2.0f) /100.0f; // X is forward usually
 vehicleLengthMeters = meshLengthMeters;
 }
 // On veut que vehicleLengthMeters devienne DesiredVehicleLengthMeters -> scale factor:
 float scaleToApply = DesiredVehicleLengthMeters / vehicleLengthMeters;
 // si on veut juste uniformiser la trajectoire par rapport au véhicule : on scale la trajectoire
 ScaleTrajectory(scaleToApply);
 UE_LOG(LogTemp, Warning, TEXT("TrajectoryActor: Scaled to match vehicle length (scale=%.3f)"), scaleToApply);
}
