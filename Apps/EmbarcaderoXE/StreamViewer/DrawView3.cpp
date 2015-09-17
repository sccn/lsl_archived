//---------------------------------------------------------------------------


#pragma hdrstop

#include "DrawView3.h"

//---------------------------------------------------------------------------
#include <vcl.h>

#include <direct.h>
#include <errno.h>

#include "UNewViewer2.h"
//#include "UXML.h"
//#include "tparams.h"


/////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////
// class CChannelN manipulates a display, draws, updates, turns on and off, etc.
//
// _____________________________IMPORTANT:________________________________
// to make sure that data is displayed correctly, when passing a CDC* to a
// method that draws on the screen, call CRTViewDlg::SetupDC() first to
// set the correct mapping mode for the device context!!!
//

// CONSTRUCTOR:
//
// viewport = specifies the region of the main dialog where the display is
// color = drawing color
// xdim = how many points to displays horizontally before rewinding
// ydim = y range in display becomes from -ydim to +ydim
//
// NOTE: xdim and ydim are the logical dimensions of the display, the
// physical dimensions are given in "viewport"

CChannelN::CChannelN (TImage * pImg, TImage * pCodeImg, int n_Chan)
{
	m_nCurPosition = 0;
	m_nChan=n_Chan;
	pImage = pImg;
	pCodeImage = pCodeImg;

	m_sSize.x=pImage->ClientRect.right;
	m_sSize.y=pImage->ClientRect.bottom;

	m_nVScale=Form1->VScale();

	m_pnDisplayState = new long[m_sSize.x*m_nChan];
	for (int i = 0; i < m_sSize.x*n_Chan; i++) {  //
		m_pnDisplayState[i] = 0;//5- rand() %10;
	}
	allegro_init();
	set_gdi_color_format();
	install_keyboard();
	install_mouse();
	install_timer();

	bmpCanvas = create_bitmap_ex(32,m_sSize.x,m_sSize.y);
	bmpCodeCanvas  = create_bitmap_ex(32,pCodeImage->ClientRect.Width(),Form1->ImageCode->ClientRect.Height());
	m_nColor=makecol24(0,255,255);

	PenYellow= CreatePen(PS_SOLID,1, 0x0000ff00);
	PenBlack = CreatePen(PS_SOLID,1, 0x00000000);
	PenGray  = CreatePen(PS_SOLID,1, clGray);
	PenRed   = CreatePen(PS_SOLID,1, clRed);
}



void CChannelN::PaintBkgr()
{
	rectfill(bmpCanvas,0,0,bmpCanvas->w,bmpCanvas->h, makecol24(0,0,0));
	rectfill(bmpCodeCanvas,0,0,bmpCodeCanvas->w,bmpCodeCanvas->h, makecol24(0,0,0));
}

void CChannelN::PutRandom()
{
	for (int i = 0; i < m_sSize.x; i++) {  //
		m_pnDisplayState[i] =5- rand() %10;
	}
}

__fastcall CChannelN::~CChannelN()
{
	delete [] m_pnDisplayState;
		DeleteObject(PenYellow);
		DeleteObject(PenBlack);
}


// Displays the next piece of incoming data.
//
// sample = new piece of data
// pDC = pointer to the device context of the main window
//



static int smaller(int a, int b) {return  a<b? a:b;};
static int bigger(int a, int b) {return  a>b? a:b;};

//#include "drlib.h"

extern void * hInStream;

int InRange(int val, int Max)
{
	return val < 0? 0:
			val>Max? Max:
			val;
}


extern TStringList * dynarr;


long GetValue(/*TDataRiver *sample,*/int i/*stream number*/,int n/*channel number*/)
{
	long val = 0;
   /*	short itsz =	sample->DataSample[i]->item_size;
	switch (itsz)
	{
		case 2: val = sample->DataSample[i]->S[n]; break;
		case 1: val = sample->DataSample[i]->C[n]; break;
		default:val = sample->DataSample[i]->L[n];
	} */
	return val;
}

