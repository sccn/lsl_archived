# LabStreamingLayer application for Emotiv (Legacy SDK only)

Authors:
David Medine at SCCN, UCSD
Chadwick Boulay at the Ottawa Hospital Research Institute

## Requirements

* Emotiv Premium SDK. This is no longer available for purchase. There is no version of this app that works with Emotiv's new pure.EEG API.
    * For compatibility with the build systems, be sure to use the installer's default directory.
* [Qt5](https://www.qt.io/download-open-source/)
* [boost](https://sourceforge.net/projects/boost/files/boost-binaries/)
* liblsl
* Build environment options:
    * [CMake](https://cmake.org/download/) if using the cmake build system and MSVC > 2008
    * Provided .sln file and MSVC 2008

## Build Instructions

### Windows

#### Using cmake

1. Open a Command Prompt and change into this directory.
1. Make a build subdirectory: `mkdir build && cd build`
1. Call cmake
    * `cmake .. -G "Visual Studio 14 2015 Win64"`
        * (other [generators](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html#visual-studio-generators))
    * You may need to specify additional cmake options if you are using different versions of MSVC, Qt, or boost.
        * For LSL, pass in -DLSL_ROOT_DIR=path\to\liblsl (default ../../LSL/liblsl)
        * For Qt, pass a cmake argument for CMAKE_PREFIX_PATH (e.g., `-CMAKE_PREFIX_PATH=C:\Qt\5.8\msvc2015_64`)
        * For boost, pass cmake arguments -DBOOST_ROOT=C:\path\to\boost\ -DBOOST_LIBRARYDIR=C:\path\to\boost\lib64-msvc-14.0\
1. Open Emotiv.sln
1. Change the configuration from Debug to Release
1. Right click on LSLEmotiv and build.
1. In the Release folder, you'll find LSLEmotiv.exe and all the necessary dll files.

Only the x64 Release build appears to be working right now.

#### Using provided .sln

Read ..\APP BUILD ENVIRONMENT.txt to make sure you have your Qt and Boost libs setup correctly.
Create an environment variable called EDK_ROOT that points to the EDK install directory.
The default is C:\Program Files (x86)\Emotiv SDK Premium Edition v3.3.3\EDK.
The vcproj is set up to find the EDK headers and libraries relative to EDK_ROOT.
Open the .sln in MSVC 2008 and build.
You will also need to copy the file $(EDK_ROOT)\x86\edk.dll into this directory in order to run the application once it is compiled.

## Usage Instructions

### Running the app

1. Run the EmotivXavierControlPanel. Note, on my Windows 10 system, I had to run it in Windows 8 compatibility mode.
1. Launch the app
1. Click on "Scan Devices" to connect to OpenVR and scan for connected devices.
1. Enter the sampling rate you would like to sample your poses at.
1. Click on the individual device(s) you want to stream, or don't click to stream all.
1. Click on "Stream Devices" to start the LSL Outlets.