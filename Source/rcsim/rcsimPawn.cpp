// Copyright Epic Games, Inc. All Rights Reserved.

#include "rcsimPawn.h"
#include "rcsimWheelFront.h"
#include "rcsimWheelRear.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "ChaosWheeledVehicleMovementComponent.h"
#include "rcsim.h"
#include "TrajectoryActor.h"
#include "Kismet/GameplayStatics.h"

#define LOCTEXT_NAMESPACE "VehiclePawn"

ArcsimPawn::ArcsimPawn()
{
	// construct the front camera boom
	FrontSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Front Spring Arm"));
	FrontSpringArm->SetupAttachment(GetMesh());
	FrontSpringArm->TargetArmLength = 0.0f;
	FrontSpringArm->bDoCollisionTest = false;
	FrontSpringArm->bEnableCameraRotationLag = true;
	FrontSpringArm->CameraRotationLagSpeed = 15.0f;
	FrontSpringArm->SetRelativeLocation(FVector(30.0f, 0.0f, 120.0f));

	FrontCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Front Camera"));
	FrontCamera->SetupAttachment(FrontSpringArm);
	FrontCamera->bAutoActivate = false;

	// construct the back camera boom
	BackSpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Back Spring Arm"));
	BackSpringArm->SetupAttachment(GetMesh());
	BackSpringArm->TargetArmLength = 650.0f;
	BackSpringArm->SocketOffset.Z = 150.0f;
	BackSpringArm->bDoCollisionTest = false;
	BackSpringArm->bInheritPitch = false;
	BackSpringArm->bInheritRoll = false;
	BackSpringArm->bEnableCameraRotationLag = true;
	BackSpringArm->CameraRotationLagSpeed = 2.0f;
	BackSpringArm->CameraLagMaxDistance = 50.0f;

	BackCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Back Camera"));
	BackCamera->SetupAttachment(BackSpringArm);

	// Configure the car mesh
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(FName("Vehicle"));

	// get the Chaos Wheeled movement component
	ChaosVehicleMovement = CastChecked<UChaosWheeledVehicleMovementComponent>(GetVehicleMovement());
}

void ArcsimPawn::BeginPlay()
{
	Super::BeginPlay();
	
	// Auto-find TrajectoryActor if not manually assigned
	if (!TrajectoryRef && bEnableGNSSReplay)
	{
		TArray<AActor*> FoundActors;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATrajectoryActor::StaticClass(), FoundActors);
		
		if (FoundActors.Num() > 0)
		{
			TrajectoryRef = Cast<ATrajectoryActor>(FoundActors[0]);
			UE_LOG(LogTemp, Warning, TEXT("GNSS Replay: Auto-found TrajectoryActor: %s"), *TrajectoryRef->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("GNSS Replay: Enabled but no TrajectoryActor found in level!"));
		}
	}

	// Initialize GNSS replay
	if (bEnableGNSSReplay && TrajectoryRef)
	{
		ResetGNSSReplay();
	}
}