int CChannelN::DisplaySample(float *fsample, int nChannels, float samplingRate) {
	double *sample = (double *) malloc(nChannels*sizeof(double));
	for(int i=0; i<nChannels; i++) {
		sample[i] = (double) fsample[i];
	}
	DisplaySample(sample, nChannels, samplingRate);
	free(sample);
}


int CChannelN::DisplaySample(double *sample, int nChannels, float samplingRate)
{
	int ThisPos= m_nCurPosition;
	int PrevPos=(ThisPos+m_sSize.x-1)%m_sSize.x;

	if (PrevPos<ThisPos)
	{
		POINT adjust={0,0};
		POINT FromPoint= {PrevPos,0};
		POINT ToPoint=  {ThisPos,0};
		m_nVScale=Form1->VScale();
		int h=pImage->ClientHeight ;
		int PenCol;


		float bw= 1000.0 / samplingRate;


		LOCK;

		DCTYPE dc;
		GET_DC(dc);
		static int oldGrid=0,newGrid=0;
		oldGrid=newGrid;
		newGrid=ThisPos* bw /100;

		if (oldGrid!=newGrid)
			SET_GRAYPEN(dc);
		else
			SET_BLACKPEN(dc);

		LINE(dc,ToPoint.x,0,ToPoint.x,h,PenCol);//r.bottom-r.top

		RGB c,rgb[8] = {
			0xff,	0,	  0,	0,
			0,	 0xff,	  0,	0,
			0xff,0xff,	  0,	0,
			0,	 0xff, 0xff, 	0,
			0xff,0x80,	  0,	0,
			0x80,0xff, 0x80,	0,
			0xff,	0, 0xff, 	0,
			0xff,0xff, 0xff, 	0
		};
		int bank = Form1->PageControl1->ActivePageIndex;
		int ch_a = 0;
		int disp_ch= 0;
		int chCount = 0;

		for (int i = 0; i < 1/*sample->nStreams*/; i++) //currently only one stream
		{
			chCount = nChannels;//sample->DataSample[i]->nItems;
			/*
			int isEvent = sample->DataSample[i]->Event;
			if (isEvent)
			{
				SET_PEN(dc,	rgb[i].r,
								rgb[i].g,
								rgb[i].b);

				LINE(dc,ToPoint.x,0,ToPoint.x,h,PenCol);//r.bottom-r.top
			}
			else
				int zz = 0;
			ShowCode(m_nCurPosition,isEvent);
			*/

			for (int ch_rd = 0; ch_rd < chCount; ch_rd++)
			{
				ch_a++;
				if (ch_a > bank*32 && ch_a <= bank*32 +32 && disp_ch < 32)
				{
					double val = sample[ch_rd];//GetValue(sample,i,ch_rd);

					double PrevVal=m_pnDisplayState[(PrevPos * m_nChan)+disp_ch];
					adjust.y=(h / m_nChan) * (disp_ch) +(h / m_nChan)/2; //Size.cx
					double ThisVal= - val /m_nVScale + (double) adjust.y;
					ThisVal = InRange(ThisVal,h);
					m_pnDisplayState[(ThisPos * m_nChan)+disp_ch] = ThisVal;
					int chcol = disp_ch % 8;

					if (Form1->ChanIsOn[bank] & (1 << disp_ch))
					{
						FromPoint.y=PrevVal;
						ToPoint.y=ThisVal;
						SET_PEN(dc,rgb[chcol].r,
							rgb[chcol].g,
							rgb[chcol].b
							);
						LINE(dc,FromPoint.x,FromPoint.y,ToPoint.x,ToPoint.y,PenCol);
					}
					disp_ch++;
				}
			}
		}
		UNLOCK;
	}
	int lastPos=	m_nCurPosition++;
	m_nCurPosition = m_nCurPosition % int(m_sSize.x);
	return lastPos;
}

