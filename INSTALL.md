# General

This manual describes the process of building liblsl from source for Windows, Mac OS X, and Linux.
Since liblsl is cross-platform (it is written in standard C++ and uses some boost libraries),
this process should be pretty straightforward.
The following paragraphs provide a step-by-step instruction of the build process on all three platforms. 

To get an overview of the project structure, the following tree lists the directory
hierarchy of the source after you've unpacked the compressed source archive:

    labstreaminglayer
    ├── Apps
    │   ├── AMTI ForcePlate
    │   ├── Examples
    │   ├── [several other apps]
    │   └── Wiimote
    └── LSL
        ├── liblsl
        │   ├── bin
        │   ├── examples
        │   ├── external
        │   │   ├── lslboost
        │   │   └── src
        │   ├── include
        │   ├── project
        │   ├── src
        │   │   ├── portable_archive
        │   │   └── pugixml
        │   └── testing
        ├── liblsl-Matlab
        ├── liblsl-Python
        └── liblsl-Java

LSL and some Apps are built with CMake, the remaining Apps use Visual Studio Solution files.

# CMake

Tested platforms:
* Windows 7, Windows 10
    - [Visual Studio 2015](https://www.visualstudio.com/vs/older-downloads/), [Visual Studio 2017](https://www.visualstudio.com/downloads/)
* Ubuntu Linux 14.04, 16.04
    - Clang 3.5
    - GCC 6.2
* MacOS Sierra
    - XCode 8.3

Prerequisites (LSL with bundled Boost)
- [CMake 3.5](https://cmake.org/download/)

Optional / required for some apps:
- [Boost](https://boost.org) (+path set with `-DBOOST_ROOT=path/to/boost`)
    - Windows: install the [precompiled binaries](https://sourceforge.net/projects/boost/files/boost-binaries/)
    - Debian / Ubuntu Linux: install the `libboost-dev` package
    - OS X: install Boost via [Homebrew](https://brew.sh/)
- [Qt](http://qt.io) (+path set with `-DQt5_DIR=C:/path_to/Qt/5.9.1/msvc2015_64/lib/cmake/Qt5/` or `set PATH=C:\Qt\5.9.1\msvc2015_64;%PATH%`)
    - Windows: use the [installer](http://download.qt.io/official_releases/online_installers/qt-unified-windows-x86-online.exe)
    - Debian / Ubuntu Linux: install the `qtbase5-dev` package

On Mac, if using homebrew Qt5, it is necessary to run the following from the project root:
`sudo bash ./fix_mac.sh`

# Build instructions

There are two build types:

1. [in tree builds](#in-tree-builds-recommended) build the LSL library and all apps you explicitely enable. This is probably what you want.
1. [out of tree builds](#out-of-tree-builds) build only a single app and require you to have a prebuilt LSL library and the exported build configuration (`LSLConfig.cmake`).

## In tree builds (recommended)

1. extract the zip file or clone the repository (`git clone https://github.com/sccn/labstreaminglayer.git`)
2. Windows only: start `build_windows.bat` and follow the instructions in step 4
3. change to the build directory (git: `cd labstreaminglayer/build`)
4. Configure the project using cmake.
    Option 1 - using the GUI
    - open the configuration UI (`cmake-gui ../`) and click on `Configure`
    - select your compiler
    - check the Apps you want to use
    - if necessary, use `Add Entry` to add paths or change options
        - location of Qt5Config.cmake (`Qt5_DIR`, PATH, e.g. `C:/Qt/5.9.2/msvc2015_64/lib/cmake/Qt5/`)
        - Boost (`BOOST_ROOT`, PATH, e.g. `C:/local/boost_1_65_1/`)
        - a path where redistributable binaries get copied (`CMAKE_INSTALL_PREFIX`)
        - build type (`CMAKE_BUILD_TYPE`, either `Release` or `Debug`). You can change this in Visual Studio later.
    - click on `Generate` to create the build files / Visual Studio Solution file (open it with `Open Project`)
    Option 2 - using commandline. The following is an example. Add/remove options as required.
    - cmake .. -G "Visual Studio 14 2015 Win64" -DQt5_DIR=C:\Qt\5.9.3\msvc2015_64\lib\cmake\Qt5 -DBOOST_ROOT=C:\local\boost_1_65_0 -DLSLAPPS_LabRecorder=ON -DLSLAPPS_XDFBrowser=ON -DLSLAPPS_OpenVR=ON
5. start the build process (`cmake --build . --config Release --target install`[*](#regarding-the-install-target)) or load the generated VS Solution file

This will create a distribution tree in the folder specified by `CMAKE_INSTALL_PREFIX`[*](#regarding-the-install-target) similar to this:

## 'installed' directory tree

    ├── AppX
    │   ├── AppX.exe
    │   ├── liblsl64.dll
    │   ├── Qt5Xml.dll
    │   ├── Qt5Gui.dll
    │   └── AppX_configuration.ini
    ├── AppY
    │   ├── AppY.exe
    │   ├── AppY_conf.exe
    │   ├── liblsl64.dll
    │   └── example.png
    ├── examples
    │   ├── CppReceive.exe
    │   ├── CppSendRand.exe
    │   ├── SendDataC.exe
    │   ├── liblsl64.dll
    └── LSL
        ├── cmake
        │   ├── LSLAppBoilerplate.cmake
        │   ├── LSLConfig.cmake
        │   └── [snip]
        ├── include
        │   ├── lsl_c.h
        │   └── lsl_cpp.h
        └── lib
            ├── liblsl64.dll
            ├── liblsl64.lib
            └── lslboost.lib

On Unix systems (Linux+OS X) the executable's library path is changed to include
`../LSL/lib/` and the executable folder (`./`) so common libraries (Qt, Boost)
can be distributed in a single library directory or put in the same folder.
On Windows, the library is copied to (and searched in) the executable folder.

The resulting folder `LSL` contains three subfolders:

`cmake` contains the exported build configuration (`LSLConfig.cmake`) that can be used to import
the library in [out of tree builds](#out-of-tree-builds).

`include` contains the include headers for C (`lsl_c.h`) and C++ (`lsl_cpp.h`) programs.

`lib` contains the library files. To run a program, you need the `liblslXY.dll` (Windows) or `.so` (Linux / OS X).

## Regarding the `install` target

CMake places built binary files as well as build sideproducts in a build tree that should be separate from
the source directory. To copy only the needed files (and additional library files they depend on) to a folder
you can share with colleagues or onto another PC, you need to 'install' them.
This doesn't mean 'installing' them in a traditional sense (with Windows installers or package managers on
Linux / OS X), but only copying them to a separate folder and fixing some hardcoded paths in the binaries.

## Out of tree builds

An out of tree build doesn't include the whole `labstreaminglayer` directory but only a single application
(a minimal example is contained in the folder `OutOfTreeTest`).

Building the LSL library should take only 1-2 minutes, so you should prefer [in tree builds](#in-tree-builds-recommended)
unless you know what you're doing.

To import the LSL library in a separate CMake build, you need to set the the 
**absolute path** to the['installed' LSL directory](#install-directory-tree)
in the `LSL_INSTALL_ROOT` variable (e.g. `-DLSL_INSTALL_ROOT=C:/LSL/build/install/lsl_Release/LSL`) or add the **absolute path** to the`LSL/cmake` subfolder
of the ['installed' LSL directory](#install-directory-tree) to your `CMAKE_PREFIX_PATH`
(`list(APPEND CMAKE_MODULE_PATH "C:/path/to/LSL/build/install/cmake/")`.