void ArcsimPawn::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// steering 
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Triggered, this, &ArcsimPawn::Steering);
		EnhancedInputComponent->BindAction(SteeringAction, ETriggerEvent::Completed, this, &ArcsimPawn::Steering);

		// throttle 
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Triggered, this, &ArcsimPawn::Throttle);
		EnhancedInputComponent->BindAction(ThrottleAction, ETriggerEvent::Completed, this, &ArcsimPawn::Throttle);

		// break 
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Triggered, this, &ArcsimPawn::Brake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Started, this, &ArcsimPawn::StartBrake);
		EnhancedInputComponent->BindAction(BrakeAction, ETriggerEvent::Completed, this, &ArcsimPawn::StopBrake);

		// handbrake 
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Started, this, &ArcsimPawn::StartHandbrake);
		EnhancedInputComponent->BindAction(HandbrakeAction, ETriggerEvent::Completed, this, &ArcsimPawn::StopHandbrake);

		// look around 
		EnhancedInputComponent->BindAction(LookAroundAction, ETriggerEvent::Triggered, this, &ArcsimPawn::LookAround);

		// toggle camera 
		EnhancedInputComponent->BindAction(ToggleCameraAction, ETriggerEvent::Triggered, this, &ArcsimPawn::ToggleCamera);

		// reset the vehicle 
		EnhancedInputComponent->BindAction(ResetVehicleAction, ETriggerEvent::Triggered, this, &ArcsimPawn::ResetVehicle);
	}
	else
	{
		UE_LOG(Logrcsim, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ArcsimPawn::Tick(float Delta)
{
	Super::Tick(Delta);

	// GNSS Replay Mode
	if (bEnableGNSSReplay && TrajectoryRef && TrajectoryRef->Positions.Num() > 0)
	{
		if (bUseTimestampPlayback && TrajectoryRef->Timestamps.Num() > 0)
		{
			// ===== TIMESTAMP-BASED REAL-TIME PLAYBACK =====
			CurrentSimTime += Delta * PlaybackSpeed;
			
			// Get trajectory start time
			double StartTime = TrajectoryRef->Timestamps[0];
			double CurrentTrajectoryTime = StartTime + CurrentSimTime;
			
			// Find the current segment based on timestamp
			bool bFoundSegment = false;
			for (int32 i = 0; i < TrajectoryRef->Timestamps.Num() - 1; ++i)
			{
				if (TrajectoryRef->Timestamps[i] <= CurrentTrajectoryTime && 
					TrajectoryRef->Timestamps[i + 1] > CurrentTrajectoryTime)
				{
					// Interpolate between points based on timestamp
					double t0 = TrajectoryRef->Timestamps[i];
					double t1 = TrajectoryRef->Timestamps[i + 1];
					float Alpha = (CurrentTrajectoryTime - t0) / (t1 - t0);
					
					FVector TargetPos = FMath::Lerp(TrajectoryRef->Positions[i], TrajectoryRef->Positions[i + 1], Alpha);
					FRotator TargetRot = FMath::Lerp(TrajectoryRef->Rotations[i], TrajectoryRef->Rotations[i + 1], Alpha);
					
					// Teleport with physics reset
					SetActorLocationAndRotation(TargetPos, TargetRot, false, nullptr, ETeleportType::TeleportPhysics);
					
					if (GetMesh())
					{
						GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);
						GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
					}
					
					CurrentTrajectoryIndex = i;
					bFoundSegment = true;
					break;
				}
			}
			
			// If we've passed the last timestamp, loop back
			if (!bFoundSegment && CurrentTrajectoryTime >= TrajectoryRef->Timestamps.Last())
			{
				UE_LOG(LogTemp, Warning, TEXT("GNSS Replay: Finished trajectory (%.2fs), looping back to start"), CurrentSimTime);
				ResetGNSSReplay();
			}
		}
		else if (bUsePhysicsMovement)
		{
			// ===== PHYSICS-BASED MOVEMENT (PID Controller) =====
			// Clamp index
			CurrentTrajectoryIndex = FMath::Clamp(CurrentTrajectoryIndex, 0, TrajectoryRef->Positions.Num() - 1);
			
			FVector TargetPos = TrajectoryRef->Positions[CurrentTrajectoryIndex];
			FVector CurrentPos = GetActorLocation();
			FVector Delta3D = TargetPos - CurrentPos;
			float Distance = Delta3D.Size();
			
			// Check if we reached the current waypoint
			if (Distance < AcceptRadius)
			{
				if (CurrentTrajectoryIndex < TrajectoryRef->Positions.Num() - 1)
				{
					CurrentTrajectoryIndex++;
					UE_LOG(LogTemp, Log, TEXT("GNSS Replay: Reached waypoint, moving to point %d/%d"), 
						CurrentTrajectoryIndex + 1, TrajectoryRef->Positions.Num());
				}
				else
				{
					// Loop back to start
					UE_LOG(LogTemp, Warning, TEXT("GNSS Replay: Finished trajectory, looping back"));
					CurrentTrajectoryIndex = 0;
				}
			}
			
			// Calculate desired heading towards target
			float DesiredYaw = Delta3D.Rotation().Yaw;
			float CurrentYaw = GetActorRotation().Yaw;
			float YawError = FMath::FindDeltaAngleDegrees(CurrentYaw, DesiredYaw);
			
			// PID controllers (simple P for now)
			float SteeringInput = FMath::Clamp(KpYaw * YawError, -1.0f, 1.0f);
			float ThrottleInput = FMath::Clamp(KpPosition * Distance, 0.0f, 1.0f);
			
			// Apply inputs to vehicle
			ChaosVehicleMovement->SetSteeringInput(SteeringInput);
			ChaosVehicleMovement->SetThrottleInput(ThrottleInput);
			ChaosVehicleMovement->SetBrakeInput(0.0f);
		}
		else
		{
			// ===== FIXED DELAY TELEPORTATION MODE =====
			TrajectoryTimer += Delta;
			
			if (TrajectoryTimer >= TrajectoryPointDelay)
			{
				TrajectoryTimer = 0.0f;
				
				if (CurrentTrajectoryIndex < TrajectoryRef->Positions.Num())
				{
					// Teleport to next GNSS point with proper physics handling
					FVector TargetPos = TrajectoryRef->Positions[CurrentTrajectoryIndex];
					FRotator TargetRot = TrajectoryRef->Rotations[CurrentTrajectoryIndex];
					
					// Use TeleportPhysics flag to avoid the warning and reset physics state
					SetActorLocationAndRotation(TargetPos, TargetRot, false, nullptr, ETeleportType::TeleportPhysics);
					
					// Reset physics velocities to avoid residual movement
					if (GetMesh())
					{
						GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);
						GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
					}
					
					UE_LOG(LogTemp, Log, TEXT("GNSS Replay: Teleporting to point %d/%d"), 
						CurrentTrajectoryIndex + 1, TrajectoryRef->Positions.Num());
					
					CurrentTrajectoryIndex++;
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("GNSS Replay: Finished trajectory, looping back to start"));
					ResetGNSSReplay();
				}
			}
		}
	}

	// add some angular damping if the vehicle is in midair
	bool bMovingOnGround = ChaosVehicleMovement->IsMovingOnGround();
	GetMesh()->SetAngularDamping(bMovingOnGround ? 0.0f : 3.0f);

	// realign the camera yaw to face front
	float CameraYaw = BackSpringArm->GetRelativeRotation().Yaw;
	CameraYaw = FMath::FInterpTo(CameraYaw, 0.0f, Delta, 1.0f);

	BackSpringArm->SetRelativeRotation(FRotator(0.0f, CameraYaw, 0.0f));
}

