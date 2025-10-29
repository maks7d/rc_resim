// GNSS Loader Widget implementation (filename matches class)

#include "GNSSLoaderWidget.h"
#include "TrajectoryActor.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include <commdlg.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif

TSharedRef<SWidget> UGNSSLoaderWidget::RebuildWidget()
{
	if (!BtnImportGNSS || !FileNameText)
	{
		CreateWidgets();
	}

	TSharedRef<SWidget> SlateWidget = Super::RebuildWidget();

	if (RootCanvas)
	{
		return RootCanvas->TakeWidget();
	}

	return SlateWidget;
}

void UGNSSLoaderWidget::CreateWidgets()
{
	RootCanvas = NewObject<UCanvasPanel>(this, UCanvasPanel::StaticClass());
	RootCanvas->SetVisibility(ESlateVisibility::Visible);

	BtnImportGNSS = NewObject<UButton>(this, UButton::StaticClass(), TEXT("BtnImportGNSS"));
	UCanvasPanelSlot* ButtonSlot = RootCanvas->AddChildToCanvas(BtnImportGNSS);
	if (ButtonSlot)
	{
		ButtonSlot->SetPosition(FVector2D(50.0f,50.0f));
		ButtonSlot->SetSize(FVector2D(200.0f,50.0f));
		ButtonSlot->SetAnchors(FAnchors(0.0f,0.0f,0.0f,0.0f));
	}

	UTextBlock* ButtonLabel = NewObject<UTextBlock>(this, UTextBlock::StaticClass());
	ButtonLabel->SetText(FText::FromString(TEXT("Import GNSS File")));
	BtnImportGNSS->AddChild(ButtonLabel);

	FileNameText = NewObject<UTextBlock>(this, UTextBlock::StaticClass(), TEXT("FileNameText"));
	FileNameText->SetText(FText::FromString(TEXT("No file loaded - Click to import CSV")));

	UCanvasPanelSlot* TextSlot = RootCanvas->AddChildToCanvas(FileNameText);
	if (TextSlot)
	{
		TextSlot->SetPosition(FVector2D(50.0f,120.0f));
		TextSlot->SetSize(FVector2D(400.0f,30.0f));
		TextSlot->SetAnchors(FAnchors(0.0f,0.0f,0.0f,0.0f));
	}

	UE_LOG(LogTemp, Log, TEXT("GNSSLoaderWidget: UI created programmatically in C++ (new file)"));
}

void UGNSSLoaderWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (BtnImportGNSS)
	{
		BtnImportGNSS->OnClicked.AddDynamic(this, &UGNSSLoaderWidget::OnImportButtonClicked);
		UE_LOG(LogTemp, Log, TEXT("GNSSLoaderWidget: Button click event bound (new file)"));
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("GNSSLoaderWidget: BtnImportGNSS is null - UI creation failed"));
	}

	if (!TrajectoryActorRef)
	{
		TArray<AActor*> Found;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ATrajectoryActor::StaticClass(), Found);
		if (Found.Num() >0)
		{
			TrajectoryActorRef = Cast<ATrajectoryActor>(Found[0]);
			UE_LOG(LogTemp, Warning, TEXT("GNSSLoaderWidget: Auto-found TrajectoryActor (new file)"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("GNSSLoaderWidget: No TrajectoryActor found in level. Add one to the scene."));
		}
	}

	if (FileNameText)
	{
		FileNameText->SetText(FText::FromString(TEXT("No file loaded - Click to import CSV")));
	}
}

void UGNSSLoaderWidget::OnImportButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("GNSSLoaderWidget: Import GNSS button clicked (new file)"));

#if PLATFORM_WINDOWS
	OPENFILENAMEW ofn;
	WCHAR szFile[260] = {0 };

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = L"CSV Files\0*.CSV\0All Files\0*.*\0";
	ofn.nFilterIndex =1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle =0;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = L"Select GNSS CSV File (format: timestamp,lat,lon,alt,heading,speed)";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	BOOL bFileSelected = GetOpenFileNameW(&ofn);
	
	if (bFileSelected)
	{
		FString FilePath(ofn.lpstrFile);
		UE_LOG(LogTemp, Warning, TEXT("GNSSLoaderWidget: Selected GNSS file: %s (new file)"), *FilePath);

		if (TrajectoryActorRef)
		{
			bool bSuccess = TrajectoryActorRef->LoadGNSSFromCSV(FilePath);
			
			if (FileNameText)
			{
				if (bSuccess)
				{
					FString FileName = FPaths::GetCleanFilename(FilePath);
					int32 NumPoints = TrajectoryActorRef->Positions.Num();
					FString StatusText = FString::Printf(TEXT("Loaded: %s (%d points)"), *FileName, NumPoints);
					FileNameText->SetText(FText::FromString(StatusText));
					FileNameText->SetColorAndOpacity(FSlateColor(FLinearColor::Green));
					UE_LOG(LogTemp, Warning, TEXT("GNSSLoaderWidget: GNSS data loaded successfully! %d points (new file)"), NumPoints);
				}
				else
				{
					FileNameText->SetText(FText::FromString(TEXT("Failed to load file - Check format")));
					FileNameText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
					UE_LOG(LogTemp, Error, TEXT("GNSSLoaderWidget: Failed to load GNSS data from file (new file)"));
				}
			}
		}
		else
		{
			if (FileNameText)
			{
				FileNameText->SetText(FText::FromString(TEXT("Error: No TrajectoryActor found")));
				FileNameText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
			}
			UE_LOG(LogTemp, Error, TEXT("GNSSLoaderWidget: TrajectoryActorRef is null - cannot load GNSS data (new file)"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("GNSSLoaderWidget: File selection cancelled (new file)"));
	}
#else
	UE_LOG(LogTemp, Error, TEXT("GNSSLoaderWidget: File dialog not implemented for this platform (new file)"));
	if (FileNameText)
	{
		FileNameText->SetText(FText::FromString(TEXT("? Platform not supported")));
		FileNameText->SetColorAndOpacity(FSlateColor(FLinearColor::Red));
	}
#endif
}
