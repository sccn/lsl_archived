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
    * `-DGDS_ROOT=path/to/gneedaccess sdk/c` if it is not installed into the default directory.
        * Default is defined in ./cmake/FindgNEEDaccessSDK.cmake
    * `-DQt5_DIR=path/to/Qt5Config.cmake` if not default
        * Default is defined in labstreaminglayer/cmake/LSLSaneDefaults.cmake
        
### Build

1. In MSVC, select the Release x64 configuration.
1. Right click on the INSTALL target and click Build.
1. Find the application and all required DLLs in labstreaminglayer/build/install/lsl_Release/gNEEDaccess/

## Usage Instructions

    * Use g.NEEDaccessDemo client app to check impedances.
        * The gNEEDaccess LSL app can refresh impedance values for some of the supported devices, but the g.NEEDaccessDemo client has a nicer user interface for this
    * Quite the g.NEEDaccessDemo client before running this LSL app.

1. Run gNEEDaccess.exe
1. Set the server IP and port, client port, then click Scan.
1. Select your device(s) from the list and check appropriate boxes, then click Connect.
    * Use ctrl + click to select more than one g.USBamp
    * The 'master' g.USBamp should be first in the list of selected devices. You can drag and drop it up.
1. Click on "Config" and configure your device.
1. Change your settings
    * Use the load/save menu to load/save configs.
1. Click OK to set the device config and close the config Window.
1. Click Go! when ready to start streaming over LSL.
1. When finished, click Stop!, then Disconnect, then close the App.

## Known Issues

* Devices use the hardware-provided defaults upon initialization. Loading the config screen will then always use the <devicename>_default.cfg file provided (currently not available for g.USBamp). You can save yourself some effort by backing up the original config, then saving your custom config over the _default.cfg file so this is automatically loaded next time you load the config screen.
