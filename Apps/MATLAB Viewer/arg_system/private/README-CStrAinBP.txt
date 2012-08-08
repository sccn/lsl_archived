This fast setdiff implementation is important for the speed of certain infrastructure functions (namely the arg system).

If you get a warning that the regular setdiff() is used as a fallback, please try to recompile the file for your architecture and put it in a subfolder (together with 
a copy of an env_add.m file from the other platform directories -- do not copy the env_add.m file from the directory in which this readme file is located).


CStrAinBP contains the following functions:
* CStrAinBP.*
* fast_setdiff.m