#include "ObserverCameraPawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

AObserverCameraPawn::AObserverCameraPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	RootComponent = SpringArm;
	SpringArm->TargetArmLength =1500.f;
	SpringArm->SetRelativeRotation(FRotator(-70.f,0.f,0.f));
	SpringArm->bDoCollisionTest = false;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	TargetActor = nullptr;
	TargetOffset = FVector(0.f,0.f,1500.f);
	InterpSpeed =2.f;
	InitialZ =0.f;
}

void AObserverCameraPawn::BeginPlay()
{
	Super::BeginPlay();

	// store initial Z so we keep camera at that base height plus any offset.Z
	InitialZ = GetActorLocation().Z;

	if (!TargetActor)
	{
		// Try player pawn first
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(),0);
		if (PlayerPawn && PlayerPawn != this)
		{
			TargetActor = PlayerPawn;
		}
		else
		{
			// Fallback: search for first vehicle pawn in world
			TArray<AActor*> Found;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), Found);
			for (AActor* A : Found)
			{
				if (A && A != this)
				{
					TargetActor = A;
					break;
				}
			}
		}
	}
}

void AObserverCameraPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TargetActor)
	{
		// Keep constant Z of the camera based on initial Z and the configured TargetOffset.Z
		FVector Current = GetActorLocation();
		FVector TargetLocation = TargetActor->GetActorLocation();
		float DesiredZ = InitialZ + TargetOffset.Z; // absolute Z
		FVector DesiredLocation = FVector(TargetLocation.X, TargetLocation.Y, DesiredZ);

		FVector NewLocation = FMath::VInterpTo(Current, DesiredLocation, DeltaTime, InterpSpeed);
		SetActorLocation(NewLocation);

		// Rotate to look at the target on yaw only
		FVector ToTarget = TargetLocation - NewLocation;
		FRotator DesiredRot = ToTarget.Rotation();
		FRotator CurrentRot = GetActorRotation();
		FRotator NewRot = FMath::RInterpTo(CurrentRot, FRotator(0.0f, DesiredRot.Yaw,0.0f), DeltaTime, InterpSpeed);
		SetActorRotation(NewRot);
	}
}
