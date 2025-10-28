# GPS Loader Widget - C++ Direct Usage Guide

## Overview

The `GPSLoaderWidget` can now be created **entirely in C++** without needing any Blueprint setup. The UI is generated programmatically.

## Method 1: Pure C++ (No Blueprint Required) ? **RECOMMENDED**

### Step 1: Create Widget from C++ Code

Add this to your **GameMode**, **PlayerController**, or **Pawn** class:

```cpp
// In your .h file
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "YourGameMode.generated.h"

class UGPSLoaderWidget;

UCLASS()
class RCSIM_API AYourGameMode : public AGameModeBase
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    UGPSLoaderWidget* GPSWidget;
};
```

```cpp
// In your .cpp file
#include "YourGameMode.h"
#include "GPSLoaderWidget.h"
#include "Blueprint/UserWidget.h"

void AYourGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Create GPS Loader Widget directly from C++ class
    GPSWidget = CreateWidget<UGPSLoaderWidget>(GetWorld(), UGPSLoaderWidget::StaticClass());
 
    if (GPSWidget)
    {
        // Add to viewport
 GPSWidget->AddToViewport();
        
        UE_LOG(LogTemp, Warning, TEXT("GPS Loader Widget created and added to viewport"));
    }
}
```

**That's it!** The widget will create its own UI (button + text) automatically.

### Step 2: Configure Vehicle

Just like before, set up your vehicle pawn:
- `Enable GPS Replay = true`
- `Trajectory Ref` = Your TrajectoryActor
- `Use Timestamp Playback = true`

### Step 3: Test

1. **Add TrajectoryActor** to your level
2. **Play** the game
3. **Click "Import GPS File"** button
4. **Select** your CSV file
5. **Done!**

---

## Method 2: Blueprint (Legacy Support)

If you prefer Blueprints, you can still create a Widget Blueprint and parent it to `GPSLoaderWidget` - the C++ class will work either way.

---

## Complete Example: GameMode with GPS Widget

Here's a complete example for your rcsim project:

```cpp
// RcsimGameMode.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RcsimGameMode.generated.h"

class UGPSLoaderWidget;
class ATrajectoryActor;

UCLASS()
class RCSIM_API ARcsimGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    ARcsimGameMode();

protected:
    virtual void BeginPlay() override;

private:
    // GPS Loader UI Widget
    UPROPERTY()
    UGPSLoaderWidget* GPSLoaderWidget;
};
```

```cpp
// RcsimGameMode.cpp
#include "RcsimGameMode.h"
#include "GPSLoaderWidget.h"
#include "TrajectoryActor.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"

ARcsimGameMode::ARcsimGameMode()
{
    // Set default pawn class, etc.
}

void ARcsimGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Create GPS Loader Widget
    GPSLoaderWidget = CreateWidget<UGPSLoaderWidget>(
     GetWorld(), 
        UGPSLoaderWidget::StaticClass()
    );

    if (GPSLoaderWidget)
    {
   // Add to screen
        GPSLoaderWidget->AddToViewport();

        // Optional: Manually assign TrajectoryActor if needed
TArray<AActor*> FoundActors;
        UGameplayStatics::GetAllActorsOfClass(
            GetWorld(), 
       ATrajectoryActor::StaticClass(), 
  FoundActors
        );
        
        if (FoundActors.Num() > 0)
        {
       GPSLoaderWidget->TrajectoryActorRef = Cast<ATrajectoryActor>(FoundActors[0]);
            UE_LOG(LogTemp, Warning, TEXT("GameMode: TrajectoryActor assigned to GPS widget"));
        }

        UE_LOG(LogTemp, Warning, TEXT("GameMode: GPS Loader Widget created successfully"));
    }
    else
    {
 UE_LOG(LogTemp, Error, TEXT("GameMode: Failed to create GPS Loader Widget"));
    }
}
```

---

## Alternative: Create from Player Controller

```cpp
// YourPlayerController.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "YourPlayerController.generated.h"

class UGPSLoaderWidget;

UCLASS()
class RCSIM_API AYourPlayerController : public APlayerController
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY()
    UGPSLoaderWidget* GPSWidget;
};
```

