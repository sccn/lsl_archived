# LabRecorder for labstreaminglayer

## Build Instructions

Requirements:

* [CMake](https://cmake.org/download/)
* [Qt5](https://www.qt.io/download-open-source/)
* liblsl binaries in ../../LSL/liblsl/bin
* Build environment
    * Tested with MSVC 2015, Xcode

Instructions:

1. Open a Command Prompt / Terminal and change into this directory.
1. Make a build subdirectory: `mkdir build && cd build`
1. Call cmake
    * Windows: `cmake .. -G "Visual Studio 14 2015 Win64"` (other [generators](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html#visual-studio-generators))
    * Mac: `cmake .. -G Xcode`
1. You may need to specify additional cmake options.
    * For Boost on Windows, Boost 1.63 in C:\local is expected but can be overridden with `-DBOOST_ROOT=<path_to_boost> -D BOOST_LIBRARYDIR=<path_to_boost_libs>`
    * For Qt, you have a few options.
        * Make sure qmake is on the path
        * Set an environment variable to your Qt directory (e.g. `SET QTDIR=C:\Qt\5.8\msvc2015_64`) before calling cmake
        * Pass a cmake argument for QT_SEARCH_PATH (e.g., `-DQT_SEARCH_PATH=C:\Qt\5.8\msvc2015_64`)
