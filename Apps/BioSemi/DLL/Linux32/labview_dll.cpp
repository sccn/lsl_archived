/*
 * Legacy Biosemi routines that call appropriate usb acquisition interface 
 * software:
 * 
 * On Windows XP or Vista32,
 *     either the Biosemi kernel mode driver
 *     or the MS 'winusb' library via bsif_winusb
 *
 * On Vista64, 
 *     the MS 'winusb' library via bsif_winusb
 *
 * On Linux or Mac,
 *     'libusb' library via bsif_libusb
 *
 * This routine compiles on Windows, Linux and Mac
 *
 * Copyright (C) 2003-2010 Biosemi <www.biosemi.com>
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#ifdef WIN32
#include <windows.h>
#include <winioctl.h>
#include <direct.h>	// for _getcwd
#include "ioctls.h"
#define LABVIEW_DLL_EXPORTS
#else
// linux, mac
#include <unistd.h>
#endif	// WIN32

#include "labview_dll.h"

#include "bsif.h"		// biosemi plugin interface

#ifdef WIN32
#define LEGACY_MSVCRT	// for xp, remove when no longer supported
#define _CRT_SECURE_NO_WARNINGS	1	// disable deprecation warnings

#define STRICMP		_stricmp
#define STRNICMP	_strnicmp
#else
// linux
#ifdef strlcpy
#define STRLCPY		strlcpy
#else
#define STRLCPY		strncpy
#endif	// strlcpy
#ifdef strlcat
#define STRLCAT		strlcat
#else
#define STRLCAT		strncat
#endif	// strlcat
#define STRICMP		strcasecmp
#define STRNICMP	strncasecmp
#endif	// WIN32

//*******************************************************************
//                         local variables
//*******************************************************************

// user controlled parameters
#ifdef DEF_LOG
static BOOL	logging=TRUE;
#else
static BOOL	logging=FALSE;
#endif	// DEF_LOG

#ifdef DEF_DEBUG
static BOOL	debugging=TRUE;
#else
static BOOL	debugging=FALSE;
#endif	// DEF_DEBUG

#ifdef DEF_SYNC
static BOOL	syncPointer=TRUE;
#else
static BOOL	syncPointer=FALSE;
#endif	// DEF_SYNC

#ifdef DEF_STRIDE
static int	stride=DEF_STRIDE;	// msec per buffer transfer
#else
static int	stride=64;		// 64 msec per buffer transfer
#endif	// DEF_STRIDE

// debug variables
static char	logFilename[4096];

// misc other variables
static char	*version="20091118";
static FILE	*logfp=NULL;
static BOOL	ini=FALSE;	// TRUE after .ini file has been read
static int	driverFound=0;	// 0=none, 1=BioSemi windows kernel, 3=other
static BOOL	useBSIFLib=FALSE;


//*******************************************************************
//                           local routines
//*******************************************************************

#ifdef WIN32
static VOID LINKAGE
//************************
PrintLastErrorAsText()
//************************
	{
	LPVOID lpMsgBuf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), 0,
  		(LPTSTR) &lpMsgBuf, 0, NULL);
	fprintf(stderr, "%s\n", (LPSTR)lpMsgBuf);
	if (logfp != (FILE *)NULL)
		fprintf(logfp, "%s\n", (LPSTR)lpMsgBuf),
			fclose(logfp),logfp=fopen(logFilename,"a");
	return;
	}
#endif	// WIN32

static PCHAR LINKAGE
//******************
GetTimeStamp()
//******************
	{
	// get current time
	time_t curtime = time (NULL);

	// convert it to local time
	struct tm *loctime = localtime (&curtime);

	return asctime(loctime);
	}


static VOID LINKAGE
//******************
ProcessIniFile()
//******************
	{
	char	iniFilename[4096];
	char	iniFilenamePrefix[4096];

	// On Windows:
	//
	// If cwd is ":\Windows\", use either "HOMEDRIVE" + "HOMEPATH" or "\TEMP"
	// as log file prefix; otherwise don't use a prefix
	//
	// Note: This is because, when used with LabView workbench on Windows,  cwd
	//	becomes "Windows\System32"; when used with a Labview generated runtime .exe,
	//	cmd is the .exe's directory */

	// On Mac and perhaps Linux:
	//
	// When this routine is called by the LabView workbench, the "current
	// working directory" becomes "/", the root directory.
	//
	// To avoid putting the .ini and .log files in the root directory,
	// if "getcwd" returns "/", get the environment variable "HOME" and use it
	// as a prefix for the .ini and .log file names.

