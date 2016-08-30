#ifndef TDDRAW_H
#define TDDRAW_H


#include <vcl.h>
#pragma hdrstop

//#define DIRECTDRAW_VERSION 0x0700
//#define DIRECTDRAW_VERSION 0x0900
#include <ddraw.h>
#include <stdio.h>
//---------------------------------------------------------------------------
class TDirectDraw//: public TObject
{
private:
        HWND hWnd;//=((TForm1*)TheForm) ->getHWnd();
public:
        RECT rDDraw;
        RECT rDDrawCode;
        LPDIRECTDRAW            lpDD;           // DirectDraw object
        LPDIRECTDRAWSURFACE     lpDDSPrimary;   // DirectDraw primary surface
        LPDIRECTDRAWSURFACE     lpDDSBack;      // DirectDraw back surface
        LPDIRECTDRAWSURFACE     lpDDSBackCode;      // DirectDraw back surface
        BOOL                    bActive;        // is DirectDraw active?
        TDirectDraw(HWND hwnd, int high, int wide);
        void BlitQuad(int x,int y, int q, RECT rDest);
        void UpdateScreen(RECT rThis);
        void ClearBackbuffer();
        void GetBackRect(RECT &rBack);
        void UpdateScreenPart(RECT rThis, int from, int to);
private:
         ~TDirectDraw()
        {
                  if(lpDD != NULL)
                  {
                        if(lpDDSPrimary != NULL)
                        {
                                lpDDSPrimary->Release();
                                lpDDSPrimary = NULL;
                        }
                        if(lpDDSBack != NULL)
                        {
                                lpDDSBack->Release();
                                lpDDSBack = NULL;
                        }
                        lpDD->Release();
                        lpDD = NULL;
                  }
        };


};

#endif

