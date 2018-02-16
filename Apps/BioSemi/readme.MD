This is the BioSemi device module for LSL (as a standalone application).

== Usage ==

This program should work out of the box once you have the BioSemi driver for it installed (see drivers-win32_64.zip).
Note that some older drivers than these are confirmed to not work with this program.
The BioSemi amp needs to be plugged in an turned on for this program to work.
The BioSemi stream is linked to the lab streaming layer by clicking link.
Note that settings that are changed while the program is linked will have no effect (you can in general not change the properties of a stream while it is online).

== Optional ==

It is possible to record only a subset of channels (see EEG Channel Subset), or at a different sampling rate.
Note that the EX channels are *always* recorded (even if they are blank).
If you uncheck "Resample to 512 Hz" the app will stream out data at the native rate of the amp, which is 2Khz or more (depending on the speed mode setting); if you check it a resampling filter will be applied, which has characteristics very similar to MATLAB's built-in resample() function in terms of pre/post-ringing and antialiasing.

The remaining settings allow to assign certain types of meta-data to the recorded stream:
* The reference channels that were used can be labeled; this is purely informational and the data
  is not in fact re-referenced.
* An .sfp electrode location file can be loaded and embedded in the per-channel meta-data (for example
  Zebris digitizer systems produce such files).
* The Cap Design can be labeled (if multiple designs are in use within the lab; to add a new design to
  the list, type it in and confirm by pressing enter.
* The cap circumference can be assigned, as well.

The configuration settings can be saved to a .cfg file (see File / Save Configuration) and subsequently loaded from such a file (via File / Load Configuration).
Importantly, the BioSemi program can be started with a command-line argument of the form "BioSemi.exe -c myconfig.cfg", which allows to load the config automatically at start-up.
The recommended procedure to use the BioSemi app in production experiments is to make a shortcut on the experimenter's desktop which points to a previously saved configuration customized to the study being recorded to minimize the chance of operator error.


== Rebuilding ==

Pre-requisites:

* Build environment (e.g., Visual Studio, Xcode, gcc+make)
* Qt
* Boost (v > 1.47.0)
* LSL (v1.02 or later, probably found in the same source tree as this application)

Build instructions:

* With [CMake](https://cmake.org/download/) (Windows/Mac/Linux):
    * `mkdir build && cd build`
    * Call `cmake ..` with optional arguments Qt5_DIR and BOOST_ROOT
        * e.g., on my Mac: `cmake .. -DBOOST_ROOT=$(brew --prefix boost@1.60)`
        * e.g., on my Win box: `cmake .. -G "Visual Studio 14 2015 Win64"`
* With Visual Studio 2008 project files:
    * Customize the QtSDK location (default is C:\DEVEL\QtSDK)
    * Set the environment variable BOOST_ROOT to point to the location of your boost_v_vv_v directory (otherwise you will have to change project settings)
    * The following settings likely have to be customized for the project:
        * In Project|Properties:
        - under Configuration Properties|C/C++|Additional Include Directories, change the location of your QtSDK headers
        - under Configuration Properties|Linker|General|Additional Library Directories, change the location of your QtSDK libraries
        - under Configuration Properties|Linker|Input|Additional Dependencies, adapt the list of your QtSDK libraries
        * If you are switching to debug build, you need to apply the same setting for this configuration, too
* With QtCreator
    * Use the attached .pro file. It probably has to be modified for your environment.