#ifdef WIN32
	if (_getcwd(iniFilenamePrefix, sizeof(iniFilenamePrefix)) == NULL)
		iniFilenamePrefix[0] = 0;
	if (STRICMP(iniFilenamePrefix+1, ":\\Windows\\") == 0)
		{
#ifdef LEGACY_MSVCRT
		char	*cp;
		if ((cp=getenv("HOMEDRIVE")) != NULL)
			strcpy(iniFilenamePrefix, cp);
		else
			iniFilenamePrefix[0] = 0;

		if ((cp=getenv("HOMEPATH")) != NULL)
			strcat(iniFilenamePrefix, cp);
		else
			strcat(iniFilenamePrefix, "\\TEMP");
#else
		size_t	envLength;
		getenv_s(&envLength, iniFilenamePrefix, sizeof(iniFilenamePrefix), "HOMEDRIVE");
		if (envLength == 0)
			iniFilenamePrefix[0] = 0;;

		getenv_s(&envLength, iniFilenamePrefix+strlen(iniFilenamePrefix),
			sizeof(iniFilenamePrefix)-strlen(iniFilenamePrefix), "HOMEPATH");
		if (envLength == 0)
			strcat_s(iniFilenamePrefix, sizeof(iniFilenamePrefix), "\\TEMP");
#endif	// LEGACY_MSVCRT
		}
	else
		iniFilenamePrefix[0] = 0;

	strcpy_s(iniFilename, sizeof(iniFilename), iniFilenamePrefix);
	if (iniFilename[0] == 0)
		strcpy_s(iniFilename, sizeof(iniFilename), "labview_dll.ini");
	else
		strcat_s(iniFilename, sizeof(iniFilename), "\\labview_dll.ini");
#else	// linux, mac
	if (getcwd(iniFilenamePrefix, sizeof(iniFilenamePrefix)) == NULL)
		iniFilenamePrefix[0] = 0;
	if (strcmp(iniFilenamePrefix, "/") == 0)
		{
		char *cp = getenv("HOME");
		if (cp != NULL)
			STRLCPY(iniFilenamePrefix, cp, sizeof(iniFilenamePrefix));
		}
	else
		iniFilenamePrefix[0] = 0;

	STRLCPY(iniFilename, iniFilenamePrefix, sizeof(iniFilename));
	if (iniFilename[0] == 0)
		STRLCPY(iniFilename, "labview_dll.ini", sizeof(iniFilename));
	else
		STRLCAT(iniFilename, "/labview_dll.ini", sizeof(iniFilename));