```cpp
// YourPlayerController.cpp
#include "YourPlayerController.h"
#include "GPSLoaderWidget.h"

void AYourPlayerController::BeginPlay()
{
 Super::BeginPlay();

    GPSWidget = CreateWidget<UGPSLoaderWidget>(this, UGPSLoaderWidget::StaticClass());
    
    if (GPSWidget)
    {
        GPSWidget->AddToViewport();
    }
}
```

---

## Features

### Automatic UI Creation
- ? Button positioned at (50, 50) with size (200, 50)
- ? Status text positioned at (50, 120) with size (400, 30)
- ? Professional styling with Roboto font
- ? Color-coded status messages (green = success, red = error)

### Smart Behavior
- ? Auto-finds TrajectoryActor in the level
- ? Handles file dialog (Windows)
- ? Updates status text with load results
- ? Shows file name and point count

### Backward Compatible
- ? Works with Blueprint widgets (if you already created one)
- ? Works pure C++ (no Blueprint needed)

---

## Customization

### Change Button Position
Edit `CreateWidgets()` in `GPSLoaderWidget.cpp`:

```cpp
ButtonSlot->SetPosition(FVector2D(100.0f, 100.0f)); // New position
ButtonSlot->SetSize(FVector2D(300.0f, 60.0f));      // New size
```

### Change Button Text
```cpp
ButtonLabel->SetText(FText::FromString(TEXT("??? Load GPS Trajectory")));
```

### Change Font Size
```cpp
FSlateFontInfo ButtonFont = FSlateFontInfo(
    FPaths::EngineContentDir() / TEXT("Slate/Fonts/Roboto-Bold.ttf"), 
    16  // Larger font size
);
```

### Add Background Color to Button
```cpp
// In CreateWidgets(), after creating BtnImportGPS:
FButtonStyle ButtonStyle;
ButtonStyle.Normal.TintColor = FSlateColor(FLinearColor(0.1f, 0.5f, 0.1f)); // Dark green
ButtonStyle.Hovered.TintColor = FSlateColor(FLinearColor(0.2f, 0.7f, 0.2f)); // Light green
ButtonStyle.Pressed.TintColor = FSlateColor(FLinearColor(0.05f, 0.3f, 0.05f)); // Very dark green
BtnImportGPS->SetStyle(ButtonStyle);
```

---

## Comparison: Blueprint vs C++

| Aspect | Blueprint | Pure C++ |
|--------|-----------|----------|
| Setup Time | 5 minutes | 2 minutes |
| Code Visibility | Visual Editor | Source Code |
| Version Control | Binary (harder) | Text (easier) |
| Customization | UI Editor | Code |
| Performance | Identical | Identical |
| Debugging | Limited | Full C++ debugging |

---

## Troubleshooting

### Widget doesn't appear
**Fix:**
```cpp
// Make sure you have a valid World context
if (GetWorld())
{
    GPSWidget = CreateWidget<UGPSLoaderWidget>(GetWorld(), UGPSLoaderWidget::StaticClass());
}
```

### Button doesn't respond
**Fix:** Check Output Log for "Button click event bound" message. If missing, the button wasn't created properly.

### File dialog doesn't open
**Fix:** Currently only Windows is supported. For other platforms, you'll need to implement platform-specific file dialogs.

---

## Benefits of C++ Approach

1. **No Blueprint setup required** - Everything is in code
2. **Better version control** - All UI in `.cpp` files
3. **Easier to modify** - Change code, recompile, done
4. **Professional** - Industry-standard approach
5. **Debuggable** - Full C++ debugging support
6. **Portable** - Copy files to other projects easily

---

## Next Steps

Now that you have a pure C++ UI:

1. ? **Customize** the appearance in code
2. ? **Add more buttons** (Clear trajectory, Export data, etc.)
3. ? **Create dropdown menus** for playback speed
4. ? **Add progress bars** for file loading
5. ? **Implement drag-and-drop** file support

Check the main `GPS_PLAYBACK_README.md` for more features and documentation.

---

**No Blueprints needed - just pure C++ power! ??**
