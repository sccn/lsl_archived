

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Mon Jan 23 15:10:14 2012
 */
/* Compiler settings for ..\inc_public\Kinect.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __NuiSensor_h__
#define __NuiSensor_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __INuiAudioBeam_FWD_DEFINED__
#define __INuiAudioBeam_FWD_DEFINED__
typedef interface INuiAudioBeam INuiAudioBeam;
#endif 	/* __INuiAudioBeam_FWD_DEFINED__ */


#ifndef __INuiFrameTexture_FWD_DEFINED__
#define __INuiFrameTexture_FWD_DEFINED__
typedef interface INuiFrameTexture INuiFrameTexture;
#endif 	/* __INuiFrameTexture_FWD_DEFINED__ */


#ifndef __INuiSensor_FWD_DEFINED__
#define __INuiSensor_FWD_DEFINED__
typedef interface INuiSensor INuiSensor;
#endif 	/* __INuiSensor_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_Kinect_0000_0000 */
/* [local] */ 

//----------------------------------------------------------------------------
// Copyright (c) Microsoft Corporation. All rights reserved.
//----------------------------------------------------------------------------

#ifndef NUIAPI
#error "You must include nuiapi.h rather than including nuisensor.h directly"
#endif

#ifndef DEFINE_UUID
#define DEFINE_UUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) extern "C"  const GUID __declspec(selectany) name = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
#endif




//1f5e088c-a8c7-41d3-9957-209677a13e85
DEFINE_UUID(IID_INuiSensor,0x1f5e088c,0xa8c7,0x41d3,0x99,0x57,0x20,0x96,0x77,0xa1,0x3e,0x85);

// 13ea17f5-ff2e-4670-9ee5-1297a6e880d1
DEFINE_UUID(IID_INuiFrameTexture,0x13ea17f5,0xff2e,0x4670,0x9e,0xe5,0x12,0x97,0xa6,0xe8,0x80,0xd1);

// {8c3cebfa-a35d-497e-bc9a-e9752a8155e0}
DEFINE_UUID(IID_INuiAudioBeam, 0x8c3cebfa, 0xa35d, 0x497e, 0xbc, 0x9a, 0xe9, 0x75, 0x2a, 0x81, 0x55, 0xe0);

#define	NUI_SKELETON_COUNT	( 6 )

typedef struct _Vector4
    {
    FLOAT x;
    FLOAT y;
    FLOAT z;
    FLOAT w;
    } 	Vector4;


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

#endif // __XNAMATH_H__

typedef 
enum _NUI_SKELETON_POSITION_INDEX
    {	NUI_SKELETON_POSITION_HIP_CENTER	= 0,
	NUI_SKELETON_POSITION_SPINE	= ( NUI_SKELETON_POSITION_HIP_CENTER + 1 ) ,
	NUI_SKELETON_POSITION_SHOULDER_CENTER	= ( NUI_SKELETON_POSITION_SPINE + 1 ) ,
	NUI_SKELETON_POSITION_HEAD	= ( NUI_SKELETON_POSITION_SHOULDER_CENTER + 1 ) ,
	NUI_SKELETON_POSITION_SHOULDER_LEFT	= ( NUI_SKELETON_POSITION_HEAD + 1 ) ,
	NUI_SKELETON_POSITION_ELBOW_LEFT	= ( NUI_SKELETON_POSITION_SHOULDER_LEFT + 1 ) ,
	NUI_SKELETON_POSITION_WRIST_LEFT	= ( NUI_SKELETON_POSITION_ELBOW_LEFT + 1 ) ,
	NUI_SKELETON_POSITION_HAND_LEFT	= ( NUI_SKELETON_POSITION_WRIST_LEFT + 1 ) ,
	NUI_SKELETON_POSITION_SHOULDER_RIGHT	= ( NUI_SKELETON_POSITION_HAND_LEFT + 1 ) ,
	NUI_SKELETON_POSITION_ELBOW_RIGHT	= ( NUI_SKELETON_POSITION_SHOULDER_RIGHT + 1 ) ,
	NUI_SKELETON_POSITION_WRIST_RIGHT	= ( NUI_SKELETON_POSITION_ELBOW_RIGHT + 1 ) ,
	NUI_SKELETON_POSITION_HAND_RIGHT	= ( NUI_SKELETON_POSITION_WRIST_RIGHT + 1 ) ,
	NUI_SKELETON_POSITION_HIP_LEFT	= ( NUI_SKELETON_POSITION_HAND_RIGHT + 1 ) ,
	NUI_SKELETON_POSITION_KNEE_LEFT	= ( NUI_SKELETON_POSITION_HIP_LEFT + 1 ) ,
	NUI_SKELETON_POSITION_ANKLE_LEFT	= ( NUI_SKELETON_POSITION_KNEE_LEFT + 1 ) ,
	NUI_SKELETON_POSITION_FOOT_LEFT	= ( NUI_SKELETON_POSITION_ANKLE_LEFT + 1 ) ,
	NUI_SKELETON_POSITION_HIP_RIGHT	= ( NUI_SKELETON_POSITION_FOOT_LEFT + 1 ) ,
	NUI_SKELETON_POSITION_KNEE_RIGHT	= ( NUI_SKELETON_POSITION_HIP_RIGHT + 1 ) ,
	NUI_SKELETON_POSITION_ANKLE_RIGHT	= ( NUI_SKELETON_POSITION_KNEE_RIGHT + 1 ) ,
	NUI_SKELETON_POSITION_FOOT_RIGHT	= ( NUI_SKELETON_POSITION_ANKLE_RIGHT + 1 ) ,
	NUI_SKELETON_POSITION_COUNT	= ( NUI_SKELETON_POSITION_FOOT_RIGHT + 1 ) 
    } 	NUI_SKELETON_POSITION_INDEX;

