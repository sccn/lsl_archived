#include "U1Disp.h"
#include "TDDraw.h"

TDirectDraw::TDirectDraw(HWND hwnd, int high, int wide): //TObject(),
        hWnd(hwnd),lpDD(0),lpDDSPrimary(0), lpDDSBack(0), bActive(false)

{
        rDDraw.left=0;
        rDDraw.top=0;
        rDDraw.right=wide;
        rDDraw.bottom=high;
        rDDrawCode=rDDraw; rDDrawCode.bottom=20;

        char buf[20];
        HRESULT hRet = DirectDrawCreate(NULL, & lpDD, NULL); //IID_IDirectDraw7,
        if(hRet == DD_OK)
        {
                hRet = lpDD->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
                if (hRet == DD_OK)
                {
                        // Create the primary surface.
                        DDSURFACEDESC ddsd;
                        ZeroMemory(&ddsd, sizeof(ddsd));
                        ddsd.dwSize = sizeof(ddsd);
                        ddsd.dwFlags = DDSD_CAPS;
                        ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
                        hRet = lpDD->CreateSurface(&ddsd, &lpDDSPrimary, NULL);
                        if (hRet == DD_OK)
                        {
                                DDSURFACEDESC ddsd;
                                ddsd.dwSize = sizeof(ddsd);

                                // Tell DirectDraw which members are valid.
                                ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;

                                // Request a simple off-screen surface, sized to Canvas
                                // (This assumes that the off-screen surface we are about
                                // to create will match the pixel format of the
                                // primary surface.)


//                                ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
//                                ddsd.ddsCaps.dwCaps = DDSCAPS_LOCALVIDMEM | DDSCAPS_VIDEOMEMORY; // works too
//                                ddsd.ddsCaps.dwCaps = DDSCAPS_SYSTEMMEMORY ; // works too
                                ddsd.ddsCaps.dwCaps = DDSCAPS_VIDEOMEMORY; // works too
                                ddsd.dwHeight = high;
                                ddsd.dwWidth = wide;

                                hRet = lpDD->CreateSurface(&ddsd, &lpDDSBack, NULL);
                                if (hRet == DD_OK) bActive=true;
                                else
                                    ShowMessage("LpDDSBack failed");
                                ddsd.dwHeight = 20;
                                ddsd.dwWidth = wide;
                                hRet = lpDD->CreateSurface(&ddsd, &lpDDSBackCode, NULL);
                                if (hRet == DD_OK) bActive=true;
                                else
                                    ShowMessage("LpDDSBackCode failed");

                        }
                        else
                                ShowMessage("The surface was not created.");
                }
                else
                    ShowMessage("Normal mode was not successful.");
        }
        else
                ShowMessage("DDraw failed");
}

enum {c0,cTL,cTR,cBL,cBR,cL,cR,cT,cB};

bool GetOverlapRect(RECT r1, RECT r2, int & x, int & y, int & quad)
{
        bool rq,lq,tq,bq;
        rq=lq=tq=bq=false;
        if (r1.right < r2.left) return true;
        if (r2.right < r1.left) return true;
        if (r1.bottom < r2.top) return true;
        if (r2.bottom < r1.top) return true;

        if (r1.left < r2.left   &&  r2.left  < r1.right )
        {
           x= r2.left - r1.left; rq=true;
        }
        if (r1.left < r2.right  &&  r2.right < r1.right )
        {
           x= r2.right - r1.left; lq=true;
        }
        if (r1.top  < r2.top    &&  r2.top   < r1.bottom)
        {
           y = r2.top- r1.top; bq=true;
        }
        if (r1.top  < r2.bottom &&  r2.bottom< r1.bottom)
        {
           y = r2.bottom-r1.top;tq=true;
        }

        if (rq && lq && bq && tq) return false;   // r2 is inside r1
        if (!rq && !lq && !bq && !tq) return false;   // r1 is inside r2

        if (rq) quad=cR;
        if (bq) quad=cB;
        if (lq) quad=cL;
        if (tq) quad=cT;
        if (rq & tq) quad=cTR;
        if (rq & bq) quad=cBR;
        if (lq & tq) quad=cTL;
        if (lq & bq) quad=cBL;
        return true;

}

int smaller(int a, int b) {return a<b? a:b;}
int bigger (int a, int b) {return a>b? a:b;}

