/************************************************************************
*                                                                       *
*   NuiSkeleton.h -- This module defines the APIs for the Natural       *
*                    User Interface(NUI) skeleton services.             *
*                                                                       *
*   Copyright (c) Microsoft Corp. All rights reserved.                  *
*                                                                       *
************************************************************************/
#pragma once

#ifndef NUIAPI
#error "You must include nuiapi.h rather than including nuiskeleton.h directly"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef FLT_EPSILON
  #define FLT_EPSILON     1.192092896e-07F        /* smallest such that 1.0+FLT_EPSILON != 1.0 */
#endif

#ifndef __NuiSensor_h__

typedef struct _Vector4
{
    FLOAT x;
    FLOAT y;
    FLOAT z;
    FLOAT w;
} Vector4;

#ifdef __XNAMATH_H__

inline XMVECTOR XMVectorFromVector4(const Vector4& vec4)
{
    return XMVectorSet(vec4.x, vec4.y, vec4.z, vec4.w);
}

inline Vector4 XMVectorToVector4(FXMVECTOR V)
{
    Vector4 vec4 = { XMVectorGetX(V), XMVectorGetY(V), XMVectorGetZ(V), XMVectorGetW(V) };
    return vec4;
}

#endif

typedef enum _NUI_SKELETON_POSITION_INDEX
{
    NUI_SKELETON_POSITION_HIP_CENTER = 0,
    NUI_SKELETON_POSITION_SPINE,
    NUI_SKELETON_POSITION_SHOULDER_CENTER,
    NUI_SKELETON_POSITION_HEAD,
    NUI_SKELETON_POSITION_SHOULDER_LEFT,
    NUI_SKELETON_POSITION_ELBOW_LEFT,
    NUI_SKELETON_POSITION_WRIST_LEFT,
    NUI_SKELETON_POSITION_HAND_LEFT,
    NUI_SKELETON_POSITION_SHOULDER_RIGHT,
    NUI_SKELETON_POSITION_ELBOW_RIGHT,
    NUI_SKELETON_POSITION_WRIST_RIGHT,
    NUI_SKELETON_POSITION_HAND_RIGHT,
    NUI_SKELETON_POSITION_HIP_LEFT,
    NUI_SKELETON_POSITION_KNEE_LEFT,
    NUI_SKELETON_POSITION_ANKLE_LEFT,
    NUI_SKELETON_POSITION_FOOT_LEFT,
    NUI_SKELETON_POSITION_HIP_RIGHT,
    NUI_SKELETON_POSITION_KNEE_RIGHT,
    NUI_SKELETON_POSITION_ANKLE_RIGHT,
    NUI_SKELETON_POSITION_FOOT_RIGHT,
    NUI_SKELETON_POSITION_COUNT
} NUI_SKELETON_POSITION_INDEX;

//
//  Number of NUI_SKELETON_DATA elements in NUI_SKELETON_FRAME
//

#define NUI_SKELETON_COUNT 6

#endif // __NuiSensor_h__

//
//  Number of NUI_SKELETON_DATA elements that can be in the NUI_SKELETON_TRACKED state
//

#define NUI_SKELETON_MAX_TRACKED_COUNT 2

//
//  Tracking IDs start at 1
//

#define NUI_SKELETON_INVALID_TRACKING_ID 0

#ifndef __NuiSensor_h__
typedef enum _NUI_SKELETON_POSITION_TRACKING_STATE
{
    NUI_SKELETON_POSITION_NOT_TRACKED = 0,
    NUI_SKELETON_POSITION_INFERRED,
    NUI_SKELETON_POSITION_TRACKED
} NUI_SKELETON_POSITION_TRACKING_STATE;

typedef enum _NUI_SKELETON_TRACKING_STATE
{
    NUI_SKELETON_NOT_TRACKED = 0,
    NUI_SKELETON_POSITION_ONLY,
    NUI_SKELETON_TRACKED
} NUI_SKELETON_TRACKING_STATE;

typedef struct _NUI_SKELETON_DATA
{
  NUI_SKELETON_TRACKING_STATE eTrackingState;
  DWORD dwTrackingID;
  DWORD dwEnrollmentIndex;
  DWORD dwUserIndex;
  Vector4 Position;
  Vector4 SkeletonPositions[NUI_SKELETON_POSITION_COUNT];
  NUI_SKELETON_POSITION_TRACKING_STATE eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_COUNT];
  DWORD dwQualityFlags;
} NUI_SKELETON_DATA;
#endif // __NuiSensor_h__


//
// Force a link error if the size of NUI_SKELETON_DATA is different
// between the static library and the header included by the title.
//

__declspec(selectany) DWORD NuiSkeletonDataSize = sizeof(NUI_SKELETON_DATA);


#define NUI_SKELETON_QUALITY_CLIPPED_RIGHT  0x00000001
#define NUI_SKELETON_QUALITY_CLIPPED_LEFT   0x00000002
#define NUI_SKELETON_QUALITY_CLIPPED_TOP    0x00000004
#define NUI_SKELETON_QUALITY_CLIPPED_BOTTOM 0x00000008

