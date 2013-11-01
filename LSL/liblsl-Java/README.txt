This interface is not yet properly documented and has not yet been compiled for all platforms (except Win32, Win64 and Mac64). Please take a look at myTesting.java to 
see how LSL can be used from Java.

=== Build prerequisites ===
* SWIG
* JDK 7 on the path (note that some absolute directories are being assumed for some OS'es below).

=== Rebuilding on Windows ===

Open the solution liblsl-Java.sln, select either Win32 (default) or x64 in the configuration manager drop-down menu, and press F7 to build the project. This will
generate a file named liblslJNI32.dll or liblslJNI64.dll and compile all generated .java files into .class (using Javac).

=== Rebuilding on MacOS ===

cd /Users/christian/Desktop/LSL/liblsl-Java
swig -c++ -java liblsl.i
javac *.java
g++ "-I/System/Library/Frameworks/JavaVM.framework/Versions/CurrentJDK/Headers" "-I/Developer/SDKs/MacOSX10.6.sdk/System/Library/Frameworks/JavaVM.framework/Versions/A/Headers" -c liblsl_wrap.cxx
g++ -dynamiclib -o liblslJNI.jnilib liblsl_wrap.o -L../liblsl/bin/ -llsl64 -Wl,-rpath,.


=== Rebuilding on Linux ===

cd /Users/christian/Desktop/LSL/liblsl-Java
swig -c++ -java liblsl.i
javac *.java
g++ "-I/opt/jdk1.7.0_15/include/" "-I/opt/jdk1.7.0_15/include/linux/" -c liblsl_wrap.cxx -fPIC
g++ -shared -o liblslJNI.so liblsl_wrap.o -L../liblsl/bin/ -llsl64 -lpthread -lrt -Wl,-rpath,.
