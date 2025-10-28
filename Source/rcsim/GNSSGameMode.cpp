// Example GameMode implementation
// This creates the GNSS Loader Widget automatically when the game starts

#include "GNSSGameMode.h"
#include "GNSSLoaderWidget.h"
#include "TrajectoryActor.h"
#include "Kismet/GameplayStatics.h"

AGNSSGameMode::AGNSSGameMode()
{
 // Set default pawn class here if needed
 // DefaultPawnClass = AYourPawn::StaticClass();
}

void AGNSSGameMode::BeginPlay()
{
 Super::BeginPlay();

 UE_LOG(LogTemp, Warning, TEXT("GNSSGameMode: Creating GNSS Loader Widget"));

 // Create GNSS Loader Widget (UI is built automatically in C++)
 GNSSLoaderWidget = CreateWidget<UGNSSLoaderWidget>(
 GetWorld(), 
 UGNSSLoaderWidget::StaticClass()
 );

 if (GNSSLoaderWidget)
 {
 // Add to viewport - this makes it visible
 GNSSLoaderWidget->AddToViewport();

 // Optional: The widget auto-finds TrajectoryActor, but you can assign it manually too
 TArray<AActor*> FoundActors;
 UGameplayStatics::GetAllActorsOfClass(
 GetWorld(), 
 ATrajectoryActor::StaticClass(), 
 FoundActors
 );
 
 if (FoundActors.Num() >0)
 {
 GNSSLoaderWidget->TrajectoryActorRef = Cast<ATrajectoryActor>(FoundActors[0]);
 UE_LOG(LogTemp, Warning, TEXT("GNSSGameMode: Found and assigned TrajectoryActor"));
 }
 else
 {
 UE_LOG(LogTemp, Warning, TEXT("GNSSGameMode: No TrajectoryActor found - add one to the level"));
 }

 UE_LOG(LogTemp, Warning, TEXT("GNSSGameMode: GNSS Loader Widget successfully created and displayed!"));
 UE_LOG(LogTemp, Warning, TEXT("GNSSGameMode: Click the 'Import GNSS File' button to load trajectory data"));
 }
 else
 {
 UE_LOG(LogTemp, Error, TEXT("GNSSGameMode: Failed to create GNSS Loader Widget"));
 }
}
