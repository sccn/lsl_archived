/************************************************************************
*                                                                       *
*   NuiApi.h -- This module aggregates all the Natural User             *
*               Interface(NUI) API headers.                             *
*                                                                       *
*   Copyright (c) Microsoft Corp. All rights reserved.                  *
*                                                                       *
************************************************************************/

#pragma once

#ifndef NUIAPI
    #define NUIAPI __declspec( dllimport ) WINAPI
#endif

#include <pshpack8.h>

#ifdef __cplusplus
extern "C" {
#endif

//
// NUI Common Initialization Declarations
//

#define NUI_INITIALIZE_FLAG_USES_AUDIO                  0x10000000
#define NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX 0x00000001
#define NUI_INITIALIZE_FLAG_USES_COLOR                  0x00000002
#define NUI_INITIALIZE_FLAG_USES_SKELETON               0x00000008  
#define NUI_INITIALIZE_FLAG_USES_DEPTH                  0x00000020

#define NUI_INITIALIZE_DEFAULT_HARDWARE_THREAD          0xFFFFFFFF


HRESULT NUIAPI NuiInitialize(
    _In_ DWORD dwFlags
    );


VOID NUIAPI NuiShutdown(
    );

//
// Define NUI specific error codes
// **** ALSO DEFINED IN NuiError.h.  Keep in sync! ****
//

#ifndef _NUI_HRESULTS
#define _NUI_HRESULTS

//
// Define NUI error codes derived from win32 errors
//

#define E_NUI_DEVICE_NOT_CONNECTED  __HRESULT_FROM_WIN32(ERROR_DEVICE_NOT_CONNECTED)
#define E_NUI_DEVICE_NOT_READY      __HRESULT_FROM_WIN32(ERROR_NOT_READY)
#define E_NUI_ALREADY_INITIALIZED   __HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED)
#define E_NUI_NO_MORE_ITEMS         __HRESULT_FROM_WIN32(ERROR_NO_MORE_ITEMS)


#define FACILITY_NUI 0x301
#define S_NUI_INITIALIZING                      MAKE_HRESULT(SEVERITY_SUCCESS, FACILITY_NUI, 1)                                             // 0x03010001
#define E_NUI_FRAME_NO_DATA                     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NUI, 1)
#define E_NUI_STREAM_NOT_ENABLED                MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NUI, 2)
#define E_NUI_IMAGE_STREAM_IN_USE               MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NUI, 3)
#define E_NUI_FRAME_LIMIT_EXCEEDED              MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NUI, 4)
#define E_NUI_FEATURE_NOT_INITIALIZED           MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NUI, 5)
#define E_NUI_NOTGENUINE                        MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NUI, 6)
#define E_NUI_INSUFFICIENTBANDWIDTH             MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NUI, 7)
#define E_NUI_NOTSUPPORTED                      MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NUI, 8)
#define E_NUI_DEVICE_IN_USE                     MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NUI, 9)

#define E_NUI_DATABASE_NOT_FOUND                MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NUI, 13)
#define E_NUI_DATABASE_VERSION_MISMATCH         MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NUI, 14)
// The requested feateure is not available on this version of the hardware
#define E_NUI_HARDWARE_FEATURE_UNAVAILABLE      MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NUI, 15)                                              // 0x8301000F
// The hub is no longer connected to the machine
#define E_NUI_NOTCONNECTED                      MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NUI, /* 20 */ ERROR_BAD_UNIT)                         // 0x83010014
// Some part of the device is not connected.
#define E_NUI_NOTREADY                          MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NUI, /* 21 */ ERROR_NOT_READY)                        // 0x83010015
// Skeletal engine is already in use
#define E_NUI_SKELETAL_ENGINE_BUSY              MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NUI, /* 170 */ ERROR_BUSY)
// The hub and motor are connected, but the camera is not
#define E_NUI_NOTPOWERED                        MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NUI, /* 639 */ ERROR_INSUFFICIENT_POWER)               // 0x8301027F
// Bad index passed in to NuiCreateInstanceByXXX
#define E_NUI_BADIINDEX                         MAKE_HRESULT(SEVERITY_ERROR, FACILITY_NUI, /* 1413 */ ERROR_INVALID_INDEX)                   // 0x83010585
#endif

// NOTE: Error codes in the range 100-199 associated with FACILITY_NUI are
// assigned to the NUI Handles API subsystem of NUI. See NuiHandles.h for
// details on those error codes.


#ifdef __cplusplus
} // end extern "C"
#endif

#ifndef __No__NuiSensor_h__
#include <NuiSensor.h>
#endif
#include <NuiImageCamera.h>
#include <NuiSkeleton.h>

#include <poppack.h>

