# LabStreamingLayer application for g.Tec g.NEEDaccess Client

## Build Instructions

Requirements:

* [CMake](https://cmake.org/download/)
* [Qt5](https://www.qt.io/download-open-source/)
* g.Tec g.NEEDaccess SDK (from manufacturer)
* liblsl
* Build environment
    * Tested with MSVC 2015 Win64 as IDE and compiler

Instructions:

1. Open a Command Prompt / Terminal and change into this directory.
1. Make a build subdirectory: `mkdir build && cd build`
1. `cmake .. -G "Visual Studio 14 2015 Win64"` (other [generators](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html#visual-studio-generators))
1. You may need to specify additional cmake options.
    * -DGDS_ROOT=path/to/gneedaccess sdk/c if it is not installed into the default directory.
        * Default is C:\Program Files\gtec\gNEEDaccess Client API\C
    * -DQt5_DIR=path/to/Qt5Config.cmake if not default
        * Default is C:\\Qt\\5.9.1\\msvc2015_64\\lib\\cmake\\Qt5

## Usage Instructions

### Running the app

TODO

### Data format

TODO