void DrawColorLine(DCTYPE dc,int pos, ULONG val);


double lastTime = 0.0;
void CChannelN::ShowString(double time /*seconds*/, char* buf)
{
	if(int(time) != int(lastTime)/*time - lastTime > 1*/) {
		PaintBkgr();
		lastTime = time;
	}
	double position /*(0..1)*/ = time - double(int(time));


	int h=Form1->ImageCode->ClientHeight;
	int w=Form1->ImageCode->ClientWidth;
	int pos = int(w * position);
	LOCKCODE;
	DCTYPE dc;
	GET_CODEDC(dc);
	int PenCol;


	Form1->Memo1->Lines->Add(buf);
	int tw;
	TEXTWIDTH(dc,buf,tw);
	SET_REDPEN(dc);
	TEXTOUT(dc,buf,pos-tw,1,PenCol); //-tw
	LINE(dc,pos,0,pos,h,PenCol);
	UNLOCKCODE;

		h=Form1->Image3->ClientHeight;
		LOCK(bmpCanvas);
		GET_DC(dc);
		SET_REDPEN(dc);
		LINE(dc,pos,0,pos,h,PenCol);
		UNLOCK(bmpCanvas);


}

void CChannelN::ShowCode(double time /*seconds*/, ULONG code)
{
	char buf[256];
	sprintf(buf,"%d", code);
	ShowString(time, buf);
}

void DrawColorLine(DCTYPE dc,int pos, ULONG val)
{
	static int col[32] = {
		0x00ffffff,
		0x00808080,
		0x000000ff,
		0x0000ffff,
		0x0000ff00,
		0x00ff00ff,
		0x00ffff00,
		0x00ff0000,
		0x00ffffff,
		0x00808080,
		0x000000ff,
		0x0000ffff,
		0x0000ff00,
		0x00ff00ff,
		0x00ffff00,
		0x00ff0000,
		0x00ffffff,
		0x00808080,
		0x000000ff,
		0x0000ffff,
		0x0000ff00,
		0x00ff00ff,
		0x00ffff00,
		0x00ff0000,
		0x00ffffff,
		0x00808080,
		0x000000ff,
		0x0000ffff,
		0x0000ff00,
		0x00ff00ff,
		0x00ffff00,
		0x00ff0000
	};
	if (val <0)
	{
		int jj=0;
	}
	ULONG b=1;
	for (int i=0;i<32; i++)
	{
		ULONG r = (b& val);
		if (r)
		{
			line(dc,pos,i*2,pos,i*2+1,col[i]);
			line(dc,pos-1,i*2,pos-1,i*2+1,col[i]);
		}
		else
		{
			line(dc,pos,i*2,pos,i*2+1,0);
		}
		b*=2;

	}
}

//============================================================================


inline double ROT_ANGLE()
{
	return TO_RAD(Form1->TrackBarRot->Position);
}

inline double TILT_ANGLE()
{
	return TO_RAD(Form1->TrackBarTilt->Position);
}

#define ANGLE_Y TILT_ANGLE()
#define ANGLE_X ROT_ANGLE()


#define CENTER (Form1->Image4->Width/2)
#define GAIN (Form1->TrackBarGain->Position)
void TPos3D::AddSample(double xx,double yy,double zz)
{
		double   ScaleFactor = 1000.0*GAIN/100.0;
		xx *=   ScaleFactor;
		yy *= 	ScaleFactor;  // y is the vertical axis , now is upright
		zz *=   ScaleFactor;  // mirror image?

		oldpos = pos;
		pos++;
		pos%=BUFSIZE3D;

		double ScaleV_Y = Sin(ANGLE_Y);
		double ScaleV_XZ = Cos(ANGLE_Y);

		double sin_x = Sin(ANGLE_X);
		double cos_x = Cos(ANGLE_X);
		xy[pos]=CENTER - xx*cos_x;
		yx[pos]=CENTER - yy*ScaleV_Y +xx*sin_x*ScaleV_XZ;

		double sin_z = Cos(ANGLE_X);
		double cos_z = Sin(ANGLE_X);

		zy[pos]=CENTER + zz*cos_z;
		yz[pos]=CENTER - yy*ScaleV_Y +zz*sin_z*ScaleV_XZ;

		yxz[pos]=CENTER - yy*ScaleV_Y +xx*sin_x*ScaleV_XZ +zz*sin_z*ScaleV_XZ;
		xzy[pos]=xy[pos] + zz*cos_z;
}

