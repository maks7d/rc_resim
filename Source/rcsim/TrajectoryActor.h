// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TrajectoryActor.generated.h"

UCLASS()
class RCSIM_API ATrajectoryActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATrajectoryActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Trajectory points in Unreal world coordinates (cm)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trajectory")
	TArray<FVector> Positions;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trajectory")
	TArray<FRotator> Rotations;

	// Timestamps for each GNSS point (in seconds)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trajectory")
	TArray<double> Timestamps;

	// Calculated speeds for each GNSS point (km/h) - computed from positions and timestamps
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trajectory")
	TArray<float> Speeds;

	// Load GPS trajectory from CSV file
	// Expected format: timestamp,lat,lon,alt,heading
	UFUNCTION(BlueprintCallable, Category = "Trajectory")
	bool LoadGNSSFromCSV(const FString& FilePath);

	// Clear all trajectory data
	UFUNCTION(BlueprintCallable, Category = "Trajectory")
	void ClearTrajectory();

	// exposer dans l'éditeur
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trajectory|Scale")
	float LatLonScale; // multiplicateur appliqué après conversion (1.0 = scale réel)

	UFUNCTION(BlueprintCallable, Category = "Trajectory|Scale")
	void ApplyAutoScalingToFit(float TargetExtentMeters);

	UFUNCTION(BlueprintCallable, Category = "Trajectory|Scale")
	void ScaleTrajectory(float ScaleFactor);

	UFUNCTION(BlueprintCallable, Category = "Trajectory|Scale")
	void ScaleToMatchVehicleLength(AActor* VehicleActor, float DesiredVehicleLengthMeters);

	// Utility: convert lat/lon/alt to Unreal space (cm)
	// scale parameter multiplies distances in meters before conversion to cm
	static FVector LatLonAltToUnreal(double lat, double lon, double alt,
		double originLat, double originLon, double originAlt, double Scale = 1.0);
};
