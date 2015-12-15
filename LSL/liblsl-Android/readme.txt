This is a minimal Android setup with LSL, using a minimal, text based android build chain. There is also a Visual Studio based build chain available at liblsl/project/android-vs2013. The Visual Studio option is more polished. However, the text based build chain demonstrates how to generate java swig wrappers. The Visual Studio option assumes that you will be writing an app that has as little Java as possible. There is nothing preventing these two options from being combined except time and desire.

These instructions assume that you have installed Android CodeWorks 1R3, by NVidia, but only so that the Android ndk has the expected contents and location. It is possible to get the Android ndk from elsewhere, but the paths below may change somewhat.

1) To regenerate the wrapper files for Python and Java after an interface change, you need to run the following two lines. Note that if all you want to do is recompile the wrapper for your particular Android version, you can skip these two lines.

>cd LSL\liblsl-Android\jni
>swig -c++ -java -package com.example.hellojni -outdir ../src/com/example/hellojni -o liblsl_wrap_java.cxx liblsl.i
>swig -python -c++ -o liblsl_wrap_python.cxx liblsl.i

2) After you are done with this, run ndk_build.cmd in this folder to rebuild liblslAndroid.so.

>cd LSL\liblsl-Android
>c:\nbs\external\lsl\LSL\trunk\LSL\liblsl-Android>c:\NVPACK\android-ndk-r10e\ndk-build

3) You can now deploy the app to an attached Android device.

>cd LSL\liblsl-Android\bin
>android update project -p . -t android-15
>cd LSL\liblsl-Android
>c:\nvpack\apache-ant-1.8.2\bin\ant debug install
>cd LSL\liblsl-Android\bin
>adb install -r HelloJni-debug.apk

The HelloJni app should appear on your device. When selected, it displays the LSL time.

4) liblsl-Android/src/ also includes the file lslAndroid.py. Note that the Python interface expects the .so file to be called _lslAndroid.so (and likes to reside in the same directory as the .py file).

Note that you need to have Python set up for use on Android to build the Python interface; by default the file liblsl_wrap_python.cxx is not included in the build sources (in Android.mk).

I (Matthew Grivich) have not tested the Python interface for Android, but I have left the comments and files from Christian Kothe.