typedef 
enum _NUI_IMAGE_TYPE
    {	NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX	= 0,
	NUI_IMAGE_TYPE_COLOR	= ( NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX + 1 ) ,
	NUI_IMAGE_TYPE_COLOR_YUV	= ( NUI_IMAGE_TYPE_COLOR + 1 ) ,
	NUI_IMAGE_TYPE_COLOR_RAW_YUV	= ( NUI_IMAGE_TYPE_COLOR_YUV + 1 ) ,
	NUI_IMAGE_TYPE_DEPTH	= ( NUI_IMAGE_TYPE_COLOR_RAW_YUV + 1 ) 
    } 	NUI_IMAGE_TYPE;

typedef 
enum _NUI_IMAGE_RESOLUTION
    {	NUI_IMAGE_RESOLUTION_INVALID	= -1,
	NUI_IMAGE_RESOLUTION_80x60	= 0,
	NUI_IMAGE_RESOLUTION_320x240	= ( NUI_IMAGE_RESOLUTION_80x60 + 1 ) ,
	NUI_IMAGE_RESOLUTION_640x480	= ( NUI_IMAGE_RESOLUTION_320x240 + 1 ) ,
	NUI_IMAGE_RESOLUTION_1280x960	= ( NUI_IMAGE_RESOLUTION_640x480 + 1 ) 
    } 	NUI_IMAGE_RESOLUTION;

typedef struct _NUI_IMAGE_VIEW_AREA
    {
    int eDigitalZoom;
    LONG lCenterX;
    LONG lCenterY;
    } 	NUI_IMAGE_VIEW_AREA;

typedef struct _NUI_TRANSFORM_SMOOTH_PARAMETERS
    {
    FLOAT fSmoothing;
    FLOAT fCorrection;
    FLOAT fPrediction;
    FLOAT fJitterRadius;
    FLOAT fMaxDeviationRadius;
    } 	NUI_TRANSFORM_SMOOTH_PARAMETERS;

typedef struct _NUI_SURFACE_DESC
    {
    UINT Width;
    UINT Height;
    } 	NUI_SURFACE_DESC;

typedef 
enum _NUI_SKELETON_POSITION_TRACKING_STATE
    {	NUI_SKELETON_POSITION_NOT_TRACKED	= 0,
	NUI_SKELETON_POSITION_INFERRED	= ( NUI_SKELETON_POSITION_NOT_TRACKED + 1 ) ,
	NUI_SKELETON_POSITION_TRACKED	= ( NUI_SKELETON_POSITION_INFERRED + 1 ) 
    } 	NUI_SKELETON_POSITION_TRACKING_STATE;

typedef 
enum _NUI_SKELETON_TRACKING_STATE
    {	NUI_SKELETON_NOT_TRACKED	= 0,
	NUI_SKELETON_POSITION_ONLY	= ( NUI_SKELETON_NOT_TRACKED + 1 ) ,
	NUI_SKELETON_TRACKED	= ( NUI_SKELETON_POSITION_ONLY + 1 ) 
    } 	NUI_SKELETON_TRACKING_STATE;

