//---------------------------------------------------------------------------

#ifndef ufilterH
#define ufilterH
//---------------------------------------------------------------------------
//#include "dataarraysx.h"
class TCumulBuffer
{
	double cumulBuf[640];
	int cnt;
public:
	TCumulBuffer();
	void ClearBuf();
	void AddSample(float  * pNewSample, int nChans);
};
extern TCumulBuffer CumulBuf;
int LowPass(int bufsize, float * pNewSample);
//int Bessel512x4(TMaxArray * input_val);
int Bessel256x4(float * pNewSample); // 4-th order  Bessel
//int Butterworth512x4(TMaxArray * pNewSample); // 4-th order  Butterworth
int Butterworth256x4(float * pNewSample); // 4-th order  Butterworth
int Fir(float * pNewSample);
int LoBessel512(float * pNewSample);

enum FREQ {F128,F256,F512,NONE};
int Bessel_x4(FREQ fr,float * pNewSample); // 4-th order  Bessel

#endif
