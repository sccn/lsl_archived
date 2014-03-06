The Java interface for the lab streaming layer should run on Window/Linux/MacOS, 32/64 bit.
The main class is edu.ucsd.sccn.LSL, which provides all necessary functions and sub-classes.
See examples/ for examples and javadoc/ for API documentation.

To use this interface in your program, you need to import the class edu.ucsd.sccn.LSL,
include the jna-4.0.0.jar package in your classpath, and have the native liblsl library 
for your platform in path where it is found (e.g., your application's root directory, 
or a system folder). This has been found to work with a variety of installations and operating 
systems.

If you get an error about some class def not found: On some systems you may have to unzip 
the jna-4.0.0.jar file to make sure that the libraries contained in it are found by Java.

If you an error like: java.lang.UnsatisfiedLinkError: Unable to load library 'liblsl64.dll':
Native library (win32-x86-64/liblsl64.dll) not found in resource path: then you need to make 
sure that your liblsl library is on a path found by the java runtime, e.g., in the path where 
you execute the java command. One location where it will always be found is a system library 
path, e.g., Windows/system32 (not SysWOW64) on Windows; this should help during debugging.
 