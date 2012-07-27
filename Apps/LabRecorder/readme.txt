This is the Lab Recorder stand-alone application. The executable is launched by clicking LabRecorder.py in the src/ folder.
To run this program, you should have installed Python(x,y)-2.6.5.6 (installer available in /home/software/Python/). NOTE: If you are also developing for SNAP (the experimentation environment), please install first Panda3D-1.7.2 from the same directory and then install Pyton(x,y)-2.6.5.6 "over" it (namely the Python path, which would be by default C:\Python26, should be changed to something like C:\Panda3d-1.7.2\python). You find more detailed instructions for this in the SNAP installation instructions. If you uninstall these packages later, uninstall first Python(x,y) and then Panda3d. Then install Pyside for Python 2.6 (from here: http://qt-project.org/wiki/PySide_Binaries_Windows). 

For using the LabRecorder, see the contained readme file (in the src/ folder).

For developing the LabRecorder program in an IDE, start Python(x,y), and from there start Eclipse. In Eclipse, right-click the left pane (projects pane) and select "Import..." and "Existing Projects into Workspace".
Then select as the root directory this folder (with the .pydevproject file) and click okay. The main executable is LabRecorder.py. This program consists mostly of GUI and uses two other libraries, one is the recording backend 
(that is written in C++ and sits in the RecorderLib sub-folder), and the other is the liblsl-Python wrapper for streaming I/O (should be in the same distribution under LSL). You usually don't have to re-build pylsl (unless
someone forgot to update it after a change to the LSL core) and most tasks don't require touching the RecorderLib either. 

The .ui file can be edited with the Qt Designer (to some extent also from within Eclipse).

If you want to rebuild the RecorderLib, open the .sln file (which is for Visual Studio 2008), and in the project options change the include and library paths that refer to the Python installation 
(Project|Properties|Configuration Properties|C/C++|Additional Include Directories and Project|Properties|Configuration Properties|Linker|Additional Library Directories). Then, rebuild. In release mode,
this will create a _RecorderLib.pyd file in the above directory (the LabRecorder), and in debug mode, this will create a regular .dll file that should be found by the TestRecorder test program for debugging.
If you change the interface of the RecorderLib dll (e.g. adding new functions), you need to re-run SWIG to re-generate the python wrapper files. This is done by running the line "swig -python RecorderLib.i" in the 
directory that contains the .i file. SWIG comes pre-installed with Python(x,y).