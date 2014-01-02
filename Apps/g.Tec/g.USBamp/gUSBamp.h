
//_____________________________________________________________________________
//
//    g.USBamp Windows - Function Prototypes
//    Copyright    (C) Guger Technologies 2004-2005.
//
//	  Created: Laundl 01. 05.2004
//   
//    Last modified : Laundl 06.08.2004
//	  Last modified : Laundl 20.06.2005	
//    Last modified : Laundl 02.12.2005
//_____________________________________________________________________________

#ifdef GUSBAMP_EXPORTS
#define GUSBAMP_API __declspec(dllexport)
#else
#define GUSBAMP_API __declspec(dllimport)
#endif

//_____________________________________________________________________________
//
//							DEFINITIONS
//_____________________________________________________________________________



// FILTER
#define F_CHEBYSHEV		0
#define F_BUTTERWORTH	1
#define F_BESSEL		2

// MODES
#define M_NORMAL		0
#define	M_IMPEDANCE		1
#define	M_CALIBRATE		2
#define M_COUNTER		3

//WAVESHAPES
#define WS_SQUARE		0x01
#define WS_SAWTOOTH		0x02
#define WS_SINE			0x03
#define WS_DRL			0x04
#define WS_NOISE		0x05


#define HEADER_SIZE		38



