# LabStreamingLayer application for Emotiv (Legacy SDK only)

Authors:
David Medine at SCCN, UCSD
Chadwick Boulay at the Ottawa Hospital Research Institute

## Requirements

* Emotiv Premium SDK. This is no longer available for purchase.
    * Emotiv's consumer-facing SDK seems to be rapidly changing. Newer SDKs seem to have subscription options only, which I do not have so cannot develop/test.
    * For compatibility with the build systems, be sure to use the installer's default directory.
* [Qt5](https://www.qt.io/download-open-source/)
* [boost](https://sourceforge.net/projects/boost/files/boost-binaries/)
* liblsl
* [CMake](https://cmake.org/download/) if using the cmake build system and MSVC > 2008
    * premade project files no longer supported.

## Build Instructions

### Windows x64 + Cmake only

1. Open a Command Prompt and change into this directory.
1. Make a build subdirectory: `mkdir build && cd build`
1. Call cmake
    * `cmake -G "Visual Studio 14 2015 Win64" ..`
        * customize with other [generators](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html#visual-studio-generators)
            * Only MSVC x64 seems to work right now
            * Compiler version must match Qt and Boost binaries.
        * customize with your own -DQt5_DIR=C:\Qt\5.9.1\msvc2015_64\lib\cmake\Qt5
        * customize with your own -DLSL_ROOT=path/to/LSL/build/install dir.
        * customize with your own -DEDK_ROOT="path\to\Emotiv SDK Premium Edition v3.3.2" if not using 3.3.2 or 3.3.3 in default directory.
        * customize with your own -DBOOST_ROOT=C:\path\to\boost_ver\
1. Open Emotiv.sln
1. Change the configuration from Debug to Release
1. Right click on INSTALL and build.
1. In the labstreaminglayer/build/lslRelease/lslinstall dir, there should be LSLEmotiv/LSLEmotiv.exe and all the necessary dll files.

## Usage Instructions

### Running the app

1. Run the EmotivXavierControlPanel. Note, on my Windows 10 system, I had to run it in Windows 8 compatibility mode.
1. Launch the LSLEmotiv.exe app
1. Select the sampling rate.
1. Click on "Link" to connect to Emotiv.