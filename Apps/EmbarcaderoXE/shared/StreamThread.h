//---------------------------------------------------------------------------

#ifndef StreamThreadH
#define StreamThreadH
//---------------------------------------------------------------------------
#include <vcl.h>
#ifdef _DEBUG
	#undef _DEBUG  //prevent loading of debug version of library.
	#include "lsl_c.h" //must be "", not <>, otherwise get data corruption in console apps when using dll functions
	#define _DEBUG
#else
	#include "lsl_c.h"
#endif

#include "general_util.h"
typedef void (* TProcessDataI32)(int * data, int size, double samplingRate);

typedef void (* TProcessDataF32)(float * data, int size, double samplingRate);

typedef void (* TProcessDataF64)(double * data, int size, double samplingRate);

typedef void (* TProcessDataString) (char * data, int size, double samplingRate);

#define MAX_CHANNELS 512

class TStreamThread: public TThread
{
public:
	lsl_streaminfo info;	/* the streaminfo returned by the resolve call */
	lsl_inlet inlet;		/* a stream inlet to get samples from */
	int errcode;			/* error code (lsl_lost_error or timeouts) */
	int nChannels;
	bool connected;
	char typeName[512];
	char * xmlHeader;


	long count;
	long resampleCount;

	int Tail;
	TProcessDataI32 ProcessDataI32;
	TProcessDataF32 ProcessDataF32;
	TProcessDataF64 ProcessDataF64;
	TProcessDataString ProcessDataString;
	TStreamThread(char * typeNam);
	TStreamThread(char * typeNam, TProcessDataI32 PD);
	TStreamThread(char * typeNam, TProcessDataF32 PD);
	TStreamThread(char * typeNam, TProcessDataF64 PD);
	TStreamThread(char * typeNam, TProcessDataString PD);
	__fastcall ~TStreamThread();
	void __fastcall Execute();
	void SetResamplingRate(double rsamplingRate);
    double GetResamplingRate();
private:
	int i32buf[MAX_CHANNELS];
	float f32buf[MAX_CHANNELS];
	double f64buf[MAX_CHANNELS];
	char *sbuf;
	char *multi_sbuf[MAX_CHANNELS];
	double samplingRate;
	double resamplingRate;
};

#endif
