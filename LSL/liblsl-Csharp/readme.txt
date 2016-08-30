The is the C# interface to the lab streaming layer. To use it, you need to include the file LSL.cs in 
your project, and make sure that the library liblsl32.dll is found (e.g., in your application's root 
directory or in a system path).

If you are deploying on a platform that requires a different binary than liblsl32.dll (e.g., liblsl64.so 
on 64-bit Linux, or liblsl64.dylib on Mac OS, then you need to replace the libname constant in LSL.cs
by the corresponding file name).