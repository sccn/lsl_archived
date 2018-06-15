//---------------------------------------------------------------------------

#ifndef MonitorDrawerH
#define MonitorDrawerH

#include "useallegro4.h"
#include "stdio.h"
#include "GazeUtil.h"
#define CDEPTH 24

class MonitorDrawer {
	private:
		HDC hdc;
		BITMAP * bmpMarker, *bmpInactive, * bmpBackground, *bmpCurrent, *bmpSprite, *tempBackground, *newBackground;
		BITMAP * bmpFullBackground, *bmpBlackFullBackground;
		int markerSize;
		int oldX, oldY;
	public:
		HWND hwnd;
		int xRes;
		int yRes;
		int  count;
		bool visible;
		double monitorWidthMM;
		double monitorHeightMM;

	InitializeParameters() {
		bmpMarker = bmpBackground = bmpCurrent = bmpSprite = tempBackground = newBackground = NULL;
		bmpFullBackground = bmpBlackFullBackground = NULL;
		hwnd = NULL;
	}
	//windowless mode, tied to monitor
	MonitorDrawer(DISPLAY_DEVICE dd) {
		InitializeParameters();
		hdc = CreateDC("DISPLAY", dd.DeviceName, NULL, NULL);
		xRes = GetDeviceCaps(hdc, HORZRES);
		yRes = GetDeviceCaps(hdc, VERTRES);

		markerSize = 50;
		bmpMarker = create_bitmap_ex(CDEPTH,markerSize*3,markerSize*3);
		bmpBackground = create_bitmap_ex(CDEPTH, markerSize*3, markerSize*3);
		bmpSprite = create_bitmap_ex(CDEPTH,markerSize*3,markerSize*3);
		tempBackground = create_bitmap_ex(CDEPTH,markerSize,markerSize);

		clear_bitmap(bmpSprite);
		floodfill(bmpSprite, 1,1,makecol(255,0,255)); //fill background transparent.
		circlefill(bmpSprite, markerSize*3/2, markerSize*3/2, markerSize/2-1, makecol(0,255,0));
		circlefill(bmpSprite, markerSize*3/2, markerSize*3/2, markerSize/10, makecol(0,0,0));
		count=0;

		bmpFullBackground = create_bitmap_ex(CDEPTH,xRes,yRes);
		bmpBlackFullBackground = create_bitmap_ex(CDEPTH,xRes,yRes);
		clear_bitmap(bmpBlackFullBackground);

		visible = true;
	}

		//windowed for hotspots
	MonitorDrawer(HWND hWnd) {
		InitializeParameters();
		hwnd = hWnd;
		hdc = GetDC(hwnd);
		RECT rect;
		GetWindowRect(hwnd,&rect);
		int err = GetLastError();
		xRes = rect.right - rect.left;
		yRes = rect.bottom - rect.top;

		markerSize = 50;
		bmpMarker = create_bitmap_ex(CDEPTH,markerSize*3,markerSize*3);
		bmpBackground = create_bitmap_ex(CDEPTH, markerSize*3, markerSize*3);
		bmpSprite = create_bitmap_ex(CDEPTH,markerSize*3,markerSize*3);
		tempBackground = create_bitmap_ex(CDEPTH,markerSize,markerSize);
		clear_bitmap(bmpSprite);
		rectfill(bmpSprite, 0,0,markerSize*3,markerSize*3,makecol(100,100,100));
		circlefill(bmpSprite, markerSize*3/2, markerSize*3/2, markerSize/2-1, makecol(0,0,255));
		circlefill(bmpSprite, markerSize*3/2, markerSize*3/2, markerSize/10, makecol(100,100,100));
		count=0;

		bmpFullBackground = create_bitmap_ex(CDEPTH,xRes,yRes);
		rectfill(bmpFullBackground,0,0,xRes,yRes,makecol(100,100,100));
		bmpBlackFullBackground = create_bitmap_ex(CDEPTH,xRes,yRes);
		clear_bitmap(bmpBlackFullBackground);
		rectfill(bmpBlackFullBackground,0,0,xRes,yRes,makecol(100,100,100));

		visible = false;

	}


