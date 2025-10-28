// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "rcsimPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UChaosWheeledVehicleMovementComponent;
class ATrajectoryActor;
struct FInputActionValue;

/**
 *  Vehicle Pawn class
 *  Handles common functionality for all vehicle types,
 *  including input handling and camera management.
 *  
 *  Specific vehicle configurations are handled in subclasses.
 */
UCLASS(abstract)
class ArcsimPawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()

	/** Spring Arm for the front camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* FrontSpringArm;

	/** Front Camera component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FrontCamera;

	/** Spring Arm for the back camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* BackSpringArm;

	/** Back Camera component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category ="Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* BackCamera;

	/** Cast pointer to the Chaos Vehicle movement component */
	TObjectPtr<UChaosWheeledVehicleMovementComponent> ChaosVehicleMovement;

protected:

	/** Steering Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* SteeringAction;

	/** Throttle Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ThrottleAction;

	/** Brake Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* BrakeAction;

	/** Handbrake Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* HandbrakeAction;

	/** Look Around Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* LookAroundAction;

	/** Toggle Camera Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ToggleCameraAction;

	/** Reset Vehicle Action */
	UPROPERTY(EditAnywhere, Category="Input")
	UInputAction* ResetVehicleAction;

	/** Keeps track of which camera is active */
	bool bFrontCameraActive = false;

public:

	/** Reference to trajectory actor for GNSS replay */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GNSS Replay")
	ATrajectoryActor* TrajectoryRef;

	/** Enable GNNS replay mode */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GNSS Replay")
	bool bEnableGNSSReplay = false;

	/** Use physics-based movement with realistic vehicle dynamics (recommended) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GNSS Replay")
	bool bUsePhysicsMovement = true;

	/** PID Proportional gain for steering control */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GNSS Replay|PID Control", meta=(EditCondition="bUsePhysicsMovement"))
	float KpYaw = 0.5f;

	/** PID Integral gain for steering control (reduces steady-state error) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GNSS Replay|PID Control", meta=(EditCondition="bUsePhysicsMovement"))
	float KiYaw = 0.01f;

	/** PID Derivative gain for steering control (reduces overshoot) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GNSS Replay|PID Control", meta=(EditCondition="bUsePhysicsMovement"))
	float KdYaw = 0.1f;

	/** PID Proportional gain for throttle/brake control */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GNSS Replay|PID Control", meta=(EditCondition="bUsePhysicsMovement"))
	float KpSpeed = 0.1f;

	/** PID Integral gain for speed control */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GNSS Replay|PID Control", meta=(EditCondition="bUsePhysicsMovement"))
	float KiSpeed = 0.005f;

	/** PID Derivative gain for speed control */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GNSS Replay|PID Control", meta=(EditCondition="bUsePhysicsMovement"))
	float KdSpeed = 0.05f;

	/** Distance threshold to consider a waypoint reached (cm) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GNSS Replay|PID Control", meta=(EditCondition="bUsePhysicsMovement"))
	float AcceptRadius = 500.0f;

	/** Look-ahead distance for path following (cm) - helps with smoother turns */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GNSS Replay|PID Control", meta=(EditCondition="bUsePhysicsMovement"))
	float LookAheadDistance = 1000.0f;

	/** Maximum steering angle adjustment per frame (prevents jerky movements) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GNSS Replay|PID Control", meta=(EditCondition="bUsePhysicsMovement"))
	float MaxSteeringRate = 2.0f;

	/** Use timestamp-based playback (deprecated - use physics mode instead) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GNSS Replay|Legacy", meta=(EditCondition="!bUsePhysicsMovement"))
	bool bUseTimestampPlayback = false;

	/** Playback speed multiplier (deprecated - use physics mode) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GNSS Replay|Legacy", meta=(EditCondition="!bUsePhysicsMovement"))
	float PlaybackSpeed = 1.0f;

	/** Time to wait between trajectory points (deprecated) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="GNSS Replay|Legacy", meta=(EditCondition="!bUsePhysicsMovement"))
	float TrajectoryPointDelay = 0.1f;

	/** Current trajectory point index */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GNSS Replay|Status")
	int32 CurrentTrajectoryIndex = 0;

	/** Target waypoint index (may be ahead for look-ahead control) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GNSS Replay|Status")
	int32 TargetWaypointIndex = 0;

	/** Current vehicle speed in km/h */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GNSS Replay|Status")
	float CurrentSpeed = 0.0f;

	/** Target speed from GNSS data in km/h */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GNSS Replay|Status")
	float TargetSpeed = 0.0f;

	/** Elapsed simulation time for timestamp-based playback */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="GNSS Replay|Status")
	float CurrentSimTime = 0.0f;

	/** Reset GNSS replay to start */
	UFUNCTION(BlueprintCallable, Category="GNSS Replay")
	void ResetGNSSReplay();

