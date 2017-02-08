Deprecated Methodology. See LSL/liblsl-Android/Android Studio for current procedure.


This is a minimal Android setup with LSL, using a text based android build chain and SWIG wrappers.

These instructions assume that you have installed Android CodeWorks 1R3, by NVidia, but only so that the Android ndk has the expected contents and location. It is possible to get the Android ndk from elsewhere, but the paths below may change somewhat.

1) To regenerate the wrapper files for Python and Java after an interface change, you need to run the following two lines. Note that if all you want to do is recompile the wrapper for your particular Android version, you can skip these two lines.

>cd LSL\liblsl-Android\Legacy\jni
>swig -c++ -java -package edu.ucsd.sccn.lsl -outdir ../src/edu/ucsd/sccn/lsl -o liblsl_wrap_java.cxx liblsl.i
>swig -python -c++ -o liblsl_wrap_python.cxx liblsl.i

2) After you are done with this, run ndk_build.cmd to rebuild liblslAndroid.so.

>cd LSL\liblsl-Android\Legacy
>c:\NVPACK\android-ndk-r10e\ndk-build

3) You can now deploy an app to an attached Android device. Replace ${Project_Name} with the app you wish to install.

>cd LSL\liblsl-Android\Legacy\${Project_Name}
>C:\nvpack\android-sdk-windows\tools\android update project -p . -t android-15
>c:\nvpack\apache-ant-1.8.2\bin\ant debug install

The ${Project_Name} app should appear on your device. When selected, it opens a stream and sends or receives simulated data. Note that apps will continue running until force quit: Settings -> Apps -> ${Proect_Name} -> Force Stop. This allows you to open both a receiver and a sender if desired, so you that you can verify that LSL is working on the network local to the device.

4) liblsl-Android/src/ also includes the file lslAndroid.py. Note that the Python interface expects the .so file to be called _lslAndroid.so (and likes to reside in the same directory as the .py file).

Note that you need to have Python set up for use on Android to build the Python interface; by default the file liblsl_wrap_python.cxx is not included in the build sources (in Android.mk).

I (Matthew Grivich) have not tested the Python interface for Android, but I have left the comments and files from Christian Kothe.