	//windowed mode, for calibration
	MonitorDrawer(HWND hWnd, int backgroundColor, double monitorWidthmm, double monitorHeightmm) {
		InitializeParameters();
		monitorWidthMM = monitorWidthmm; //storing this for public access
		monitorHeightMM = monitorHeightmm; //storing this for public access
		hwnd = hWnd;
		hdc = GetDC(hwnd);
		RECT rect;
		GetWindowRect(hwnd,&rect);
		int err = GetLastError();
		xRes = rect.right - rect.left;
		yRes = rect.bottom - rect.top;

		markerSize = 100;//50;
		bmpSprite = create_bitmap_ex(CDEPTH,markerSize,markerSize);
		clear_bitmap(bmpSprite);
		rectfill(bmpSprite, 0,0,markerSize,markerSize,backgroundColor);
	  //		rectfill(bmpSprite, 0,0,markerSize,markerSize,makecol(0,0,0));
		circlefill(bmpSprite, markerSize/2, markerSize/2, markerSize/2-1, makecol(255,0,0));
		circlefill(bmpSprite, markerSize/2, markerSize/2, markerSize/20, makecol(0,0,0));

		bmpBackground = create_bitmap_ex(CDEPTH,markerSize,markerSize);
		clear_bitmap(bmpBackground);
		rectfill(bmpBackground, 0,0,markerSize,markerSize,backgroundColor);

		bmpInactive = create_bitmap_ex(CDEPTH,markerSize,markerSize);
		clear_bitmap(bmpInactive);
		rectfill(bmpInactive, 0,0,markerSize,markerSize,backgroundColor);
	  //		rectfill(bmpBackground, 0,0,markerSize,markerSize,makecol(0,0,0));
		circlefill(bmpInactive, markerSize/2, markerSize/2, markerSize/2-1, makecol(0,0,255));
		circlefill(bmpInactive, markerSize/2, markerSize/2, markerSize/20, makecol(0,0,0));

		bmpBlackFullBackground = create_bitmap_ex(CDEPTH,xRes,yRes);
		clear_bitmap(bmpBlackFullBackground);
		rectfill(bmpBlackFullBackground,0,0,xRes,yRes,backgroundColor);
		//	rectfill(bmpBlackFullBackground,0,0,xRes,yRes,makecol(0,0,0));

		visible = false;

	}

	void drawMarkers(double xc, double yc, int active) {

		if(_isnan(xc) || _isnan(yc)) return;
		int x = xc*xRes;
		int y = yRes - yc*yRes;

		if(active == 1) draw_to_hdc (hdc,bmpSprite,x-markerSize/2,y-markerSize/2);
		else if(active == 0) draw_to_hdc(hdc,bmpInactive,x-markerSize/2,y-markerSize/2);
		else  draw_to_hdc(hdc,bmpBackground,x-markerSize/2,y-markerSize/2);
	}

	void setVisible(int nCmdShw) {

		ShowWindow(hwnd,nCmdShw);
		if(nCmdShw == SW_SHOW) {
			draw_to_hdc(hdc,bmpBlackFullBackground,0,0);
			visible = true;
		} else {
			visible = false;
		}
	}





//---------------------------------------------------------------------------


