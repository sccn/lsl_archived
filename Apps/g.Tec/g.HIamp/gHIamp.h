//_____________________________________________________________________________________
//
//    g.HIamp Windows - Function Prototypes
//    Copyright (c) 2010-2012 Guger Technologies.
//_____________________________________________________________________________________

#ifndef GHIAMP_H
#define GHIAMP_H

#pragma pack(push)

#ifdef GHIAMP_EXPORTS
#define GHIAMP_API __declspec(dllexport)
#else
#define GHIAMP_API __declspec(dllimport)
#endif

//_____________________________________________________________________________________
//
//									DEFINITIONS
//_____________________________________________________________________________________

// FILTER
#define F_CHEBYSHEV		0
#define F_BUTTERWORTH	1
#define F_BESSEL		2

//TESTSIGNAL
#define WS_SQUARE		0x01
#define TESTSIGNAL_AMPLITUDE	7622.83
#define TESTSIGNAL_OFFSET		-TESTSIGNAL_AMPLITUDE

//DEVICEINFO SIZE
#define EEPROM_SIZE_BYTES		64*1024
#define MAX_NUMBER_OF_CHANNELS	256
#define MAX_USB_PACKET_SIZE		1024

#ifdef __cplusplus
extern "C" {
#endif


//_____________________________________________________________________________________
//
//									DATA STRUCTURES
//_____________________________________________________________________________________

#pragma pack(1)
typedef struct _DAC
{
	BOOL Enabled;
	BYTE WaveShape;
	double Amplitude;
	WORD Frequency;
	double Offset;

	_DAC()
		: Amplitude(TESTSIGNAL_AMPLITUDE), Offset(TESTSIGNAL_OFFSET), Enabled(FALSE), WaveShape(WS_SQUARE), Frequency(0)
	{
	}
} DAC, *PDAC;

#pragma pack(1)
typedef struct _SCALE
{
	float factor[MAX_NUMBER_OF_CHANNELS];
	float offset[MAX_NUMBER_OF_CHANNELS];
} SCALE, *PSCALE;

#pragma pack(1)
typedef struct _GT_HIAMP_CHANNEL_IMPEDANCES
{
	BOOL IsActiveElectrode[MAX_NUMBER_OF_CHANNELS];
	double Impedance[MAX_NUMBER_OF_CHANNELS];
} GT_HIAMP_CHANNEL_IMPEDANCES, *PGT_HIAMP_CHANNEL_IMPEDANCES;

#pragma pack(1)
typedef struct _GT_DEVICEINFO
{
	char deviceInfo[EEPROM_SIZE_BYTES];
} GT_DEVICEINFO, *PGT_DEVICEINFO;

#pragma pack(1)
typedef struct _FILT
{
	float fu;
	float fo;
	float fs;
	float type;
	float order;
} FILT, *PFILT;

#pragma pack(1)
typedef struct _GT_HIAMP_CHANNEL_CONFIGURATION
{
	WORD ChannelNumber;
	BOOL Available;
	BOOL Acquire;
	int BandpassFilterIndex;
	int NotchFilterIndex;
	WORD BipolarChannel;
} GT_HIAMP_CHANNEL_CONFIGURATION, *PGT_HIAMP_CHANNEL_CONFIGURATION;

#pragma pack(1)
typedef struct _GT_HIAMP_CONFIGURATION
{
	WORD SampleRate;
	WORD BufferSize;
	BOOL TriggerLineEnabled;
	BOOL HoldEnabled;
	BOOL IsSlave;
	BOOL CounterEnabled;
	DAC InternalSignalGenerator;
	GT_HIAMP_CHANNEL_CONFIGURATION Channels[MAX_NUMBER_OF_CHANNELS];
} GT_HIAMP_CONFIGURATION, *PGT_HIAMP_CONFIGURATION;

//_____________________________________________________________________________________
//
//									COMMON FUNCTIONS
//_____________________________________________________________________________________

GHIAMP_API		FLOAT	__stdcall	GT_GetDriverVersion(void);
GHIAMP_API		FLOAT	__stdcall	GT_GetHWVersion(HANDLE hDevice);
GHIAMP_API		HANDLE	__stdcall	GT_OpenDevice(int iPortNumber);
GHIAMP_API		HANDLE	__stdcall	GT_OpenDeviceEx(LPSTR lpSerial);
GHIAMP_API		BOOL 	__stdcall	GT_CloseDevice(HANDLE *hDevice);
GHIAMP_API		BOOL	__stdcall	GT_GetData(HANDLE hDevice, BYTE *pData, DWORD dwSzBuffer, OVERLAPPED *ov);
GHIAMP_API		BOOL	__stdcall	GT_GetOverlappedResult(HANDLE hDevice, LPOVERLAPPED lpOverlapped, LPDWORD lpNumberOfBytesTransferred, BOOL bWait);
GHIAMP_API		BOOL	__stdcall	GT_Start(HANDLE hDevice);
GHIAMP_API		BOOL	__stdcall	GT_Stop(HANDLE hDevice);
GHIAMP_API		BOOL	__stdcall	GT_GetLastError(WORD *wErrorCode, char *pLastError);
GHIAMP_API		BOOL	__stdcall	GT_ResetTransfer(HANDLE hDevice);
GHIAMP_API		BOOL	__stdcall	GT_GetSerial(HANDLE hDevice, LPSTR lpstrSerial, UINT uiSize);
GHIAMP_API		BOOL	__stdcall	GT_VR(int nargin, void *varargin[], int nargout, void *varargout[]);
GHIAMP_API		BOOL	__stdcall	GT_Calibrate(HANDLE hDevice, PSCALE scaling);
GHIAMP_API		BOOL	__stdcall	GT_SetScale(HANDLE hDevice, PSCALE scaling);
GHIAMP_API		BOOL	__stdcall	GT_GetScale(HANDLE hDevice, PSCALE scaling);
GHIAMP_API		BOOL	__stdcall	GT_GetFactoryScaleSettings(HANDLE hDevice, PSCALE factoryScaling);
GHIAMP_API		BOOL	__stdcall	GT_GetImpedance(HANDLE hDevice, GT_HIAMP_CHANNEL_IMPEDANCES *channelImpedances);
GHIAMP_API		BOOL	__stdcall	GT_GetAvailableChannels(HANDLE hDevice, UCHAR *availableChannels, WORD availableChannelsLength); 
GHIAMP_API		BOOL	__stdcall	GT_GetNumberOfSupportedSampleRates(HANDLE hDevice, int *numberOfSupportedSampleRates);
GHIAMP_API		BOOL	__stdcall	GT_GetSupportedSampleRates(HANDLE hDevice, float* supportedSampleRates);
GHIAMP_API		BOOL	__stdcall	GT_SetConfiguration(HANDLE hDevice, GT_HIAMP_CONFIGURATION configuration);
GHIAMP_API		BOOL	__stdcall	GT_GetConfiguration(HANDLE hDevice, GT_HIAMP_CONFIGURATION *configuration);


//_____________________________________________________________________________________
//
//									FILTER
//_____________________________________________________________________________________							

GHIAMP_API		BOOL	__stdcall	GT_GetFilterSpec(FILT *filterSpec);
GHIAMP_API		BOOL	__stdcall	GT_GetNumberOfFilter(int* nof);

GHIAMP_API		BOOL	__stdcall	GT_GetNotchSpec(FILT *filterSpec);
GHIAMP_API		BOOL	__stdcall	GT_GetNumberOfNotch(int* nof);

#ifdef __cplusplus
}
#endif

#pragma pack(pop)

#endif //GHIAMP_H