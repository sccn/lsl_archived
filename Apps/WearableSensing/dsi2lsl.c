/*

# This file is part of the Application Programmer's Interface (API) for Dry
# Sensor Interface (DSI) EEG systems by Wearable Sensing. The API consists of
# code, headers, dynamic libraries and documentation.  The API allows software
# developers to interface directly with DSI systems to control and to acquire
# data from them.
#
# The API is not certified to any specific standard. It is not intended for
# clinical use. The API, and software that makes use of it, should not be used
# for diagnostic or other clinical purposes.  The API is intended for research
# use and is provided on an "AS IS" basis.  WEARABLE SENSING, INCLUDING ITS
# SUBSIDIARIES, DISCLAIMS ANY AND ALL WARRANTIES EXPRESSED, STATUTORY OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO ANY IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, NON-INFRINGEMENT OR THIRD
# PARTY RIGHTS.
#
# Copyright (c) 2014-2016 Wearable Sensing LLC

This file implements the integration between Wearable Sensing DSI C/C++ API and
the LSL library. This example has been contributed by Qusp based on the demo.c
example brovided by Wearable Sensing.

Copyright (C) 2014-2016 Syntrogi Inc (dba Qusp). All rights reserved.

1. Getting started

To create a DSI application:

(1) Add the file DSI_API_Loader.c to your project (can be compiled as C or C++)

(2) #include "DSI.h" in your own C or C++ code

(3) Call Load_DSI_API() in your code before calling any of the other functions.
    The argument to Load_DSI_API may be a string that explicitly specifies the
    name of the dynamic library file to load. Alternatively, if you pass NULL
    then the default name will be used.  This default can be manipulated at
    compile time by defining the DSI_PLATFORM symbol.

    For example, using Visual C++ to compile a 32-bit binary on a Windows
    machine, you could say:

        cl /DDSI_PLATFORM=-Windows-i386  demo.c DSI_API_Loader.c

    which would cause Load_DSI_API(NULL) to look for "libDSI-Windows-i386.dll"
    instead of just "libDSI.dll".

    As an another example, using gcc on Mac OSX on a 64-bit intel machine:

        gcc -DDSI_PLATFORM=-Darwin-x86_64  demo.c DSI_API_Loader.c

    will cause Load_DSI_API(NULL) to look for "libDSI-Darwin-x86_64.dylib"
    instead of just "libDSI.dylib".

    Finally, on 64-bit Linux:

        gcc -DDSI_PLATFORM=-Linux-x86_64  demo.c DSI_API_Loader.c -ldl -lm

    will cause Load_DSI_API(NULL) to look for "libDSI-Linux-x86_64.so"
    instead of just "libDSI.so"  (note also that Linux compilation will likely
    require you to add linker flags explicitly at the end of the command: the
    -ldl flag is needed in all cases, and -lm is needed for the current demo.c
    because of its reference to the sin function). See also section 7.3 for more
    Linux notes.

(4) Check the return value of Load_DSI_API before proceeding. It will be zero on
    success. Any non-zero value means the API failed to load and other API calls
    (function names starting with DSI_) should not be called.

    If the return value is negative, it means that the libDSI file could not be
    located or opened. If this is the case, check the following rules for locating
    dynamic libraries at runtime:

    * On Windows, the .dll file needs to be either in the current working directory,
      or in one of the directories listed (separated by semicolons) in the Path
      environment variable.

    * On Mac OSX, the .dylib file needs to be either in the current working
      directory, or in one of the directories listed (separated by colons) in the
      DYLD_LIBRARY_PATH or LD_LIBRARY_PATH environment variables.

    * On Linux, the .so file must be in one of the directories listed (separated
      by colons) in the LD_LIBRARY_PATH environment variable (the current working
      directory will not be searched by default).  Currently, your system's version
      of GLIBC (the GNU C Library shared object) must also be 2.14 or later,
      otherwise the libDSI shared library will fail to load. See also the caveats
      and notes in section 7.3 below.

This file may be compiled. In it, we will build documented example applications
for recording signal or impedance values to a .csv file.  The `main()` function,
below, illustrates how to load the API and check for errors.

The file provides demos of various different aspects of API functionality.
The first command-line argument allows you to select which demo mode you are
running The default demo mode is "signals" but the following are recognized:

	version, info, buffering, montages, naming, impedances, signals

Call with the --help flag for global help, or with the help flag following
one of the modes above for mode-specific help.
                                                                              */


                                                                              /*
First let's get the header includes and advance declarations out of the way:
                                                                              */
    ///////////////////////////////////////////////////////////////////////////////

