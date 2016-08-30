//---------------------------------------------------------------------------


#pragma hdrstop

#include "ufilter.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)


#define bufsize 256
float buf[bufsize][640];//[bufsize][maxChannels]
void TCumulBuffer::ClearBuf()
	{
		cnt=0;

		for(int j=0; j<640; j++) {
			cumulBuf[j] = 0;
		}
		for(int i=0; i<256; i++) {
			for(int j=0; j<640; j++) {
				buf[i][j] = 0.0;
			}
		}
	};

TCumulBuffer::TCumulBuffer()
	{
			ClearBuf();
	};


void TCumulBuffer::AddSample(float  * pNewSample, int nChans)
	{


		int out = bufsize +cnt;
		out %= int(bufsize);

		//buf[out].drf.nItems = pNewSample->drf.nItems;// SEE THIS! nEegChans;
		if (cnt>bufsize)
			for (int i=0;i< nChans/*buf[out].drf.nItems*/;i++)
			{
				cumulBuf[i] -= buf[out][i];
			}

		for(int i = 0; i<nChans; i++) {

			buf[out][i] = pNewSample[i];//Op1;
		}

		for (int i=0;i< nChans/*buf[out].drf.nItems*/;i++)
		{

			cumulBuf[i] += buf[out][i];
			long m =cumulBuf[i]/ (bufsize+1);
			pNewSample[i] -= m;
		}
		cnt++;
	};