#ifdef __cplusplus
extern "C" {
#endif


//_____________________________________________________________________________
//
//							DATA STRUCTURES
//_____________________________________________________________________________


typedef struct _DigitalIO
{
   BOOL	 DIN1; // True if set (high voltage level)
   BOOL  DIN2;
   BOOL  DOUT1;
   BOOL  DOUT2;
} DigitalIO, *PDigitalIO;

typedef struct _GND
{
	BOOL GND1;
	BOOL GND2;
	BOOL GND3;
	BOOL GND4;
} GND, *PGND;

typedef struct _REF
{
	BOOL ref1;
	BOOL ref2;
	BOOL ref3;
	BOOL ref4;
} REF, *PREF;

typedef struct _DAC
{
	BYTE WaveShape;
	WORD Amplitude;
	WORD Frequency;
	WORD Offset;
} DAC, *PDAC;

typedef struct _CHANNEL
{
	UCHAR Channel1;
	UCHAR Channel2;
	UCHAR Channel3;
	UCHAR Channel4;
	UCHAR Channel5;
	UCHAR Channel6;
	UCHAR Channel7;
	UCHAR Channel8;
	UCHAR Channel9;
	UCHAR Channel10;
	UCHAR Channel11;
	UCHAR Channel12;
	UCHAR Channel13;
	UCHAR Channel14;
	UCHAR Channel15;
	UCHAR Channel16;
} CHANNEL, *PCHANNEL;


typedef struct _SCALE
{
	float factor[16];
	float offset[16];
} SCALE, *PSCALE;


#ifndef GUSBAMP_EXPORTS

typedef struct _FILT
{
	float fu;
	float fo;
	float fs;
	float type;
	float order;

} FILT, *PFILT;
#endif

//_____________________________________________________________________________
//
//							COMMOMN FUNCTIONS
//_____________________________________________________________________________



GUSBAMP_API		FLOAT	__stdcall	GT_GetDriverVersion(void);
GUSBAMP_API		HANDLE	__stdcall	GT_OpenDevice(int iPortNumber);
GUSBAMP_API		HANDLE	__stdcall	GT_OpenDeviceEx(LPSTR lpSerial);
GUSBAMP_API		BOOL 	__stdcall	GT_CloseDevice(HANDLE *hDevice);
GUSBAMP_API		BOOL	__stdcall	GT_GetData(HANDLE hDevice, BYTE *pData, DWORD dwSzBuffer, OVERLAPPED *ov);
GUSBAMP_API		BOOL	__stdcall	GT_SetBufferSize(HANDLE hDevice, WORD wBufferSize);
GUSBAMP_API		BOOL	__stdcall	GT_SetSampleRate(HANDLE hDevice, WORD wSampleRate);
GUSBAMP_API		BOOL	__stdcall	GT_Start(HANDLE hDevice);
GUSBAMP_API		BOOL	__stdcall	GT_Stop(HANDLE hDevice);
GUSBAMP_API		BOOL	__stdcall	GT_SetChannels(HANDLE hDevice, UCHAR *ucChannels, UCHAR ucSizeChannels);
GUSBAMP_API		BOOL	__stdcall	GT_SetDigitalOut(HANDLE hDevice, UCHAR ucNumber, UCHAR ucValue);
GUSBAMP_API		BOOL	__stdcall	GT_GetDigitalIO(HANDLE hDevice, PDigitalIO pDIO);
GUSBAMP_API		BOOL	__stdcall	GT_GetLastError(WORD * wErrorCode, char *pLastError);
GUSBAMP_API		BOOL	__stdcall	GT_ResetTransfer(HANDLE hDevice);
GUSBAMP_API		BOOL	__stdcall	GT_GetSerial(HANDLE hDevice, LPSTR lpstrSerial,UINT uiSize);
GUSBAMP_API		BOOL	__stdcall	GT_VR(int nargin, void *varargin[],int nargout, void *varargout[]);
GUSBAMP_API		BOOL	__stdcall	GT_EnableTriggerLine(HANDLE hDevice, BOOL bEnable);
GUSBAMP_API		BOOL	__stdcall	GT_GetImpedance(HANDLE hDevice, UCHAR Channel, double* Impedance);
GUSBAMP_API		BOOL	__stdcall	GT_Calibrate(HANDLE hDevice,PSCALE Scaling);

//_____________________________________________________________________________________
//
//									FILTER
//_____________________________________________________________________________________							

GUSBAMP_API		BOOL	__stdcall	GT_SetScale(HANDLE hDevice, PSCALE Scaling);
GUSBAMP_API		BOOL	__stdcall	GT_GetScale(HANDLE hDevice, PSCALE Scaling);

GUSBAMP_API		BOOL	__stdcall	GT_GetFilterSpec(FILT *FilterSpec);
GUSBAMP_API		BOOL	__stdcall	GT_GetNumberOfFilter(int* nof);
GUSBAMP_API		BOOL	__stdcall	GT_SetBandPass(HANDLE hDevice, UCHAR ucChannel, int index);

GUSBAMP_API		BOOL	__stdcall	GT_GetNotchSpec(FILT *FilterSpec);
GUSBAMP_API		BOOL	__stdcall	GT_GetNumberOfNotch(int* nof);
GUSBAMP_API		BOOL	__stdcall	GT_SetNotch(HANDLE hDevice, UCHAR ucChannel, int index);

//_____________________________________________________________________________________
//
//									MODE
//_____________________________________________________________________________________							


GUSBAMP_API		BOOL	__stdcall	GT_SetMode(HANDLE hDevice, UCHAR ucMode);
GUSBAMP_API		BOOL	__stdcall	GT_GetMode(HANDLE hDevice, UCHAR* ucMode);
GUSBAMP_API		BOOL	__stdcall	GT_SetGround(HANDLE hDevice, GND CommonGround);
GUSBAMP_API		BOOL	__stdcall	GT_GetGround(HANDLE hDevice, GND* CommonGround);
GUSBAMP_API		BOOL	__stdcall	GT_SetReference(HANDLE hDevice, REF CommonReference);
GUSBAMP_API		BOOL	__stdcall	GT_GetReference(HANDLE hDevice, REF* CommonReference);
GUSBAMP_API		BOOL	__stdcall	GT_SetBipolar(HANDLE hDevice, CHANNEL bipoChannel);
GUSBAMP_API		BOOL	__stdcall	GT_SetDRLChannel(HANDLE hDevice, CHANNEL drlChannel);
GUSBAMP_API		BOOL	__stdcall	GT_EnableSC(HANDLE hDevice, BOOL bEnable);
GUSBAMP_API		BOOL	__stdcall	GT_SetSlave(HANDLE hDevice, BOOL bSlave);

//_____________________________________________________________________________________
//
//									ANALOG OUTPUT
//_____________________________________________________________________________________							


GUSBAMP_API		BOOL	__stdcall	GT_SetDAC(HANDLE hDevice, DAC AnalogOut);




#ifdef __cplusplus
}
#endif