#   include "DSI.h"    // Wearable Sensing DSI API header
#   include "../../LSL/liblsl/include/lsl_c.h"  // LSL header

#   include <stdio.h>   // C standard library
#   include <string.h>  // Needed for processing strings
#   include <stdlib.h>  // Needed for allocating memory on demand
#   include <signal.h>  // Needed for implementing the Ctrl+C signaling for exiting the program

///////////////////////////////////////////////////////////////////////////////
// Helper functions and macros that will be defined down bellow
int               StartUp( int argc, const char * argv[], DSI_Headset *headsetOut, int * helpOut );
int                Finish( DSI_Headset h );
int            GlobalHelp( int argc, const char * argv[] );
lsl_outlet        InitLSL( DSI_Headset h, const char * streamName);
void             OnSample( DSI_Headset h, double packetOffsetTime, void * userData);
void      getRandomString( char *s, const int len);
const char * GetStringOpt( int argc, const char * argv[], const char * keyword1, const char * keyword2 );
int         GetIntegerOpt( int argc, const char * argv[], const char * keyword1, const char * keyword2, int defaultValue );

float *sample;
static volatile int KeepRunning = 1;
void  QuitHandler(int a){ KeepRunning = 0;}

#   define REPORT( fmt, x )  fprintf( stderr, #x " = " fmt "\n", ( x ) )
int CheckError( void ){
  if( DSI_Error() ) return fprintf( stderr, "%s\n", DSI_ClearError() );
  else return 0;
}
#   define CHECK   if( CheckError() != 0 ) return -1;
///////////////////////////////////////////////////////////////////////////////

                                                                              /*
Now, our main() function will illustrate how to load the DSI API, assuming
DSI_API_Loader.c has also been compiled and linked into your project:
                                                                              */
int main( int argc, const char * argv[] )
{
  // First load the libDSI dynamic library
  const char * dllname = NULL;
  // dllname = NULL causes the default name for the dynamic library to be used
  // (incorporating the DSI_PLATFORM macro, if defined).

  int load_error = Load_DSI_API( dllname );
  // The macro that Load_DSI_API uses to detect whether the name is NULL, and
  // to substitute the default name if so, is called DSI_DYLIB_NAME. We'll
  // make use of it directly in the error messages:
  if( load_error < 0 ) return fprintf( stderr, "failed to load dynamic library \"%s\"\n", DSI_DYLIB_NAME( dllname ) );
  if( load_error > 0 ) return fprintf( stderr, "failed to import %d functions from dynamic library \"%s\"\n", load_error, DSI_DYLIB_NAME( dllname ) );
  // Print version information - warn if the header and dynamic-library versions don't match
  fprintf( stderr, "DSI API version %s loaded\n", DSI_GetAPIVersion() );
	if( strcmp( DSI_GetAPIVersion(), DSI_API_VERSION ) != 0 ) fprintf( stderr, "WARNING - mismatched versioning: program was compiled with DSI.h version %s but just loaded shared library version %s. You should ensure that you are using matching versions of the API files - contact Wearable Sensing if you are missing a file.\n", DSI_API_VERSION, DSI_GetAPIVersion() );

  // Implements a Ctrl+C signal handler to quit the program (some terminals actually use Ctrl+Shift+C instead)
  signal(SIGINT, QuitHandler);

  //printf("Initializing the headset\n");
  DSI_Headset h;
  int help, error;
  error = StartUp( argc, argv, &h, &help ); // common routine for all demos, to initialize the headset
  if( error || help){
    GlobalHelp(argc, argv);
    return error;
  }

  // Initialize LSL outlet
  const char * streamName = GetStringOpt(  argc, argv, "lsl-stream-name",   "m" );
  if (!streamName) streamName = "WS-default";
  printf("Initializing %s outlet\n", streamName);
  lsl_outlet outlet = InitLSL(h, streamName); CHECK; /* stream outlet */

  // Set the sample callback (forward every data sample received to LSL)
  DSI_Headset_SetSampleCallback( h, OnSample, outlet ); CHECK
  // This registers our `DSI_SampleCallback`, to be called on every
  // new sample during `DSI_Headset_Idle()`.

  // Start data acquisition
  printf("Starting data acquisition\n");
  DSI_Headset_StartDataAcquisition( h ); CHECK

  // Start streaming
  printf("Streaming...\n");
  while( KeepRunning==1 ){
    DSI_Headset_Idle( h, 0.0 ); CHECK
  }

  // Gracefully exit the program
  printf("\n%s will exit now.\nGood bye!\n", argv[ 0 ]);
  lsl_destroy_outlet(outlet);
  return Finish( h );
}



