%module RecorderLib
%{
#include "RecorderLib.h"
%}

%include "cstring.i"
%include "cpointer.i"
%include "cdata.i"
%include "carrays.i"
%include "RecorderLib.h"
%array_functions(void*,info)
%array_functions(char*,str)