	/**
	 * xc runs from 0..1, and yc runs from 0..1. 0,0 is in the lower left.
	 */
	void drawMarkerSmooth(double xc, double yc) {
	   //	if(count++ % 100000 == 0) {
		int x,y;
		if(_isnan(xc) || _isnan(yc)) {
			x = -1000;
			y = -1000;
		} else {
			x = xc*xRes;
			y = yRes - yc*yRes;

		}
	  //	if(count %100 != 0) { x=oldX;y=oldY;} else {
	  //	 printf("x: %d  y: %d\n", x, y);
	   //	}
		if(count % 10 ==0) {
		if(count == 0) {

			blit_from_hdc(hdc, bmpBackground, x-markerSize*3/2,y-markerSize*3/2,0,0, 3*markerSize,3*markerSize);
			blit(bmpBackground, bmpMarker, 0, 0, 0,0, 3*markerSize,3*markerSize);
			masked_blit(bmpSprite, bmpMarker, 0,0,0,0,3*markerSize, 3*markerSize);
			draw_to_hdc (hdc,bmpMarker,x-markerSize*3/2,y-markerSize*3/2);
			oldX=x;
			oldY=y;
			count++;

			return;
		}
	 //	else if (count == 1) { x = 154; y = 24; }
  //		else if (count == 2) { x= 146; y = 21;}
	   //	else if (count == 3) { x= 146; y = 21;}
	//  else return;

   //		blit_from_hdc(hdc, bmpMarker, x-markerSize,y-markerSize,0,0, 2*markerSize,2*markerSize);

		if((x <  -markerSize*3/2 || x > xRes + markerSize*3/2 || y < -markerSize*3/2 || y > yRes + markerSize*3/2) &&
			(oldX <  -markerSize*3/2 || oldX > xRes + markerSize*3/2 || oldY <  -markerSize*3/2 || oldY > yRes + markerSize*3/2)) {
		  //	 blit_from_hdc(hdc, bmpBackground, x-markerSize*3/2,y-markerSize*3/2,0,0, 3*markerSize,3*markerSize);

			oldX = x;
			oldY = y;
			return;
		}

		if((abs(x-oldX) < markerSize) && (abs(y-oldY) < markerSize)) {

			blit_from_hdc(hdc, bmpMarker, x-markerSize*3/2,y-markerSize*3/2,0,0, 3*markerSize,3*markerSize);

			//clipping occurs, bmpMarker now clean
			blit(bmpBackground, bmpMarker, x-oldX, y-oldY, 0,0, 3*markerSize-(x-oldX),3*markerSize-(y-oldY));


			blit(bmpMarker, bmpBackground, 0,0,0,0,3*markerSize,3*markerSize); //save for later
			masked_blit(bmpSprite, bmpMarker, 0,0,0,0,3*markerSize, 3*markerSize);
			draw_to_hdc (hdc,bmpMarker,x-markerSize*3/2,y-markerSize*3/2);

		} else  {

			blit(bmpBackground,tempBackground, markerSize,markerSize,0,0,markerSize,markerSize);

			blit_from_hdc(hdc, bmpMarker, x-markerSize*3/2,y-markerSize*3/2,0,0, 3*markerSize,3*markerSize);
			if((abs(x-oldX) < markerSize*3) && (abs(y-oldY) < markerSize*3))
				blit(bmpBackground, bmpMarker, x-oldX, y-oldY, 0,0, 3*markerSize-(x-oldX),3*markerSize-(y-oldY)); //clipping occurs, bmpMarker now clean
			blit(bmpMarker, bmpBackground, 0,0,0,0,3*markerSize,3*markerSize); //save for later

			//draw marker
			masked_blit(bmpSprite, bmpMarker, 0,0,0,0,3*markerSize, 3*markerSize);
			draw_to_hdc (hdc,bmpMarker,x-markerSize*3/2,y-markerSize*3/2);

			//erase old marker.
			draw_to_hdc(hdc,tempBackground, oldX-markerSize/2, oldY-markerSize/2);

		}



 //				draw_to_hdc(hdc, bmpBackground, 0,800);
		oldX = x;
		oldY = y;
		}
		count++;
	}

	void blackenWindow() {
		draw_to_hdc(hdc,bmpBlackFullBackground, 0,0);
	}

	void restoreMonitor() {
		draw_to_hdc(hdc,bmpFullBackground,0,0);
	}

	~MonitorDrawer() {
		if(hwnd != NULL) CloseWindow(hwnd);

		//codeguard indicates that these are all ready deleted.
		//must happen automatically in allegro somewhere.
  /*		if(bmpMarker) destroy_bitmap(bmpMarker);
		bmpMarker = NULL;
		if(bmpBackground) destroy_bitmap(bmpBackground);
		bmpBackground = NULL;
		if(bmpCurrent) destroy_bitmap(bmpCurrent);
		bmpCurrent = NULL;
		if(bmpSprite) destroy_bitmap(bmpSprite);
		bmpSprite = NULL;
		if(tempBackground) destroy_bitmap(tempBackground);
		tempBackground = NULL;
		if(newBackground) destroy_bitmap(newBackground);
		newBackground = NULL;
		if(bmpFullBackground) destroy_bitmap(bmpFullBackground);
		bmpFullBackground = NULL;
		if(bmpBlackFullBackground) destroy_bitmap(bmpBlackFullBackground);
		bmpBlackFullBackground = NULL;
   */
	}
};

//---------------------------------------------------------------------------
#endif
