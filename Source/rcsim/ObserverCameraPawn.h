#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "ObserverCameraPawn.generated.h"

class USpringArmComponent;
class UCameraComponent;

UCLASS()
class RCSIM_API AObserverCameraPawn : public APawn
{
	GENERATED_BODY()

public:
	AObserverCameraPawn();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// The actor the camera will follow. If null, will use the player pawn.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer")
	AActor* TargetActor;

	// Offset applied to the target location (world units)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer")
	FVector TargetOffset;

	// Interpolation speed used when following
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Observer")
	float InterpSpeed;

protected:
	// Components
	UPROPERTY(VisibleAnywhere, Category = "Components")
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	UCameraComponent* Camera;

	// Fixed camera height (world Z) used to keep camera at constant altitude
	float InitialZ;
};