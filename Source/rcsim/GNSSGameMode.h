// Example GameMode that creates GNSS Loader UI entirely in C++
// Copy this to your project and use it as your GameMode

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GNSSLoaderWidget.h"
#include "GNSSGameMode.generated.h"

class UGNSSLoaderWidget;

/**
 * Example GameMode that automatically creates GNSS Loader UI
 * No Blueprint required!
 */
UCLASS()
class RCSIM_API AGNSSGameMode : public AGameModeBase
{
 GENERATED_BODY()

public:
 AGNSSGameMode();

protected:
 virtual void BeginPlay() override;

private:
 UPROPERTY()
 UGNSSLoaderWidget* GNSSLoaderWidget;
};
