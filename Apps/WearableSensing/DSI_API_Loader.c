/*

# This file is part of the Application Programmer's Interface (API) for Dry Sensor Interface
# (DSI) EEG systems by Wearable Sensing. The API consists of code, headers, dynamic libraries
# and documentation.  The API allows software developers to interface directly with DSI
# systems to control and to acquire data from them.
# 
# The API is not certified to any specific standard. It is not intended for clinical use.
# The API, and software that makes use of it, should not be used for diagnostic or other
# clinical purposes.  The API is intended for research use and is provided on an "AS IS"
# basis.  WEARABLE SENSING, INCLUDING ITS SUBSIDIARIES, DISCLAIMS ANY AND ALL WARRANTIES
# EXPRESSED, STATUTORY OR IMPLIED, INCLUDING BUT NOT LIMITED TO ANY IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT OR THIRD PARTY RIGHTS.
# 
# (c) 2014-2016 Wearable Sensing LLC

Add this file to your C or C++ project.  Call Load_DSI_API() and check that its return
value is 0 before proceeding to call any DSI_... functions. See README.txt for more details.

*/
 
#include "DSI.h"

const char * DSI_Stub( void ) { static char msg[] = "DSI API is not loaded"; return msg; }
#define DSI_API_FUNC( type, name, args, implementation )   type ( *name ) args = ( type ( * ) args )DSI_Stub;
#include "DSI.h"

#ifdef _WIN32
#include <windows.h>
#else
#include <dlfcn.h>
#define HINSTANCE                void *
#define LoadLibrary( a )         dlopen( a, RTLD_NOW | RTLD_GLOBAL )
#define GetProcAddress( a, b )   dlsym( ( a ), ( b ) )
#endif /* _WIN32 */


int Load_DSI_API( const char *dllname )
{
	int failures = 0;
	HINSTANCE dll = LoadLibrary( DSI_DYLIB_NAME( dllname ) );
	if( !dll ) return -1;

#define SUPPRESS_EXTERNC
#define DSI_API_FUNC( type, name, args, implementation )  \
	if ( ( name = ( type ( * ) args )GetProcAddress( dll , #name ) ) == 0 ) failures++;
#include "DSI.h"
	return failures;
}
