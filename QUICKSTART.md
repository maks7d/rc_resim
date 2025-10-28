# Quick Start Guide - GPS Trajectory Playback

## Two Setup Options

### ?? Option A: Pure C++ (2 minutes) - **RECOMMENDED**

**No Blueprint needed!** Skip to [C++ Setup](#cpp-setup) below.

### ?? Option B: Blueprint UI (5 minutes) - Legacy

Follow the [Blueprint Setup](#blueprint-setup) if you prefer visual editing.

---

## ?? C++ Setup (Pure Code, No Blueprint)

### Step 1: Create Widget from Code (1 min)

Add this to your **GameMode** or **PlayerController**:

```cpp
// In BeginPlay() or similar
#include "GPSLoaderWidget.h"

UGPSLoaderWidget* GPSWidget = CreateWidget<UGPSLoaderWidget>(
    GetWorld(), 
    UGPSLoaderWidget::StaticClass()
);

if (GPSWidget)
{
    GPSWidget->AddToViewport();
}
```

**That's it!** The UI is created automatically.

### Step 2: Add TrajectoryActor & Configure Vehicle (1 min)

Same as Blueprint method:
1. Add `TrajectoryActor` to your level
2. Set vehicle: `Enable GPS Replay = true`, `Trajectory Ref = TrajectoryActor`

### Step 3: Test!

Play ? Click "Import GPS File" ? Select CSV ? Done! ??

?? **Full C++ details:** See `CPP_UI_GUIDE.md`

---

## ?? Blueprint Setup (Visual Editor)

## Step-by-Step Setup (5 minutes)

### Part 1: Create the UI Widget (2 min)

1. **Open Unreal Editor** with your rcsim project

2. **Create Widget Blueprint:**
   - Content Browser ? Right-click ? User Interface ? Widget Blueprint
   - Name: `WBP_GPSLoader`
   - Double-click to open

3. **Design the UI:**
   ```
   Canvas Panel (root)
   ?? Button [BtnImportGPS]
   ?  ?? Text: "?? Import GPS File"
   ?     Position: (50, 50)
   ?     Size: (200, 50)
   ?
   ?? Text Block [FileNameText]
      ?? Text: "No file loaded"
         Position: (50, 120)
    Size: (400, 30)
   ```

4. **Set Parent Class:**
   - Click **File** ? **Reparent Blueprint**
   - Search for `GPSLoaderWidget`
   - Select it and click **Reparent**

5. **IMPORTANT - Bind Widgets:**
   - Select the Button ? Details panel ? **Is Variable** ?
   - Name it exactly: `BtnImportGPS`
   - Select the Text Block ? **Is Variable** ?  
   - Name it exactly: `FileNameText`
   
   *These names must match the C++ UPROPERTY declarations!*

6. **Compile & Save**

### Part 2: Add TrajectoryActor to Level (1 min)

1. **Place Actors panel** (or Window ? Place Actors)
2. Search: `TrajectoryActor`
3. **Drag** it into your level (anywhere is fine)
4. The actor has no visible mesh - it just stores GPS data

### Part 3: Display the UI Widget (1 min)

**Option A: From GameMode (Recommended)**

1. Open your GameMode Blueprint (or create one)
2. **Event BeginPlay:**
   ```
   Event BeginPlay
       ?
   Create Widget
   • Class: WBP_GPSLoader
       ?
 Add to Viewport
     ?
   Set Input Mode UI Only (optional)
 ```

**Option B: From Player Controller**

Same as above but in your PlayerController Blueprint.

**Option C: Quick Test - From Level Blueprint**

1. Open **Level Blueprint** (Blueprints ? Open Level Blueprint)
2. Same setup as GameMode above

### Part 4: Configure Vehicle for GPS Replay (1 min)

1. **Select your vehicle pawn** in the level (or open its blueprint)

2. **Find "GPS Replay" section** in Details panel

3. **Set these properties:**
   ```
   GPS Replay
   ?? Enable GPS Replay: ? TRUE
   ?? Trajectory Ref: [Drag TrajectoryActor from level]
   ?? Use Timestamp Playback: ? TRUE
   ?? Playback Speed: 1.0
   ?? Use Physics Movement: ? false (for now)
   ```

4. **Save all**

### Part 5: Test! (1 min)

1. **Click Play** ??
2. **Click "Import GPS File"** button
3. **Select** `GPS_Data_Sample.csv` (in project root)
4. **Watch** your vehicle follow the GPS trajectory!

---

## Creating Your Own GPS Data

### Format Template:
```csv
timestamp,lat,lon,alt,heading,speed
1698200000.000,45.123456,5.678901,250.5,0.0,0.0
1698200001.000,45.123457,5.678902,250.5,45.0,5.0
1698200002.000,45.123458,5.678903,250.5,90.0,8.0
```

### Field Meanings:
- **timestamp**: Unix time in seconds (or any increasing number)
- **lat**: Latitude (-90 to 90)
- **lon**: Longitude (-180 to 180)
- **alt**: Altitude in meters
- **heading**: Direction in degrees (0=North, 90=East, 180=South, 270=West)
- **speed**: Speed in m/s (informational only for now)

### Tips:
- First point becomes origin (0,0,0) in Unreal
- Use 0.1s intervals (100ms) for smooth playback
- Make sure timestamps are increasing
- Heading determines vehicle rotation

---

## Playback Options Explained

### Timestamp-Based (Default) ? **RECOMMENDED**
- Plays GPS data at **real-time speed** based on timestamps
- Most accurate replay of recorded data
- Set: `Use Timestamp Playback = true`

### Fixed Delay Mode
- Steps through points with fixed delay
- Good for debugging
- Set: `Use Timestamp Playback = false`, `Trajectory Point Delay = 0.1`

### Physics-Based (Advanced)
- Vehicle physically drives to waypoints
- More realistic but slower
- Set: `Use Physics Movement = true`

---

## Common Issues & Fixes

### ? "Cannot find GPSLoaderWidget"
**Fix:** Make sure you compiled the C++ code in Visual Studio before creating the blueprint.

### ? Button click does nothing
**Fix:** 
1. Widget variable names must be **exact**: `BtnImportGPS` and `FileNameText`
2. Must check **Is Variable** checkbox for both
3. Reparent blueprint to `GPSLoaderWidget` class

### ? Vehicle doesn't move
**Fix:**
1. ? `Enable GPS Replay = true`
2. ? `Trajectory Ref` must be set to TrajectoryActor
3. ? GPS file must be loaded successfully (check Output Log)

### ? Can't see trajectory points
**Fix:**
- Green spheres are huge (5m radius) but might be far from camera
- Check Output Log for "Loaded X GPS points" message
- Trajectory visualization only shows when game is running (Tick function)

### ? File dialog doesn't open (non-Windows)
**Fix:** 
Currently only Windows is supported. For other platforms, you'll need to:
- Use Unreal's File Dialog plugin, or
- Hardcode a file path for testing

---

## Performance Notes

- **< 1000 points**: No performance impact
- **1000-10000 points**: Slight visualization overhead (debug spheres)
- **> 10000 points**: Disable debug visualization in `TrajectoryActor::Tick()`

To disable visualization:
```cpp
// In TrajectoryActor.cpp, Tick() function
// Comment out the drawing code or add a toggle:
if (bShowDebugVisualization) // Add this property
{
    // ... DrawDebugSphere/Line code ...
}
```

---

## Next Steps

? **Try different playback speeds:**
   - `Playback Speed = 0.5` ? Half speed (slow motion)
   - `Playback Speed = 2.0` ? Double speed
   - `Playback Speed = 10.0` ? Fast forward

? **Record your own data:**
   - Use GPS logger app on phone
   - Export as CSV matching the format
   - Import and replay!

? **Multiple vehicles:**
   - Create multiple pawns
   - Each references same or different TrajectoryActor
   - Synchronized playback

? **Add camera follow:**
   - Set vehicle as camera target
   - Cinematic replay of your data

---

## Support

Check `GPS_PLAYBACK_README.md` for full technical documentation.

**Log Messages:**
- All GPS loading shows in **Output Log** (Window ? Developer Tools ? Output Log)
- Look for "TrajectoryActor:" and "GPSLoaderWidget:" messages

**Debug Tips:**
- Press ` (tilde) in-game to open console
- Type `stat fps` to show performance
- Use `F8` to eject from pawn and fly around to see trajectory

---

**That's it! You now have a complete GPS trajectory replay system! ??**