private:

	/** Timer for trajectory playback (legacy mode) */
	float TrajectoryTimer = 0.0f;

	/** PID controller state - accumulated error for integral term (yaw) */
	float IntegralErrorYaw = 0.0f;

	/** PID controller state - previous error for derivative term (yaw) */
	float PreviousErrorYaw = 0.0f;

	/** PID controller state - accumulated error for integral term (speed) */
	float IntegralErrorSpeed = 0.0f;

	/** PID controller state - previous error for derivative term (speed) */
	float PreviousErrorSpeed = 0.0f;

	/** Smooth steering input value to prevent sudden changes */
	float SmoothedSteeringInput = 0.0f;

public:
	ArcsimPawn();

	// Begin Pawn interface

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	// End Pawn interface

	// Begin Actor interface
	
	virtual void BeginPlay() override;

	virtual void Tick(float Delta) override;

	// End Actor interface

protected:

	/** Handles steering input */
	void Steering(const FInputActionValue& Value);

	/** Handles throttle input */
	void Throttle(const FInputActionValue& Value);

	/** Handles brake input */
	void Brake(const FInputActionValue& Value);

	/** Handles brake start/stop inputs */
	void StartBrake(const FInputActionValue& Value);
	void StopBrake(const FInputActionValue& Value);

	/** Handles handbrake start/stop inputs */
	void StartHandbrake(const FInputActionValue& Value);
	void StopHandbrake(const FInputActionValue& Value);

	/** Handles look around input */
	void LookAround(const FInputActionValue& Value);

	/** Handles toggle camera input */
	void ToggleCamera(const FInputActionValue& Value);

	/** Handles reset vehicle input */
	void ResetVehicle(const FInputActionValue& Value);

public:

	/** Handle steering input by input actions or mobile interface */
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoSteering(float SteeringValue);

	/** Handle throttle input by input actions or mobile interface */
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoThrottle(float ThrottleValue);

	/** Handle brake input by input actions or mobile interface */
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoBrake(float BrakeValue);

	/** Handle brake start input by input actions or mobile interface */
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoBrakeStart();

	/** Handle brake stop input by input actions or mobile interface */
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoBrakeStop();

	/** Handle handbrake start input by input actions or mobile interface */
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoHandbrakeStart();

	/** Handle handbrake stop input by input actions or mobile interface */
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoHandbrakeStop();

	/** Handle look input by input actions or mobile interface */
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoLookAround(float YawDelta);

	/** Handle toggle camera input by input actions or mobile interface */
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoToggleCamera();

	/** Handle reset vehicle input by input actions or mobile interface */
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoResetVehicle();

protected:

	/** Called when the brake lights are turned on or off */
	UFUNCTION(BlueprintImplementableEvent, Category="Vehicle")
	void BrakeLights(bool bBraking);

public:
	/** Returns the front spring arm subobject */
	FORCEINLINE USpringArmComponent* GetFrontSpringArm() const { return FrontSpringArm; }
	/** Returns the front camera subobject */
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FrontCamera; }
	/** Returns the back spring arm subobject */
	FORCEINLINE USpringArmComponent* GetBackSpringArm() const { return BackSpringArm; }
	/** Returns the back camera subobject */
	FORCEINLINE UCameraComponent* GetBackCamera() const { return BackCamera; }
	/** Returns the cast Chaos Vehicle Movement subobject */
	FORCEINLINE const TObjectPtr<UChaosWheeledVehicleMovementComponent>& GetChaosVehicleMovement() const { return ChaosVehicleMovement; }
};
