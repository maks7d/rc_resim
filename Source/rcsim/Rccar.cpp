// Fill out your copyright notice in the Description page of Project Settings.

#include "Rccar.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "TrajectoryActor.h"

ARccar::ARccar()
{
	PrimaryActorTick.bCanEverTick = true;

	// Auto possess player0
	AutoPossessPlayer = EAutoReceiveInput::Player0;

	// Create body mesh
	BodyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BodyMesh"));
	RootComponent = BodyMesh;
	BodyMesh->SetSimulatePhysics(true);
	BodyMesh->SetEnableGravity(true);
	BodyMesh->SetNotifyRigidBodyCollision(true);

	// Movement component for simple pawn movement
	MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));
	MovementComponent->UpdatedComponent = RootComponent;

	// Spring arm and camera
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(BodyMesh);
	SpringArm->TargetArmLength = 300.0f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 3.0f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);

	// Default tuning
	ThrottleForce = 1000.0f;
	TurnSpeed = 45.0f;

	KpYaw = 0.01f;
	KpPos = 0.01f;
	AcceptRadius = 100.0f; // cm

	CurrentTrajectoryIndex = 0;
	TrajectoryRef = nullptr;
}

void ARccar::BeginPlay()
{
	Super::BeginPlay();

	// Auto-find trajectory actor if not set
	if (!TrajectoryRef)
	{
		TArray<AActor*> Found;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATrajectoryActor::StaticClass(), Found);
		if (Found.Num() > 0)
		{
			TrajectoryRef = Cast<ATrajectoryActor>(Found[0]);
		}
	}

	// If trajectory exists and empty, load dummy GPS
	if (TrajectoryRef && TrajectoryRef->Positions.Num() == 0)
	{
		TrajectoryRef->LoadDummyGPS();
	}
}

void ARccar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If trajectory present, run simple P controllers to set inputs
	if (TrajectoryRef && TrajectoryRef->Positions.Num() > 0)
	{
		// Clamp index
		CurrentTrajectoryIndex = FMath::Clamp(CurrentTrajectoryIndex, 0, TrajectoryRef->Positions.Num() - 1);
		FVector Target = TrajectoryRef->Positions[CurrentTrajectoryIndex];
		FVector Delta = Target - GetActorLocation();
		float Distance = Delta.Size();

		// Advance to next point if close
		if (Distance < AcceptRadius)
		{
			if (CurrentTrajectoryIndex < TrajectoryRef->Positions.Num() - 1)
			{
				CurrentTrajectoryIndex++;
			}
		}

		float DesiredYaw = Delta.Rotation().Yaw;
		float CurrentYaw = GetActorRotation().Yaw;
		float YawError = FMath::FindDeltaAngleDegrees(CurrentYaw, DesiredYaw);

		float Steering = KpYaw * YawError; // P controller
		float Throttle = KpPos * Distance; // P controller

		Steering = FMath::Clamp(Steering, -1.0f, 1.0f);
		Throttle = FMath::Clamp(Throttle, 0.0f, 1.0f);

		// If using Chaos Wheeled Vehicle, set inputs; otherwise apply simple force/rotation
		UChaosWheeledVehicleMovementComponent* VehicleMovement = nullptr;
		APawn* Pawn = Cast<APawn>(this);
		if (Pawn)
		{
			VehicleMovement = Cast<UChaosWheeledVehicleMovementComponent>(Pawn->GetMovementComponent());
		}

		if (VehicleMovement)
		{
			VehicleMovement->SetThrottleInput(Throttle);
			VehicleMovement->SetSteeringInput(Steering);
		}
		else
		{
			// Fallback: apply force and rotate

			if (FMath::Abs(Throttle) > KINDA_SMALL_NUMBER)
			{
				FVector Forward = GetActorForwardVector();
				FVector Force = Forward * Throttle * ThrottleForce * DeltaTime;

				if (BodyMesh && BodyMesh->IsSimulatingPhysics())
				{
					BodyMesh->AddForce(Force);
				}
				else if (MovementComponent)
				{
					MovementComponent->AddInputVector(Forward * Throttle);
				}
			}

			if (FMath::Abs(Steering) > KINDA_SMALL_NUMBER)
			{
				FRotator DeltaRot = FRotator(0.f, Steering * TurnSpeed * DeltaTime, 0.f);
				AddActorLocalRotation(DeltaRot);
			}
		}
	}
}