void OnSample( DSI_Headset h, double packetOffsetTime, void * outlet)
{
  unsigned int channelIndex;
  unsigned int numberOfChannels = DSI_Headset_GetNumberOfChannels( h );
  if (sample==NULL) sample = (float *)malloc( numberOfChannels * sizeof(float));
  for(channelIndex=0; channelIndex < numberOfChannels; channelIndex++){
    sample[channelIndex] = (float) DSI_Channel_GetSignal( DSI_Headset_GetChannelByIndex( h, channelIndex ) );
  }
	lsl_push_sample_f(outlet, sample);
}

int Message( const char * msg, int debugLevel ){
  return fprintf( stderr, "DSI Message (level %d): %s\n", debugLevel, msg );
}
/*
4. Connecting/disconnecting the headset

The `StartUp()` function below gives examples of `DSI_Headset_...()` function
calls that might commonly be used to initialize the headset.  We'll call this
function in all our subsequent demos.
                                                                              */
int StartUp( int argc, const char * argv[], DSI_Headset * headsetOut, int * helpOut )
{
  DSI_Headset h;

  // First, let's figure out some of the command-line options that affect
  // headset initialization, for the purposes of this demo program.
  // (NB: `GetStringOpt` and `GetIntegerOpt` are not part of the DSI API -
  // they're defined below as part of this demo program.  But you will
  // presumably have your own mechanisms for allowing the user to
  // configure your program.)
  //
  //   "[I]t is more powerful than the sum total of all the computers on
  //    this planet... But over ninety-five percent of that power is used
  //    in simply understanding what it is you want it to do."
  //            --Douglas Adams,  Dirk Gently's Holistic Detective Agency
  int          help       = GetStringOpt(  argc, argv, "help",      "h" ) != NULL;
  const char * serialPort = GetStringOpt(  argc, argv, "port",      "p" );
  const char * montage    = GetStringOpt(  argc, argv, "montage",   "m" );
  const char * reference  = GetStringOpt(  argc, argv, "reference", "r" );
  int          verbosity  = GetIntegerOpt( argc, argv, "verbosity", "v", 2 );
  if( headsetOut ) *headsetOut = NULL;
  if( helpOut ) *helpOut = help;
  if( help ) return 0;

  // OK, now we can begin:
  h = DSI_Headset_New( NULL ); CHECK
  // Passing NULL defers setup of the serial port connection until later...

  DSI_Headset_SetMessageCallback( h, Message ); CHECK
  DSI_Headset_SetVerbosity( h, verbosity ); CHECK
  // ...which allows us to configure the way we handle any debugging messages
  // that occur during connection (see our definition of the `DSI_MessageCallback`
  // function `Message()` above).

  DSI_Headset_Connect( h, serialPort ); CHECK
  // Now we establish the serial port connection and initialize the headset.
  // In this demo program, the string supplied in the --port command-line
  // option is used as the serial port address (if this string is empty, the
  // API will automatically look for an environment variable called
  // DSISerialPort).

  DSI_Headset_ChooseChannels( h, montage, reference, 1 ); CHECK
  // Sets up the montage according to strings supplied in the --montage and
  // --reference command-line options, if any.

  fprintf( stderr, "%s\n", DSI_Headset_GetInfoString( h ) ); CHECK
  // prints an overview of what is known about the headset

  if( headsetOut ) *headsetOut = h;
  if( helpOut ) *helpOut = help;
  return 0;
}
                                                                              /*
While we're at it let's write the corresponding wrap-up steps that allow
the headset to back out gracefully from whatever task it was performing
before finally de-initializing the DSI_Headset object:
                                                                              */
