# LabStreamingLayer application for g.Tec g.NEEDaccess Client

## Build Instructions

### Requirements

* [CMake](https://cmake.org/download/)
* [Qt5](https://www.qt.io/download-open-source/)
* g.Tec g.NEEDaccess SDK (from manufacturer)
* liblsl
* Build environment
    * Tested with MSVC 2015 Win64

### Configure Project

#### Alongside root LSL project

Pass the additional -DLSLAPPS_gNEEDaccess=ON argument to the cmake command at the root of the labstreaminglayer tree.
(See root LSL instructions for details.)

#### After root LSL is built

1. Open a Command Prompt / Terminal and change into this directory.
1. Make a build subdirectory: `mkdir build && cd build`
1. `cmake .. -G "Visual Studio 14 2015 Win64"` (other [generators](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html#visual-studio-generators))
1. You may need to specify additional cmake options.
    * -DGDS_ROOT=path/to/gneedaccess sdk/c if it is not installed into the default directory.
        * Default is C:\Program Files\gtec\gNEEDaccess Client API\C
    * -DQt5_DIR=path/to/Qt5Config.cmake if not default
        * Default is C:\\Qt\\5.9.1\\msvc2015_64\\lib\\cmake\\Qt5
        
### Build

1. In MSVC, select the Release x64 configuration.
1. Right click on the INSTALL target and click Build.
1. Find the application and all required DLLs in labstreaminglayer/build/lsl_Release/lslinstall/Apps/gNEEDaccess/

## Usage Instructions

### Running the app

1. Run the g.NEEDaccess - Demo Client.
1. Find connected devices...
1. Click on your device, then click on Select.
1. In the new Device Configuration window, configure your device, then click on OK.
1. If in the configuration window you clicked on any of the check boxes for Amplifier Settings
    * Click on Start in the Single Device Client and leave it open during acquisition.
    * Else close the client.
1. Open gNEEDaccess.exe
1. Set the server IP and port, client port, then click Scan.
1. Select your device from the list of available devices then click Connect.
1. Click Go! when ready to start streaming over LSL.
1. When finished, click Stop!, then Disconnect, then close the App.

## Known Issues

* g.USBamp and G.HIamp are not supported because I do not have a device for testing.
* If requested, I can add the device-specific Amplifier Settings check boxes to this App so it is no longer necessary to run the Single Device Client.
* Only a single device is supported per instance. If you want to stream from another device then another instance of g.NEEDaccess must be opened.