Prerequisites 
=============
This is the Lab Recorder stand-alone application. The executable is launched by clicking LabRecorder.py in the src/ folder. This program requires that you have installed either Python-2.6 (http://www.python.org/ftp/python/2.6/python-2.6.msi) and PySide for Python 2.6 (http://releases.qt-project.org/pyside/1.1.1/PySide-1.1.1qt474.win32-py2.6.exe) or Python 2.7 (http://www.python.org/ftp/python/2.7/python-2.7.msi) and PySide for Python 2.7 (http://releases.qt-project.org/pyside/1.1.1/PySide-1.1.1qt474.win32-py2.7.exe). If you currently don't have a Python installation, we recommend that you use the newer version. 

Experimenters
=============
For using the LabRecorder, see the contained readme file (in the src/ folder).



SCCN Users
==========
Note for SCCN users: If you are at the Swartz Center you will probably want to install a more complete Python distribution (Python(x,y)), especially if you are developing for SNAP (the experimentation environment). 
For this please install first:
* either Panda3D-1.7.2 and then install Python(x,y)-2.6.5.6 "over" it (namely the Python path, which would be by default C:\Python26, should be changed to something like C:\Panda3d-1.7.2\python)
* or Panda3d-1.8.0 and then Python(x,y)-2.7.2.3 "over" it (and when you are prompted to pick the Python path, replace the default C:\Python27 by something like C:\Panda3d-1.8.0\python, depending on where you installed Panda).
If you are unsure, go for the 2.6 version for now, but we will upgrade the labs to 2.7 relatively soon. When you're done install PySide by typing (in the console): easy_install pyside. If you uninstall these packages later, 
uninstall them in reverse order, i.e. first PySide, then Python(x,y), and then Panda3d. You find more detailed instructions for this in your SNAP package.

SCCN Developers
===============
If you are planning to edit the LabRecorder program in an IDE, start Python(x,y), and from there start Eclipse. In Eclipse, right-click the left pane (projects pane) and select "Import..." and "Existing Projects into Workspace". Then select as the root directory this folder (with the .pydevproject file) and click okay. The main executable is LabRecorder.py. This program consists mostly of GUI and uses two other libraries, one is the recording backend (that is written in C++ and sits in the RecorderLib sub-folder), and the other is the liblsl-Python wrapper for streaming I/O (should be in the same distribution under LSL). You usually don't have to re-build pylsl (unless someone forgot to update it after a change to the LSL core) and most tasks don't require touching the RecorderLib either. 

The .ui file can be edited with the Qt Designer (to some extent also from within Eclipse).

If you want to rebuild the RecorderLib, you find a readme file for this in the respective sub-directory.