TDisplay3D::TDisplay3D(TImage * pImg,int nChan)//:CChannelN(pImg,Form1->ImageCode,nChan)
{
	pImage = pImg;
	int x=pImage->ClientRect.right;
	int y=pImage->ClientRect.bottom;
	bmpCanvasBackground = create_bitmap_ex(32,x,y);
	bmpCanvas = create_bitmap_ex(32,x,y);
	for (int i=0; i < 72; i++)
		Display3DBuffer[i].AddSample(0,0,0);
}

#define XGRID_2 GAIN
#define XGRID (XGRID_2*2)
void PutGrid(BITMAP * dc)
{
	double yr = TO_RAD(Form1->TrackBarTilt->Position);
	float sin_y = Sin(ANGLE_Y);
	float cos_y = Cos(ANGLE_Y);

	float ScaleV_Y = sin_y;
	float ScaleV_XZ = cos_y;
	float sin_x = Sin(ANGLE_X);
	float cos_x = Cos(ANGLE_X);
	int PenCol;
	SET_PEN(dc,128,0,0);
	LINE(dc,CENTER+XGRID*cos_x,CENTER-XGRID*sin_x*ScaleV_XZ-XGRID_2*ScaleV_Y,
			CENTER-XGRID*cos_x,CENTER+XGRID*sin_x*ScaleV_XZ-XGRID_2*ScaleV_Y,PenCol);
	LINE(dc,CENTER+XGRID*cos_x,CENTER-XGRID*sin_x*ScaleV_XZ,
			CENTER-XGRID*cos_x,CENTER+XGRID*sin_x*ScaleV_XZ,PenCol);
	LINE(dc,CENTER+XGRID*cos_x,CENTER-XGRID*sin_x*ScaleV_XZ+XGRID_2*ScaleV_Y,
			CENTER-XGRID*cos_x,CENTER+XGRID*sin_x*ScaleV_XZ+XGRID_2*ScaleV_Y,PenCol);

	LINE(dc	,CENTER+XGRID*cos_x,CENTER-XGRID*sin_x*ScaleV_XZ-XGRID_2*ScaleV_Y
			,CENTER+XGRID*cos_x,CENTER-XGRID*sin_x*ScaleV_XZ+XGRID_2*ScaleV_Y			,PenCol);
	LINE(dc	,CENTER+XGRID_2*cos_x,CENTER-XGRID_2*sin_x*ScaleV_XZ-XGRID_2*ScaleV_Y
			,CENTER+XGRID_2*cos_x,CENTER-XGRID_2*sin_x*ScaleV_XZ+XGRID_2*ScaleV_Y			,PenCol);
	LINE(dc	,CENTER-XGRID*cos_x,CENTER+XGRID*sin_x*ScaleV_XZ-XGRID_2*ScaleV_Y
			,CENTER-XGRID*cos_x,CENTER+XGRID*sin_x*ScaleV_XZ+XGRID_2*ScaleV_Y			,PenCol);
	LINE(dc	,CENTER-XGRID_2*cos_x,CENTER+XGRID_2*sin_x*ScaleV_XZ-XGRID_2*ScaleV_Y
			,CENTER-XGRID_2*cos_x,CENTER+XGRID_2*sin_x*ScaleV_XZ+XGRID_2*ScaleV_Y			,PenCol);

	float sin_z = cos_x;//Sin(ANGLE_Z);
	float cos_z = sin_x;//Cos(ANGLE_Z);

	SET_PEN(dc,0,96,96);//                SelectObject(dc,PenYellow);
	LINE(dc,CENTER-XGRID*cos_z, CENTER - XGRID*sin_z*ScaleV_XZ - XGRID_2*ScaleV_Y,
			CENTER+XGRID*cos_z, CENTER + XGRID*sin_z*ScaleV_XZ - XGRID_2*ScaleV_Y,PenCol);
	LINE(dc,CENTER-XGRID*cos_z, CENTER - XGRID*sin_z*ScaleV_XZ,//*ScaleV_Y    ,
			CENTER+XGRID*cos_z, CENTER + XGRID*sin_z*ScaleV_XZ /**ScaleV_Y*/    ,PenCol);
	LINE(dc,CENTER-XGRID*cos_z, CENTER - XGRID*sin_z*ScaleV_XZ +XGRID_2*ScaleV_Y,
			CENTER+XGRID*cos_z, CENTER + XGRID*sin_z*ScaleV_XZ +XGRID_2*ScaleV_Y,PenCol);

	LINE(dc,CENTER+XGRID*cos_z, CENTER + XGRID*sin_z*ScaleV_XZ-XGRID_2*ScaleV_Y
		   ,CENTER+XGRID*cos_z, CENTER + XGRID*sin_z*ScaleV_XZ+XGRID_2*ScaleV_Y			,PenCol);
	LINE(dc	,CENTER-XGRID_2*cos_z,CENTER-XGRID_2*sin_z*ScaleV_XZ-XGRID_2*ScaleV_Y
			,CENTER-XGRID_2*cos_z,CENTER-XGRID_2*sin_z*ScaleV_XZ+XGRID_2*ScaleV_Y			,PenCol);
	LINE(dc,CENTER-XGRID*cos_z, CENTER - XGRID*sin_z*ScaleV_XZ-XGRID_2*ScaleV_Y
		   ,CENTER-XGRID*cos_z, CENTER - XGRID*sin_z*ScaleV_XZ+XGRID_2*ScaleV_Y			,PenCol);
	LINE(dc	,CENTER+XGRID_2*cos_z,CENTER+XGRID_2*sin_z*ScaleV_XZ-XGRID_2*ScaleV_Y
			,CENTER+XGRID_2*cos_z,CENTER+XGRID_2*sin_z*ScaleV_XZ+XGRID_2*ScaleV_Y			,PenCol);

	SET_PEN(dc,64,64,64);//                SelectObject(dc,PenYellow);
	LINE(dc,CENTER,CENTER-XGRID_2*ScaleV_Y,
			CENTER,CENTER+XGRID_2*ScaleV_Y,PenCol);
	circlefill(dc,
			CENTER+XGRID*cos_z, CENTER + XGRID*sin_z*ScaleV_XZ //*sin_y
			,3,makecol24(0,128,0));
	circlefill(dc,
			CENTER,CENTER-XGRID_2*ScaleV_Y
			,3,makecol24(128,128,0));
}

