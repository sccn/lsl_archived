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

Include this file as a header for your C or C++ code. See README.txt for more details.

*/


#undef EXTERNC
#if defined __cplusplus && !defined SUPPRESS_EXTERNC
extern "C" {
#define EXTERNC
#endif /* #ifdef __cplusplus */


#define DSI_API_MAJOR_VERSION  1  /* NB: Use DSI_GetAPIVersion(), rather than these      */
#define DSI_API_MINOR_VERSION 13  /*     values, to find out the version of the API that */
#define DSI_API_BUILD          1  /*     was used to compile your dynamic library.       */
                                  /*     Verify that the result matches the values here. */

#define DSI_API_VERSION        STRINGIFY_VERSION( DSI_API_MAJOR_VERSION, DSI_API_MINOR_VERSION, DSI_API_BUILD )
#define STRINGIFY_VERSION( major, minor, build ) _STRINGIFY( major ) "." STRINGIFY( minor ) "." STRINGIFY( build )
#define  STRINGIFY( s ) _STRINGIFY( s )
#define _STRINGIFY( s ) #s


/* ************************************************************************************ */
#ifndef INCLUDED_DSI_H
#define INCLUDED_DSI_H

#include <stddef.h> /* for size_t */
typedef int bool_t;

typedef char   *  DSI_Headset;
typedef short  *  DSI_Channel;
typedef long   *  DSI_Source;
typedef double *  DSI_ProcessingStage;

typedef  void ( *DSI_SampleCallback  )( DSI_Headset h, double packetTime, void * userData );
typedef   int ( *DSI_MessageCallback )( const char * msg, int debugLevel );

typedef enum {
    ActiveSensors       =   1,
    IgnoredSensors      =   2,
    UnconnectedSensors  =   4,
    Reference           =   8,
    Triggers            =  16,
    Clocks              =  32,
    CommonModeSignal    =  64,
    NullSignal          = 128,
    TriggerBits         = 256,
    ConnectedSensors    = ( ActiveSensors + IgnoredSensors ),
    AllSensors          = ( ConnectedSensors + UnconnectedSensors ),
    Default             = ( AllSensors + Triggers ),
    Everything          = 0xffff
} DSI_SourceSelection;

extern int Load_DSI_API( const char * dllname );

#if defined _WIN32
#  define DYLIB_PREFIX "lib"
#  define DYLIB_EXTENSION ".dll"
#elif defined __APPLE__ 
#  define DYLIB_PREFIX "lib"
#  define DYLIB_EXTENSION ".dylib"
#else
#  define DYLIB_PREFIX "lib"
#  define DYLIB_EXTENSION ".so"
#endif /* #if defined _WIN32 */
#ifndef DSI_PLATFORM    /*   Define the DSI_PLATFORM macro when compiling your app to make it easier   */ 
#  define DSI_PLATFORM  /*   for the app to find the correct build of the dynamic library by default   */
#  define DSI_DYLIB_NAME( name ) ( ( name ) ? ( name ) : ( DYLIB_PREFIX "DSI" DYLIB_EXTENSION ) )
#else                   /*   Example: if you used the -DDSI_PLATFORM=-Darwin-x86_64 compiler flag then DSI_DYLIB_NAME( NULL ) would expand to "libDSI-Darwin-x86_64.dylib"   */
#  define DSI_DYLIB_NAME( name ) ( ( name ) ? ( name ) : ( DYLIB_PREFIX "DSI" STRINGIFY( DSI_PLATFORM ) DYLIB_EXTENSION ) )
#endif /* #ifndef DSI_PLATFORM */
#ifndef DSI_API_FUNC
#  ifdef DSI_STATIC
#    define DSI_API_FUNC( type, name, args, implementation )  type name  args; /* required if linking statically against DSI_C_Interface.cpp */
#  else
#    define DSI_API_FUNC( type, name, args, implementation )  extern type ( *name ) args;  /* required if linking dynamically (default, normal use-case) */
#  endif /* #ifdef DSI_STATIC */
#endif /* #ifndef DSI_API_FUNC */

#endif /* #ifndef INCLUDED_DSI_H */
/* ************************************************************************************ */


/* **************************************************** */
/* General-purpose functions                            */
/* **************************************************** */

DSI_API_FUNC(  const char *   ,    DSI_Error                                     ,  ( void )                                                                 ,   { return gDSIErrorString; }                                                                                    )
DSI_API_FUNC(  const char *   ,    DSI_ClearError                                ,  ( void )                                                                 ,   { const char * p = gDSIErrorString; gDSIErrorString = NULL; return p; }                                        )
DSI_API_FUNC(  void           ,    DSI_SetErrorCallback                          ,  ( DSI_MessageCallback func )                                             ,   { gDSIErrorHandler = func; }                                                                                   )

DSI_API_FUNC(  void           ,    DSI_SanityCheck                               ,  ( void )                                                                 ,   DO( DSI::SanityCheck() )                                                                                       )
DSI_API_FUNC(  int            ,    DSI_Console                                   ,  ( const char * msg, int debugLevel )                                     ,   RETURN( 0, DSI::Console( INPUTSTRING( msg ), debugLevel ) )                                                    )
DSI_API_FUNC(  void           ,    DSI_Sleep                                     ,  ( double seconds )                                                       ,   DO( DSI::Sleep( seconds ) )                                                                                    )
DSI_API_FUNC(  const char *   ,    DSI_PythonString                              ,  ( const void * raw, size_t nBytes, bool_t allhex )                       ,   RETURNSTRING( DSI::PythonString( raw, nBytes, allhex ) )                                                       )

DSI_API_FUNC(  const char *   ,    DSI_GetAPIVersion                             ,  ( void )                                                                 ,   RETURNSTRING( DSI_API_VERSION )                                                                                )

DSI_API_FUNC(  const char *   ,    DSI_GetDefaultPort                            ,  ( void )                                                                 ,   RETURNSTRING( DSI::DefaultPort() )                                                                             )


/* **************************************************** */
/* DSI_Headset methods                                  */
/* **************************************************** */

DSI_API_FUNC(  DSI_Headset    ,    DSI_Headset_New                               ,  ( const char * port )                                                    ,   RETURN( NULL, ( DSI_Headset )( ( port && *port ) ? new DSI::Headset( port ) : new DSI::Headset() ) )           )
DSI_API_FUNC(  void           ,    DSI_Headset_Delete                            ,  ( DSI_Headset h )                                                        ,   DO( delete HEADSET )                                                                                           )

DSI_API_FUNC(  void           ,    DSI_Headset_SetVerbosity                      ,  ( DSI_Headset h, int level )                                             ,   DO( HEADSET->SetVerbosity( level ) )                                                                           )
DSI_API_FUNC(  void           ,    DSI_Headset_SetMessageCallback                ,  ( DSI_Headset h, DSI_MessageCallback func )                              ,   DO( HEADSET->SetMessageCallback( func ) )                                                                      )
DSI_API_FUNC(  void           ,    DSI_Headset_SetSampleCallback                 ,  ( DSI_Headset h, DSI_SampleCallback func, void * userData )              ,   DO( HEADSET->SetSampleCallback( ( DSI::SampleCallback )func, userData ) )                                      )

DSI_API_FUNC(  void           ,    DSI_Headset_Connect                           ,  ( DSI_Headset h, const char * port )                                     ,   DO( HEADSET->Connect( INPUTSTRING( port ) ) )                                                                  )
DSI_API_FUNC(  bool_t         ,    DSI_Headset_IsConnected                       ,  ( DSI_Headset h )                                                        ,   RETURN( 0, HEADSET->Connected() )                                                                              )
DSI_API_FUNC(  double         ,    DSI_Headset_SecondsSinceConnection            ,  ( DSI_Headset h )                                                        ,   RETURN( -1.0, HEADSET->Elapsed() )                                                                             )
DSI_API_FUNC(  const char *   ,    DSI_Headset_GetPort                           ,  ( DSI_Headset h )                                                        ,   RETURNSTRING( HEADSET->Port() )                                                                                )
DSI_API_FUNC(  void           ,    DSI_Headset_Disconnect                        ,  ( DSI_Headset h )                                                        ,   DO( HEADSET->Disconnect() )                                                                                    )

DSI_API_FUNC(  void           ,    DSI_Headset_ChooseChannels                    ,  ( DSI_Headset h, const char * spec, const char * ref, bool_t autoswap )  ,   DO( HEADSET->ChooseChannels( INPUTSTRING( spec ), INPUTSTRING( ref ), autoswap ) )                             )
DSI_API_FUNC(  void           ,    DSI_Headset_AddChannelToMontage_FromSource    ,  ( DSI_Headset h, DSI_Source s )                                          ,   DO( HEADSET->AddChannelToMontage( SOURCE ) )                                                                   )
DSI_API_FUNC(  void           ,    DSI_Headset_AddChannelToMontage_FromString    ,  ( DSI_Headset h, const char * spec, bool_t autoswap )                    ,   DO( HEADSET->AddChannelToMontage( INPUTSTRING( spec ), autoswap ) )                                            )
DSI_API_FUNC(  const char *   ,    DSI_Headset_SetTraditionalReference           ,  ( DSI_Headset h, bool_t autoswap )                                       ,   RETURNSTRING( HEADSET->SetTraditionalReference( autoswap ) )                                                   )
DSI_API_FUNC(  void           ,    DSI_Headset_SetDefaultReference               ,  ( DSI_Headset h, const char * spec, bool_t autoswap )                    ,   DO( HEADSET->SetDefaultReference( INPUTSTRING( spec ), autoswap ) )                                            )
DSI_API_FUNC(  const char *   ,    DSI_Headset_GetFactoryReferenceString         ,  ( DSI_Headset h )                                                        ,   RETURNSTRING( HEADSET->FactoryReferenceString() )                                                              )
DSI_API_FUNC(  const char *   ,    DSI_Headset_GetReferenceString                ,  ( DSI_Headset h )                                                        ,   RETURNSTRING( HEADSET->ReferenceString() )                                                                     )
DSI_API_FUNC(  void           ,    DSI_Headset_ForgetMontage                     ,  ( DSI_Headset h )                                                        ,   DO( HEADSET->ForgetMontage() )                                                                                 )
DSI_API_FUNC(  void           ,    DSI_Headset_UseNamingScheme                   ,  ( DSI_Headset h, const char * scheme )                                   ,   DO( HEADSET->UseNamingScheme( INPUTSTRING( scheme ) ) )                                                        )
DSI_API_FUNC(  bool_t         ,    DSI_Headset_RenameSource                      ,  ( DSI_Headset h, const char * from, const char * to )                    ,   RETURN( 0, HEADSET->RenameSource( INPUTSTRING( from ), INPUTSTRING( to ) ) )                                   )
DSI_API_FUNC(  bool_t         ,    DSI_Headset_AddSourceAliases                  ,  ( DSI_Headset h, const char * aliases )                                  ,   RETURN( 0, HEADSET->AddSourceAliases( INPUTSTRING( aliases ) ) )                                               )

DSI_API_FUNC(  int            ,    DSI_Headset_GetNumberOfChannels               ,  ( DSI_Headset h )                                                        ,   RETURN( 0, ( int )HEADSET->Montage()->size() )                                                                 )
DSI_API_FUNC(  int            ,    DSI_Headset_GetNumberOfSources                ,  ( DSI_Headset h )                                                        ,   RETURN( 0, ( int )HEADSET->Sources( false )->size() )                                                          )
DSI_API_FUNC(  DSI_Channel    ,    DSI_Headset_GetChannelByIndex                 ,  ( DSI_Headset h, unsigned int index )                                    ,   RETURN( ( DSI_Channel )NULL, ( DSI_Channel )HEADSET->GetChannelByIndex( index ) )                              )
DSI_API_FUNC(  DSI_Channel    ,    DSI_Headset_GetChannelByName                  ,  ( DSI_Headset h, const char * name )                                     ,   RETURN( ( DSI_Channel )NULL, ( DSI_Channel )HEADSET->GetChannelByName( INPUTSTRING( name ), true ) )           )
DSI_API_FUNC(  DSI_Source     ,    DSI_Headset_GetSourceByIndex                  ,  ( DSI_Headset h, unsigned int index )                                    ,   RETURN( ( DSI_Source  )NULL, ( DSI_Source  )HEADSET->GetSourceByIndex(  index, false ) )                       )
DSI_API_FUNC(  DSI_Source     ,    DSI_Headset_GetSourceByName                   ,  ( DSI_Headset h, const char * name )                                     ,   RETURN( ( DSI_Source  )NULL, ( DSI_Source  )HEADSET->MatchSource( INPUTSTRING( name ), true ) )                )  /* string should contain either a channel name or a 1-based index */

DSI_API_FUNC(  bool_t         ,    DSI_Headset_IsBlueToothInitialized            ,  ( DSI_Headset h )                                                        ,   RETURN( 0, HEADSET->BlueToothInitialized() )                                                                   )
DSI_API_FUNC(  void           ,    DSI_Headset_ReallocateBuffers                 ,  ( DSI_Headset h, double secondsForSignal, double secondsForImpedance )   ,   DO( HEADSET->ReallocateBuffers( secondsForSignal, secondsForImpedance ) )                                      )
DSI_API_FUNC(  void           ,    DSI_Headset_FlushBuffers                      ,  ( DSI_Headset h )                                                        ,   DO( HEADSET->FlushBuffers() )                                                                                  )
DSI_API_FUNC(  size_t         ,    DSI_Headset_GetNumberOfBufferedSamples        ,  ( DSI_Headset h )                                                        ,   RETURN( 0, HEADSET->Buffered() )                                                                               )
DSI_API_FUNC(  size_t         ,    DSI_Headset_GetNumberOfOverflowedSamples      ,  ( DSI_Headset h )                                                        ,   RETURN( 0, HEADSET->BufferOverflow() )                                                                         )
DSI_API_FUNC(  void           ,    DSI_Headset_ConfigureBufferingController      ,  ( DSI_Headset h, double secondsBetweenUpdates, double smoothing, double P, double I, double D ) ,  DO( HEADSET->ConfigureBufferingController( secondsBetweenUpdates, smoothing, P, I, D ) ) )
DSI_API_FUNC(  void           ,    DSI_Headset_ConfigureBatch                    ,  ( DSI_Headset h, unsigned int nSamples, double targetDelaySeconds )      ,   DO( HEADSET->ConfigureBatch( nSamples, targetDelaySeconds ) )                                                  )
DSI_API_FUNC(  int            ,    DSI_Headset_StartBackgroundAcquisition        ,  ( DSI_Headset h )                                                        ,   RETURN( false, HEADSET->StartBackgroundAcquisition() )                                                         )
DSI_API_FUNC(  void           ,    DSI_Headset_StopBackgroundAcquisition         ,  ( DSI_Headset h )                                                        ,   DO( HEADSET->StopBackgroundAcquisition() )                                                                     )
DSI_API_FUNC(  void           ,    DSI_Headset_WaitForBatch                      ,  ( DSI_Headset h )                                                        ,   DO( HEADSET->WaitForBatch() )                                                                                  )
DSI_API_FUNC(  void           ,    DSI_Headset_Idle                              ,  ( DSI_Headset h, double seconds )                                        ,   DO( HEADSET->Idle( seconds ) )                                                                                 )
DSI_API_FUNC(  void           ,    DSI_Headset_Receive                           ,  ( DSI_Headset h, double seconds, double idleAfter )                      ,   DO( HEADSET->Receive( seconds, idleAfter ) )                                                                   )
DSI_API_FUNC(  void           ,    DSI_Headset_KillDataStream                    ,  ( DSI_Headset h, bool_t expectReply )                                    ,   DO( HEADSET->KillDataStream( expectReply ) )                                                                   )
DSI_API_FUNC(  int            ,    DSI_Headset_GetAlarm                          ,  ( DSI_Headset h, bool_t remove )                                         ,   RETURN( 0, HEADSET->GetAlarm( remove ) )                                                                       )
DSI_API_FUNC(  size_t         ,    DSI_Headset_GetNumberOfAlarms                 ,  ( DSI_Headset h )                                                        ,   RETURN( 0, HEADSET->GetNumberOfAlarms() )                                                                      )
DSI_API_FUNC(  void           ,    DSI_Headset_ClearAlarms                       ,  ( DSI_Headset h )                                                        ,   DO( HEADSET->ClearAlarms() )                                                                                   )

/* DSI_Headset getters */
DSI_API_FUNC(  void           ,    DSI_Headset_SendBatteryQuery                  ,  ( DSI_Headset h )                                                        ,   DO( HEADSET->StartBatteryMonitor() )                                                                           )
DSI_API_FUNC(  double         ,    DSI_Headset_GetBatteryLevel                   ,  ( DSI_Headset h, int whichBattery )                                      ,   RETURN( -1.0, HEADSET->BatteryLevel( whichBattery ) )                                                          )
DSI_API_FUNC(  const char *   ,    DSI_Headset_GetBatteryLevelString             ,  ( DSI_Headset h )                                                        ,   RETURNSTRING( HEADSET->BatteryLevel() )                                                                        )
DSI_API_FUNC(  const char *   ,    DSI_Headset_GetFirmwareRevision               ,  ( DSI_Headset h )                                                        ,   RETURNSTRING( HEADSET->FirmwareRevision() )                                                                    )
DSI_API_FUNC(  const char *   ,    DSI_Headset_GetMontageString                  ,  ( DSI_Headset h )                                                        ,   RETURNSTRING( HEADSET->MontageString( true ) )                                                                 )
DSI_API_FUNC(  const char *   ,    DSI_Headset_GetSourceNames                    ,  ( DSI_Headset h, DSI_SourceSelection selection )                         ,   RETURNSTRING( HEADSET->SourceNames( selection ) )                                                              )
DSI_API_FUNC(  const char *   ,    DSI_Headset_GetInfoString                     ,  ( DSI_Headset h )                                                        ,   RETURNSTRING( HEADSET->InfoString() )                                                                          )
DSI_API_FUNC(  unsigned int   ,    DSI_Headset_GetSensorCount                    ,  ( DSI_Headset h )                                                        ,   RETURN( 0, HEADSET->SensorCount() )                                                                            )
DSI_API_FUNC(  unsigned int   ,    DSI_Headset_GetSerialNumber                   ,  ( DSI_Headset h )                                                        ,   RETURN( 0, HEADSET->SerialNumber() )                                                                           )
DSI_API_FUNC(  double         ,    DSI_Headset_GetSamplingRate                   ,  ( DSI_Headset h )                                                        ,   RETURN( -1.0, HEADSET->SamplingRate() )                                                                        )
DSI_API_FUNC(  int            ,    DSI_Headset_GetFilterMode                     ,  ( DSI_Headset h )                                                        ,   RETURN( -1, HEADSET->FilterMode() )                                                                            )
DSI_API_FUNC(  int            ,    DSI_Headset_GetDataAcquisitionMode            ,  ( DSI_Headset h )                                                        ,   RETURN( -1, HEADSET->DataAcquisitionMode() )                                                                   )
DSI_API_FUNC(  int            ,    DSI_Headset_GetImpedanceDriverMode            ,  ( DSI_Headset h )                                                        ,   RETURN( -1, HEADSET->ImpedanceDriverMode() )                                                                   )
DSI_API_FUNC(  int            ,    DSI_Headset_GetAnalogResetMode                ,  ( DSI_Headset h )                                                        ,   RETURN( -1, HEADSET->AnalogResetMode() )                                                                       )
DSI_API_FUNC(  double         ,    DSI_Headset_GetImpedanceCMF                   ,  ( DSI_Headset h )                                                        ,   RETURN( 0.0, HEADSET->ImpedanceCMF() )                                                                         )

/* DSI_Headset low-level commands: */      
DSI_API_FUNC(  void           ,    DSI_Headset_StartDataAcquisition              ,  ( DSI_Headset h )                                                        ,   DO( HEADSET->StartDataAcquisition() )                                                                          )
DSI_API_FUNC(  void           ,    DSI_Headset_StopDataAcquisition               ,  ( DSI_Headset h )                                                        ,   DO( HEADSET->StopDataAcquisition() )                                                                           )
DSI_API_FUNC(  void           ,    DSI_Headset_StartImpedanceDriver              ,  ( DSI_Headset h )                                                        ,   DO( HEADSET->StartImpedanceDriver() )                                                                          )
DSI_API_FUNC(  void           ,    DSI_Headset_StopImpedanceDriver               ,  ( DSI_Headset h )                                                        ,   DO( HEADSET->StopImpedanceDriver() )                                                                           )
DSI_API_FUNC(  void           ,    DSI_Headset_StartAnalogReset                  ,  ( DSI_Headset h )                                                        ,   DO( HEADSET->StartAnalogReset( false ) )                                                                       )
DSI_API_FUNC(  void           ,    DSI_Headset_LockAnalogReset                   ,  ( DSI_Headset h )     /* NB: invalidates signal data */                  ,   DO( HEADSET->LockAnalogReset() )                                                                               )
DSI_API_FUNC(  void           ,    DSI_Headset_ReleaseAnalogReset                ,  ( DSI_Headset h )                                                        ,   DO( HEADSET->ReleaseAnalogReset() )                                                                            )
DSI_API_FUNC(  void           ,    DSI_Headset_Shutdown                          ,  ( DSI_Headset h )                                                        ,   DO( HEADSET->Shutdown() )                                                                                      )

/* **************************************************** */
/* DSI_Headset methods supporting plug-in processing    */
/* stages, and DSI_ProcessingStage methods              */
/* **************************************************** */

DSI_API_FUNC(  DSI_ProcessingStage ,  DSI_Headset_AddProcessingStage             ,  ( DSI_Headset h, const char * name, DSI_SampleCallback func, void * paramData, DSI_ProcessingStage input ) , RETURN( ( DSI_ProcessingStage )NULL, ( DSI_ProcessingStage )HEADSET->AddProcessingStage( INPUTSTRING( name ), ( DSI::SampleCallback )func, paramData, ( DSI::ProcessingStage * )input ) )   )
DSI_API_FUNC(  void                ,  DSI_Headset_ReallocateStageBuffers         ,  ( DSI_Headset h, DSI_ProcessingStage stage, double seconds )             ,   DO( HEADSET->ReallocateStageBuffers( ( DSI::ProcessingStage * )stage, seconds ) )                              )
DSI_API_FUNC(  unsigned int        ,  DSI_Headset_GetNumberOfProcessingStages    ,  ( DSI_Headset h )                                                        ,   RETURN( 0, HEADSET->NumberOfProcessingStages() )                                                               )
DSI_API_FUNC(  DSI_ProcessingStage ,  DSI_Headset_GetProcessingStageByIndex      ,  ( DSI_Headset h, unsigned int index )                                    ,   RETURN( ( DSI_ProcessingStage )NULL, ( DSI_ProcessingStage )HEADSET->GetProcessingStageByIndex( index ) )      )
DSI_API_FUNC(  DSI_ProcessingStage ,  DSI_Headset_GetProcessingStageByName       ,  ( DSI_Headset h, const char * name )                                     ,   RETURN( ( DSI_ProcessingStage )NULL, ( DSI_ProcessingStage )HEADSET->MatchProcessingStage( INPUTSTRING( name ), true ) )      )

DSI_API_FUNC(  DSI_Channel         ,  DSI_ProcessingStage_GetChannelByIndex      ,  ( DSI_ProcessingStage p, unsigned int index )                            ,   RETURN( ( DSI_Channel )NULL, ( DSI_Channel )STAGE->GetChannelByIndex( index ) )                                )
DSI_API_FUNC(  DSI_Channel         ,  DSI_ProcessingStage_GetChannelByName       ,  ( DSI_ProcessingStage p, const char * name )                             ,   RETURN( ( DSI_Channel )NULL, ( DSI_Channel )STAGE->GetChannelByName( INPUTSTRING( name ), true ) )             )
DSI_API_FUNC(  void                ,  DSI_ProcessingStage_ClearChannels          ,  ( DSI_ProcessingStage p )                                                ,   DO( STAGE->ClearChannels() )                                                                                   )
DSI_API_FUNC(  DSI_Channel         ,  DSI_ProcessingStage_AddChannel             ,  ( DSI_ProcessingStage p, const char * name, size_t bufferSamples )       ,   RETURN( ( DSI_Channel )NULL, ( DSI_Channel )STAGE->AddChannel( INPUTSTRING( name ), bufferSamples ) )          )
DSI_API_FUNC(  DSI_ProcessingStage ,  DSI_ProcessingStage_GetInput               ,  ( DSI_ProcessingStage p )                                                ,   RETURN( ( DSI_ProcessingStage )NULL, ( DSI_ProcessingStage )STAGE->Input() )                                   )
DSI_API_FUNC(  const char *        ,  DSI_ProcessingStage_GetName                ,  ( DSI_ProcessingStage p )                                                ,   RETURNSTRING( STAGE->Name() )                                                                                  )
DSI_API_FUNC(  void *              ,  DSI_ProcessingStage_ParamData              ,  ( DSI_ProcessingStage p )                                                ,   RETURN( NULL, STAGE->ParamData() )                                                                             )
DSI_API_FUNC(  unsigned int        ,  DSI_ProcessingStage_GetNumberOfChannels    ,  ( DSI_ProcessingStage p )                                                ,   RETURN( 0, STAGE->NumberOfChannels() )                                                                         )
DSI_API_FUNC(  double              ,  DSI_ProcessingStage_Read                   ,  ( DSI_ProcessingStage p, unsigned int channel, size_t lookbackSteps )    ,   RETURN( 0.0, STAGE->Read( channel, lookbackSteps ) )                                                           )
DSI_API_FUNC(  void                ,  DSI_ProcessingStage_Write                  ,  ( DSI_ProcessingStage p, unsigned int channel, double value )            ,   DO( STAGE->Write( channel, value ) )                                                                           )


/* **************************************************** */
/* DSI_Channel methods                                  */
/* **************************************************** */

DSI_API_FUNC(  double         ,    DSI_Channel_LookBack                          ,  ( DSI_Channel c, size_t nSteps )                                         ,   RETURN( 0.0, CHANNEL->LookBack( nSteps, false ) )                                                              )
DSI_API_FUNC(  double         ,    DSI_Channel_ReadBuffered                      ,  ( DSI_Channel c )                                                        ,   RETURN( 0.0, CHANNEL->ReadBuffered() )                                                                         )
DSI_API_FUNC(  size_t         ,    DSI_Channel_GetNumberOfBufferedSamples        ,  ( DSI_Channel c )                                                        ,   RETURN( 0, CHANNEL->Buffered() )                                                                               )
DSI_API_FUNC(  size_t         ,    DSI_Channel_GetNumberOfOverflowedSamples      ,  ( DSI_Channel c )                                                        ,   RETURN( 0, CHANNEL->BufferOverflow() )                                                                         )
DSI_API_FUNC(  size_t         ,    DSI_Channel_GetBufferCapacity                 ,  ( DSI_Channel c )                                                        ,   RETURN( 0, CHANNEL->BufferSize() )                                                                             )
DSI_API_FUNC(  void           ,    DSI_Channel_FlushOutputBuffer                 ,  ( DSI_Channel c )                                                        ,   DO( CHANNEL->FlushOutputBuffer() )                                                                             )
DSI_API_FUNC(  void           ,    DSI_Channel_SetName                           ,  ( DSI_Channel c, const char * name )                                     ,   DO( CHANNEL->SetName( INPUTSTRING( name ) ) )                                                                  )
DSI_API_FUNC(  const char *   ,    DSI_Channel_GetName                           ,  ( DSI_Channel c )                                                        ,   RETURNSTRING( CHANNEL->Name() )                                                                                )
DSI_API_FUNC(  const char *   ,    DSI_Channel_GetString                         ,  ( DSI_Channel c )                                                        ,   RETURNSTRING( CHANNEL->String( true ) )                                                                        )
DSI_API_FUNC(  double         ,    DSI_Channel_GetSignal                         ,  ( DSI_Channel c )                                                        ,   RETURN( 0.0, CHANNEL->Signal() )                                                                               )
DSI_API_FUNC(  double         ,    DSI_Channel_CalculateRawSignal                ,  ( DSI_Channel c )                                                        ,   RETURN( 0.0, CHANNEL->CalculateRawSignal() )                                                                   )
DSI_API_FUNC(  bool_t         ,    DSI_Channel_IsReferentialEEG                  ,  ( DSI_Channel c )                                                        ,   RETURN( 0, CHANNEL->IsReferentialEEG() )                                                                       )
DSI_API_FUNC(  bool_t         ,    DSI_Channel_IsTrigger                         ,  ( DSI_Channel c )                                                        ,   RETURN( 0, CHANNEL->IsTrigger() )                                                                              )


/* **************************************************** */
/* DSI_Source methods                                   */
/* **************************************************** */

DSI_API_FUNC(  bool_t         ,    DSI_Source_IsTrigger                          ,  ( DSI_Source s )                                                         ,   RETURN( 0, SOURCE->IsTrigger() )                                                                               )
DSI_API_FUNC(  bool_t         ,    DSI_Source_IsSensor                           ,  ( DSI_Source s )                                                         ,   RETURN( 0, SOURCE->IsSensor() )                                                                                )
DSI_API_FUNC(  bool_t         ,    DSI_Source_IsReferentialEEG                   ,  ( DSI_Source s )                                                         ,   RETURN( 0, SOURCE->IsReferentialEEG() )                                                                        )
DSI_API_FUNC(  bool_t         ,    DSI_Source_IsFactoryReference                 ,  ( DSI_Source s )                                                         ,   RETURN( 0, SOURCE->IsFactoryReference() )                                                                      )
DSI_API_FUNC(  bool_t         ,    DSI_Source_IsCommonModeSignal                 ,  ( DSI_Source s )                                                         ,   RETURN( 0, SOURCE->IsCommonModeSignal() )                                                                      )
DSI_API_FUNC(  void           ,    DSI_Source_SetName                            ,  ( DSI_Source s, const char * name )                                      ,   DO( SOURCE->SetName( INPUTSTRING( name ) ) )                                                                   )
DSI_API_FUNC(  const char *   ,    DSI_Source_GetName                            ,  ( DSI_Source s )                                                         ,   RETURNSTRING( SOURCE->Name() )                                                                                 )
DSI_API_FUNC(  double         ,    DSI_Source_GetSignal                          ,  ( DSI_Source s )                                                         ,   RETURN( 0.0, SOURCE->Signal() )                                                                                )
DSI_API_FUNC(  double         ,    DSI_Source_GetGain                            ,  ( DSI_Source s )                                                         ,   RETURN( 0.0, SOURCE->Gain() )                                                                                  )
DSI_API_FUNC(  double         ,    DSI_Source_GetImpedanceEEG                    ,  ( DSI_Source s )                                                         ,   RETURN( 0.0, SOURCE->ImpedanceEEG() )                                                                          )
DSI_API_FUNC(  double         ,    DSI_Source_GetImpedanceCMF                    ,  ( DSI_Source s )  /* deprecated: use DSI_Headset_GetImpedanceCMF */      ,   RETURN( 0.0, SOURCE->ImpedanceCMF() )                                                                          )
DSI_API_FUNC(  double         ,    DSI_Source_GetDCOffset                        ,  ( DSI_Source s )                                                         ,   RETURN( 0.0, SOURCE->DCOffset() )                                                                              )




#ifdef EXTERNC
} /* ends extern "C" block */
#endif /* #ifdef EXTERNC */
#undef DSI_API_FUNC