void ArcsimPawn::Steering(const FInputActionValue& Value)
{
	// route the input
	DoSteering(Value.Get<float>());
}

void ArcsimPawn::Throttle(const FInputActionValue& Value)
{
	// route the input
	DoThrottle(Value.Get<float>());
}

void ArcsimPawn::Brake(const FInputActionValue& Value)
{
	// route the input
	DoBrake(Value.Get<float>());
}

void ArcsimPawn::StartBrake(const FInputActionValue& Value)
{
	// route the input
	DoBrakeStart();
}

void ArcsimPawn::StopBrake(const FInputActionValue& Value)
{
	// route the input
	DoBrakeStop();
}

void ArcsimPawn::StartHandbrake(const FInputActionValue& Value)
{
	// route the input
	DoHandbrakeStart();
}

void ArcsimPawn::StopHandbrake(const FInputActionValue& Value)
{
	// route the input
	DoHandbrakeStop();
}

void ArcsimPawn::LookAround(const FInputActionValue& Value)
{
	// route the input
	DoLookAround(Value.Get<float>());
}

void ArcsimPawn::ToggleCamera(const FInputActionValue& Value)
{
	// route the input
	DoToggleCamera();
}

void ArcsimPawn::ResetVehicle(const FInputActionValue& Value)
{
	// route the input
	DoResetVehicle();
}

