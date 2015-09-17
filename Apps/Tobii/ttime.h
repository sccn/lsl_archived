/*  
 *	ttime.h :	Tobii Eye Tracker Low Level API. The Tobii Time interface.
 *	
 *				Ver 2005-03-22 LK
 *
 *  Copyright (C) Tobii Technology 2003-2005, all rights reserved.
 */

#ifndef TTIME_H_
#define TTIME_H_


#if defined(__cplusplus)
extern "C" {
#endif


#if defined(_WIN32)
	/* Enable Windows dll export											*/
	#define	DLL_EXPORT __declspec (dllexport) 
	/* Ensures MS VC Compiler packs struct byte by byte (fully aligned)		*/
	#pragma pack(push, mypack, 1)		
#else
	#define	DLL_EXPORT
#endif


/* The Tobii time stamp.													*/
typedef struct _STimeStamp {
	long second;		
	long microsecond;
} STimeStamp;


/* Calibration information returned from TT_GetCurrentTime.					*/
typedef struct _STTime { 
	short year;		
	short month;	/* January = 1, February = 2, and so on.				*/
	short dayofweek;/* day of the week; Sunday = 0, Monday = 1, and so on.	*/
	short day; 
	short hour; 
	short minute; 
	short second; 
	short milliseconds; 
} STTime; 



DLL_EXPORT long __stdcall TT_IsSupported(void);

DLL_EXPORT void __stdcall TT_GetLocalTimeStamp(STimeStamp *pTimeStamp);

DLL_EXPORT long __stdcall TT_GetServerTimeStamp(STimeStamp *pTimeStamp);

DLL_EXPORT long __stdcall TT_ElapsedTime(STimeStamp *pStart, STimeStamp *pStop, STimeStamp *pRes);

DLL_EXPORT void __stdcall TT_AddTime(STimeStamp *pTime1, STimeStamp *pTime2, STimeStamp *pRes);

DLL_EXPORT void __stdcall TT_AverageTime(STimeStamp *pTime1, STimeStamp *pTime2, STimeStamp *pRes);

DLL_EXPORT long __stdcall TT_Compare(STimeStamp *pTime1, STimeStamp *pTime2);

DLL_EXPORT void __stdcall TT_GetCurrentTime(STTime *pCurrentTime);



#if defined(_WIN32)
	/* Restore MS VC Compiler packing setting								*/
	#pragma pack(pop, mypack)			
#endif


#if defined(__cplusplus)
}
#endif


#endif /* TTIME_H_ */