typedef struct _NUI_SKELETON_DATA
    {
    NUI_SKELETON_TRACKING_STATE eTrackingState;
    DWORD dwTrackingID;
    DWORD dwEnrollmentIndex;
    DWORD dwUserIndex;
    Vector4 Position;
    Vector4 SkeletonPositions[ 20 ];
    NUI_SKELETON_POSITION_TRACKING_STATE eSkeletonPositionTrackingState[ 20 ];
    DWORD dwQualityFlags;
    } 	NUI_SKELETON_DATA;


#pragma pack(push, 16)
typedef struct _NUI_SKELETON_FRAME
    {
    LARGE_INTEGER liTimeStamp;
    DWORD dwFrameNumber;
    DWORD dwFlags;
    Vector4 vFloorClipPlane;
    Vector4 vNormalToGravity;
    NUI_SKELETON_DATA SkeletonData[ 6 ];
    } 	NUI_SKELETON_FRAME;


#pragma pack(pop)
#ifdef _MIDL_OUTPUT_
typedef struct _NUI_LOCKED_RECT
    {
    INT Pitch;
    INT size;
    void *pBits;
    } 	NUI_LOCKED_RECT;

#else
typedef struct _NUI_LOCKED_RECT
{
    INT                         Pitch;
    INT                         size;
    byte                        *pBits;
} NUI_LOCKED_RECT;
#endif _MIDL_OUTPUT_
#define MICARRAY_ADAPTIVE_BEAM 0x1100


extern RPC_IF_HANDLE __MIDL_itf_Kinect_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_Kinect_0000_0000_v0_0_s_ifspec;






/* [helpstring][version][uuid] */ 

typedef struct _NUI_IMAGE_FRAME
    {
    LARGE_INTEGER liTimeStamp;
    DWORD dwFrameNumber;
    NUI_IMAGE_TYPE eImageType;
    NUI_IMAGE_RESOLUTION eResolution;
    INuiFrameTexture *pFrameTexture;
    DWORD dwFrameFlags;
    NUI_IMAGE_VIEW_AREA ViewArea;
    } 	NUI_IMAGE_FRAME;




#ifndef __INuiAudioBeam_INTERFACE_DEFINED__
#define __INuiAudioBeam_INTERFACE_DEFINED__

/* interface INuiAudioBeam */
/* [object][uuid] */ 


