# LabRecorder for labstreaminglayer

## Requirements:

* liblsl binaries (by default in `../../LSL/liblsl/build/install`)
* Build environment
    * Tested with Windows 10 MSVC 2015, MacOS Sierra Xcode, Ubuntu 16.04 build-essentials (gcc 5.4.0)

## Build Instructions:

1. Make sure you have the library dependencies.
    * Windows:
        * [CMake](https://cmake.org/download/)
        * [Qt5](https://www.qt.io/download-open-source/)
        * [Boost](https://sourceforge.net/projects/boost/files/boost-binaries/). Be sure to choose the version that matches your version of MSVC.
    * Mac - Use [homebrew](https://brew.sh/)
        * `brew install cmake qt boost`
    * Ubuntu (/Debian)
        * `sudo apt-get install build-essential cmake qt5-default libboost-all-dev`
1. Open a Command Prompt / Terminal (Windows: MSVC Command Prompt) and change into this directory.
1. Make a build subdirectory: `mkdir build && cd build`
1. Call cmake
    * `cmake ..`
    * If your liblsl binaries are not in `../../LSL/liblsl/build/install`, add the path (`cmake -DLSL_INSTALL_ROOT=/path/to/liblsl/binaries ..`)
    * Optional: Use a [generator](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html#visual-studio-generators)
1. You may need to specify additional cmake options.
    * For Boost on Windows, Boost in `C:/local` is expected but can be overridden with `-DBOOST_ROOT=<path_to_boost>`
    * For Qt on Windows, you have a few options.
        * Make sure qmake is on the path
        * Set an environment variable to your Qt directory (e.g. `SET QTDIR=C:/Qt/5.8/msvc2015_64`) before calling cmake
        * Pass a cmake argument for `Qt5_DIR` (e.g., `-DQt5_DIR=C:/Qt/5.8/msvc2015_64/lib/cmake/Qt5`)
1. Build everything and copy the files to the `install` folder:
    * `cmake --build . --target install`
