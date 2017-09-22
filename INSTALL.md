General
=======

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

CMake
=====

Tested platforms:
* Windows 7, Visual Studio 2015
* Ubuntu Linux 14.04
* MacOS Sierra

Prerequisites (LSL with bundled Boost)
- CMake 3.5
- Python 2.7 or Python 3
- (7zip)
- Windows:
    - C++ compiler (tested: VS2015)
    - optional: [Ninja](https://ninja-build.org/)
- Linux
    - C++ compiler (tested: clang 3.5, GCC 6.2)
- OS X
    - C++ compiler (tested: XCode 8.3?)

Optional / required for some apps:
- Boost (+path set with `-DBOOST_ROOT=path/to/boost`)
    - to compile it yourself (in case you didn't download a precompiled version or use a different compiler) download [zlib](https://zlib.net/zlib-1.2.11.tar.xz) and extract it to the boost dir
    - open the `x64 native command prompt for VS2015`
    - `cd C:/path/to/boost_1_xy`
    - `bootstrap.bat`
    - `b2 -sZLIB_SOURCE="C:/path/to/zlib" -j8 address-model=64 --stagedir=./stage/x64 variant=debug,release link=static,shared --build-dir=build --with-chrono --with-date_time --with-filesystem --with-iostreams --with-regex --with-serialization --with-system --with-thread stage`
    - if everything goes smoothly you should now have the libraries in `stage/x64/lib`
- Qt (+path set with `-DQt5_DIR=C:/path_to/Qt/5.9.1/msvc2015_64/lib/cmake/Qt5/` or `set PATH=C:\Qt\5.9.1\msvc2015_64;%PATH%`)

On Mac, if using homebrew Qt5, it is necessary to run the following from the project root:
`chmod +x ./fix_mac.sh`
`sudo ./fix_mac.sh`

To build LSL, call either `lslbuild.py` (see `lslbuild.py --help`) from a valid build environment (e.g. the Visual C++ command line) or do it manually:

- extract the zip file or clone the repository (`git clone https://github.com/sccn/labstreaminglayer.git`)
- change to this directory (git: `cd labstreaminglayer`)
- create a build directory (`mkdir build`) and enter it (`cd build`)
- create the generator files and define the build variables (`cmake -DVARX=Y -G "Generator Name" ../`). Notable switches are:
	 - `-DLSL_USE_SYSTEM_BOOST` to compile against the bundled (`=Off`) or system (`=On`) Boost installation
    - `-DBOOST_ROOT=/path/to/boost` to set the Boost path
    - `-DLSLAPPS_Examples=On` to also build the "Examples"-App (for a complete list see `Apps/CMakeLists.txt`
    - `-DCMAKE_BUILD_TYPE=Release` (`Release` or `Debug`) to compile with debug information or optimizations
    - `-DCMAKE_INSTALL_PREFIX` to set where to install the LSL distribution
    - `-G` to set the generator type (call without a parameter to see a list).
- start the build process (all generators: `cmake --build .`) or load the generated VS Solution file
- copy the generated files to an installation folder: `cmake --build . --target install` or use the `INSTALL` target in Visual Studio

This will create a distribution tree similar to this:

    ├── AppX
    │   ├── AppX.exe
    │   ├── liblsl64_MSVC14.dll
	│   ├── Qt5Xml.dll
	│   ├── Qt5Gui.dll
    │   └── AppX_configuration.ini
    ├── AppY
    │   ├── AppY.exe
    │   ├── AppY_conf.exe
	│   ├── liblsl64_MSVC14.dll
    │   └── example.png
    ├── examples
    │   ├── CppReceive.exe
    │   ├── CppSendRand.exe
    │   ├── SendDataC.exe
	│   ├── liblsl64_MSVC14.dll
    └── LSL
        ├── cmake
        │   ├── LSLAppBoilerplate.cmake
        │   ├── LSLConfig.cmake
        │   └── [snip]
        ├── include
        │   ├── lsl_c.h
        │   └── lsl_cpp.h
        └── lib
            ├── liblsl64_MSVC14.dll
            ├── liblsl64_MSVC14.lib
            └── lslboost.lib

On Unix systems (Linux+OS X) the executable's library path is changed to include
`../LSL/lib/` and the executable folder (`./`) so common libraries (Qt, Boost)
can be distributed in a single library directory or put in the same folder.
On Windows, the library is copied to (and search in) the executable folder.

If you want to build against the compiled liblsl, please see `OutOfTreeTest/CMakeLists.txt` for an example.