int Finish( DSI_Headset h )
{
  DSI_Headset_SetSampleCallback( h, NULL, NULL ); CHECK
  // This stops our application from responding to received samples.

  DSI_Headset_StopDataAcquisition( h ); CHECK
  // This send a command to the headset to tell it to stop sending samples.

  DSI_Headset_Idle( h, 1.0 ); CHECK
  // This allows more than enough time to receive any samples that were
  // sent before the stop command is carried out, along with the alarm
  // signal that the headset sends out when it stops

  DSI_Headset_Delete( h ); CHECK
  // This is the only really necessary step. Disconnects from the serial
  // port, frees memory, etc.

  return 0;
}


void getRandomString(char *s, const int len)
{
  int i = 0;
  static const char alphanum[] =     "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  for (i=0; i < len; ++i){ s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];}
  s[len] = 0;
}

lsl_outlet InitLSL(DSI_Headset h, const char * streamName)
{
  unsigned int channelIndex;
  unsigned int numberOfChannels = DSI_Headset_GetNumberOfChannels( h );
  int samplingRate = DSI_Headset_GetSamplingRate( h );

  lsl_streaminfo info;         /* out stream declaration object */
	lsl_xml_ptr desc, chn, chns, ref; /* some xml element pointers */
	int imax = 16;
  char source_id[imax];
  char *long_label;
  char *short_label;
  char *reference;
  getRandomString(source_id, imax);

	/* declare a new streaminfo (name: WearableSensing, content type: EEG, number of channels, srate, float values, source id*/
	info = lsl_create_streaminfo((char*)streamName,"EEG",numberOfChannels,samplingRate,cft_float32,source_id);

	/* add some meta-data fields to it */
	/* (for more standard fields, see https://github.com/sccn/xdf/wiki/Meta-Data) */
	desc = lsl_get_desc(info);
	lsl_append_child_value(desc,"manufacturer","WearableSensing");
	chns = lsl_append_child(desc,"channels");
	for( channelIndex=0; channelIndex < numberOfChannels ; channelIndex++)
  {
    chn = lsl_append_child(chns,"channel");
		//lsl_append_child_value(chn,"label", (char*) DSI_Channel_GetString( DSI_Headset_GetChannelByIndex( h, channelIndex ) ) );
    //channel_label = (char*) DSI_Channel_GetString( DSI_Headset_GetChannelByIndex( h, channelIndex ) );

    long_label = (char*) DSI_Channel_GetString( DSI_Headset_GetChannelByIndex( h, channelIndex ) );
    short_label = strtok(long_label, "-");
    if(short_label == NULL)
      short_label = long_label;
    lsl_append_child_value(chn,"label", short_label);
		lsl_append_child_value(chn,"unit","microvolts");
		lsl_append_child_value(chn,"type","EEG");
	}
  reference = (char*)DSI_Headset_GetReferenceString(h);
  ref = lsl_append_child(desc,"reference");
  lsl_append_child_value(ref,"label", reference);
  printf("REF: %s\n", reference);

	/* make a new outlet (chunking: default, buffering: 360 seconds) */
	return lsl_create_outlet(info,0,360);
}


