// GNSS Loader Widget for importing CSV trajectory files

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GNSSLoaderWidget.generated.h"

class ATrajectoryActor;
class UButton;
class UTextBlock;
class UCanvasPanel;
class UCanvasPanelSlot;

/**
 * Widget for loading GNSS trajectory data from CSV files
 * Can be used directly from C++ (creates UI programmatically) or as a Blueprint parent class
 * Expected CSV format: timestamp,lat,lon,alt,heading,speed
 */
UCLASS()
class RCSIM_API UGNSSLoaderWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Reference to the trajectory actor to load data into
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GNSS")
	ATrajectoryActor* TrajectoryActorRef;

	// Called when the Import button is clicked
	UFUNCTION(BlueprintCallable, Category = "GNSS")
	void OnImportButtonClicked();

	// Text to display the loaded file name and status
	UPROPERTY(BlueprintReadOnly, Category = "GNSS")
	UTextBlock* FileNameText;

	// Button to trigger file import
	UPROPERTY(BlueprintReadOnly, Category = "GNSS")
	UButton* BtnImportGNSS;

protected:
	virtual void NativeConstruct() override;
	
	// Called to create UI widgets programmatically (if not created in Blueprint)
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	// Root canvas panel
	UPROPERTY()
	UCanvasPanel* RootCanvas;

	// Create the UI widgets programmatically
	void CreateWidgets();
};
