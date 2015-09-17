/************************************************************************
*                                                                       *
*   NuiImageCamera.h -- This module defines the APIs for the Natural    *
*                       User Interface(NUI) image and camera services.  *
*                                                                       *
*   Copyright (c) Microsoft Corp. All rights reserved.                  *
*                                                                       *
************************************************************************/
#pragma once

#ifndef NUIAPI
#error "You must include nuiapi.h rather than including nuiimagecamera.h directly"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __NuiSensor_h__
typedef enum _NUI_IMAGE_TYPE
{
  NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX = 0,            // USHORT
  NUI_IMAGE_TYPE_COLOR,                                 // RGB32 data
  NUI_IMAGE_TYPE_COLOR_YUV,                             // YUY2 stream from camera h/w, but converted to RGB32 before user getting it.
  NUI_IMAGE_TYPE_COLOR_RAW_YUV,                         // YUY2 stream from camera h/w.
  NUI_IMAGE_TYPE_DEPTH,                                 // USHORT
} NUI_IMAGE_TYPE;

typedef enum _NUI_IMAGE_RESOLUTION
{
  NUI_IMAGE_RESOLUTION_INVALID = -1,
  NUI_IMAGE_RESOLUTION_80x60 = 0,
  NUI_IMAGE_RESOLUTION_320x240,
  NUI_IMAGE_RESOLUTION_640x480,
  NUI_IMAGE_RESOLUTION_1280x960                         // for hires color only
} NUI_IMAGE_RESOLUTION;
#endif // __NuiSensor_h__

inline void NuiImageResolutionToSize( _In_ NUI_IMAGE_RESOLUTION res, _Out_ DWORD & refWidth, _Out_ DWORD & refHeight )
{
    switch( res )
    {
    case NUI_IMAGE_RESOLUTION_80x60:
        refWidth = 80;
        refHeight = 60;
        break;
    case NUI_IMAGE_RESOLUTION_320x240:
        refWidth = 320;
        refHeight = 240;
        break;
    case NUI_IMAGE_RESOLUTION_640x480:
        refWidth = 640;
        refHeight = 480;
        break;
    case NUI_IMAGE_RESOLUTION_1280x960 :
        refWidth = 1280;
        refHeight = 960;
        break;
    default:
        refWidth = 0;
        refHeight = 0;
        break;
    }
}

#define NUI_IMAGE_PLAYER_INDEX_SHIFT          3
#define NUI_IMAGE_PLAYER_INDEX_MASK           ((1 << NUI_IMAGE_PLAYER_INDEX_SHIFT)-1)
#define NUI_IMAGE_DEPTH_MAXIMUM               ((4000 << NUI_IMAGE_PLAYER_INDEX_SHIFT) | NUI_IMAGE_PLAYER_INDEX_MASK)
#define NUI_IMAGE_DEPTH_MINIMUM               (800 << NUI_IMAGE_PLAYER_INDEX_SHIFT)
#define NUI_IMAGE_DEPTH_MAXIMUM_NEAR_MODE     ((3000 << NUI_IMAGE_PLAYER_INDEX_SHIFT) | NUI_IMAGE_PLAYER_INDEX_MASK)
#define NUI_IMAGE_DEPTH_MINIMUM_NEAR_MODE     (400 << NUI_IMAGE_PLAYER_INDEX_SHIFT)
#define NUI_IMAGE_DEPTH_NO_VALUE              0
#define NUI_IMAGE_DEPTH_TOO_FAR_VALUE         (0x0fff << NUI_IMAGE_PLAYER_INDEX_SHIFT)
#define NUI_DEPTH_DEPTH_UNKNOWN_VALUE         (0x1fff << NUI_IMAGE_PLAYER_INDEX_SHIFT)

#define NUI_CAMERA_DEPTH_NOMINAL_FOCAL_LENGTH_IN_PIXELS         (285.63f)   // Based on 320x240 pixel size.
#define NUI_CAMERA_DEPTH_NOMINAL_INVERSE_FOCAL_LENGTH_IN_PIXELS (3.501e-3f) // (1/NUI_CAMERA_DEPTH_NOMINAL_FOCAL_LENGTH_IN_PIXELS)
#define NUI_CAMERA_DEPTH_NOMINAL_DIAGONAL_FOV                   (70.0f)
#define NUI_CAMERA_DEPTH_NOMINAL_HORIZONTAL_FOV                 (58.5f)
#define NUI_CAMERA_DEPTH_NOMINAL_VERTICAL_FOV                   (45.6f)

#define NUI_CAMERA_COLOR_NOMINAL_FOCAL_LENGTH_IN_PIXELS         (531.15f)   // Based on 640x480 pixel size.
#define NUI_CAMERA_COLOR_NOMINAL_INVERSE_FOCAL_LENGTH_IN_PIXELS (1.83e-3f)  // (1/NUI_CAMERA_COLOR_NOMINAL_FOCAL_LENGTH_IN_PIXELS)
#define NUI_CAMERA_COLOR_NOMINAL_DIAGONAL_FOV                   ( 73.9f)
#define NUI_CAMERA_COLOR_NOMINAL_HORIZONTAL_FOV                 ( 62.0f)
#define NUI_CAMERA_COLOR_NOMINAL_VERTICAL_FOV                   ( 48.6f)


//
// Unpacks the depth value from the packed pixel format
//
inline USHORT NuiDepthPixelToDepth(USHORT packedPixel)
{
    return packedPixel >> NUI_IMAGE_PLAYER_INDEX_SHIFT;
}

