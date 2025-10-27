// Fill out your copyright notice in the Description page of Project Settings.


#include "TrajectoryActor.h"
#include "DrawDebugHelpers.h"

// Sets default values
ATrajectoryActor::ATrajectoryActor()
{
 	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATrajectoryActor::BeginPlay()
{
	Super::BeginPlay();

	// For quick testing, populate with dummy GPS points if none provided in editor
	if (Positions.Num() == 0)
	{
		LoadDummyGPS();
		UE_LOG(LogTemp, Warning, TEXT("TrajectoryActor: Loaded %d GPS points"), Positions.Num());
		
		// Log each position for debugging
		for (int32 i = 0; i < Positions.Num(); ++i)
		{
			UE_LOG(LogTemp, Warning, TEXT("Point %d: X=%.1f Y=%.1f Z=%.1f"), 
				i, Positions[i].X, Positions[i].Y, Positions[i].Z);
		}
	}
}

// Called every frame
void ATrajectoryActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Draw trajectory for debugging
	const int32 Num = Positions.Num();
	
	// Debug log (only once per second to avoid spam)
	static float TimeSinceLastLog = 0.0f;
	TimeSinceLastLog += DeltaTime;
	if (TimeSinceLastLog > 1.0f)
	{
		UE_LOG(LogTemp, Log, TEXT("TrajectoryActor Tick: Drawing %d points"), Num);
		TimeSinceLastLog = 0.0f;
	}
	
	for (int32 i = 0; i < Num; ++i)
	{
		const FVector& P = Positions[i];
		
		// BEAUCOUP plus grosses sphères pour un Open World (5 mètres de rayon)
		DrawDebugSphere(GetWorld(), P, 500.0f, 16, FColor::Green, false, -1.0f, 0, 5.0f);
		
		// Ajouter aussi du texte pour identifier chaque point
		DrawDebugString(GetWorld(), P + FVector(0, 0, 600.0f), 
			FString::Printf(TEXT("GPS Point %d"), i), 
			nullptr, FColor::White, -1.0f, true, 2.0f);
		
		if (i > 0)
		{
			// Lignes jaunes plus épaisses
			DrawDebugLine(GetWorld(), Positions[i - 1], P, FColor::Yellow, false, -1.0f, 0, 10.0f);
		}
	}

}

void ATrajectoryActor::LoadDummyGPS()
{
    // Larger loop circuit (points spaced by ~0.002 deg ≈222 m and diagonals ≈315 m)
    TArray<std::tuple<double, double, double>> gpsPoints = {
        {45.00000,5.00200,0.0}, // East
        {45.001414,5.001414,0.0}, // NE
        {45.00200,5.00000,0.0}, // North
        {45.001414,4.998586,0.0}, // NW
        {45.00000,4.99800,0.0}, // West
        {44.998586,4.998586,0.0}, // SW
        {44.99800,5.00000,0.0}, // South
        {44.998586,5.001414,0.0} // SE
    };

    // Use the first point as origin for conversion
    double originLat = std::get<0>(gpsPoints[0]);
    double originLon = std::get<1>(gpsPoints[0]);
    double originAlt = std::get<2>(gpsPoints[0]);

    Positions.Empty();
    Rotations.Empty();

    for (int32 i =0; i < gpsPoints.Num(); ++i)
    {
        double lat = std::get<0>(gpsPoints[i]);
        double lon = std::get<1>(gpsPoints[i]);
        double alt = std::get<2>(gpsPoints[i]);

        FVector Pos = LatLonAltToUnreal(lat, lon, alt, originLat, originLon, originAlt);
        Positions.Add(Pos);

        // Compute heading towards next point for rotation (yaw)
        int32 nextIndex = (i +1) % gpsPoints.Num();
        double nextLat = std::get<0>(gpsPoints[nextIndex]);
        double nextLon = std::get<1>(gpsPoints[nextIndex]);
        double nextAlt = std::get<2>(gpsPoints[nextIndex]);
        FVector NextPos = LatLonAltToUnreal(nextLat, nextLon, nextAlt, originLat, originLon, originAlt);

        FVector Dir = (NextPos - Pos).GetSafeNormal();
        float Yaw = Dir.Rotation().Yaw;
        Rotations.Add(FRotator(0.0f, Yaw,0.0f));
    }
}

FVector ATrajectoryActor::LatLonAltToUnreal(double lat, double lon, double alt,
    double originLat, double originLon, double originAlt)
{
    // Pour prototype, approx simple : 1 deg lat ≈ 111 km, 1 deg lon ≈ 111km*cos(lat)
    double dLat = lat - originLat;
    double dLon = lon - originLon;
    double x = dLon * 111000.0; // East
    double y = dLat * 111000.0; // North
    double z = alt - originAlt; // Up

    return FVector(x * 100.0, y * 100.0, z * 100.0); // cm
}