#endif	// WIN32

	FILE *cfgfp=fopen(iniFilename, "r");
	if (cfgfp != (FILE *)NULL)
		{
		char	aline[80];
		debugging = logging = syncPointer = FALSE;
		for (;;)
			if (fgets((char *)&aline, sizeof(aline), cfgfp) == (char *)NULL)
				break;
			else
				{
				int len = (int)strlen(aline);
				if (aline[len-1] == '\n')
					aline[len-1] = 0;	// drop cr
				if (aline[0] == ';')
					continue;
				if (STRICMP(aline, "debug") == 0)
					debugging = logging = TRUE;
				else if (STRICMP(aline, "log") == 0)
					logging = TRUE;
				else if (STRICMP(aline, "sync") == 0)
					syncPointer = TRUE;
				else if (STRICMP(aline, "nosync") == 0)
					syncPointer = FALSE;
				else if (STRNICMP(aline, "stride ", 7) == 0)
					{
					int msec = atoi(aline+7);
					if (msec > 0)
						stride = msec;
					}
				}
		fclose(cfgfp);

		if ((logging == TRUE) && (logfp == (FILE *)NULL))
			{
#ifdef WIN32
#ifdef LEGACY_MSVCRT
			strcpy(logFilename, iniFilenamePrefix);
			if (logFilename[0] == 0)
				strcpy(logFilename, "labview_dll.log");
			else
				strcat(logFilename, "\\labview_dll.log");
#else
			strcpy_s(logFilename, sizeof(logFilename), iniFilenamePrefix);
			if (logFilename[0] == 0)
				strcpy_s(logFilename, sizeof(logFilename), "labview_dll.log");
			else
				strcat_s(logFilename, sizeof(logFilename), "\\labview_dll.log");
#endif	// LEGACY_MSVCRT
#else	// linux, mac
			STRLCPY(logFilename, iniFilenamePrefix, sizeof(logFilename));
			if (logFilename[0] == 0)
				STRLCPY(logFilename, "labview_dll.log", sizeof(logFilename));
			else
				STRLCAT(logFilename, "/labview_dll.log", sizeof(logFilename));
#endif	// WIN32

			logfp = fopen(logFilename, "a");

			fprintf(logfp, "***************\n");
			fprintf(logfp, "Log started at: %s", GetTimeStamp());
			fprintf(logfp, "Version %s\n", version);
			fprintf(logfp, "Using %s\n", iniFilename);
			fprintf(logfp, "***************\n"),
				fclose(logfp),logfp=fopen(logFilename, "a");
			}

		else if ((logging == FALSE) && (logfp != (FILE *)NULL))
			fclose(logfp), logfp = (FILE *)NULL;
		}
	return;
	}

//*******************************************************************
//                         global subroutines
//*******************************************************************

#ifdef WIN32
// windows
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call,
	LPVOID lpReserved )
	{
	switch (ul_reason_for_call)
		{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
			if (logfp != (FILE *)NULL)
			  	fprintf(logfp, "attached\n"),
					fclose(logfp),logfp=fopen(logFilename, "a");

			break;
		case DLL_PROCESS_DETACH:
		case DLL_THREAD_DETACH:
			if (logfp != (FILE *)NULL)
				fprintf(logfp, "detached\n"),
					fclose(logfp),logfp=fopen(logFilename, "a");
			break;
		}
	return TRUE;
	}

#else
// linux

void __attribute__ ((constructor)) my_load(void);
void __attribute__ ((destructor)) my_unload(void);

// Called when the library is loaded and before dlopen() returns
void my_load(void)
	{
	// initialization code
	BSIF_MY_LOAD();		// initialize bsif module

	if (logfp != (FILE *)NULL)
	  	fprintf(logfp, "attached\n"),
			fclose(logfp),logfp=fopen(logFilename, "a");
	}

// Called when the library is unloaded and before dlclose() returns
void my_unload(void)
	{
	// clean-up code

	BSIF_MY_UNLOAD();	// cleanup bsif module

	if (logfp != (FILE *)NULL)
		fprintf(logfp, "detached\n"),
			fclose(logfp),logfp=fopen(logFilename, "a");
	}	

#endif	// WIN32

//LABVIEW_DLL_API
//************************
HANDLE LINKAGE OPEN_DRIVER_ASYNC(void)
//************************
	{// no longer async ... provided for compatibility 
	return OPEN_DRIVER();
	}

