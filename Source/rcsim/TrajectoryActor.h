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

	UFUNCTION(BlueprintCallable, Category = "Trajectory")
	void LoadDummyGPS();

	// Utility: convert lat/lon/alt to Unreal space (cm)
	static FVector LatLonAltToUnreal(double lat, double lon, double alt,
		double originLat, double originLon, double originAlt);
};
