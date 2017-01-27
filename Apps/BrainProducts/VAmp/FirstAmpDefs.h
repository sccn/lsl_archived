/*----------------------------------------------------------------------------*/
/*
  FirstAmpDefs.h defines entry methods for FirstAmp.dll
*/
/*----------------------------------------------------------------------------*/
#ifndef FirstAmm_DEFS_H
#define FirstAmp_DEFS_H

#include "stdafx.h"
#include "FirstAmp.h"

/*----------------------------------------------------------------------------*/
/* Define function prototypes					  */
/*----------------------------------------------------------------------------*/

typedef int(WINAPI* FAGetIdProc)(unsigned int Number);

typedef unsigned int (WINAPI* faGetCountProc)(void);

typedef int (WINAPI* faOpenProc)(int Id);

typedef int (WINAPI* faCloseProc)(int Id);

typedef int (WINAPI* faStartProc)(int Id);

typedef int (WINAPI* faStopProc)(int Id);

typedef int (WINAPI* faGetDataProc)(int Id, void *Buffer, unsigned int Size);

typedef int (WINAPI* faGetInformationProc)(int Id, t_faInformation *Information);

typedef int (WINAPI* faGetPropertyProc)(int Id, t_faProperty *Property);

typedef int (WINAPI* faStartImpedanceProc)(int Id);

typedef int (WINAPI* faStopImpedanceProc)(int Id);

typedef int (WINAPI*  faGetImpedanceProc)(int Id, unsigned int *Buffer, unsigned int Size);

typedef int (WINAPI* WINAPI faStartCalibrationProc)(int Id);

typedef int (WINAPI* faStopCalibrationProc)(int Id);

typedef int (WINAPI* faGetIoProc)(int Id, t_faIo *Io);

typedef int (WINAPI* faSetIoProc)(int Id, t_faIo *Io);

typedef int (WINAPI* faSetBitmapProc)(int Id, HBITMAP Bitmap);

typedef int (WINAPI* faGetContrastProc)(int Id, unsigned int *Contrast);

typedef int (WINAPI* faSetContrastProc)(int Id, unsigned int Contrast);

typedef int (WINAPI* faGetBrightnessProc)(int Id, unsigned int *Brightness);

typedef int (WINAPI* faSetBrightnessProc)(int Id, unsigned int Brightness);

typedef int (WINAPI*  faGetUserDataProc)(int Id, void *Buffer, unsigned int Size, unsigned int *Count);

typedef int (WINAPI* faSetUserDataProc)(int Id, void *Buffer, unsigned int Size, unsigned int *Count);

typedef int (WINAPI* faGetDataStateProc)(int Id, t_faDataState *DataState);

typedef int (WINAPI* faGetDataModeProc)(int Id, t_faDataMode *Mode, t_faDataModeSettings *Settings);

typedef int (WINAPI* faSetDataModeProc)(int Id, t_faDataMode Mode, t_faDataModeSettings *Settings);


/*----------------------------------------------------------------------------*/
/* Define Method types*/
/*----------------------------------------------------------------------------*/
faStartProc FAStart;

faStopProc FAStop;

faGetDataProc FAGetData;

faGetInformationProc FAGetInformation;

faGetPropertyProc FAGetProperty;

faStartImpedanceProc FAStartImpedance;

faStopImpedanceProc FAStopImpedance;

faGetImpedanceProc FAGetImpedance;

faStartCalibrationProc FAStartCalibration;

faStopCalibrationProc FAStopCalibration;

faGetIoProc FAGetIo;

faSetIoProc FASetIo;

faSetBitmapProc FASetBitmap;

faGetContrastProc FAGetContrast;

faSetContrastProc FASetContrast;

faGetBrightnessProc FAGetBrightness;

faSetBrightnessProc FASetBrightness;

faGetUserDataProc FAGetUserData;

faSetUserDataProc FASetUserData;

faGetDataStateProc FAGetDataState;

faGetDataModeProc FAGetDataMode;

faSetDataModeProc FASetDataMode;

FAGetIdProc	FAGetId;

faGetCountProc FAGetCount;

faOpenProc FAOpen;

faCloseProc FAClose;

/*----------------------------------------------------------------------------*/
/* Import methods from dll - get pointers to functions using GetProcAddress() */
/*----------------------------------------------------------------------------*/
inline void Import(HMODULE hLib)
{
	// successful
	FAGetId = (FAGetIdProc) GetProcAddress(hLib, "faGetId");

	FAGetCount = (faGetCountProc) GetProcAddress(hLib, "faGetCount");

	FAOpen = (faOpenProc) GetProcAddress(hLib, "faOpen");

	FAClose = (faCloseProc) GetProcAddress(hLib, "faClose");
	
	FAStart = (faStartProc) GetProcAddress(hLib, "faStart");

	FAStop = (faStopProc) GetProcAddress(hLib, "faStop");

	FAGetData = (faGetDataProc) GetProcAddress(hLib, "faGetData");

	FAGetInformation =	(faGetInformationProc) GetProcAddress(hLib, "faGetInformation");

	FAGetProperty =	(faGetPropertyProc) GetProcAddress(hLib, "faGetProperty");

	FAStartImpedance =	(faStartImpedanceProc) GetProcAddress(hLib, "faStartImpedance");

	FAStopImpedance = (faStopImpedanceProc) GetProcAddress(hLib, "faStopImpedance");

	FAGetImpedance = (faGetImpedanceProc) GetProcAddress(hLib, "faGetImpedance");

	FAStartCalibration = (faStartCalibrationProc) GetProcAddress(hLib, "faStartCalibration");

	FAStopCalibration = (faStopCalibrationProc) GetProcAddress(hLib, "faStopCalibration");

	FAGetIo	= (faGetIoProc) GetProcAddress(hLib, "faGetIo");

	FASetIo	= (faSetIoProc) GetProcAddress(hLib, "faSetIo");

	FASetBitmap	= (faSetBitmapProc) GetProcAddress(hLib, "faSetBitmap");

	FAGetContrast	= (faGetContrastProc) GetProcAddress(hLib, "faGetContrast");

	FASetContrast	= (faSetContrastProc) GetProcAddress(hLib, "faSetContrast");

	FAGetBrightness	= (faGetBrightnessProc) GetProcAddress(hLib, "faGetBrightness");

	FASetBrightness	= (faSetBrightnessProc) GetProcAddress(hLib, "faSetBrightness");

	FAGetUserData	= (faGetUserDataProc) GetProcAddress(hLib, "faGetUserData");

	FASetUserData	= (faSetUserDataProc) GetProcAddress(hLib, "faSetUserData");

	FAGetDataState	= (faGetDataStateProc) GetProcAddress(hLib, "faGetDataState");

	FAGetDataMode	= (faGetDataModeProc) GetProcAddress(hLib, "faGetDataMode");

	FASetDataMode	= (faSetDataModeProc) GetProcAddress(hLib, "faSetDataMode");
};

#endif // FirstAmp_DEFS_H