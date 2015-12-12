This is an experimental Android setup which should build with a recent NDK; 
however, there is still at least one showstopper bug yet to be fixed (creating 
an outlet crashes), which we have not yet debugged. Let us know if you have a fix.

I (Matthew Grivich) suspect that this bug can be fixed by adding 
In addition, see a different, fully functional Android setup at LSL/liblsl/project/android-vs2013. 
I (Matthew Grivich) have left liblsl-Android because it gives hints on how to use automatic java wrappers, while
the version at android-vs2013 uses a very minimal java wrapper and does almost everything in
native.