TCumulBuffer CumulBuf;

 /*
int LowPass(int fltsize, TMaxArray * pNewSample)
{

	static int oldfltsize=fltsize;
//	static __int64 cumulBuf[400]; // need 289
	static double cumulBuf[400]; // need 289
	static __int64 n=-1;
	if (oldfltsize != fltsize) n = -1;
	if (n==-1)
	{
		memset(cumulBuf,0,sizeof cumulBuf);
		n=0;
	}

//	static TMaxArray  buf[100];
	static double  buf[100][400];

	if (fltsize==1) return pNewSample->drf.nItems;
	double bsz = fltsize;
	int indexin = n%fltsize;
	for (int i=0;i<pNewSample->drf.nItems;i++)   // code is at 0
	{
		double val = pNewSample->Data[i]/bsz;
//		buf[indexin].Data[i]= val;
		buf[indexin][i]= val;
		cumulBuf [i] += val;
	}

	n++;
	int indexout = n%fltsize;
	int nch = 0;
	for (int i=0;i<pNewSample->drf.nItems;i++)   // code is at 0
	{
		nch++;
		if (n>fltsize)
			cumulBuf[i] -= buf[indexout][i];
		pNewSample->Data[i]=cumulBuf[i];
	}
	oldfltsize = fltsize;
	return nch;
};



#define NZEROS 4
#define NPOLES 4

static float xv[640][NZEROS+1], yv[640][NPOLES+1];


//enum FREQ {F256,F512};

const double Gain[sizeof(FREQ)] = {2.958252661e+02,3.638524126e+01,6.118884437e+00};
const double COEF[sizeof(FREQ)][4] = {
			{-0.1553203977,	 0.9281300733, -2.1569887450,  2.3300930875},
			{-0.0236412934,	 0.1849849897, -0.6166927902,  1.0156103580},
			{-0.0149935159,	-0.1325001994, -0.5410799111, -0.9262820982}
			};

//#define GAIN_BESSEL4   3.638524126e+01

int Bessel_x4(FREQ fr,TMaxArray * pNewSample) // 4-th order  Bessel
{
	for (int i=0;i<pNewSample->drf.nItems;i++)   // code is at 0
	{
		xv[i][0] = xv[i][1];
		xv[i][1] = xv[i][2];
		xv[i][2] = xv[i][3];
		xv[i][3] = xv[i][4];
		xv[i][4] = pNewSample->Data[i] / Gain[fr];//GAIN_BESSEL4;
		yv[i][0] = yv[i][1];
		yv[i][1] = yv[i][2];
		yv[i][2] = yv[i][3];
		yv[i][3] = yv[i][4];
		yv[i][4] =   (xv[i][0] + xv[i][4]) + 4 * (xv[i][1] + xv[i][3]) + 6 * xv[i][2]
//					 + ( -0.0236412934 * yv[i][0]) + (  0.1849849897 * yv[i][1])
//					 + ( -0.6166927902 * yv[i][2]) + (  1.0156103580 * yv[i][3]);
					 + ( COEF[fr][0] * yv[i][0]) + (  COEF[fr][1]  * yv[i][1])
					 + ( COEF[fr][2]  * yv[i][2]) + ( COEF[fr][3]  * yv[i][3]);
		pNewSample->Data[i] = yv[i][4];
	  }
  }

#define GAIN_LOW_BESSEL512 6.118884437e+00
int LoBessel512(TMaxArray * pNewSample) // 4-th order  Butterworth
{
	for (int i=0;i<pNewSample->drf.nItems;i++)   // code is at 0
	{
		xv[i][0] = xv[i][1];
		xv[i][1] = xv[i][2];
		xv[i][2] = xv[i][3];
		xv[i][3] = xv[i][4];
		xv[i][4] = pNewSample->Data[i] / GAIN_LOW_BESSEL512;
		yv[i][0] = yv[i][1];
		yv[i][1] = yv[i][2];
		yv[i][2] = yv[i][3];
		yv[i][3] = yv[i][4];

		yv[i][4] =   (xv[i][0] + xv[i][4]) + 4 * (xv[i][1] + xv[i][3]) + 6 * xv[i][2]
					 + ( -0.0149935159 * yv[i][0]) + ( -0.1325001994 * yv[i][1])
					 + ( -0.5410799111 * yv[i][2]) + ( -0.9262820982 * yv[i][3]);


		pNewSample->Data[i] = yv[i][4];
	  }
  }




//#define GAIN_BUTTERWORTH4   1.064046542e+01
#define GAIN_BUTTERWORTH4   9.794817390e+01


//int Butterworth512x4(TMaxArray * pNewSample) // 4-th order  Butterworth
int Butterworth256x4(TMaxArray * pNewSample) // 4-th order  Butterworth
{
	for (int i=0;i<pNewSample->drf.nItems;i++)   // code is at 0
	{
		xv[i][0] = xv[i][1];
		xv[i][1] = xv[i][2];
		xv[i][2] = xv[i][3];
		xv[i][3] = xv[i][4];
		xv[i][4] = pNewSample->Data[i] / GAIN_BUTTERWORTH4;
		yv[i][0] = yv[i][1];
		yv[i][1] = yv[i][2];
		yv[i][2] = yv[i][3];
		yv[i][3] = yv[i][4];
		yv[i][4] =   (xv[i][0] + xv[i][4]) + 4 * (xv[i][1] + xv[i][3]) + 6 * xv[i][2]
//					 + ( -0.0176648009 * yv[i][0]) + ( -0.0000000000 * yv[i][1])
//					 + ( -0.4860288221 * yv[i][2]) + ( -0.0000000000 * yv[i][3]);
					 + ( -0.1203895999 * yv[i][0]) + (  0.7244708295 * yv[i][1])
					 + ( -1.7358607092 * yv[i][2]) + (  1.9684277869 * yv[i][3]);
		pNewSample->Data[i] = yv[i][4];
	  }
  }

#define NTAP 19

double FIR[NTAP] = {	0.00484874022568424,0.0118031915315730,0.00977043538849916,-0.0101797506598715,-0.0389718530767887,
						-0.0417118589387027,0.0174456999807252,0.135480622103821,0.257069330562621,0.308890885764878,0.257069330562621,
						0.135480622103821,0.0174456999807252,-0.0417118589387027,-0.0389718530767887,-0.0101797506598715,0.00977043538849916,0.0118031915315730,0.00484874022568424};
static double xvf[640][NTAP];
static double yvf[640][NTAP];

int Fir(TMaxArray * pNewSample)
{

	for (int i=0;i<pNewSample->drf.nItems;i++)   // code is at 0
	{
		for (int j  = 1; j < NTAP; j++)
		{
			xvf[i][j-1] = xvf[i][j];
		}
		xvf[i][NTAP-1] = pNewSample->Data[i];


		double out = 0;
		for (int j  = 0; j < NTAP; j++)
		{
			yvf[i][j-1] = xvf[i][j]*FIR[j];
			out+= yvf[i][j-1];
		}

		pNewSample->Data[i] = out;
	}
	return pNewSample->drf.nItems;
}


	 */
