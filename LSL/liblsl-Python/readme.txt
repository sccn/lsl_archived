To use the pylsl module in a Python program, take the files pylsl.py, win32/_pylsl.pyd, and liblsl32.dll and put them in a path that is found by your Python installation (such as C:\Python26\Lib on some Windows setups). For other OSes and configurations, use the other versions of the .pyd and liblsl files. Use SWIG to re-generate the pylsl_wrap.cpp file and build the python library with your toolkit of choice.

Swig command line: swig -c++ -python pylsl.i
