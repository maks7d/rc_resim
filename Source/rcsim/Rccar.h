// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"

class UStaticMeshComponent;
class UFloatingPawnMovement;
class USpringArmComponent;
class UCameraComponent;
class ATrajectoryActor;

#include "Rccar.generated.h"

UCLASS(Blueprintable)
class RCSIM_API ARccar : public APawn
{
	GENERATED_BODY()

public:
	ARccar();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UFloatingPawnMovement* MovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* Camera;

	// Movement tuning
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float ThrottleForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float TurnSpeed;

	// Trajectory follow
	UPROPERTY(EditAnywhere, Category = "Trajectory")
	ATrajectoryActor* TrajectoryRef;

	UPROPERTY(EditAnywhere, Category = "Trajectory")
	float KpYaw;

	UPROPERTY(EditAnywhere, Category = "Trajectory")
	float KpPos;

	UPROPERTY(EditAnywhere, Category = "Trajectory")
	float AcceptRadius;

protected:
	int32 CurrentTrajectoryIndex;
};
