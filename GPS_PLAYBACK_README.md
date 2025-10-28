# GPS Trajectory Playback System - Implementation Guide

## Overview

This system allows you to load real GNSS/GPS data from CSV files and replay them in real-time within Unreal Engine 5. The implementation replaces all dummy/fake GPS data with a complete file-based loading and playback system.

## Features Implemented

? **CSV File Loading**: Import GPS trajectory data from standard CSV files  
? **Real-Time Playback**: Play back trajectories at real-time speed based on timestamps  
? **User Interface**: Simple UI widget for file selection and status display  
? **Multiple Playback Modes**:
   - Timestamp-based (real-time with timestamps)
   - Fixed delay (simple sequential playback)
   - Physics-based (PID controller following waypoints)

## CSV File Format

The expected CSV format is:
```csv
timestamp,lat,lon,alt,heading,speed
1698200000.000,45.123456,5.678901,250.5,120.0,2.5
1698200000.100,45.123457,5.678902,250.6,121.0,2.7
1698200000.200,45.123458,5.678904,250.7,122.0,2.8
```

**Column Descriptions:**
- `timestamp`: Unix timestamp in seconds (with decimal for subseconds)
- `lat`: Latitude in decimal degrees
- `lon`: Longitude in decimal degrees
- `alt`: Altitude in meters
- `heading`: Heading/yaw in degrees (0-360, where 0 = North, 90 = East)
- `speed`: Speed in meters per second

## Files Modified/Created

### New Files:
1. **`Source/rcsim/GPSLoaderWidget.h`** - UI widget header
2. **`Source/rcsim/GPSLoaderWidget.cpp`** - UI widget implementation
3. **`GPS_Data_Sample.csv`** - Sample GPS data file

### Modified Files:
1. **`Source/rcsim/TrajectoryActor.h`** - Added CSV loading, removed dummy GPS
2. **`Source/rcsim/TrajectoryActor.cpp`** - Implemented CSV parser
3. **`Source/rcsim/rcsimPawn.h`** - Added timestamp-based playback
4. **`Source/rcsim/rcsimPawn.cpp`** - Implemented real-time GPS replay

## Setup Instructions

### 1. Add TrajectoryActor to Your Level

1. In Unreal Editor, go to **Place Actors** panel
2. Search for "TrajectoryActor"
3. Drag it into your level
4. The actor will visualize the loaded GPS trajectory

### 2. Create the GPS Loader UI Widget Blueprint

1. In **Content Browser**, right-click ? **User Interface** ? **Widget Blueprint**
2. Name it `WBP_GPSLoader`
3. Open the widget and add the following components:
   - **Button** named `BtnImportGPS` with text "Import GPS File"
   - **Text Block** named `FileNameText` for status display
4. In the **Graph** tab:
   - Set the **Parent Class** to `GPSLoaderWidget` (your C++ class)
5. Compile and save

### 3. Add the Widget to Your Game Mode or Player Controller

**Option A: Blueprint**
1. Open your GameMode or PlayerController blueprint
2. In **Event BeginPlay**:
   - Add node: **Create Widget** (select `WBP_GPSLoader`)
   - Add node: **Add to Viewport**

**Option B: C++ (already done in code)**
The widget can be spawned from C++ - see `rcsimPawn.cpp` for examples.

### 4. Configure Your Vehicle Pawn

Select your vehicle pawn in the level or blueprint and configure these properties:

**GPS Replay Settings:**
- ? `bEnableGPSReplay` = true
- `TrajectoryRef` = (assign your TrajectoryActor from the level)
- `bUseTimestampPlayback` = true (for real-time playback)
- `PlaybackSpeed` = 1.0 (1.0 = real-time, 2.0 = 2x speed)

**Alternative Playback Modes:**
- `bUsePhysicsMovement` = true ? Uses PID controller (more realistic but slower)
- `bUseTimestampPlayback` = false ? Fixed delay mode

## Usage

### At Runtime:

1. **Launch your game/simulation**
2. **Click "Import GPS File" button**
3. **Select your CSV file** (use `GPS_Data_Sample.csv` to test)
4. **The vehicle will automatically start replaying** the GPS trajectory

### Playback Controls:

The system automatically:
- Loads GPS coordinates and converts them to Unreal world space
- Interpolates between points for smooth motion
- Loops back to start when finished
- Shows real-time status in the UI

## Technical Details

### Coordinate Conversion

