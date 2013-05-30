1) To regenerate the wrapper files for Python and Java after an interface change, you need to run the following two lines. Note that if all you want to do is recompile the wrapper for your particular Android version, you can skip these two lines.

swig -c++ -java -package com.example.hellojni -outdir ../src/com/example/hellojni -o liblsl_wrap_java.cxx liblsl.i
swig -python -c++ -o liblsl_wrap_python.cxx liblsl.i

2) After you are done with this, run ndk_build.cmd in this folder to rebuild liblslAndroid.so.

3) Once you have that, the .java files that make up the interface are in liblsl-Android/src/, this also includes the file lslAndroid.py. There is a file that should display the current time using the LSL clock in a dialog box. But note that the Python interface expects the .so file to be called _lslAndroid.so (and likely to reside in the same directory as the .py file).

Note that you need to have Python set up for use on Android to build the Python interface; by default the file liblsl_wrap_python.cxx is not included in the build sources (in Android.mk).
