This is currently the most robust and straightforward way of making Android apps that incorporate LSL. These instructions apply to Android Studio 3.1.2. You will need to get jna-4.2.2.jar (from https://mvnrepository.com/artifact/net.java.dev.jna/jna/4.2.2) and put it in LSL/liblsl-Android/AndroidStudio/libs. You will need to get the native jna libraries from https://github.com/java-native-access/jna/tree/4.2.2/lib/native. Select branch 4.2.2 and download android-*.jar. Extract each of these jars, using 7-zip or similar, and create the following directory tree:
LSL/liblsl-Android/AndroidStudio/jniLibs
    arm64-v8a
        libjnidispatch.so
    armeabi
        libjnidispatch.so
    armeabi-v7a
        libjnidispatch.so
    mips
        libjnidispatch.so
    mips64
        libjnidispatch.so
    x86
        libjnidispatch.so
    x86_64
        libjnidispatch.so

Yes the platform names are different in the jar files than in the required the directory structure, and yes the names are important. If you do this incorrectly, you get errors related to "missing libjnidispatch.so".

If you get errors related to native, built in functions such as fgetpos and fsetpos in cstdio, this implies a problem with the version of the ndk that you are using (revision 15c and 16b are broken for several platforms). Download Revision 14b from https://developer.android.com/ndk/downloads/older_releases.html to a volume with a lot of space (it is 2.36 GB). In Android Studio, go to File -> Project Structure -> SDK Location -> Android NDK location and set the path to point at android-ndk-r14b.

To run, select the desired module from the list box the the left of the green arrow, and click the green arrow.

The majority of the native build instructions are in CMakeLists.txt, with some in the project build.gradle files.

At this point, every module rebuilds its own version of liblsl.so. Hopefully Android Studio will one day support building a purely native library without redundancy.

If you would like to use LSL in an android app built in a different way, it is recommended that you build one project (such as ReceiveStringMarkers) and extract the needed compiled library files from the .apk file (using 7-zip or similar).

To make other Apps in Android Studio, the easiest way is to exit Android Studio, copy the SendStringMarkers folder and change every reference to SendStringMarkers in the copied folder to a new name (using a text editor).

To see more examples of Java with LSL, look in LSL/liblsl-Java. Those examples should copy of to Android with minimal effort.


