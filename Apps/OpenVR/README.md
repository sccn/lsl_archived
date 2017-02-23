# LabStreamingLayer application for OpenVR

## Build Instructions

Requirements:

* [CMake](https://cmake.org/download/)
* [Qt5](https://www.qt.io/download-open-source/)
* [OpenVR](https://github.com/ValveSoftware/openvr)
* liblsl
* Build environment
    * Tested with MSVC 2015, Xcode, and Qt Creator

Instructions:

1. Open a Command Prompt / Terminal and change into this directory.
1. Make a build subdirectory: `mkdir build && cd build`
1. Call cmake
    * Windows: `cmake .. -G "Visual Studio 14 2015 Win64"` (other [generators](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html#visual-studio-generators))
    * Mac: `cmake .. -G Xcode`
1. You may need to specify additional cmake options.
    * set OpenVR_ROOT_DIR to the directory where you downloaded openvr
    * For Qt, you have a few options.
        * Make sure qmake is on the path
        * Set an environment variable to your Qt directory (e.g. `SET QTDIR=C:\Qt\5.8\msvc2015_64`) before calling cmake
        * Pass a cmake argument for QT_SEARCH_PATH (e.g., `-DQT_SEARCH_PATH=C:\Qt\5.8\msvc2015_64`)

## Usage Instructions

### Running the app

1. Launch the app
1. Click on "Scan Devices" to connect to OpenVR and scan for connected devices.
1. Enter the sampling rate you would like to sample your poses at.
1. Click on the individual device(s) you want to stream, or don't click to stream all.
1. Click on "Stream Devices" to start the LSL Outlets.

### Data format

* One stream will be started for all devices' pose data. Each device gets 12 channels, corresponding to the 12 cells in a 3x4 transformation matrix.
    * The channel label prefix (#_) indicates the device ID.
    * The channel label suffix (_#) indicates the transformation matrix cell location.

```
00	01	02	X
10	11	12	Y
20	21	22	Z
```

Where X,Y,Z refer to the 3-D position.

Another stream will be started for each controller's button press events.
This is a single int32 channel with each value having the following format XXYYYYYZZ

* XX : Device ID. Up to 16.
* YYYYY : [vr::EVREventType](https://github.com/ValveSoftware/openvr/blob/master/headers/openvr.h#L449-L585)
* ZZ : [vr::EVRButtonId](https://github.com/ValveSoftware/openvr/blob/master/headers/openvr.h#L600-L626)

## Known Issues

* Cannot build debug app in Windows because it tries to link against non-debug liblsl64.lib, even though liblsl64-debug.lib is correctly specified in the project settings. I'm confused.
* Sometimes an OpenVR controller will sleep/disconnect but it'll still be returned in the list of devices. Sleeping controllers will stream 0's for their pose data.