void TDirectDraw::BlitQuad(int x,int y, int q, RECT rDest)
{
        DDSURFACEDESC ddsd;
        ddsd.dwSize=sizeof ddsd;
        lpDDSBack->GetSurfaceDesc(&ddsd);
        RECT rSource={0,0,ddsd.dwWidth,ddsd.dwHeight};
        RECT rDestCopy=rDest;
        RECT rSourceCopy=rSource;
        if (q==cL || q==cTL || q==cBL)
        {
                rSource.left=x;
                rDest.left+=x;
                lpDDSPrimary->Blt(&rDest,lpDDSBack,&rSource,0,0);
                rSource=rSourceCopy;
                rDest=rDestCopy;
        }
        if (q==cT || q==cTL || q==cTR)
        {
                rSource.top=y;
                rDest.top += y;
                lpDDSPrimary->Blt(&rDest,lpDDSBack,&rSource,0,0);
                rSource=rSourceCopy;
                rDest=rDestCopy;
        }
        if (q==cR || q==cTR || q==cBR)
        {
                rSource.right=x;
                rDest.right = rDest.left+x;
                lpDDSPrimary->Blt(&rDest,lpDDSBack,&rSource,0,0);
                rSource=rSourceCopy;
                rDest=rDestCopy;
        }
        if (q==cB || q==cBL || q==cBR)
        {
                rSource.bottom=y;
                rDest.bottom = rDest.top + y;
                lpDDSPrimary->Blt(&rDest,lpDDSBack,&rSource,0,0);

        }
}

void TDirectDraw::GetBackRect(RECT &rBack)
{
        DDSURFACEDESC ddsd;
        ddsd.dwSize=sizeof ddsd;
        lpDDSBack->GetSurfaceDesc(&ddsd);
        rBack.left=0;
        rBack.top=0;
        rBack.right=ddsd.dwWidth;
        rBack.bottom=ddsd.dwHeight;
}

void TDirectDraw::UpdateScreen(RECT rThis)
{
        HWND hFWnd=GetForegroundWindow();
        if (hWnd==hFWnd)
                lpDDSPrimary->Blt(&rThis,lpDDSBack,&rDDraw,0,0);
        else
        {

                RECT rForegr;
                GetWindowRect(hFWnd,&rForegr);
                int X=0,Y=0,quad=0;
              if (GetOverlapRect(rThis, rForegr, X,Y,quad))
                {
/*
                        HDC DC;
                        lpDDSBack->GetDC(&DC);
                        SetBkColor(DC, 0x0000ff00);
                        SetTextColor(DC, 0x00000000);
                        if (quad==0)
                                SetBkColor(DC, 0x000000ff);

                        char buf[20];
                        sprintf(buf,"x=%d,y=%d,q=%d        ",X,Y,quad);
                        TextOut(DC,10, 10, buf,strlen(buf));
                        lpDDSBack->ReleaseDC(&DC);
*/
                        if (quad==0)
                                lpDDSPrimary->Blt(&rThis,lpDDSBack,&rDDraw,0,0);
                        else    BlitQuad(X,Y,quad,rThis);
                }

        }

}

void TDirectDraw::UpdateScreenPart(RECT rThis, int from, int to)
{
        rThis.right=rThis.left+to;
        rThis.left+=from;

        RECT rSource=rDDraw;
        rSource.left=from;
        rSource.right=to;

        HWND hFWnd=GetForegroundWindow();
        if (hWnd==hFWnd)
                lpDDSPrimary->Blt(&rThis,lpDDSBack,&rSource,0,0);
        else
        {

                RECT rForegr;
                GetWindowRect(hFWnd,&rForegr);


                int X=0,Y=0,quad=0;
                if (GetOverlapRect(rThis, rForegr, X,Y,quad))
                {
                        HDC DC;
                        lpDDSBack->GetDC(&DC);
                        SetBkColor(DC, 0x0000ff00);
                        SetTextColor(DC, 0x00000000);
                        if (quad==0)
                                SetBkColor(DC, 0x000000ff);
                        char buf[20];
                        sprintf(buf,"x=%d,y=%d,q=%d        ",X,Y,quad);
                        TextOut(DC,10, 10, buf,strlen(buf));
                        lpDDSBack->ReleaseDC(&DC);

                        if (quad==0)
                                lpDDSPrimary->Blt(&rThis,lpDDSBack,&rSource,0,0);
                        else    BlitQuad(X,Y,quad,rThis);
                }

        }

}




/*
*/



void TDirectDraw::ClearBackbuffer()
{
        //TODO: Add your source code here
        DDBLTFX ddbltfx;
        ddbltfx.dwSize = sizeof(ddbltfx);
        ddbltfx.dwFillColor =0;// 0x0000a0;

        HRESULT ddrval = lpDDSBack->Blt(
                NULL,        // Destination is entire surface
                NULL,        // No source surface
                NULL,        // No source rectangle
                DDBLT_COLORFILL, &ddbltfx);
        ddrval = lpDDSBackCode->Blt(
                NULL,        // Destination is entire surface
                NULL,        // No source surface
                NULL,        // No source rectangle
                DDBLT_COLORFILL, &ddbltfx);
}