#ifndef __NuiSensor_h__
#pragma pack(push, 16)
typedef struct _NUI_SKELETON_FRAME
{
  LARGE_INTEGER         liTimeStamp;
  DWORD                 dwFrameNumber;
  DWORD                 dwFlags;
  Vector4              vFloorClipPlane;
  Vector4              vNormalToGravity;
  NUI_SKELETON_DATA     SkeletonData[NUI_SKELETON_COUNT];
} NUI_SKELETON_FRAME;
#pragma pack(pop)

typedef struct _NUI_TRANSFORM_SMOOTH_PARAMETERS
{
    FLOAT   fSmoothing;             // [0..1], lower values closer to raw data
    FLOAT   fCorrection;            // [0..1], lower values slower to correct towards the raw data
    FLOAT   fPrediction;            // [0..n], the number of frames to predict into the future
    FLOAT   fJitterRadius;          // The radius in meters for jitter reduction
    FLOAT   fMaxDeviationRadius;    // The maximum radius in meters that filtered positions are allowed to deviate from raw data
} NUI_TRANSFORM_SMOOTH_PARAMETERS;
#endif // __NuiSensor_h__

#define NUI_SKELETON_TRACKING_FLAG_SUPPRESS_NO_FRAME_DATA       0x00000001
#define NUI_SKELETON_TRACKING_FLAG_TITLE_SETS_TRACKED_SKELETONS 0x00000002

_Check_return_ HRESULT NUIAPI NuiSkeletonTrackingEnable(
    _In_opt_ HANDLE hNextFrameEvent,
    _In_     DWORD  dwFlags
    );

_Check_return_ HRESULT NUIAPI NuiSkeletonTrackingDisable(
    );

_Check_return_ HRESULT NUIAPI NuiSkeletonGetNextFrame(
    _In_  DWORD dwMillisecondsToWait,
    _Inout_ NUI_SKELETON_FRAME *pSkeletonFrame
    );

_Check_return_ HRESULT NUIAPI NuiSkeletonSetTrackedSkeletons(
    _In_count_(NUI_SKELETON_MAX_TRACKED_COUNT) DWORD TrackingIDs[NUI_SKELETON_MAX_TRACKED_COUNT]
    );

#ifdef __cplusplus
} //close extern "C"

// Assuming a pixel resolution of 320x240
// x_meters = (x_pixelcoord - 160) * NUI_CAMERA_DEPTH_IMAGE_TO_SKELETON_MULTIPLIER_320x240 * z_meters;
// y_meters = (y_pixelcoord - 120) * NUI_CAMERA_DEPTH_IMAGE_TO_SKELETON_MULTIPLIER_320x240 * z_meters;
#define NUI_CAMERA_DEPTH_IMAGE_TO_SKELETON_MULTIPLIER_320x240 (NUI_CAMERA_DEPTH_NOMINAL_INVERSE_FOCAL_LENGTH_IN_PIXELS)
 
// Assuming a pixel resolution of 320x240
// x_pixelcoord = (x_meters) * NUI_CAMERA_SKELETON_TO_DEPTH_IMAGE_MULTIPLIER_320x240 / z_meters + 160;
// y_pixelcoord = (y_meters) * NUI_CAMERA_SKELETON_TO_DEPTH_IMAGE_MULTIPLIER_320x240 / z_meters + 120;
#define NUI_CAMERA_SKELETON_TO_DEPTH_IMAGE_MULTIPLIER_320x240 (NUI_CAMERA_DEPTH_NOMINAL_FOCAL_LENGTH_IN_PIXELS)

inline
VOID
NuiTransformSkeletonToDepthImage(
    _In_ Vector4 vPoint,
    _Out_ LONG *plDepthX,
    _Out_ LONG *plDepthY,
    _Out_ USHORT *pusDepthValue,
    _In_ NUI_IMAGE_RESOLUTION eResolution
    )
{
    if((plDepthX == NULL) || (plDepthY == NULL) || (pusDepthValue == NULL))
    {
        return;
    }
    
    //
    // Requires a valid depth value.
    //
    
    if(vPoint.z > FLT_EPSILON)
    {
        DWORD width;
        DWORD height;
        NuiImageResolutionToSize( eResolution, width, height );

        //
        // Center of depth sensor is at (0,0,0) in skeleton space, and
        // and (width/2,height/2) in depth image coordinates.  Note that positive Y
        // is up in skeleton space and down in image coordinates.
        //
        // The 0.5f is to correct for casting to int truncating, not rounding

        *plDepthX = static_cast<INT>( width / 2 + vPoint.x * (width/320.f) * NUI_CAMERA_SKELETON_TO_DEPTH_IMAGE_MULTIPLIER_320x240 / vPoint.z + 0.5f);
        *plDepthY = static_cast<INT>( height / 2 - vPoint.y * (height/240.f) * NUI_CAMERA_SKELETON_TO_DEPTH_IMAGE_MULTIPLIER_320x240 / vPoint.z + 0.5f);
        
        //
        //  Depth is in meters in skeleton space.
        //  The depth image pixel format has depth in millimeters shifted left by 3.
        //
        
        *pusDepthValue = static_cast<USHORT>(vPoint.z *1000) << 3;
    } else
    {
        *plDepthX = 0;
        *plDepthY = 0;
        *pusDepthValue = 0;
    }
}