//************************
HANDLE LINKAGE OPEN_DRIVER(void)
//************************
	{
	if (ini == FALSE)
		{
		ProcessIniFile();
		ini = TRUE;
		}

	useBSIFLib = FALSE;
	driverFound = 0;	// 0=none

	HANDLE	ret;

#ifdef WIN32
	if (debugging == TRUE)
		fprintf(stderr, "open_driver called\n");

	if (logfp != (FILE *)NULL)
		fprintf(logfp, "open_driver called\n"),
			fclose(logfp),logfp=fopen(logFilename,"a");

	// on windows, try kernel driver first
	ret = CreateFile("\\\\.\\BIOSEMI", GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
	if (ret != INVALID_HANDLE_VALUE)
		{
		driverFound = 1;	// 1=BioSemi windows kernel

		if (debugging == TRUE)
			fprintf(stderr, "Using BioSemi windows kernel USB driver\n");

		if (logfp != (FILE *)NULL)
			fprintf(logfp, "Using BioSemi windows kernel USB driver\n"),
				fclose(logfp),logfp=fopen(logFilename,"a");
		return ret;
		}
#endif	// WIN32

	ret = BSIF_OPEN_DRIVER();
	if (ret != 0)
		{
		driverFound = 3;	// 3=other
		useBSIFLib = TRUE;

		if (debugging == TRUE)
			fprintf(stderr, "Using bsif_xxx driver\n");
		if (logfp != (FILE *)NULL)
			fprintf(logfp, "Using bsif_xxx driver\n"),
				fclose(logfp),logfp=fopen(logFilename,"a");

		BSIF_SET_LOG(logging);

		BSIF_SET_DEBUG(debugging);
		BSIF_SET_SYNC(syncPointer);
		BSIF_SET_STRIDE(stride);

		}
	else
		{
		driverFound = 0;
		useBSIFLib = FALSE;

		fprintf(stderr, "No USB driver found\n");
		if (logfp != (FILE *)NULL)
			fprintf(logfp, "No USB driver found\n"),
				fclose(logfp),logfp=fopen(logFilename,"a");
		}

	return ret;
	}

//************************
BOOL LINKAGE READ_MULTIPLE_SWEEPS(HANDLE hdevice,PCHAR data,INT_PTR nBytesToRead)
//************************
	{
	if (useBSIFLib == TRUE)
		return BSIF_READ_MULTIPLE_SWEEPS(hdevice, data, nBytesToRead);

#ifdef WIN32
	if (debugging == TRUE)
		fprintf(stderr, "read_multiple_sweeps called, buffer size = %d\n",
			nBytesToRead);

	if (logfp != (FILE *)NULL)
		fprintf(logfp, "read_multiple_sweeps called, buffer size = %d\n",
			nBytesToRead),
			fclose(logfp),logfp=fopen(logFilename,"a");

	OVERLAPPED gOverlapped;
	gOverlapped.Offset = 0;
	gOverlapped.OffsetHigh = 0;	

	gOverlapped.hEvent=CreateEvent(NULL,FALSE,FALSE,NULL);

	DWORD nBytesRead;
	BOOL ret;

	ret = ReadFile(hdevice, data, (DWORD)nBytesToRead, &nBytesRead,
		&gOverlapped);
	
	return ret;
#endif	// WIN32
	return FALSE;
	}

//LABVIEW_DLL_API
//************************
BOOL LINKAGE WRITE_SINGLE_SWEEP(HANDLE hdevice,PCHAR data,DWORD nBytesToWrite)
//************************
	{
	if (useBSIFLib == TRUE)
		{
		fprintf(stderr, "*** write_single_sweep not implemented! ***\n");

		if (logfp != (FILE *)NULL)
			fprintf(logfp, "*** write_single_sweep not implemented! ***\n"),
				fclose(logfp),logfp=fopen(logFilename,"a");

		return FALSE;
		}

#ifdef WIN32
	if (debugging == TRUE)	
		fprintf(stderr, "write_single_sweep called\n");
	if (logfp != (FILE *)NULL)
		fprintf(logfp, "write_single_sweep called\n"),
			fclose(logfp),logfp=fopen(logFilename,"a");

	DWORD nBytesWritten;
	return WriteFile(hdevice, data, nBytesToWrite , &nBytesWritten ,NULL);
#endif	// WIN32
	return FALSE;
	}

//************************
BOOL LINKAGE WRITE_SINGLE_SWEEP_ASYNC(HANDLE hdevice,PCHAR data,DWORD nBytesToWrite)
//************************
	{
	if (useBSIFLib == TRUE)
		{
		fprintf(stderr, "*** write_single_sweep_async not implemented! ***\n");

		if (logfp != (FILE *)NULL)
			fprintf(logfp, "*** write_single_sweep_async not implemented! ***\n"),
				fclose(logfp),logfp=fopen(logFilename,"a");

		return FALSE;
		}

#ifdef WIN32
	if (debugging == TRUE)	
		fprintf(stderr, "write_single_sweep_async called\n");

	if (logfp != (FILE *)NULL)
		fprintf(logfp, "write_single_sweep_async called\n"),
			fclose(logfp),logfp=fopen(logFilename,"a");

	DWORD nBytesWritten;
	OVERLAPPED gOverlapped;
	gOverlapped.Offset = 0;
	gOverlapped.OffsetHigh = 0;	

	gOverlapped.hEvent=CreateEvent(NULL,FALSE,FALSE,NULL);

	return WriteFile(hdevice, data, nBytesToWrite , &nBytesWritten ,&gOverlapped);
#endif	// WIN32
	return FALSE;
	}

//************************
BOOL LINKAGE READ_POINTER(HANDLE hdevice, PINT_PTR pointer)
//************************
	{
	if (useBSIFLib == TRUE)
		return BSIF_READ_POINTER(hdevice, pointer);

#ifdef WIN32
	// windows kernel driver

	BOOL	ret;

	if (debugging == TRUE)
		fprintf(stderr, "read_pointer called\n"),
			fprintf(logfp, "read_pointer called\n"),
				fclose(logfp),logfp=fopen(logFilename,"a");
	DWORD junk;

	ret = DeviceIoControl(hdevice,IOCTL_BUFFER_POINTER , NULL, 0, pointer,
		sizeof(INT_PTR), &junk, NULL);
	if (debugging == TRUE)
		fprintf(stderr, "read_pointer returns %d\n", *pointer),
			fprintf(logfp, "read_pointer returns %d\n", *pointer),
				fclose(logfp),logfp=fopen(logFilename,"a");
	return ret;
#endif	// WIN32
	return FALSE;
	}

//LABVIEW_DLL_API
//************************
BOOL LINKAGE USB_WRITE(HANDLE hdevice, PCHAR data)
//************************
	{
	if (useBSIFLib == TRUE)
		return BSIF_USB_WRITE(hdevice, data);

#ifdef WIN32
	// windows kernel driver

	if (debugging == TRUE)
		fprintf(stderr, "usb_write called, [0]=%x - %s\n",
			(int)data[0], (data[0]==0)?"disable":"enable");

	if (logfp != (FILE *)NULL)
		fprintf(logfp, "usb_write called, [0]=%x - %s\n",
			(int)data[0], (data[0]==0)?"disable":"enable"),
			fclose(logfp),logfp=fopen(logFilename,"a");

	DWORD junk;
//	OVERLAPPED gOverlapped;
//	gOverlapped.Offset = 0;
//	gOverlapped.OffsetHigh = 0;	

//	gOverlapped.hEvent=CreateEvent(NULL,FALSE,FALSE,NULL);
	
	return DeviceIoControl(hdevice,IOCTL_USB_WRITE, NULL, 0, data, 64, &junk,NULL);
#endif	// WIN32
	return FALSE;
	}

//************************
BOOL LINKAGE CLOSE_DRIVER_ASYNC(HANDLE hdevice)
//************************
	{// no longer async ... provided for compatibility
	return CLOSE_DRIVER(hdevice);
	}

//************************
BOOL LINKAGE CLOSE_DRIVER(HANDLE hdevice)
//************************
	{
	if (useBSIFLib == TRUE)
		return BSIF_CLOSE_DRIVER(hdevice);

#ifdef WIN32
	// windows kernel driver

	if (debugging == TRUE)
		fprintf(stderr, "close_driver called\n");

	if (logfp != (FILE *)NULL)
		fprintf(logfp, "close_driver called\n"),
			fclose(logfp),logfp=fopen(logFilename,"a");

	CancelIo(hdevice);

	return CloseHandle(hdevice);
#endif	// WIN32
	return FALSE;
	}

//************************
BOOL LINKAGE DOWNLOAD_FX2(void)
//************************
	{
	if (useBSIFLib == TRUE)
		{
		fprintf(stderr, "*** download_fx2 not implemented! ***\n");

		if (logfp != (FILE *)NULL)
			fprintf(logfp, "*** download_fx2 not implemented! ***\n"),
				fclose(logfp),logfp=fopen(logFilename,"a");

		return FALSE;
		}

#ifdef WIN32
	if (debugging == TRUE)	
		fprintf(stderr, "download_fx2 called\n");

	if (logfp != (FILE *)NULL)
		fprintf(logfp, "download_fx2 called\n"),
			fclose(logfp),logfp=fopen(logFilename,"a");

	BOOL result=FALSE;
	HANDLE hdevice;
	DWORD junk;
	FILE *fp;
	PUCHAR data[MAX_FILE_SIZE];
	ANCHOR_DOWNLOAD_CONTROL downloadControl;

	downloadControl.Offset=0;

	if ((fp = fopen("C:/BiosemiCAD/loopback/firmware/USB2a.bix","rb")) != NULL)
		{
		fread(data,sizeof(unsigned char),MAX_FILE_SIZE,fp);
		fclose(fp);
				
		hdevice = CreateFile("\\\\.\\BIOSEMI", GENERIC_WRITE,
			FILE_SHARE_WRITE, NULL, OPEN_EXISTING,0 , NULL);
	
		if(hdevice != INVALID_HANDLE_VALUE)
			{
			result = DeviceIoControl(hdevice, IOCTL_DOWNLOAD_FX2,
				&downloadControl,sizeof(ANCHOR_DOWNLOAD_CONTROL),
				data, MAX_FILE_SIZE, &junk, NULL);
			CloseHandle(hdevice);
			}
		}
	return result;
#endif	// WIN32
	return FALSE;
	}



//************************
BOOL LINKAGE RESET_FX2(BOOL RESET)
//************************
	{
	if (useBSIFLib == TRUE)
		{
		fprintf(stderr, "*** reset_fx2 not implemented! ***\n");

		if (logfp != (FILE *)NULL)
			fprintf(logfp, "*** reset_fx2 not implemented! ***\n"),
				fclose(logfp),logfp=fopen(logFilename,"a");

		return FALSE;
		}
#ifdef WIN32

	if (debugging == TRUE)	
		fprintf(stderr, "reset_fx2 called\n");

	if (logfp != (FILE *)NULL)
		fprintf(logfp, "reset_fx2 called\n"),
			fclose(logfp),logfp=fopen(logFilename,"a");

	BOOLEAN bResult	= FALSE;
	int	nBytes = 0;
	
	VENDOR_REQUEST_IN myRequest;
	HANDLE hdevice;			
	myRequest.bRequest = 0xA0;
	myRequest.wValue = 0xE600; // using CPUCS.0 in FX2
	myRequest.wIndex = 0x00;
	myRequest.wLength = 0x01;
	myRequest.bData = (BYTE)RESET;
	myRequest.direction = 0x00;
				
	hdevice = CreateFile("\\\\.\\BIOSEMI", GENERIC_WRITE, FILE_SHARE_WRITE,
		NULL, OPEN_EXISTING, 0, NULL);

	if(hdevice != INVALID_HANDLE_VALUE)
		{
		bResult = (BOOLEAN)DeviceIoControl (hdevice,
			IOCTL_VENDOR_REQUEST,
			&myRequest,
			sizeof(VENDOR_REQUEST_IN),
			NULL,
			0,
			(unsigned long *)&nBytes,
			NULL);

		CloseHandle(hdevice);
		}
	
	return bResult;
#endif	// WIN32
	return FALSE;
	}
			
//************************
BOOL LINKAGE DOWNLOAD_FX2_EEPROM(void)
//************************
	{
	ProcessIniFile();

	if (logfp != (FILE *)NULL)
		fprintf(logfp, "download_fx2_eeprom called\n"),
			fclose(logfp),logfp=fopen(logFilename,"a");

#ifndef WIN32
// Linux
	BSIF_SET_LOG(logging);
	return BSIF_DOWNLOAD_FX2_EEPROM();

#else
// Windows

	int i;

	WORD Total_Read;
	BOOL result=FALSE;
	HANDLE hdevice;
	DWORD junk;
	FILE *fp;
	UCHAR data[MAX_FILE_SIZE];
	ANCHOR_DOWNLOAD_CONTROL downloadControl;

	downloadControl.Offset=0;

	for(i=0;i<MAX_FILE_SIZE;i++)data[i]=0xff;

	if (((fp = fopen("USB2a.iic","rb")) != NULL) ||
		((fp = fopen("C:/BiosemiCAD/loopback/firmware/USB2a.iic","rb")) != NULL))
		{
		Total_Read=(WORD)fread(data,sizeof(unsigned char),MAX_FILE_SIZE,fp);
		fclose(fp);
		
		// try kernel driver first		
		hdevice = CreateFile("\\\\.\\BIOSEMI", GENERIC_WRITE,
			FILE_SHARE_WRITE, NULL, OPEN_EXISTING,0 , NULL);	
		if(hdevice != INVALID_HANDLE_VALUE)
			{
			if (logfp != (FILE *)NULL)
				fprintf(logfp, "Using Biosemi kernel driver\n"),
					fclose(logfp),logfp=fopen(logFilename,"a");

			result = DeviceIoControl(hdevice,
				IOCTL_DOWNLOAD_FX2_EEPROM, &downloadControl,
				sizeof(ANCHOR_DOWNLOAD_CONTROL),
				data,Total_Read, &junk,NULL);
			if (result == FALSE)
				{
				PrintLastErrorAsText();

				if (logfp != (FILE *)NULL)
					fprintf(logfp, "*** File 'USB2a.iic' NOT downloaded!!! ***\n"),
						fclose(logfp),logfp=fopen(logFilename,"a");
				}
			else
				if (logfp != (FILE *)NULL)
					fprintf(logfp, "File 'USB2a.iic' downloaded\n"),
						fclose(logfp),logfp=fopen(logFilename,"a");


			CloseHandle(hdevice);
			}
		else
			{
			// try ms winusb driver
			if (logfp != (FILE *)NULL)
				fprintf(logfp, "Using MS WinUsb driver\n"),
					fclose(logfp),logfp=fopen(logFilename,"a");

			BSIF_SET_LOG(logging);
			return BSIF_DOWNLOAD_FX2_EEPROM();
			}
		}
	else
		{
		fprintf(stderr, "*** File 'USB2a.iic' not found!!! ***\n");
		if (logfp != (FILE *)NULL)
			fprintf(logfp, "*** File 'USB2a.iic' not found!!! ***\n"),
				fclose(logfp),logfp=fopen(logFilename,"a");
		}

	return result;
#endif	// WIN32
	}

//************************
BOOL LINKAGE DOWNLOAD_FX2_IIC_FIRMWARE(void)
//************************
	{
	ProcessIniFile();

	if (logfp != (FILE *)NULL)
		fprintf(logfp, "download_fx2_iic_firmware called\n"),
			fclose(logfp),logfp=fopen(logFilename,"a");
#ifndef WIN32
// Linux
	BSIF_SET_LOG(logging);
	return BSIF_DOWNLOAD_FX2_IIC_FIRMWARE();

#else
// Windows

	BOOL result=FALSE;
	HANDLE hdevice;
	DWORD junk;
	FILE *fp;
	PUCHAR data[MAX_FILE_SIZE];
	ANCHOR_DOWNLOAD_CONTROL downloadControl;

	downloadControl.Offset=0;

	if (((fp = fopen("Vend_Ax.bix","rb")) != NULL) ||
		((fp = fopen("C:/BiosemiCAD/loopback/Vend_ax/Vend_Ax.bix","rb")) != NULL))
		{
		fread(data,sizeof(unsigned char),MAX_FILE_SIZE,fp);				
		fclose(fp);
		
		// try kernel driver first		
		hdevice = CreateFile("\\\\.\\BIOSEMI", GENERIC_WRITE,
			FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
	
		if(hdevice != INVALID_HANDLE_VALUE)
			{
			if (logfp != (FILE *)NULL)
				fprintf(logfp, "Using Biosemi kernel driver\n"),
					fclose(logfp),logfp=fopen(logFilename,"a");

			result = DeviceIoControl(hdevice, IOCTL_DOWNLOAD_FX2,
				&downloadControl, sizeof(ANCHOR_DOWNLOAD_CONTROL),
				data,MAX_FILE_SIZE, &junk, NULL);
			if (result == FALSE)
				{
				PrintLastErrorAsText();

				if (logfp != (FILE *)NULL)
					fprintf(logfp, "*** File 'Vend_Ax.bix' NOT downloaded!!! ***\n"),
						fclose(logfp),logfp=fopen(logFilename,"a");
				}
			else
				if (logfp != (FILE *)NULL)
					fprintf(logfp, "File 'Vend_Ax.bix' downloaded\n"),
						fclose(logfp),logfp=fopen(logFilename,"a");

			CloseHandle(hdevice);
			}
		else
			{
			// try ms winusb driver
			if (logfp != (FILE *)NULL)
				fprintf(logfp, "Using MS WinUsb driver\n"),
					fclose(logfp),logfp=fopen(logFilename,"a");

			BSIF_SET_LOG(logging);
			return BSIF_DOWNLOAD_FX2_IIC_FIRMWARE();
			}
		}
	else
		{
		if (logfp != (FILE *)NULL)
			fprintf(logfp, "*** File 'Vend_Ax.bix' not found!!! ***\n"),
				fclose(logfp),logfp=fopen(logFilename,"a");
		}

	return result;
#endif	// WIN32
	}

//************************
PCHAR LINKAGE GET_DRIVER_INFO(PCHAR infoBuffer, SIZE_T infoSize)
//************************
	{
	char	stringBuffer[200];

	if (driverFound == 3)
		BSIF_GET_DRIVER_INFO(infoBuffer, infoSize);
	else
		{
		if (driverFound == 1)
#ifdef WIN32
#ifdef LEGACY_MSVCRT
			sprintf(stringBuffer, 
#else
			sprintf_s(stringBuffer, sizeof(stringBuffer),
#endif	// LEGACY_MSVCRT
#else	// linux, mac
			snprintf(stringBuffer, sizeof(stringBuffer),
#endif	// WIN32
				"V%s using: BioSemi kernel driver",
				version);
		else
#ifdef WIN32
#ifdef LEGACY_MSVCRT
			sprintf(stringBuffer, 
#else
			sprintf_s(stringBuffer, sizeof(stringBuffer),
#endif	// LEGACY_MSVCRT
#else	// linux, mac
			snprintf(stringBuffer, sizeof(stringBuffer),
#endif	// WIN32
				"V%s: No driver found!",
				version);
	
		int retlen = (int)strlen(stringBuffer);
		if (retlen+1 > (int)infoSize)
			retlen = (int)infoSize - 1;

#ifdef WIN32
#ifdef LEGACY_MSVCRT
		strncpy(infoBuffer, stringBuffer, (SIZE_T)retlen);
#else
		strncpy_s(infoBuffer, (SIZE_T)infoSize, stringBuffer, (SIZE_T)retlen);
#endif	// LEGACY_MSVCRT
#else	// linux, mac
		strncpy(infoBuffer, stringBuffer, (size_t)retlen);
#endif	// WIN32
		infoBuffer[retlen] = 0;
		}

	return infoBuffer;
	}
