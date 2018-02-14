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

The usage instructions are a bit complicated right now until the GUI elements for each device are built out.

    * For g.USBamp(s), the LSL app should have all the functionality you need except for checking impedances.
        * Use g.NEEDaccessDemo client app to check impedances.
    * For g.Nautilus, use the g.NEEDaccessDemo client to configure the device, which gets saved to the device (yay!)
        * Some parameters don't get saved (boo!). Use the LSL app's config menu to set these.
    * For g.HIamp, use the g.NEEDaccessDemo client to configure the device AND start acquisition.
        * The LSL app will not have ownership over the device connection.

### Preliminary device configuration (g.Nautilus, g.HIamp only)

1. Run the g.NEEDaccess - Demo Client.
1. Find connected devices...
1. Click on your device, then click on Select.
1. In the new Device Configuration window, configure your channels, then click on OK.
1. For g.Nautilus, close the client.
1. For g.HIamp, start acquisition.

### Running the app

1. Run gNEEDaccess.exe
1. Set the server IP and port, client port, then click Scan.
1. Select your device(s) from the list and check appropriate boxes, then click Connect.
    * Use ctrl + click to select more than one g.USBamp
    * The 'master' g.USBamp should be first in the list of selected devices. You can drag and drop it up.
1. Click on "Config" and configure your device. Then click OK to set the device config.
1. Click Go! when ready to start streaming over LSL.
1. When finished, click Stop!, then Disconnect, then close the App.

## Known Issues

* Requiring the demo client to configure devices is less than ideal. This will go away when all the GUI elements are done.