void ArcsimPawn::DoSteering(float SteeringValue)
{
	// add the input
	ChaosVehicleMovement->SetSteeringInput(SteeringValue);
}

void ArcsimPawn::DoThrottle(float ThrottleValue)
{
	// add the input
	ChaosVehicleMovement->SetThrottleInput(ThrottleValue);

	// reset the brake input
	ChaosVehicleMovement->SetBrakeInput(0.0f);
}

void ArcsimPawn::DoBrake(float BrakeValue)
{
	// add the input
	ChaosVehicleMovement->SetBrakeInput(BrakeValue);

	// reset the throttle input
	ChaosVehicleMovement->SetThrottleInput(0.0f);
}

void ArcsimPawn::DoBrakeStart()
{
	// call the Blueprint hook for the brake lights
	BrakeLights(true);
}

void ArcsimPawn::DoBrakeStop()
{
	// call the Blueprint hook for the brake lights
	BrakeLights(false);

	// reset brake input to zero
	ChaosVehicleMovement->SetBrakeInput(0.0f);
}

void ArcsimPawn::DoHandbrakeStart()
{
	// add the input
	ChaosVehicleMovement->SetHandbrakeInput(true);

	// call the Blueprint hook for the break lights
	BrakeLights(true);
}

void ArcsimPawn::DoHandbrakeStop()
{
	// add the input
	ChaosVehicleMovement->SetHandbrakeInput(false);

	// call the Blueprint hook for the break lights
	BrakeLights(false);
}

void ArcsimPawn::DoLookAround(float YawDelta)
{
	// rotate the spring arm
	BackSpringArm->AddLocalRotation(FRotator(0.0f, YawDelta, 0.0f));
}

void ArcsimPawn::DoToggleCamera()
{
	// toggle the active camera flag
	bFrontCameraActive = !bFrontCameraActive;

	FrontCamera->SetActive(bFrontCameraActive);
	BackCamera->SetActive(!bFrontCameraActive);
}

void ArcsimPawn::DoResetVehicle()
{
	// reset to a location slightly above our current one
	FVector ResetLocation = GetActorLocation() + FVector(0.0f, 0.0f, 50.0f);

	// reset to our yaw. Ignore pitch and roll
	FRotator ResetRotation = GetActorRotation();
	ResetRotation.Pitch = 0.0f;
	ResetRotation.Roll = 0.0f;

	// teleport the actor to the reset spot and reset physics
	SetActorTransform(FTransform(ResetRotation, ResetLocation, FVector::OneVector), false, nullptr, ETeleportType::TeleportPhysics);

	GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);
}

void ArcsimPawn::ResetGNSSReplay()
{
	CurrentTrajectoryIndex = 0;
	CurrentSimTime = 0.0f;
	TrajectoryTimer = 0.0f;
	
	if (TrajectoryRef && TrajectoryRef->Positions.Num() > 0)
	{
		// Teleport to first GNSS point
		SetActorLocationAndRotation(
			TrajectoryRef->Positions[0], 
			TrajectoryRef->Rotations[0], 
			false, 
			nullptr, 
			ETeleportType::TeleportPhysics
		);
		
		if (GetMesh())
		{
			GetMesh()->SetPhysicsLinearVelocity(FVector::ZeroVector);
			GetMesh()->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
		}
		
		UE_LOG(LogTemp, Warning, TEXT("GNSS Replay: Reset to start position"));
	}
}

#undef LOCTEXT_NAMESPACE