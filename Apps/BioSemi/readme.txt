This is the BioSemi device module for LSL (as a standalone application).

To rebuild this program you need to have the following things installed:
* Visual Studio 2008
* Qt SDK (tested with 4.8.1)
  the project files refer to the installation location C:\DEVEL\QtSDK (will have to be customized to your location)
* boost 1.47.0 (installer found at http://www.boostpro.com/download/)
  note: after installation, set the environment variable BOOST_ROOT to point to the location of your boost_1_47_0 directory (otherwise you will have to change project settings)
* LSL-1.10 or later (likely found in the same source tree, under LSL)


The following settings likely have to be customized for the project:
* In Project|Properties:
  - under Configuration Properties|C/C++|Additional Include Directories, change the location of your QtSDK headers
  - under Configuration Properties|Linker|General|Additional Library Directories, change the location of your QtSDK libraries
  - under Configuration Properties|Linker|Input|Additional Dependencies, adapt the list of your QtSDK libraries 
* If you are switching to debug build, you need to apply the same setting for this configuration, too

The .pro file is a project file for the Qt Creator (which comes with the Qt SDK), and can be used to edit the project from a Qt perspective (e.g. GUI-related things)
The .ui file can be opened directly with the Qt Designer for UI design.