void TDisplay3D::PaintBkgr()
{
	rectfill(bmpCanvasBackground,0,0,
			bmpCanvasBackground->w,
			bmpCanvasBackground->h,
			makecol24(0,0,0));
	PutGrid(bmpCanvasBackground);
	blit(bmpCanvasBackground, bmpCanvas, 0, 0, 0, 0, bmpCanvas->w, bmpCanvas->h);
}



#include "ULeds.h"
double x0=0,y0=0,z0=0;
bool zero=false;


int TDisplay3D::DisplaySample(float *fsample, int nChannels, int channelsPerMarker, float samplingRate) {
	double *sample = (double *) malloc(nChannels*sizeof(double));
	for(int i=0; i<nChannels; i++) {
		sample[i] = (double) fsample[i];
	}
	DisplaySample(sample, nChannels, channelsPerMarker, samplingRate);
	free(sample);
}

int TDisplay3D::DisplaySample(/*TDataRiver *sample*/double *sample, int nChannels, int channelsPerMarker, float samplingRate)
{
  //	TMaxArray * pMS = (TMaxArray * )sample;
	DCTYPE dc;
	GET_DC(dc);
	int PenCol;
	int pos1;
	double x,y,z;

	blit(bmpCanvasBackground, bmpCanvas, 0, 0, 0, 0, bmpCanvas->w, bmpCanvas->h);



	//int i = Form1->udMoCh->Position;


	if (zero)
	{
		x0 = sample[0];//GetValue(sample,i,0);
		y0 = sample[1];//GetValue(sample,i,1);
		z0 = sample[2];//GetValue(sample,i,2);
		zero=false;
	}

	int chCount = nChannels/channelsPerMarker;
	if (chCount > Form1->UpDown2->Position)
		chCount = Form1->UpDown2->Position;
	for (int ch = 0; ch < chCount; ch++)
		if (FormLEDs->CheckBoxLED[ch]->Checked)
		{
			x =(sample[ch*channelsPerMarker]-x0)* Form1->TrackBarGain->Position/100;
			y =(sample[1+ch*channelsPerMarker]-y0)* Form1->TrackBarGain->Position/100;
			z =(sample[2+ch*channelsPerMarker]-z0)* Form1->TrackBarGain->Position/100;;

			Display3DBuffer[ch].AddSample(x,y,z);
			pos1 = Display3DBuffer[ch].pos;


			int r = 255;
			int g = 255;
			if (x<0) r = 128;
			if (z<0) g = 128;
			int is3 = (ch % 3)==2;
			if (Form1->CheckBoxZY->Checked)
			{
				circlefill(dc,
						Display3DBuffer[ch].zy[pos1],
						Display3DBuffer[ch].yz[pos1],
						4,makecol24(0,96,96));
				if (Form1->CheckBoxXYZ->Checked)
				{
						SET_PEN(dc,180,0,0);//
						LINE(dc,Display3DBuffer[ch].xzy[pos1],
							Display3DBuffer[ch].yxz[pos1],
							Display3DBuffer[ch].zy[pos1],
							Display3DBuffer[ch].yz[pos1],PenCol);
				}
			}
			if (Form1->CheckBoxXY->Checked)
			{
				circlefill(dc,
						Display3DBuffer[ch].xy[pos1],
						Display3DBuffer[ch].yx[pos1],
						4,makecol24(96,0,0));
				if (Form1->CheckBoxXYZ->Checked)
				{
						SET_PEN(dc,0,96,96);//                SelectObject(dc,PenYellow);
						LINE(dc,Display3DBuffer[ch].xzy[pos1],
							Display3DBuffer[ch].yxz[pos1],
							Display3DBuffer[ch].xy[pos1],
							Display3DBuffer[ch].yx[pos1],PenCol);
				}
			}
			if (Form1->CheckBoxXYZ->Checked)
			{
				static int n=0;
				circlefill(dc,
						Display3DBuffer[ch].xzy[pos1],
						Display3DBuffer[ch].yxz[pos1],
						4,makecol24(255,255,0));
				n++;
				if (n % 10 == 0)
				if (is3 && Form1->CheckBoxConn3->Checked)
				{
						SET_PEN(dc,0,255,0);//                SelectObject(dc,PenYellow);
						LINE(dc,
							Display3DBuffer[ch-1].xzy[pos1],
							Display3DBuffer[ch-1].yxz[pos1],
							Display3DBuffer[ch].xzy[pos1],
							Display3DBuffer[ch].yxz[pos1],
							PenCol);
						SET_PEN(dc,0,255,255);//                SelectObject(dc,PenYellow);
						LINE(dc,
							CENTER,
							CENTER,
							Display3DBuffer[ch-2].xzy[pos1],
							Display3DBuffer[ch-2].yxz[pos1],
							PenCol);
				}
			}
	}
	return 0;// does not matter
}


#pragma package(smart_init)