inline
VOID
NuiTransformSkeletonToDepthImage(
    _In_ Vector4 vPoint,
    _Out_ LONG *plDepthX,
    _Out_ LONG *plDepthY,
    _Out_ USHORT *pusDepthValue
    )
{
    NuiTransformSkeletonToDepthImage( vPoint, plDepthX, plDepthY, pusDepthValue, NUI_IMAGE_RESOLUTION_320x240);
}


inline
VOID
NuiTransformSkeletonToDepthImage(
    _In_ Vector4 vPoint,
    _Out_ FLOAT *pfDepthX,
    _Out_ FLOAT *pfDepthY,
    _In_ NUI_IMAGE_RESOLUTION eResolution
    )
{
    if((pfDepthX == NULL) || (pfDepthY == NULL))
    {
        return;
    }
    
    //
    // Requires a valid depth value.
    //
    
    if(vPoint.z > FLT_EPSILON)
    {
        DWORD width;
        DWORD height;
        NuiImageResolutionToSize( eResolution, width, height );

        //
        // Center of depth sensor is at (0,0,0) in skeleton space, and
        // and (width/2,height/2) in depth image coordinates.  Note that positive Y
        // is up in skeleton space and down in image coordinates.
        //
        
        *pfDepthX = width / 2 + vPoint.x * (width/320.f) * NUI_CAMERA_SKELETON_TO_DEPTH_IMAGE_MULTIPLIER_320x240 / vPoint.z;
        *pfDepthY = height / 2 - vPoint.y * (height/240.f) * NUI_CAMERA_SKELETON_TO_DEPTH_IMAGE_MULTIPLIER_320x240 / vPoint.z;
        
    } else
    {
        *pfDepthX = 0.0f;
        *pfDepthY = 0.0f;
    }
}

inline
VOID
NuiTransformSkeletonToDepthImage(
    _In_ Vector4 vPoint,
    _Out_ FLOAT *pfDepthX,
    _Out_ FLOAT *pfDepthY
    )
{
    NuiTransformSkeletonToDepthImage(vPoint, pfDepthX, pfDepthY, NUI_IMAGE_RESOLUTION_320x240);
}

inline
Vector4
NuiTransformDepthImageToSkeleton(
    _In_ LONG lDepthX,
    _In_ LONG lDepthY,
    _In_ USHORT usDepthValue,
    _In_ NUI_IMAGE_RESOLUTION eResolution
    )
{
    DWORD width;
    DWORD height;
    NuiImageResolutionToSize( eResolution, width, height );

    //
    //  Depth is in meters in skeleton space.
    //  The depth image pixel format has depth in millimeters shifted left by 3.
    //
    
    FLOAT fSkeletonZ = static_cast<FLOAT>(usDepthValue >> 3) / 1000.0f;

    //
    // Center of depth sensor is at (0,0,0) in skeleton space, and
    // and (width/2,height/2) in depth image coordinates.  Note that positive Y
    // is up in skeleton space and down in image coordinates.
    //
    
    FLOAT fSkeletonX = (lDepthX - width/2.0f) * (320.0f/width) * NUI_CAMERA_DEPTH_IMAGE_TO_SKELETON_MULTIPLIER_320x240 * fSkeletonZ;
    FLOAT fSkeletonY = -(lDepthY - height/2.0f) * (240.0f/height) * NUI_CAMERA_DEPTH_IMAGE_TO_SKELETON_MULTIPLIER_320x240 * fSkeletonZ;

    //
    // Return the result as a vector.
    //
        
    Vector4 v4;
    v4.x = fSkeletonX;
    v4.y = fSkeletonY;
    v4.z = fSkeletonZ;
    v4.w = 1.0f;
    return v4;
}

inline
Vector4
NuiTransformDepthImageToSkeleton(
    _In_ LONG lDepthX,
    _In_ LONG lDepthY,
    _In_ USHORT usDepthValue
    )
{
    return NuiTransformDepthImageToSkeleton(lDepthX, lDepthY, usDepthValue, NUI_IMAGE_RESOLUTION_320x240);
}

#endif

#ifdef __cplusplus
extern "C" {
#endif

_Check_return_ HRESULT NUIAPI NuiTransformSmooth(
         _Inout_ NUI_SKELETON_FRAME *pSkeletonFrame,
         _In_opt_ CONST NUI_TRANSFORM_SMOOTH_PARAMETERS *pSmoothingParams
    );

#ifdef __cplusplus
} //close extern "C"
#endif

