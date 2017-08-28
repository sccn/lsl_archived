//---------------------------------------------------------------------------


#pragma hdrstop

#include "DrawView2.h"

//---------------------------------------------------------------------------
#include <vcl.h>

#include <direct.h>
#include <errno.h>

#include "UNewViewer2.h"
#include "UXML.h"
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
//	m_nFromCh=0;//Form1->FromChannel;

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
	bmpCodeCanvas2 = create_bitmap_ex(32,Form1->Image2->ClientRect.Width(),Form1->Image2->ClientRect.Height());
	m_nColor=makecol24(0,255,255);

	PenYellow= CreatePen(PS_SOLID,1, 0x0000ff00);
	PenBlack = CreatePen(PS_SOLID,1, 0x00000000);
	PenGray  = CreatePen(PS_SOLID,1, clGray);
	PenRed   = CreatePen(PS_SOLID,1, clRed);
}


#define MAXVAL 256.0///m_nVScale)//ScaleFactor) // temp!

void CChannelN::PaintBkgr()
{
	rectfill(bmpCanvas,0,0,bmpCanvas->w,bmpCanvas->h, makecol24(0,0,0));
	rectfill(bmpCodeCanvas,0,0,bmpCodeCanvas->w,bmpCodeCanvas->h, makecol24(0,0,0));
	rectfill(bmpCodeCanvas2,0,0,bmpCodeCanvas2->w,bmpCodeCanvas2->h, makecol24(0,0,0));
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

#include "drlib.h"

//extern TMMFhandle * hInStream;
extern void * hInStream;

int InRange(int val, int Max)
{
	return val < 0? 0:
			val>Max? Max:
			val;
}


extern TStringList * dynarr;

union Tul
{
	long l;
	unsigned long u;
};

int CChannelN::DisplaySample(TMaxArray *sample)
{
	int ThisPos= m_nCurPosition;
	int PrevPos=(ThisPos+m_sSize.x-1)%m_sSize.x;

	if (PrevPos<ThisPos)
	{
		int ch;
		POINT adjust={0,0};
		POINT FromPoint= {PrevPos,0};
		POINT ToPoint=  {ThisPos,0};
		m_nVScale=Form1->VScale();
		int h=pImage->ClientHeight ;
		int PenCol;

		LOCK;

		DCTYPE dc;
		GET_DC(dc);
		static int oldGrid=0,newGrid=0;


		TXMLDocument * xd = new TXMLDocument(Form1);
		xd->Active = true;
		AnsiString sx = ds_GetXMLPtr(ds_WrHandle(hInStream));
		xd->LoadFromXML(sx);
		float sr = GetAttribute(xd->DocumentElement,"sampling_rate");
		delete xd;
		float bw= 1000.0 / sr;


		oldGrid=newGrid;
		newGrid=ThisPos* bw /100;

		if (oldGrid!=newGrid)
			SET_GRAYPEN(dc);
		else
			SET_BLACKPEN(dc);

		bool isEvent = low8((*sample)[0]);
		for (int i = 0;i < dynarr->Count-1;i++)
		{
			TFindNode * pFN = (TFindNode * ) dynarr->Objects[i];
			isEvent = (*sample)[pFN->i]; //evch
		}
		if (isEvent)
			SET_REDPEN(dc);

		LINE(dc,ToPoint.x,0,ToPoint.x,h,PenCol);//r.bottom-r.top

		SET_YELLOWPEN(dc);//                SelectObject(dc,PenYellow);
		RGB c,rgb[6] = {
			0xff,	0,	0,	0,
			0,	 0xff,	0,	0,
			0xff,0xff,	0,	0,
			0,	 0xff, 0xff, 0,
			0xff,0xff, 0xff, 0,
			0xff, 	0,	0xff, 0
		};
		int bank = Form1->PageControl1->ActivePageIndex;
		int m_nFromCh=bank*32;
		for (ch=0;ch<m_nChan;ch++)
		{
			int chmask = ch ;
			int chcol = ch % 6;

			if (Form1->ChanIsOn[bank] & (1 << chmask))
			{
				SET_PEN(dc,rgb[chcol].r,
						rgb[chcol].g,
						rgb[chcol].b
						);//                SelectObject(dc,PenYellow);
				long PrevVal=m_pnDisplayState[(PrevPos * m_nChan)+ch];
				long chNmb = ch+m_nFromCh;
				if (chNmb<sample->nItems)
				{
					adjust.y=(h / m_nChan) * ch +(h / m_nChan)/2; //Size.cx
					long val = (*sample)[chNmb];
					long ThisVal= - val /m_nVScale + adjust.y;
					ThisVal = InRange(ThisVal,h);
					m_pnDisplayState[(ThisPos * m_nChan)+ch] =ThisVal;

					FromPoint.y=PrevVal;
					ToPoint.y=ThisVal;
					LINE(dc,FromPoint.x,FromPoint.y,ToPoint.x,ToPoint.y,PenCol);
				}
			}
		}
		UNLOCK;
	}
	int lastPos=	m_nCurPosition++;
	try {
		ULONG code = (*sample)[0];
		if (code != 0) {
			int zz=0;
		}
		ShowCode(lastPos,code);
		m_nCurPosition = m_nCurPosition % int(m_sSize.x);
	} catch (...) {
	}
	return lastPos;
}

void DrawColorLine(DCTYPE dc,int pos, ULONG val);

void CChannelN::ShowCode(int pos, ULONG code)
{
		int h=Form1->ImageCode->ClientHeight;
		LOCKCODE;
		DCTYPE dc;
		GET_CODEDC(dc);
		int PenCol;
		static int newcode=0;
		int oldcode = newcode;
		if (code) newcode = code;
		if (low8(code))
		{
				char buf[10];
				char buf2[20];
				sprintf(buf,"%d(%c)",code, code & 0x7f);
				switch (low8(code))
				{
					case 0:;
					case 1:;
					case 4:;
					case 5:;
					case 6:;
					case 16:;
					case 129:break;

					default:
					sprintf(buf2,"%x,%x",oldcode,code);
					Form1->Memo1->Lines->Add(buf2);
				}
				int tw;
				TEXTWIDTH(dc,buf,tw);
//                SET_YELLOWPEN(dc);
				SET_REDPEN(dc);
				TEXTOUT(dc,buf,pos-tw,1,PenCol); //-tw
				LINE(dc,pos,0,pos,h,PenCol);

		}
		else
		{
				SET_BLACKPEN(dc);
				LINE(dc,pos,0,pos,h,PenCol);
		}
		UNLOCKCODE;

		h=Form1->Image2->ClientHeight;
		LOCKCODE2(bmpCodeCanvas2);
		GET_CODEDC2(dc,bmpCodeCanvas2);
		DrawColorLine(dc,pos,code);
		UNLOCKCODE2(bmpCodeCanvas2);


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
#define GAIN (Form1->TrackBarGain->Position) // *3)

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
//#define ANGLE_Z (PI/2 -ANGLE_X)

#define CENTER (Form1->Image4->Width/2)

void TPos3D::AddSample(int xx,int yy,int zz)
{
		float ScaleFactor = GAIN/100.0;
		xx *= ScaleFactor;
		yy *= ScaleFactor;
		zz *= ScaleFactor;

		oldpos = pos;
		pos++;
		pos%=BUFSIZE3D;

		float ScaleV_Y = Sin(ANGLE_Y);
		float ScaleV_XZ = Cos(ANGLE_Y);

		float sin_x = Sin(ANGLE_X);
		float cos_x = Cos(ANGLE_X);
		xy[pos]=CENTER - xx*cos_x;
		yx[pos]=CENTER - yy*ScaleV_Y +xx*sin_x*ScaleV_XZ;

		float sin_z = Cos(ANGLE_X);
		float cos_z = Sin(ANGLE_X);

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
	for (int i=0; i < 32; i++)
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


TDisplay3Dtraj::TDisplay3Dtraj(TImage * pImg,int nChan):TDisplay3D(pImg,nChan)
{
}

#include "ULeds.h"
int x0=0,y0=0,z0=0;
bool zero=false;

int TDisplay3Dtraj::DisplaySample(TMaxArray *sample)
{
	static n =0;
	n++;
	n%=10;
	if (n) return 0;


	TMaxArray * pMS = (TMaxArray * )sample;

	DCTYPE dc;
	GET_DC(dc);
	int PenCol;
	TPoint FromPoint,ToPoint;
	int pos1;
	int pos2;
	int x,y,z;


	blit(bmpCanvasBackground, bmpCanvas, 0, 0, 0, 0, bmpCanvas->w, bmpCanvas->h);
	int c=Form1->LabeledEdit3->Text.ToInt(); // first

	//  get the position of the first sensor:
	if (zero)
	{
		x0 =(*sample)[c+1];
		y0 =(*sample)[c+2];
		z0 =(*sample)[c+3];
		zero=false;
	}
	for (int ch = 0; ch< Form1->UpDown2->Position; ch++)
	{
		x =((*sample)[++c]-x0)* Form1->TrackBarGain->Position/100;
		y =((*sample)[++c]-y0)* Form1->TrackBarGain->Position/100;
		z =((*sample)[++c]-z0)* Form1->TrackBarGain->Position/100;
		Display3DBuffer[ch].AddSample(x,y,z);
	}

	int ns = Form1->UpDown3->Position;
	for (int ch = 0; ch< Form1->UpDown2->Position; ch++)
	{
		if (FormLEDs->CheckBoxLED[ch]->Checked)
		{
			pos1 = (Display3DBuffer[ch].pos+1) %ns;
			if (Form1->CheckBoxXY->Checked)
			{
				pos1 = (Display3DBuffer[ch].pos+1) %ns;
				SET_PEN(dc,180,0,0);//                SelectObject(dc,PenYellow);
				for (int n=0;n<(ns-1);n++)
				{
					pos2 = (pos1+1)%ns;
					FromPoint.x=InRange(Display3DBuffer[ch].xy[pos1],CENTER*2);
					FromPoint.y=InRange(Display3DBuffer[ch].yx[pos1],CENTER*2);
					ToPoint.x=InRange(Display3DBuffer[ch].xy[pos2],CENTER*2);
					ToPoint.y=InRange(Display3DBuffer[ch].yx[pos2],CENTER*2);
					LINE(dc,FromPoint.x,FromPoint.y,ToPoint.x,ToPoint.y,PenCol);
					pos1++;
					pos1 %=ns;
				}
			}

			if (Form1->CheckBoxZY->Checked)
			{
				pos1 = (Display3DBuffer[ch].pos+1) %ns;
				SET_PEN(dc,0,180,180);//                SelectObject(dc,PenYellow);
				for (int n=0;n<(ns-1);n++)
				{
					pos2 = (pos1+1)%ns;
					FromPoint.x=InRange(Display3DBuffer[ch].zy[pos1],CENTER*2);
					FromPoint.y=InRange(Display3DBuffer[ch].yz[pos1],CENTER*2);

					ToPoint.x=InRange(Display3DBuffer[ch].zy[pos2],CENTER*2);
					ToPoint.y=InRange(Display3DBuffer[ch].yz[pos2],CENTER*2);
					LINE(dc,FromPoint.x,FromPoint.y,ToPoint.x,ToPoint.y,PenCol);
					pos1++;
					pos1 %=ns;
				}
			}


			if (Form1->CheckBoxXYZ->Checked)
			{
				static int n=0;
				pos1 = (Display3DBuffer[ch].pos+1) %ns;
				SET_PEN(dc,255,255,0);//                SelectObject(dc,PenYellow);
				for (int n=0;n<(ns-1);n++)
				{
					pos2 = (pos1+1)%ns;
					FromPoint.x=InRange(Display3DBuffer[ch].xzy[pos1],CENTER*2);
					FromPoint.y=InRange(Display3DBuffer[ch].yxz[pos1],CENTER*2);
					ToPoint.x=InRange(Display3DBuffer[ch].xzy[pos2],CENTER*2);
					ToPoint.y=InRange(Display3DBuffer[ch].yxz[pos2],CENTER*2);
					LINE(dc,FromPoint.x,FromPoint.y,ToPoint.x,ToPoint.y,PenCol);
					pos1++;
					pos1 %=ns;
				}
			}
		}
	}
	return 0;// does not matter
}

int TDisplay3D::DisplaySample(TMaxArray *sample)
{
//	static n =0;
//	n++;
//	n%=10;
//	if (n) return 0;
	TMaxArray * pMS = (TMaxArray * )sample;
	DCTYPE dc;
	GET_DC(dc);
	int PenCol;
	int pos1;
	int x,y,z;

	blit(bmpCanvasBackground, bmpCanvas, 0, 0, 0, 0, bmpCanvas->w, bmpCanvas->h);


	int c=Form1->LabeledEdit3->Text.ToInt();
	if (zero)
	{
		x0 =(*sample)[c+1];
		y0 =(*sample)[c+2];
		z0 =(*sample)[c+3];
		zero=false;
	}
	int nOn =0;
	for (int ch = 0; ch< Form1->UpDown2->Position; ch++)
		if (FormLEDs->CheckBoxLED[ch]->Checked)
		{
			x =((*sample)[c+1 + ch*3]-x0)* Form1->TrackBarGain->Position/100;
			y =((*sample)[c+2 + ch*3]-y0)* Form1->TrackBarGain->Position/100;
			z =((*sample)[c+3 + ch*3]-z0)* Form1->TrackBarGain->Position/100;;
			Display3DBuffer[nOn++].AddSample(x,-z,-y);
		}

	for (int ch = 0; ch< nOn; ch++)
	{
		int r = 255;
		int g = 255;
				if (x<0) r = 128;
				if (z<0) g = 128;
		int is3 = (ch % 3)==2;
		pos1 = Display3DBuffer[ch].pos;
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
//					LINE(dc,
//						Display3DBuffer[ch-2].xzy[pos1],
//						Display3DBuffer[ch-2].yxz[pos1],
//						Display3DBuffer[ch-1].xzy[pos1],
//						Display3DBuffer[ch-1].yxz[pos1],
//						PenCol);
//					LINE(dc,
//						Display3DBuffer[ch-2].xzy[pos1],
//						Display3DBuffer[ch-2].yxz[pos1],
//						Display3DBuffer[ch].xzy[pos1],
//						Display3DBuffer[ch].yxz[pos1],
//						PenCol);
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
