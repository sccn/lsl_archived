The Java interface for the lab streaming layer should run on Window/Linux/MacOS, 32/64 bit.
The main class is edu.ucsd.sccn.LSL, which provides all necessary functions and sub-classes.
See examples/ for examples and javadoc/ for API documentation.

To use this interface in your program, you need to import the class edu.ucsd.sccn.LSL,
include the jna-4.0.0.jar package in your classpath, and have the native liblsl library 
for your platform in path where it is found (e.g., your application's root directory, 
or a system folder).
