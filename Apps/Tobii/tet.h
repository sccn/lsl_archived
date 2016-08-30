/*  
 *	tet.h :	Tobii Eye Tracker Low Level API. The TETServer interface.
 *	
 *			Ver 2005-03-22 LK
 *
 *  Copyright (C) Tobii Technology 2002-2005, all rights reserved.
 */

#ifndef TET_H_
#define TET_H_


#if defined(__cplusplus)
extern "C" {
#endif


#if defined(_WIN32)
	/* Enable Windows dll export */
	#define	DLL_EXPORT __declspec (dllexport) 
#else
	#define	DLL_EXPORT
#endif



/* Return codes from all Tet_ where noting else is stated.		*/ 
/* When TET_ERROR is returned, call Tet_GetLastError			*/
/* for the specific error code.									*/
#define TET_ERROR					-1	/* Was error			*/
#define TET_NO_ERROR				0	/* Was no error			*/


/* These are possible error codes returned by					*/ 
/* Tet_GetLastError.											*/
#define TET_ERR_NO_ERROR						 0
#define TET_ERR_SERVER_IS_NOT_CONNECTED			 1
#define TET_ERR_SERVER_COMMUNICATION			 2
#define TET_ERR_FILE_OPEN						 3
#define TET_ERR_FILE_READ						 4
#define TET_ERR_INTERNAL						 5
#define TET_ERR_MEMORY							 6
#define TET_ERR_CAMERA							 7
#define TET_ERR_DIOD							 8
#define TET_ERR_LOCKED_SYSTEM					 9
#define TET_ERR_CALIB_INCOMPATIBLE_DATA_FORMAT	10
#define TET_ERR_CALIB_INSUFFICIENT_DATA			11
#define TET_ERR_CALIB_NO_DATA_SET				12
#define TET_ERR_INVALID_STATE					13
#define TET_ERR_INCOMPATIBLE_SERVER_VERSION		14
#define TET_ERR_UNKNOWN_OR_INVALID_PARAMETER	15


/* Default IP address and port number.							*/
#define LOCALHOST_IPADDRESS				"127.0.0.1"
#define SERVER_PORT_NUMBER				4455



/* Synchronization mode parameter to Tet_Connect. 				*/
typedef enum _ETet_SynchronizationMode {
	TET_SYNC_NONE = 1,
	TET_SYNC_SERVER = 2,
	TET_SYNC_LOCAL = 3
} ETet_SynchronizationMode;



/* The reason why Tet_CallbackFunction was called.				*/
typedef enum _ETet_CallbackReason {
	TET_CALLBACK_GAZE_DATA = 1,
	TET_CALLBACK_TIMER = 2
} ETet_CallbackReason;



/* Gaze data passed to the user defined callback				*/
/* function Tet_CallbackFunction when callback reason is		*/
/* TET_CALLBACK_GAZE_DATA.										*/
typedef struct _STet_GazeData {
	long timestamp_sec;
	long timestamp_microsec;
	float x_gazepos_lefteye;
	float y_gazepos_lefteye;
	float x_camerapos_lefteye;
	float y_camerapos_lefteye;
	float diameter_pupil_lefteye;
	float distance_lefteye;
	long validity_lefteye;
	float x_gazepos_righteye;
	float y_gazepos_righteye;
	float x_camerapos_righteye;
	float y_camerapos_righteye;
	float diameter_pupil_righteye;
	float distance_righteye;
	long validity_righteye;
} STet_GazeData;



/* Callback function definition to implement and set as			*/
/* argument to Tet_Start and Tet_CalibAddPoint.					*/
typedef void  (__stdcall *Tet_CallbackFunction)(
					ETet_CallbackReason reason, 
					void *pData, 
					void *pApplicationData);



/* Calibration information returned from Tet_CalibGetResult.	*/
typedef struct _STet_CalibAnalyzeData {
	float truePointX;
	float truePointY;
	float leftMapX;
	float leftMapY;
	long leftValidity;
	float leftQuality;
	float rightMapX;
	float rightMapY;
	long rightValidity;
	float rightQuality;
} STet_CalibAnalyzeData;



/* Eye parameter to Tet_CalibRemovePoints.						*/
typedef enum _ETet_Eye {
	TET_EYE_LEFT	= 1,
	TET_EYE_RIGHT	= 2,
	TET_EYE_BOTH	= 3
} ETet_Eye;



DLL_EXPORT long __stdcall Tet_Connect(char *pServerAddress, long portnumber, ETet_SynchronizationMode syncmode);

DLL_EXPORT long __stdcall Tet_Disconnect(void);

DLL_EXPORT long __stdcall Tet_Start(Tet_CallbackFunction Fnc, void *pApplicationData, long interval);

DLL_EXPORT long __stdcall Tet_Stop(void);

DLL_EXPORT long __stdcall Tet_CalibLoadFromFile(char *pFile);

DLL_EXPORT long __stdcall Tet_CalibSaveToFile(char *pFile);

DLL_EXPORT long __stdcall Tet_CalibClear(void);	

DLL_EXPORT long __stdcall Tet_CalibAddPoint(float x, float y, long nrofdata, Tet_CallbackFunction Fnc, void *pApplicationData, long interval);

DLL_EXPORT long __stdcall Tet_CalibRemovePoints(ETet_Eye eye, float x, float y, float radius);

DLL_EXPORT long __stdcall Tet_CalibCalculateAndSet(void);	

DLL_EXPORT long __stdcall Tet_CalibGetResult(char *pFile, STet_CalibAnalyzeData *pData, long *pLen);

DLL_EXPORT long __stdcall Tet_PerformSystemCheck(void);

DLL_EXPORT long __stdcall Tet_GetSerialNumber(char *pSerialDiodeController, char *pSerialCamera);

DLL_EXPORT long __stdcall Tet_GetLastError(void);

DLL_EXPORT char * __stdcall Tet_GetLastErrorAsText(char *pError);



#if defined(__cplusplus)
}
#endif


#endif /* TET_H_ */
