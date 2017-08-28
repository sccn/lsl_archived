//---------------------------------------------------------------------------

#ifndef DrawView2H
#define DrawView2H
//---------------------------------------------------------------------------


#include "useallegro.h" //???
#include <vcl.h>
//#include "BiosemiSample.h"
#include "BiosemiSample.h"
#include "tddraw.h"

		#define LOCK
		//acquire_bitmap(bmpCanvas)
		#define	UNLOCK
		//release_bitmap(bmpCanvas)
		#define GET_DC(DC) DC= bmpCanvas

		#define LOCKCODE
		//acquire_bitmap(bmpCodeCanvas)
		#define UNLOCKCODE
		//release_bitmap(bmpCodeCanvas)
		#define GET_CODEDC(DC) DC= bmpCodeCanvas
		#define GET_SCALEDC(DC) DC= bmpCodeScale

		#define LOCKCODE2(canv)
		//acquire_bitmap(canv)
		#define UNLOCKCODE2(canv)
		//release_bitmap(canv)
		#define GET_CODEDC2(DC,canv) DC= canv
		#define GET_SCALEDC2(DC,scale) DC= scale

		#define DCTYPE BITMAP *
		#define LINE(_DC,X1,Y1,X2,Y2,Col) line(_DC,X1,Y1, X2,Y2,Col)//makecol24(255,255,0))
		#define SET_BLACKPEN(DC)   PenCol = 0x00//makecol24(0,0,0)
		#define SET_YELLOWPEN(DC)  PenCol = 0x00ffff//makecol24(255,255,0)
		#define SET_PEN(DC,r,g,b)  PenCol = makecol24(r,g,b)

		#define SET_GRAYPEN(DC)   PenCol = 0x777777

		#define SET_REDPEN(DC)   PenCol = 0xff0000

		#define TEXTOUT(DC,BUF,X,Y,COL)  textout(DC,font,BUF,X,Y,COL)
		#define TEXTWIDTH(DC,BUF,PT) PT= text_length(font,BUF)




class CChannelN //:public TImage
{
// For a description of the methods, see implementation
//
// Member variables:
//
//		m_nCurPosition = the current loogical position of the oscilloscope.  When
//					  a new piece of data is plotted, move one to the right
//					  (i.e., increment by 1) and wrap if it exceeds the
//					  logical x dimension of the display.  Tells the methods
//					  "this is the position where the next piece of data
//					  should be plotted"
//		m_sSize = the logical size of the display (hor,vert), in physical units.
//					  For example, m_sSize.x = 150 means that the display fits
//					  150 data points before it wraps and starts paining over.
//		m_pnDisplayState = an array that contains the physical
//					  state of the display.  When a point is updated on the
//					  screen, it is also updated in m_pnDisplayState.
//					  The size of the array is equal to the logical width of the display
//					  (m_sSize.x) * m_nChan, allocated dynamically.
//
//		int m_nChan	= number of channels to display
//		COLORREF m_nColor = the drawing color
//		CSize m_sSize	=  the size of the window (H x V), in physical units


public:
	BITMAP * bmpCanvas;
	BITMAP * bmpCodeCanvas;
	BITMAP * bmpCodeCanvas2;
	TDirectDraw  * DDraw;	// User declarations


	TImage * pImage;
	TImage * pCodeImage;

	long 	m_nVScale;
	int 	m_nChan;
	int 	m_nCurPosition;
	POINT 	m_sSize;				//h x v, in physical units

	virtual int DisplaySample(TMaxArray *sample);
	CChannelN (TImage * pImg,TImage * pCodeImg,int nChan); //CRect viewport,
	__fastcall ~CChannelN();

	virtual void PaintBkgr();		// paints the background in the color of the active brush
	void PutRandom();		// for testing


private:
	long *		m_pnDisplayState;	// vis.buffer
	COLORREF 	m_nColor;

	HPEN PenYellow;//=CreatePen(PS_SOLID,1, 0x0000ffff);
	HPEN PenBlack ;//=CreatePen(PS_SOLID,1, 0x00000000);
	HPEN PenGray ;//=CreatePen(PS_SOLID,1, 0x00000000);
	HPEN PenRed ;//=CreatePen(PS_SOLID,1, 0x00000000);
	void ShowCode(int pos, ULONG code);
};

/////////////////////////////////////////////////////////////////////////////

#define BUFSIZE3D 20
struct TPos3D
{
	int xy[BUFSIZE3D];
	int yx[BUFSIZE3D];
	int zy[BUFSIZE3D];
	int yz[BUFSIZE3D];
	int xzy[BUFSIZE3D];
	int yxz[BUFSIZE3D];
	int pos;      // points to the last added
	int oldpos;
	TPos3D()
	{
		ZeroMemory( xy, sizeof xy );
		ZeroMemory( yx, sizeof yx );
		ZeroMemory( zy, sizeof zy );
		ZeroMemory( yz, sizeof yz );
		ZeroMemory( xzy, sizeof xzy );
		ZeroMemory( yxz, sizeof yxz );
		pos=0;
		oldpos=-1;
	};
	void AddSample(int xx,int yy,int zz);
};

class TDisplay3D //:public CChannelN
{
public:
	TImage * pImage;
	BITMAP * bmpCanvasBackground;
	BITMAP * bmpCanvas;
	TPos3D Display3DBuffer[32];	// vis.buffer
	TDisplay3D(TImage * pImg,int nChan); //CRect viewport,
	void PaintBkgr();		// paints the background in the color of the active brush
	virtual int DisplaySample(TMaxArray *sample);
};

class TDisplay3Dtraj:public TDisplay3D
{
public:
	TDisplay3Dtraj(TImage * pImg,int nChan);
	int DisplaySample(TMaxArray *sample);
};

#define PI 3.1415926535897932385
double inline TO_RAD(double deg)
{
	return deg / 360.0 * (PI*2);
}

double inline TO_DEG(double rad)
{
	return rad * 360.0 / (PI*2);
}

#endif