//
// Unpacks the player index value from the packed pixel format
//
inline USHORT NuiDepthPixelToPlayerIndex(USHORT packedPixel)
{
    return packedPixel & NUI_IMAGE_PLAYER_INDEX_MASK;
}

typedef enum _NUI_IMAGE_DIGITALZOOM
{
    NUI_IMAGE_DIGITAL_ZOOM_1X = 0,
} NUI_IMAGE_DIGITALZOOM;


#ifndef __NuiSensor_h__
typedef struct _NUI_IMAGE_VIEW_AREA
{
    NUI_IMAGE_DIGITALZOOM  eDigitalZoom;
    LONG                   lCenterX;
    LONG                   lCenterY;
} NUI_IMAGE_VIEW_AREA;
#endif __NuiSensor_h__

#define NUI_IMAGE_FRAME_FLAG_NONE              0x00000000
#define NUI_IMAGE_FRAME_FLAG_VIEW_AREA_UNKNOWN 0x00000001

#ifndef __NuiSensor_h__
// Note:  NOT identical to D3DLOCKED_RECT
typedef struct _NUI_LOCKED_RECT
{
    INT                 Pitch;
    int                 size;   // Size of pBits, in bytes.
    BYTE*               pBits;
} NUI_LOCKED_RECT;

// Note:  NOT identical to D3DSURFACE_DESC
typedef struct _NUI_SURFACE_DESC
{
    UINT                Width;
    UINT                Height;
} NUI_SURFACE_DESC;

struct INuiFrameTexture;

typedef struct _NUI_IMAGE_FRAME
{
  LARGE_INTEGER             liTimeStamp;
  DWORD                     dwFrameNumber;
  NUI_IMAGE_TYPE            eImageType;
  NUI_IMAGE_RESOLUTION      eResolution;
  INuiFrameTexture          *pFrameTexture;
  DWORD                     dwFrameFlags;  
  NUI_IMAGE_VIEW_AREA       ViewArea;
} NUI_IMAGE_FRAME;
#endif __NuiSensor_h__

// return S_FALSE instead of E_NUI_FRAME_NO_DATA if NuiImageStreamGetNextFrame( ) doesn't have a frame ready and a timeout != INFINITE is used
#define NUI_IMAGE_STREAM_FLAG_SUPPRESS_NO_FRAME_DATA              0x00010000
// Set the depth stream to near mode
#define NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE                    0x00020000
// Use distinct values for depth values that are either too close, too far or unknown
#define NUI_IMAGE_STREAM_FLAG_DISTINCT_OVERFLOW_DEPTH_VALUES      0x00040000

// the max # of NUI output frames you can hold w/o releasing
#define NUI_IMAGE_STREAM_FRAME_LIMIT_MAXIMUM 4

_Check_return_ HRESULT NUIAPI NuiImageStreamSetImageFrameFlags(
    _In_  HANDLE hStream,
    _In_  DWORD dwImageFrameFlags
    );

_Check_return_ HRESULT NUIAPI NuiImageStreamGetImageFrameFlags(
    _In_  HANDLE hStream,
    _Out_  DWORD *pdwImageFrameFlags
    );

_Check_return_ HRESULT NUIAPI NuiSetFrameEndEvent(
    _In_ HANDLE hEvent,
    _In_ DWORD dwFrameEventFlag
    );

_Check_return_ HRESULT NUIAPI NuiImageStreamOpen(
    _In_ NUI_IMAGE_TYPE eImageType,
    _In_ NUI_IMAGE_RESOLUTION eResolution,
    _In_ DWORD dwImageFrameFlags,
    _In_ DWORD dwFrameLimit,
    _In_opt_ HANDLE hNextFrameEvent,
    _Out_ HANDLE *phStreamHandle
    );

_Check_return_ HRESULT NUIAPI NuiImageStreamGetNextFrame(
    _In_  HANDLE hStream,
    _In_  DWORD dwMillisecondsToWait,
    _Deref_out_ CONST NUI_IMAGE_FRAME **ppcImageFrame
    );

_Check_return_ HRESULT NUIAPI NuiImageStreamReleaseFrame(
    _In_ HANDLE hStream,
    _In_ CONST NUI_IMAGE_FRAME *pImageFrame
    );

_Check_return_ HRESULT NUIAPI NuiImageGetColorPixelCoordinatesFromDepthPixel(
    _In_ NUI_IMAGE_RESOLUTION eColorResolution,
    _In_opt_ CONST NUI_IMAGE_VIEW_AREA *pcViewArea,
    _In_ LONG   lDepthX,
    _In_ LONG   lDepthY,
    _In_ USHORT usDepthValue,
    _Out_ LONG *plColorX,
    _Out_ LONG *plColorY
    );

_Check_return_ HRESULT NUIAPI NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(
    _In_ NUI_IMAGE_RESOLUTION eColorResolution,
    _In_ NUI_IMAGE_RESOLUTION eDepthResolution,
    _In_opt_ CONST NUI_IMAGE_VIEW_AREA *pcViewArea,
    _In_ LONG   lDepthX,
    _In_ LONG   lDepthY,
    _In_ USHORT usDepthValue,
    _Out_ LONG *plColorX,
    _Out_ LONG *plColorY
    );    

#define NUI_CAMERA_ELEVATION_MAXIMUM  27
#define NUI_CAMERA_ELEVATION_MINIMUM (-27)

_Check_return_ HRESULT NUIAPI NuiCameraElevationGetAngle(
    _Out_ LONG * plAngleDegrees
    );

_Check_return_ HRESULT NUIAPI NuiCameraElevationSetAngle(
    _In_ LONG lAngleDegrees
    );

#ifdef __cplusplus
} //close extern "C"
#endif