EXTERN_C const IID IID_INuiAudioBeam;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("8C3CEBFA-A35D-497E-BC9A-E9752A8155E0")
    INuiAudioBeam : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetBeam( 
            /* [retval][out] */ double *angle) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetBeam( 
            /* [in] */ double angle) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPosition( 
            /* [out] */ double *angle,
            /* [out] */ double *confidence) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INuiAudioBeamVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INuiAudioBeam * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INuiAudioBeam * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INuiAudioBeam * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetBeam )( 
            INuiAudioBeam * This,
            /* [retval][out] */ double *angle);
        
        HRESULT ( STDMETHODCALLTYPE *SetBeam )( 
            INuiAudioBeam * This,
            /* [in] */ double angle);
        
        HRESULT ( STDMETHODCALLTYPE *GetPosition )( 
            INuiAudioBeam * This,
            /* [out] */ double *angle,
            /* [out] */ double *confidence);
        
        END_INTERFACE
    } INuiAudioBeamVtbl;

    interface INuiAudioBeam
    {
        CONST_VTBL struct INuiAudioBeamVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INuiAudioBeam_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INuiAudioBeam_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INuiAudioBeam_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INuiAudioBeam_GetBeam(This,angle)	\
    ( (This)->lpVtbl -> GetBeam(This,angle) ) 

#define INuiAudioBeam_SetBeam(This,angle)	\
    ( (This)->lpVtbl -> SetBeam(This,angle) ) 

#define INuiAudioBeam_GetPosition(This,angle,confidence)	\
    ( (This)->lpVtbl -> GetPosition(This,angle,confidence) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INuiAudioBeam_INTERFACE_DEFINED__ */


#ifndef __INuiFrameTexture_INTERFACE_DEFINED__
#define __INuiFrameTexture_INTERFACE_DEFINED__

/* interface INuiFrameTexture */
/* [object][uuid] */ 


EXTERN_C const IID IID_INuiFrameTexture;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("13ea17f5-ff2e-4670-9ee5-1297a6e880d1")
    INuiFrameTexture : public IUnknown
    {
    public:
        virtual int STDMETHODCALLTYPE BufferLen( void) = 0;
        
        virtual int STDMETHODCALLTYPE Pitch( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LockRect( 
            UINT Level,
            /* [ref] */ NUI_LOCKED_RECT *pLockedRect,
            /* [unique] */ RECT *pRect,
            DWORD Flags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLevelDesc( 
            UINT Level,
            NUI_SURFACE_DESC *pDesc) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UnlockRect( 
            /* [in] */ UINT Level) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INuiFrameTextureVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INuiFrameTexture * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INuiFrameTexture * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INuiFrameTexture * This);
        
        int ( STDMETHODCALLTYPE *BufferLen )( 
            INuiFrameTexture * This);
        
        int ( STDMETHODCALLTYPE *Pitch )( 
            INuiFrameTexture * This);
        
        HRESULT ( STDMETHODCALLTYPE *LockRect )( 
            INuiFrameTexture * This,
            UINT Level,
            /* [ref] */ NUI_LOCKED_RECT *pLockedRect,
            /* [unique] */ RECT *pRect,
            DWORD Flags);
        
        HRESULT ( STDMETHODCALLTYPE *GetLevelDesc )( 
            INuiFrameTexture * This,
            UINT Level,
            NUI_SURFACE_DESC *pDesc);
        
        HRESULT ( STDMETHODCALLTYPE *UnlockRect )( 
            INuiFrameTexture * This,
            /* [in] */ UINT Level);
        
        END_INTERFACE
    } INuiFrameTextureVtbl;

    interface INuiFrameTexture
    {
        CONST_VTBL struct INuiFrameTextureVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INuiFrameTexture_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INuiFrameTexture_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INuiFrameTexture_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INuiFrameTexture_BufferLen(This)	\
    ( (This)->lpVtbl -> BufferLen(This) ) 

#define INuiFrameTexture_Pitch(This)	\
    ( (This)->lpVtbl -> Pitch(This) ) 

#define INuiFrameTexture_LockRect(This,Level,pLockedRect,pRect,Flags)	\
    ( (This)->lpVtbl -> LockRect(This,Level,pLockedRect,pRect,Flags) ) 

#define INuiFrameTexture_GetLevelDesc(This,Level,pDesc)	\
    ( (This)->lpVtbl -> GetLevelDesc(This,Level,pDesc) ) 

#define INuiFrameTexture_UnlockRect(This,Level)	\
    ( (This)->lpVtbl -> UnlockRect(This,Level) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INuiFrameTexture_INTERFACE_DEFINED__ */


#ifndef __INuiSensor_INTERFACE_DEFINED__
#define __INuiSensor_INTERFACE_DEFINED__

/* interface INuiSensor */
/* [object][local][uuid] */ 


EXTERN_C const IID IID_INuiSensor;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1f5e088c-a8c7-41d3-9957-209677a13e85")
    INuiSensor : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE NuiInitialize( 
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual void STDMETHODCALLTYPE NuiShutdown( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NuiSetFrameEndEvent( 
            /* [in] */ HANDLE hEvent,
            /* [in] */ DWORD dwFrameEventFlag) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NuiImageStreamOpen( 
            /* [in] */ NUI_IMAGE_TYPE eImageType,
            /* [in] */ NUI_IMAGE_RESOLUTION eResolution,
            /* [in] */ DWORD dwImageFrameFlags,
            /* [in] */ DWORD dwFrameLimit,
            /* [in] */ HANDLE hNextFrameEvent,
            /* [out] */ HANDLE *phStreamHandle) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NuiImageStreamSetImageFrameFlags( 
            /* [in] */ HANDLE hStream,
            /* [in] */ DWORD dwImageFrameFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NuiImageStreamGetImageFrameFlags( 
            /* [in] */ HANDLE hStream,
            /* [retval][out] */ DWORD *pdwImageFrameFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NuiImageStreamGetNextFrame( 
            /* [in] */ HANDLE hStream,
            /* [in] */ DWORD dwMillisecondsToWait,
            /* [retval][out] */ NUI_IMAGE_FRAME *pImageFrame) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NuiImageStreamReleaseFrame( 
            /* [in] */ HANDLE hStream,
            /* [in] */ NUI_IMAGE_FRAME *pImageFrame) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NuiImageGetColorPixelCoordinatesFromDepthPixel( 
            /* [in] */ NUI_IMAGE_RESOLUTION eColorResolution,
            /* [in] */ const NUI_IMAGE_VIEW_AREA *pcViewArea,
            /* [in] */ LONG lDepthX,
            /* [in] */ LONG lDepthY,
            /* [in] */ USHORT usDepthValue,
            /* [out] */ LONG *plColorX,
            /* [out] */ LONG *plColorY) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution( 
            /* [in] */ NUI_IMAGE_RESOLUTION eColorResolution,
            /* [in] */ NUI_IMAGE_RESOLUTION eDepthResolution,
            /* [in] */ const NUI_IMAGE_VIEW_AREA *pcViewArea,
            /* [in] */ LONG lDepthX,
            /* [in] */ LONG lDepthY,
            /* [in] */ USHORT usDepthValue,
            /* [out] */ LONG *plColorX,
            /* [out] */ LONG *plColorY) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NuiImageGetColorPixelCoordinateFrameFromDepthPixelFrameAtResolution( 
            /* [in] */ NUI_IMAGE_RESOLUTION eColorResolution,
            /* [in] */ NUI_IMAGE_RESOLUTION eDepthResolution,
            /* [in] */ DWORD cDepthValues,
            /* [size_is][in] */ USHORT *pDepthValues,
            /* [in] */ DWORD cColorCoordinates,
            /* [size_is][out][in] */ LONG *pColorCoordinates) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NuiCameraElevationSetAngle( 
            /* [in] */ LONG lAngleDegrees) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NuiCameraElevationGetAngle( 
            /* [retval][out] */ LONG *plAngleDegrees) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NuiSkeletonTrackingEnable( 
            /* [in] */ HANDLE hNextFrameEvent,
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NuiSkeletonTrackingDisable( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NuiSkeletonSetTrackedSkeletons( 
            /* [size_is][in] */ DWORD *TrackingIDs) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NuiSkeletonGetNextFrame( 
            /* [in] */ DWORD dwMillisecondsToWait,
            /* [out][in] */ NUI_SKELETON_FRAME *pSkeletonFrame) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NuiTransformSmooth( 
            NUI_SKELETON_FRAME *pSkeletonFrame,
            const NUI_TRANSFORM_SMOOTH_PARAMETERS *pSmoothingParams) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE NuiGetAudioSource( 
            /* [out] */ INuiAudioBeam **ppDmo) = 0;
        
        virtual int STDMETHODCALLTYPE NuiInstanceIndex( void) = 0;
        
        virtual BSTR STDMETHODCALLTYPE NuiDeviceConnectionId( void) = 0;
        
        virtual BSTR STDMETHODCALLTYPE NuiUniqueId( void) = 0;
        
        virtual BSTR STDMETHODCALLTYPE NuiAudioArrayId( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NuiStatus( void) = 0;
        
        virtual DWORD STDMETHODCALLTYPE NuiInitializationFlags( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INuiSensorVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INuiSensor * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INuiSensor * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INuiSensor * This);
        
        HRESULT ( STDMETHODCALLTYPE *NuiInitialize )( 
            INuiSensor * This,
            /* [in] */ DWORD dwFlags);
        
        void ( STDMETHODCALLTYPE *NuiShutdown )( 
            INuiSensor * This);
        
        HRESULT ( STDMETHODCALLTYPE *NuiSetFrameEndEvent )( 
            INuiSensor * This,
            /* [in] */ HANDLE hEvent,
            /* [in] */ DWORD dwFrameEventFlag);
        
        HRESULT ( STDMETHODCALLTYPE *NuiImageStreamOpen )( 
            INuiSensor * This,
            /* [in] */ NUI_IMAGE_TYPE eImageType,
            /* [in] */ NUI_IMAGE_RESOLUTION eResolution,
            /* [in] */ DWORD dwImageFrameFlags,
            /* [in] */ DWORD dwFrameLimit,
            /* [in] */ HANDLE hNextFrameEvent,
            /* [out] */ HANDLE *phStreamHandle);
        
        HRESULT ( STDMETHODCALLTYPE *NuiImageStreamSetImageFrameFlags )( 
            INuiSensor * This,
            /* [in] */ HANDLE hStream,
            /* [in] */ DWORD dwImageFrameFlags);
        
        HRESULT ( STDMETHODCALLTYPE *NuiImageStreamGetImageFrameFlags )( 
            INuiSensor * This,
            /* [in] */ HANDLE hStream,
            /* [retval][out] */ DWORD *pdwImageFrameFlags);
        
        HRESULT ( STDMETHODCALLTYPE *NuiImageStreamGetNextFrame )( 
            INuiSensor * This,
            /* [in] */ HANDLE hStream,
            /* [in] */ DWORD dwMillisecondsToWait,
            /* [retval][out] */ NUI_IMAGE_FRAME *pImageFrame);
        
        HRESULT ( STDMETHODCALLTYPE *NuiImageStreamReleaseFrame )( 
            INuiSensor * This,
            /* [in] */ HANDLE hStream,
            /* [in] */ NUI_IMAGE_FRAME *pImageFrame);
        
        HRESULT ( STDMETHODCALLTYPE *NuiImageGetColorPixelCoordinatesFromDepthPixel )( 
            INuiSensor * This,
            /* [in] */ NUI_IMAGE_RESOLUTION eColorResolution,
            /* [in] */ const NUI_IMAGE_VIEW_AREA *pcViewArea,
            /* [in] */ LONG lDepthX,
            /* [in] */ LONG lDepthY,
            /* [in] */ USHORT usDepthValue,
            /* [out] */ LONG *plColorX,
            /* [out] */ LONG *plColorY);
        
        HRESULT ( STDMETHODCALLTYPE *NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution )( 
            INuiSensor * This,
            /* [in] */ NUI_IMAGE_RESOLUTION eColorResolution,
            /* [in] */ NUI_IMAGE_RESOLUTION eDepthResolution,
            /* [in] */ const NUI_IMAGE_VIEW_AREA *pcViewArea,
            /* [in] */ LONG lDepthX,
            /* [in] */ LONG lDepthY,
            /* [in] */ USHORT usDepthValue,
            /* [out] */ LONG *plColorX,
            /* [out] */ LONG *plColorY);
        
        HRESULT ( STDMETHODCALLTYPE *NuiImageGetColorPixelCoordinateFrameFromDepthPixelFrameAtResolution )( 
            INuiSensor * This,
            /* [in] */ NUI_IMAGE_RESOLUTION eColorResolution,
            /* [in] */ NUI_IMAGE_RESOLUTION eDepthResolution,
            /* [in] */ DWORD cDepthValues,
            /* [size_is][in] */ USHORT *pDepthValues,
            /* [in] */ DWORD cColorCoordinates,
            /* [size_is][out][in] */ LONG *pColorCoordinates);
        
        HRESULT ( STDMETHODCALLTYPE *NuiCameraElevationSetAngle )( 
            INuiSensor * This,
            /* [in] */ LONG lAngleDegrees);
        
        HRESULT ( STDMETHODCALLTYPE *NuiCameraElevationGetAngle )( 
            INuiSensor * This,
            /* [retval][out] */ LONG *plAngleDegrees);
        
        HRESULT ( STDMETHODCALLTYPE *NuiSkeletonTrackingEnable )( 
            INuiSensor * This,
            /* [in] */ HANDLE hNextFrameEvent,
            /* [in] */ DWORD dwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *NuiSkeletonTrackingDisable )( 
            INuiSensor * This);
        
        HRESULT ( STDMETHODCALLTYPE *NuiSkeletonSetTrackedSkeletons )( 
            INuiSensor * This,
            /* [size_is][in] */ DWORD *TrackingIDs);
        
        HRESULT ( STDMETHODCALLTYPE *NuiSkeletonGetNextFrame )( 
            INuiSensor * This,
            /* [in] */ DWORD dwMillisecondsToWait,
            /* [out][in] */ NUI_SKELETON_FRAME *pSkeletonFrame);
        
        HRESULT ( STDMETHODCALLTYPE *NuiTransformSmooth )( 
            INuiSensor * This,
            NUI_SKELETON_FRAME *pSkeletonFrame,
            const NUI_TRANSFORM_SMOOTH_PARAMETERS *pSmoothingParams);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *NuiGetAudioSource )( 
            INuiSensor * This,
            /* [out] */ INuiAudioBeam **ppDmo);
        
        int ( STDMETHODCALLTYPE *NuiInstanceIndex )( 
            INuiSensor * This);
        
        BSTR ( STDMETHODCALLTYPE *NuiDeviceConnectionId )( 
            INuiSensor * This);
        
        BSTR ( STDMETHODCALLTYPE *NuiUniqueId )( 
            INuiSensor * This);
        
        BSTR ( STDMETHODCALLTYPE *NuiAudioArrayId )( 
            INuiSensor * This);
        
        HRESULT ( STDMETHODCALLTYPE *NuiStatus )( 
            INuiSensor * This);
        
        DWORD ( STDMETHODCALLTYPE *NuiInitializationFlags )( 
            INuiSensor * This);
        
        END_INTERFACE
    } INuiSensorVtbl;

    interface INuiSensor
    {
        CONST_VTBL struct INuiSensorVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INuiSensor_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define INuiSensor_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define INuiSensor_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define INuiSensor_NuiInitialize(This,dwFlags)	\
    ( (This)->lpVtbl -> NuiInitialize(This,dwFlags) ) 

#define INuiSensor_NuiShutdown(This)	\
    ( (This)->lpVtbl -> NuiShutdown(This) ) 

#define INuiSensor_NuiSetFrameEndEvent(This,hEvent,dwFrameEventFlag)	\
    ( (This)->lpVtbl -> NuiSetFrameEndEvent(This,hEvent,dwFrameEventFlag) ) 

#define INuiSensor_NuiImageStreamOpen(This,eImageType,eResolution,dwImageFrameFlags,dwFrameLimit,hNextFrameEvent,phStreamHandle)	\
    ( (This)->lpVtbl -> NuiImageStreamOpen(This,eImageType,eResolution,dwImageFrameFlags,dwFrameLimit,hNextFrameEvent,phStreamHandle) ) 

#define INuiSensor_NuiImageStreamSetImageFrameFlags(This,hStream,dwImageFrameFlags)	\
    ( (This)->lpVtbl -> NuiImageStreamSetImageFrameFlags(This,hStream,dwImageFrameFlags) ) 

#define INuiSensor_NuiImageStreamGetImageFrameFlags(This,hStream,pdwImageFrameFlags)	\
    ( (This)->lpVtbl -> NuiImageStreamGetImageFrameFlags(This,hStream,pdwImageFrameFlags) ) 

#define INuiSensor_NuiImageStreamGetNextFrame(This,hStream,dwMillisecondsToWait,pImageFrame)	\
    ( (This)->lpVtbl -> NuiImageStreamGetNextFrame(This,hStream,dwMillisecondsToWait,pImageFrame) ) 

#define INuiSensor_NuiImageStreamReleaseFrame(This,hStream,pImageFrame)	\
    ( (This)->lpVtbl -> NuiImageStreamReleaseFrame(This,hStream,pImageFrame) ) 

#define INuiSensor_NuiImageGetColorPixelCoordinatesFromDepthPixel(This,eColorResolution,pcViewArea,lDepthX,lDepthY,usDepthValue,plColorX,plColorY)	\
    ( (This)->lpVtbl -> NuiImageGetColorPixelCoordinatesFromDepthPixel(This,eColorResolution,pcViewArea,lDepthX,lDepthY,usDepthValue,plColorX,plColorY) ) 

#define INuiSensor_NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(This,eColorResolution,eDepthResolution,pcViewArea,lDepthX,lDepthY,usDepthValue,plColorX,plColorY)	\
    ( (This)->lpVtbl -> NuiImageGetColorPixelCoordinatesFromDepthPixelAtResolution(This,eColorResolution,eDepthResolution,pcViewArea,lDepthX,lDepthY,usDepthValue,plColorX,plColorY) ) 

#define INuiSensor_NuiImageGetColorPixelCoordinateFrameFromDepthPixelFrameAtResolution(This,eColorResolution,eDepthResolution,cDepthValues,pDepthValues,cColorCoordinates,pColorCoordinates)	\
    ( (This)->lpVtbl -> NuiImageGetColorPixelCoordinateFrameFromDepthPixelFrameAtResolution(This,eColorResolution,eDepthResolution,cDepthValues,pDepthValues,cColorCoordinates,pColorCoordinates) ) 

#define INuiSensor_NuiCameraElevationSetAngle(This,lAngleDegrees)	\
    ( (This)->lpVtbl -> NuiCameraElevationSetAngle(This,lAngleDegrees) ) 

#define INuiSensor_NuiCameraElevationGetAngle(This,plAngleDegrees)	\
    ( (This)->lpVtbl -> NuiCameraElevationGetAngle(This,plAngleDegrees) ) 

#define INuiSensor_NuiSkeletonTrackingEnable(This,hNextFrameEvent,dwFlags)	\
    ( (This)->lpVtbl -> NuiSkeletonTrackingEnable(This,hNextFrameEvent,dwFlags) ) 

#define INuiSensor_NuiSkeletonTrackingDisable(This)	\
    ( (This)->lpVtbl -> NuiSkeletonTrackingDisable(This) ) 

#define INuiSensor_NuiSkeletonSetTrackedSkeletons(This,TrackingIDs)	\
    ( (This)->lpVtbl -> NuiSkeletonSetTrackedSkeletons(This,TrackingIDs) ) 

#define INuiSensor_NuiSkeletonGetNextFrame(This,dwMillisecondsToWait,pSkeletonFrame)	\
    ( (This)->lpVtbl -> NuiSkeletonGetNextFrame(This,dwMillisecondsToWait,pSkeletonFrame) ) 

#define INuiSensor_NuiTransformSmooth(This,pSkeletonFrame,pSmoothingParams)	\
    ( (This)->lpVtbl -> NuiTransformSmooth(This,pSkeletonFrame,pSmoothingParams) ) 

#define INuiSensor_NuiGetAudioSource(This,ppDmo)	\
    ( (This)->lpVtbl -> NuiGetAudioSource(This,ppDmo) ) 

#define INuiSensor_NuiInstanceIndex(This)	\
    ( (This)->lpVtbl -> NuiInstanceIndex(This) ) 

#define INuiSensor_NuiDeviceConnectionId(This)	\
    ( (This)->lpVtbl -> NuiDeviceConnectionId(This) ) 

#define INuiSensor_NuiUniqueId(This)	\
    ( (This)->lpVtbl -> NuiUniqueId(This) ) 

#define INuiSensor_NuiAudioArrayId(This)	\
    ( (This)->lpVtbl -> NuiAudioArrayId(This) ) 

#define INuiSensor_NuiStatus(This)	\
    ( (This)->lpVtbl -> NuiStatus(This) ) 

#define INuiSensor_NuiInitializationFlags(This)	\
    ( (This)->lpVtbl -> NuiInitializationFlags(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __INuiSensor_INTERFACE_DEFINED__ */



/* interface __MIDL_itf_Kinect_0001_0066 */
/* [local] */ 

_Check_return_ HRESULT NUIAPI NuiGetSensorCount( _In_ int * pCount );
_Check_return_ HRESULT NUIAPI NuiCreateSensorByIndex( _In_ int index, _Out_ INuiSensor ** ppNuiSensor );
_Check_return_ HRESULT NUIAPI NuiCreateSensorById( _In_ const OLECHAR *strInstanceId, _Out_ INuiSensor ** ppNuiSensor );
_Check_return_ HRESULT NUIAPI NuiGetAudioSource( _Out_ INuiAudioBeam ** ppDmo );

typedef void (CALLBACK* NuiStatusProc)( HRESULT hrStatus, const OLECHAR* instanceName, const OLECHAR* uniqueDeviceName, void* pUserData );
void NUIAPI NuiSetDeviceStatusCallback( NuiStatusProc callback, void* pUserData );

_Check_return_ inline bool HasSkeletalEngine(_In_ INuiSensor *pNuiSensor)
{
    if (!pNuiSensor) return false;
    return (pNuiSensor->NuiInitializationFlags() & NUI_INITIALIZE_FLAG_USES_SKELETON) || (pNuiSensor->NuiInitializationFlags() & NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX);
};

#define MAX_DEV_STR_LEN 512
typedef struct
{
    wchar_t szDeviceName[MAX_DEV_STR_LEN];
    wchar_t szDeviceID[MAX_DEV_STR_LEN];
    int iDeviceIndex;
} NUI_MICROPHONE_ARRAY_DEVICE, *PNUI_MICROPHONE_ARRAY_DEVICE;

//Fills the array of KINECT_AUDIO_DEVICE structs with the Kinect devices found on the system. 
//
//pDeviceInfo: Array allocated by the caller, upon return contains the device information for up to piDeviceCount devices. Can be null
//to just retrieve the number of items in piDeviceCount
//size [in]: The number of items in the array
//piDeviceCount [out]: The actual number of devices found.
_Check_return_ HRESULT NuiGetMicrophoneArrayDevices( _Out_opt_cap_post_count_(size, *piDeviceCount) PNUI_MICROPHONE_ARRAY_DEVICE pDeviceInfo, _In_ int size,  _Out_ int *piDeviceCount);

typedef struct
{
    wchar_t szDeviceName[MAX_DEV_STR_LEN];
    int iDeviceIndex;
    bool fDefault;
} NUI_SPEAKER_DEVICE, *PNUI_SPEAKER_DEVICE;

//Fills the array of NUI_SPEAKER_DEVICE structs with the active speaker devices found on the system. 
//
//pDeviceInfo: Array allocated by the caller, upon return contains the device information for up to piDeviceCount devices. Can be null
//to just retrieve the number of items in piDeviceCount
//size [in]: The number of items in the array
//piDeviceCount [out]: The actual number of devices found.
_Check_return_ HRESULT NuiGetSpeakerDevices( _Out_opt_cap_post_count_(size, *piDeviceCount) PNUI_SPEAKER_DEVICE pDeviceInfo, _In_ int size,  _Out_ int *piDeviceCount);


extern RPC_IF_HANDLE __MIDL_itf_Kinect_0001_0066_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_Kinect_0001_0066_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