GPS coordinates (latitude/longitude) are converted to Unreal Engine's coordinate system:
- **Origin**: First GPS point becomes (0, 0, 0) in Unreal
- **X axis** (East): Longitude difference × 111,000 m/degree × 100 (cm)
- **Y axis** (North): Latitude difference × 111,000 m/degree × 100 (cm)
- **Z axis** (Up): Altitude difference × 100 (cm)

*Note: This is a simple approximation suitable for small areas. For large-scale simulations, consider using a proper map projection.*

### Timestamp-Based Playback

The system tracks elapsed time and interpolates between GPS points:

```cpp
CurrentSimTime += DeltaTime * PlaybackSpeed;
double CurrentTrajectoryTime = StartTime + CurrentSimTime;

// Find segment where current time falls
for (int32 i = 0; i < Timestamps.Num() - 1; ++i)
{
    if (Timestamps[i] <= CurrentTrajectoryTime && Timestamps[i+1] > CurrentTrajectoryTime)
    {
        float Alpha = (CurrentTrajectoryTime - Timestamps[i]) / (Timestamps[i+1] - Timestamps[i]);
   FVector InterpolatedPos = Lerp(Positions[i], Positions[i+1], Alpha);
      // Apply position...
    }
}
```

## Playback Modes Comparison

| Mode | Description | Use Case |
|------|-------------|----------|
| **Timestamp-based** | Uses real timestamps for accurate replay | Replaying logged data at actual speeds |
| **Fixed Delay** | Steps through points with fixed time delay | Simple testing, debugging |
| **Physics-based (PID)** | Vehicle follows waypoints using physics | Realistic vehicle behavior |

## Troubleshooting

### "No TrajectoryActor found"
- Make sure you added a TrajectoryActor to your level
- Check that it's not deleted or hidden

### "Failed to load file"
- Verify CSV format matches exactly
- Check file path is accessible
- Ensure first line is the header: `timestamp,lat,lon,alt,heading,speed`

### Vehicle doesn't move
- Check `bEnableGPSReplay` is set to **true**
- Verify GPS data was loaded (check Output Log)
- Ensure `TrajectoryRef` points to the correct TrajectoryActor

### Trajectory not visible
- GPS points are drawn as green spheres in the world
- Make sure you're close enough to see them (500cm/5m radius)
- Check the **Show** ? **Visualizers** menu in the editor

## API Reference

### TrajectoryActor

**`bool LoadGPSFromCSV(const FString& FilePath)`**
- Loads GPS data from CSV file
- Returns true on success
- Automatically converts coordinates to Unreal space

**`void ClearTrajectory()`**
- Clears all loaded GPS data

**Arrays:**
- `TArray<FVector> Positions` - Unreal world positions (cm)
- `TArray<FRotator> Rotations` - Headings at each point
- `TArray<double> Timestamps` - Unix timestamps
- `TArray<float> Speeds` - Speeds in m/s

### GPSLoaderWidget

**`void OnImportButtonClicked()`**
- Opens file dialog and loads GPS file
- Updates UI with status
- Automatically called when button is clicked

### Vehicle Pawn

**`void ResetGPSReplay()`**
- Resets playback to the beginning
- Teleports vehicle to first GPS point

## Example: Creating GPS Data from ROS Bag

If you have ROS bag files with GPS data:

```python
import rosbag
import csv

bag = rosbag.Bag('your_data.bag')
with open('gps_output.csv', 'w') as f:
    writer = csv.writer(f)
    writer.writerow(['timestamp', 'lat', 'lon', 'alt', 'heading', 'speed'])
    
for topic, msg, t in bag.read_messages(topics=['/gps/fix', '/imu/heading']):
   if topic == '/gps/fix':
    writer.writerow([
       msg.header.stamp.to_sec(),
            msg.latitude,
        msg.longitude,
        msg.altitude,
    heading,  # from IMU
        speed     # from GPS or odometry
        ])
```

## Future Enhancements

Potential improvements:
- [ ] Support for multiple trajectory formats (KML, GPX, NMEA)
- [ ] Real-time streaming from GPS device
- [ ] Advanced coordinate transformations (UTM, Lambert, etc.)
- [ ] Trajectory editing tools
- [ ] Speed-based playback (instead of timestamp)
- [ ] Multi-vehicle synchronized playback

## License

Part of the rcsim project. See project LICENSE file.

---

**Last Updated**: Implementation completed with timestamp-based real-time playback
**Version**: 1.0
