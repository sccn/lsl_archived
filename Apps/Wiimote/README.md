# Wiimote App for labstreaminglayer

## Building

### Dependencies

    * cmake
    * Qt5
    * Compiler (+ IDE)
        * Tested with Visual Studio 2015 32-bit and 64-bit, Xcode clang
    * Windows-only:
        * [WinHID](https://developer.microsoft.com/en-us/windows/hardware/download-kits-windows-hardware-development)
            * You may already have this installed with Visual Studio.

### 

```
mkdir build
cd build
cmake ..
make
```

You may have to modify the cmake command if you are using a version of Qt5
other than 5.7 or if Qt5 was not installed via the installer in Windows
or via homebrew in MacOS.

`cmake .. -DCMAKE_PREFIX_PATH="path/to/Qt5/<version>"`

You may also want to modify the cmake command with a different generator
if you want to build 64-bit binaries in Windows or use Xcode in Mac.
See [cmake generators](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html).

## Usage instructions

The app executable will be in your build directory.
In Windows this might be in a Release or Debug subdirectory.
In Windows, if you wish to move the app to another location,
then bring along the DLLs found in its directory.

### Windows - Pair devices in advance

First pair your device to the PC. To do so, go into the Bluetooth settings,
let it scan for devices while you tap the little orange button under the
battery cover on your device. Eventually you should see a device named
something like "Nintendo RVL-CNT-01". Click on the device then click on pair.
It may ask you for a code, leave this empty then press next.
If the pairing dialog does not close immediately, cancel to close it yourself
after about 1 second. The bluetooth manager will continue to pair. Make sure
the device does not turn off during this process by pressing the 1+2 buttons
on the face. The pairing progress bar should complete and the device should be
listed as paired. The LEDs may continue to flash even after pairing.

Now you may run with Wiimote executable. When you press "Link", it will
connect to up to 4 Wii devices that are paired and connected to your system.

### Mac

The Wii-device communication library [wiiuse](https://github.com/rpavlik/wiiuse)
has a bit of a strange way of accessing the Wii devices in Mac. The device
cannot be paired to the system prior to use. If you have already paired the
device then you'll have to open your Bluetooth preferences and remove it.

The device also must not be connected. Click on the bluetooth icon in the
menu bar. If any Nintendo devices are listed there then select them and
click on 'disconnect'.

You may now run Wiimote.app by double-clicking on it.
A little GUI will appear with a link button. Do not press it yet.
Put all the devices you wish to record from in discoverable mode
by pressing the orange button on the inside of the battery cover.
While they are still in discoverable model, click on the Link button.