int GlobalHelp( int argc, const char * argv[] )
{
  fprintf( stderr,
            "Usage: %s [ --OPTIONS... ]\n\n"
            "With the exception of --help,\n"
            "the options should be given in --NAME=VALUE format.\n"
            "\n"
            "  --help\n"
            "       Displays this help text.\n"
            "\n"
            "  --port\n"
            "       Specifies the serial port address (e.g. --port=COM4 on Windows,\n"
            "       --port=/dev/cu.DSI24-023-BluetoothSeri on OSX, or --port=/dev/rfcomm0 on Linux) on which to connect.\n"
            "       Note: if you omit this option, or use an empty string or the string\n"
            "       \"default\", then the API will look for an environment variable called\n"
            "       DSISerialPort and use the content of that, if available.\n"
            "\n"
            "  --montage\n"
            "       A list of channel specifications, comma-separated without spaces,\n"
            "       (can also be space-delimited, but then you would need to enclose the\n"
            "       option in quotes on the command-line).\n"
            "\n"
            "  --reference\n"
            "       The name of sensor (or linear combination of sensors, without spaces)\n"
            "       to be used as reference. Defaults to a \"traditional\" averaged-ears or\n"
            "       averaged-mastoids reference if available, or the factory reference\n"
            "       (typically Pz) if these sensors are not available.\n"
            "\n"
            "  --verbosity\n"
            "       The higher the number, the more messages the headset will send to the\n"
            "       registered `DSI_MessageCallback` function, and hence to the console\n"
            "       (and the more low-level they will tend to be)\n"
            "\n"
            "  --lsl-stream-name\n"
            "       The name of the LSL outlet that will be created to stream the samples\n"
            "       received from the device. If omitted, the stream will be given the name WS-default.\n"
            "\n"
        , argv[ 0 ] );
        return 0;
    }


// NOTHING TO SEE HERE; MOVE ALONG.

// Helper function for figuring out command-line input flags like --port=COM4
// or /port:COM4 (either syntax will work).  Returns NULL if the specified
// option is absent. Returns a pointer to the argument value if the option
// is present (the pointer will point to '\0' if the argument value is empty
// or not supplied as part of the option string).  Better getopt
// implementations exist, but if you want something to work cross-platform
// without hassle, you have to do it yourself...

const char * GetStringOpt( int argc, const char * argv[], const char * keyword1, const char * keyword2 )
{
    int i, j;
    const char * result = NULL;
    const char * keyword;
    for( i = 1; i < argc; i++ )
    {
        int isopt = 0;
        const char * arg = argv[ i ];
        if( !arg ) continue;
        for( j = 0; arg[ j ]; j++ ) isopt |= arg[ j ] == '-' || arg[ j ] == '=' || arg[ j ] == '/' || arg[ j ] == ':';
        if( *arg == '-' || *arg == '/' ) ++arg;
        if( *arg == '-' || *arg == '/' ) ++arg;
        for( j = 0, keyword = keyword1; j < 2; j++, keyword = keyword2  )
        {
            if( keyword && strncmp( arg, keyword, strlen( keyword ) ) == 0 )
            {
                const char * potential = arg + strlen( keyword );
                if( *potential == '=' || *potential == ':' ) result = potential + 1;
                else if( *potential == '\0' || ( *keyword == '\0' && !isopt ) ) result = potential;
            }
        }
    }
    return result;
}
int GetIntegerOpt( int argc, const char * argv[], const char * keyword1, const char * keyword2, int defaultValue )
{
    char * end;
    int result;
    const char * stringValue = GetStringOpt( argc, argv, keyword1, keyword2 );
    if( !stringValue || !*stringValue ) return defaultValue;
    result = ( int ) strtol( stringValue, &end, 10 );
    if( !end || !*end ) return result;
    fprintf( stderr, "WARNING: could not interpret \"%s\" as a valid integer value for the \"%s\" option - reverting to default value %s=%g\n", stringValue, keyword1, keyword1, ( double )defaultValue );
    return defaultValue;
}
