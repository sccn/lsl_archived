/*
Main form for GazeStream.

Written by Matthew Grivich and Andre Vankov, Swartz Center for Computational Neuroscience, UCSD, 2011-2013
*/
//---------------------------------------------------------------------------
#include "LinearAlgebra.h"
#include "useallegro4.h"
#include <vcl.h>
#pragma hdrstop
#include "math_util.h"
#include "UMainCapture.h"
#include "stdio.h"
#include "Shlobj.h"
#include "float.h"
#include "TFrameThread.h"
#include "CaptureWorkerForm.h"
#include "Math.hpp"
#include "StreamThread.h"


//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TMainCaptureForm *MainCaptureForm;
//---------------------------------------------------------------------------
__fastcall TMainCaptureForm::TMainCaptureForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------


#include "CircleFunction.h"
#include "Fitter.h"

bool storeFit = false;


int nFrames;



TFrameThread *frameThread;
TFrameThread *frameThread1;
TFrameThread *frameThread2;
TFrameThread *frameThread3;

HANDLE hMutex = 0;
HANDLE hMutex1 = 0;
HANDLE hMutex2 = 0;
HANDLE hMutex3 = 0;


lsl_outlet outlet = 0;
lsl_outlet outlet1 = 0;
lsl_outlet outlet2 = 0;
lsl_outlet outlet3 = 0;

TStreamThread *gazestreamThread = 0;

BITMAP * bmpCanvas = NULL;
BITMAP * bmpCanvas1 = NULL;
BITMAP * bmpCanvas2 = NULL;
BITMAP * bmpCanvas3 = NULL;

int frameDivisor = 1;
int spatialDivisor = 1;



#define CDEPTH 24



void AssignListToComboBox (TComboBox* ComboBox, String List, int Index)
{
   ComboBox->Items->Text = List;
   if (ComboBox->Items->Count > 0) {
	  ComboBox->ItemIndex = Index;
   }
}


void __fastcall TMainCaptureForm::FormCreate(TObject *Sender)
{

	CaptureWorkerForm = new TCaptureWorkerForm(this);
	CaptureWorkerForm1 = new TCaptureWorkerForm(this);
	CaptureWorkerForm2 = new TCaptureWorkerForm(this);
	CaptureWorkerForm3 = new TCaptureWorkerForm(this);
     MainCaptureForm->Caption = UnicodeString("CES Video Stream, version ") + getVersion();
	//There has to be some option other than silent failure and dialog box, but I haven't found it yet.
	//	_control87(MCW_EM, MCW_EM);  //Turn off FPU generated floating point exceptions. Threads still fail however.
	_control87( 0x1372, 0x137F ); //turns on dialog exceptions, but not in TThread. Exceptions in TThread cause the thread to die.


	pBmpRec=new Graphics::TBitmap;
	pBmpRec-> LoadFromFile("Rec.bmp");
	pBmpRecGr=new Graphics::TBitmap;
	pBmpRecGr-> LoadFromFile("Rec-gr.bmp");
	pBmpPau=new Graphics::TBitmap;
	pBmpPau->LoadFromFile("Pau.bmp");
	pBmpPauGr=new Graphics::TBitmap;
	pBmpPauGr->LoadFromFile("Pau-gr.bmp");

	//set default output folder to c:\Users\currentUser\Desktop\capture
 //	char pathC[MAX_PATH];
 //	SHGetSpecialFolderPath(NULL, pathC, CSIDL_DESKTOP, 1);
 //	edOutput->Text = UnicodeString(pathC) + "\\capture";

	frameThread = NULL;
	frameThread1 = NULL;
	frameThread2 = NULL;
	frameThread3 = NULL;
	hMutex = CreateMutex(0,false,0);
	hMutex1 = CreateMutex(0,false,0);
	hMutex2 = CreateMutex(0,false,0);
	hMutex3 = CreateMutex(0,false,0);
	BitBtnStop->Enabled = false;
	FrameDivisorEditChange(this);
	SpatialDivisorEditChange(this);



	allegro_init();
	set_gdi_color_format();
	set_color_depth(CDEPTH);

	nFrames=0;
	SetToVideoMode();

	   // let's refresh the controls according to the TVideoGrabber's states
   AssignListToComboBox (cbVideoInputDevice, CaptureWorkerForm->VideoGrabber->VideoDevices, CaptureWorkerForm->VideoGrabber->VideoDevice);
   AssignListToComboBox (cbVideoInputDevice1, CaptureWorkerForm1->VideoGrabber->VideoDevices, CaptureWorkerForm1->VideoGrabber->VideoDevice);
   AssignListToComboBox (cbVideoInputDevice2, CaptureWorkerForm2->VideoGrabber->VideoDevices, CaptureWorkerForm2->VideoGrabber->VideoDevice);
   AssignListToComboBox (cbVideoInputDevice3, CaptureWorkerForm3->VideoGrabber->VideoDevices, CaptureWorkerForm3->VideoGrabber->VideoDevice);

}


int acqWidth=0, acqHeight=0;

void __fastcall TMainCaptureForm::cbVideoInputDeviceChange(TObject *Sender)
{
	if(cbVideoInputDevice->ItemIndex != -1) {
		CaptureWorkerForm->VideoGrabber->VideoDevice = cbVideoInputDevice->ItemIndex;

		AssignListToComboBox (cbVideoInputFormat, CaptureWorkerForm->VideoGrabber->VideoSizes, CaptureWorkerForm->VideoGrabber->VideoSize);
		AssignListToComboBox (cbVideoInput, CaptureWorkerForm->VideoGrabber->VideoInputs, CaptureWorkerForm->VideoGrabber->VideoInput);

		cbVideoInputFormatChange(this);
		cbVideoInputChange(this);
	}
}

void __fastcall TMainCaptureForm::cbVideoInputDevice1Change(TObject *Sender)
{
	if(cbVideoInputDevice1->ItemIndex != -1) {
		CaptureWorkerForm1->VideoGrabber->VideoDevice = cbVideoInputDevice1->ItemIndex;
		AssignListToComboBox (cbVideoInputFormat, CaptureWorkerForm1->VideoGrabber->VideoSizes, CaptureWorkerForm1->VideoGrabber->VideoSize);
		AssignListToComboBox (cbVideoInput, CaptureWorkerForm1->VideoGrabber->VideoInputs, CaptureWorkerForm1->VideoGrabber->VideoInput);

		cbVideoInputFormatChange(this);
		cbVideoInputChange(this);

	}
}

void __fastcall TMainCaptureForm::cbVideoInputDevice2Change(TObject *Sender)
{
	if(cbVideoInputDevice2->ItemIndex != -1) {
		CaptureWorkerForm2->VideoGrabber->VideoDevice = cbVideoInputDevice2->ItemIndex;
		AssignListToComboBox (cbVideoInputFormat, CaptureWorkerForm2->VideoGrabber->VideoSizes, CaptureWorkerForm2->VideoGrabber->VideoSize);
		AssignListToComboBox (cbVideoInput, CaptureWorkerForm2->VideoGrabber->VideoInputs, CaptureWorkerForm2->VideoGrabber->VideoInput);

		cbVideoInputFormatChange(this);
		cbVideoInputChange(this);

	}
}

void __fastcall TMainCaptureForm::cbVideoInputDevice3Change(TObject *Sender)
{
	if(cbVideoInputDevice3->ItemIndex != -1) {
		CaptureWorkerForm3->VideoGrabber->VideoDevice = cbVideoInputDevice3->ItemIndex;
		AssignListToComboBox (cbVideoInputFormat, CaptureWorkerForm3->VideoGrabber->VideoSizes, CaptureWorkerForm3->VideoGrabber->VideoSize);
		AssignListToComboBox (cbVideoInput, CaptureWorkerForm3->VideoGrabber->VideoInputs, CaptureWorkerForm3->VideoGrabber->VideoInput);

		cbVideoInputFormatChange(this);
		cbVideoInputChange(this);

	}
}
//---------------------------------------------------------------------------



void __fastcall TMainCaptureForm::cbVideoInputChange(TObject *Sender)
{
	if(cbVideoInput->ItemIndex != -1) {
		CaptureWorkerForm->VideoGrabber->VideoInput = cbVideoInput->ItemIndex;
		CaptureWorkerForm1->VideoGrabber->VideoInput = cbVideoInput->ItemIndex;
		CaptureWorkerForm2->VideoGrabber->VideoInput = cbVideoInput->ItemIndex;
		CaptureWorkerForm3->VideoGrabber->VideoInput = cbVideoInput->ItemIndex;
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainCaptureForm::cbVideoInputFormatChange(TObject *Sender)
{
	if (cbVideoInputFormat->ItemIndex != -1) {
		CaptureWorkerForm->VideoGrabber->VideoSize = cbVideoInputFormat->ItemIndex;
		CaptureWorkerForm->VideoGrabber->VideoInput = cbVideoInput->ItemIndex;
		CaptureWorkerForm1->VideoGrabber->VideoSize = cbVideoInputFormat->ItemIndex;
		CaptureWorkerForm1->VideoGrabber->VideoInput = cbVideoInput->ItemIndex;
		CaptureWorkerForm2->VideoGrabber->VideoSize = cbVideoInputFormat->ItemIndex;
		CaptureWorkerForm2->VideoGrabber->VideoInput = cbVideoInput->ItemIndex;
		CaptureWorkerForm3->VideoGrabber->VideoSize = cbVideoInputFormat->ItemIndex;
		CaptureWorkerForm3->VideoGrabber->VideoInput = cbVideoInput->ItemIndex;

		acqWidth = CaptureWorkerForm->VideoGrabber->Display_Width;
		acqHeight = CaptureWorkerForm->VideoGrabber->Display_Height;



		tPanel->Width = acqWidth/spatialDivisor;
		tPanel->Height = acqHeight/spatialDivisor;
		tPanel1->Width = acqWidth/spatialDivisor;
		tPanel1->Height = acqHeight/spatialDivisor;
		tPanel2->Width = acqWidth/spatialDivisor;
		tPanel2->Height = acqHeight/spatialDivisor;
		tPanel3->Width = acqWidth/spatialDivisor;
		tPanel3->Height = acqHeight/spatialDivisor;

		tPanel->Left = 440;
		tPanel1->Left = 440+tPanel->Width;
		tPanel2->Left = 440;
		tPanel3->Left = 440+tPanel->Width;

		tPanel->Top = 10;
		tPanel1->Top = 10;
		tPanel2->Top = 10+tPanel->Height;
		tPanel3->Top = 10+tPanel->Height;




		if(bmpCanvas) {
			destroy_bitmap(bmpCanvas);
			bmpCanvas = NULL;
		}
		bmpCanvas = create_bitmap_ex(CDEPTH,acqWidth/spatialDivisor,acqHeight/spatialDivisor);
		clear_bitmap(bmpCanvas);


		if(bmpCanvas1) {
			destroy_bitmap(bmpCanvas1);
			bmpCanvas1 = NULL;
		}
		bmpCanvas1 = create_bitmap_ex(CDEPTH,acqWidth/spatialDivisor,acqHeight/spatialDivisor);
		clear_bitmap(bmpCanvas1);

		if(bmpCanvas2) {
			destroy_bitmap(bmpCanvas2);
			bmpCanvas2 = NULL;
		}
		bmpCanvas2 = create_bitmap_ex(CDEPTH,acqWidth/spatialDivisor,acqHeight/spatialDivisor);
		clear_bitmap(bmpCanvas2);

		if(bmpCanvas3) {
			destroy_bitmap(bmpCanvas3);
			bmpCanvas3 = NULL;
		}
		bmpCanvas3 = create_bitmap_ex(CDEPTH,acqWidth/spatialDivisor,acqHeight/spatialDivisor);
		clear_bitmap(bmpCanvas3);
	}

}
//---------------------------------------------------------------------------

void __fastcall TMainCaptureForm::btStopClick(TObject *Sender)
{

	if(cbRecord->Checked == true) {
		CaptureWorkerForm->VideoGrabber->StopRecording();
		CaptureWorkerForm1->VideoGrabber->StopRecording();
		CaptureWorkerForm2->VideoGrabber->StopRecording();
		CaptureWorkerForm3->VideoGrabber->StopRecording();
	} else {
		CaptureWorkerForm->VideoGrabber->StopPreview();
		CaptureWorkerForm1->VideoGrabber->StopPreview();
		CaptureWorkerForm2->VideoGrabber->StopPreview();
		CaptureWorkerForm3->VideoGrabber->StopPreview();
	}

	BitBtnStop->Enabled=false;
	BitBtnPlay->Enabled=true;
	BitBtnPlay1->Enabled=true;
	BitBtnPlay2->Enabled=true;
	BitBtnPlay3->Enabled=true;

	cbRecord->Enabled =true;

	if(frameThread) {
		frameThread->Terminate();
		delete frameThread; //will not delete till terminated, by VCL design.
		frameThread = NULL;
	}

	if(frameThread1) {
		frameThread1->Terminate();
		delete frameThread1; //will not delete till terminated, by VCL design.
		frameThread1 = NULL;
	}


	if(frameThread2) {
		frameThread2->Terminate();
		delete frameThread2; //will not delete till terminated, by VCL design.
		frameThread2 = NULL;
	}

	if(frameThread3) {
		frameThread3->Terminate();
		delete frameThread3; //will not delete till terminated, by VCL design.
		frameThread3 = NULL;
	}

	nFrames = 0;

	cbVideoInputDevice->Enabled = true;
	cbVideoInputDevice1->Enabled = true;
	cbVideoInputDevice2->Enabled = true;
	cbVideoInputDevice3->Enabled = true;
	cbVideoInput->Enabled = true;
	cbVideoInputFormat->Enabled = true;
	SpatialDivisorEdit->Enabled = true;

}

//---------------------------------------------------------------------------

void __fastcall TMainCaptureForm::Start(TCaptureWorkerForm *localCaptureWorkerForm, UnicodeString saveRoot, queue<BITMAP*> &bmpQueue, HANDLE hMutex, lsl_outlet outlet, double requestedFrameRate)
{

		localCaptureWorkerForm->VideoGrabber->AnalogVideoStandard = localCaptureWorkerForm->VideoGrabber->AnalogVideoStandardIndex ("NTSC M");


		localCaptureWorkerForm->VideoGrabber->RecordingInNativeFormat = false;

		localCaptureWorkerForm->VideoGrabber->FrameRate = edtRequestedFrameRate->Text.ToDouble();

		if(cbRecord->Checked == true) {
		  //	localCaptureWorkerForm->VideoGrabber->VideoProcessing_FlipVertical = true;//HACK FOR POINT GREY BUG, RECORD ONLY, flipVertCheckbox->Checked;


			localCaptureWorkerForm->SetQueue(bmpQueue, hMutex, outlet, requestedFrameRate);

			//save to specified filename
			int n=0;
			UnicodeString outputFileName;
			while(true) {
				if(cbCompress->Checked) {
					outputFileName =  saveRoot /*edOutput0->Text*/+ IntToStr(n)+".asf";
				} else {
					outputFileName =  saveRoot /*edOutput0->Text*/+ IntToStr(n)+".avi";
				}

				if(!FileExists(outputFileName)) break;
				++n;
			}
			MkDir(outputFileName);
			if(DirectoryExists(outputFileName)) RmDir(outputFileName);
			else {
				Application->MessageBoxA(L"Invalid path for writing movie.", L"Error", MB_OK);
				return;
			}

			localCaptureWorkerForm->VideoGrabber->RecordingFileName= outputFileName;

			//record audio?
			localCaptureWorkerForm->VideoGrabber->AudioRecording = cbRecordAudio->Checked;

			//turn off some unused features
			localCaptureWorkerForm->VideoGrabber->FrameGrabber = TFrameGrabber (0);

			//save as compressed asf
			localCaptureWorkerForm->VideoGrabber->VideoCompressor = cbVideoCodecs->ItemIndex;

			if(cbCompress->Checked) {
				localCaptureWorkerForm->VideoGrabber->CompressionMode = cm_CompressOnTheFly;
				localCaptureWorkerForm->VideoGrabber->RecordingMethod = rm_ASF;
			} else {

				localCaptureWorkerForm->VideoGrabber->CompressionMode = cm_NoCompression;
				localCaptureWorkerForm->VideoGrabber->RecordingMethod = rm_AVI;
			}


			localCaptureWorkerForm->VideoGrabber->HoldRecording = false;

			localCaptureWorkerForm->VideoGrabber->StartRecording();

		} else {
			localCaptureWorkerForm->VideoGrabber->VideoProcessing_FlipVertical = false;//HACK FOR POINT GREY BUG, RECORD ONLY, flipVertCheckbox->Checked;

			localCaptureWorkerForm->SetQueue(bmpQueue, hMutex, outlet, requestedFrameRate);

			localCaptureWorkerForm->VideoGrabber->StartPreview();

		}

}


void __fastcall TMainCaptureForm::FormDestroy(TObject *Sender)
{

	if(outlet) {
		lsl_destroy_outlet(outlet);
		outlet = NULL;
	}
	if(outlet1) {
		lsl_destroy_outlet(outlet1);
		outlet1 = NULL;
	}
	if(outlet2) {
		lsl_destroy_outlet(outlet2);
		outlet2 = NULL;
	}
	if(outlet3) {
		lsl_destroy_outlet(outlet3);
		outlet3 = NULL;
	}
	if(gazestreamThread) {
		gazestreamThread->Terminate();
		delete gazestreamThread;
		gazestreamThread = NULL;
	}


}



double firstTimestamp, oldTimestamp, currentTimestamp;



void __fastcall TMainCaptureForm::DoFrame(BITMAP *aBmp, HANDLE hmut)
{

		bool paint = false;

		if(hmut == hMutex) {
			if(CaptureWorkerForm->nFrames % frameDivisor == 0)	paint = true;
			if(paint) {
				edFrame->Text = CaptureWorkerForm->nFrames;
				edTimestamp->Text = CaptureWorkerForm->timestamp;
				stretch_blit(aBmp, bmpCanvas,
					0,0, aBmp->w, aBmp->h,
					0,0, bmpCanvas->w,bmpCanvas->h);

				HWND hWnd = tPanel->Handle;
				HDC  hDC  = GetDC(hWnd);
				draw_to_hdc (hDC,bmpCanvas,0,0);
				ReleaseDC(hWnd,hDC);

			}
		}

		if(hmut == hMutex1) {
			if(CaptureWorkerForm1->nFrames % frameDivisor == 0)	paint = true;
			if(paint) {
				stretch_blit(aBmp, bmpCanvas1,
					0,0, aBmp->w, aBmp->h,
					0,0, bmpCanvas1->w,bmpCanvas1->h);

				HWND hWnd = tPanel1->Handle;
				HDC  hDC  = GetDC(hWnd);
				draw_to_hdc (hDC,bmpCanvas1,0,0);
				ReleaseDC(hWnd,hDC);

			}
		}

		if(hmut == hMutex2) {
			if(CaptureWorkerForm2->nFrames % frameDivisor == 0)	paint = true;
			if(paint) {
				stretch_blit(aBmp, bmpCanvas2,
					0,0, aBmp->w, aBmp->h,
					0,0, bmpCanvas2->w,bmpCanvas2->h);

				HWND hWnd = tPanel2->Handle;
				HDC  hDC  = GetDC(hWnd);
				draw_to_hdc (hDC,bmpCanvas2,0,0);
				ReleaseDC(hWnd,hDC);
	
			}
		}

		if(hmut == hMutex3) {
			if(CaptureWorkerForm3->nFrames % frameDivisor == 0)	paint = true;
			if(paint) {
				stretch_blit(aBmp, bmpCanvas3,
					0,0, aBmp->w, aBmp->h,
					0,0, bmpCanvas3->w,bmpCanvas3->h);

				HWND hWnd = tPanel3->Handle;
				HDC  hDC  = GetDC(hWnd);
				draw_to_hdc (hDC,bmpCanvas3,0,0);
				ReleaseDC(hWnd,hDC);

			}
		}



}

void __fastcall TMainCaptureForm::cbRecordClick(TObject *Sender)
{
	nFrames=0;
	edFrame->Text = nFrames;
	BitBtnPlay->Glyph = cbRecord->Checked ? pBmpRec:pBmpRecGr;
	BitBtnPlay1->Glyph = cbRecord->Checked ? pBmpRec:pBmpRecGr;
	BitBtnPlay2->Glyph = cbRecord->Checked ? pBmpRec:pBmpRecGr;
	BitBtnPlay3->Glyph = cbRecord->Checked ? pBmpRec:pBmpRecGr;
}
void __fastcall TMainCaptureForm::BitBtnPlayClick(TObject *Sender)
{

	if(outlet) {
		lsl_destroy_outlet(outlet);
		outlet = NULL;
	}
	double requestedFrameRate = edtRequestedFrameRate->Text.ToDouble();
	lsl_streaminfo info = lsl_create_streaminfo("VideoStream_0","VideoStream_0",1,requestedFrameRate,cft_int32,"");
	lsl_xml_ptr desc = lsl_get_desc(info);
	lsl_xml_ptr chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name","frame");
	lsl_append_child_value(chn,"unit","number");

	lsl_xml_ptr sync = lsl_append_child(desc, "synchronization");
	lsl_append_child_value(sync, "can_drop_samples", "true");

	outlet = lsl_create_outlet(info,0,360);


	cbVideoInputDevice->Enabled = false;
	cbVideoInput->Enabled = false;
	cbVideoInputFormat->Enabled = false;
	SpatialDivisorEdit->Enabled = false;
	cbRecord->Enabled =false;

	Start(CaptureWorkerForm, edOutput0->Text, bmpQueue, hMutex, outlet, requestedFrameRate);
	frameThread = new TFrameThread(this, bmpQueue, hMutex, false);

	BitBtnStop->Enabled=true;
	BitBtnPlay->Enabled=false;

	edtFrameRate->Text = FormatFloat ("0.00", CaptureWorkerForm->VideoGrabber->CurrentFrameRate);


	double ratio = requestedFrameRate/CaptureWorkerForm->VideoGrabber->CurrentFrameRate;
	if(ratio > 1.02 || ratio < .98) {
		edtFrameRate->Text = FormatFloat ("0.00", CaptureWorkerForm->VideoGrabber->CurrentFrameRate);
		Application->MessageBoxA(L"Camera 0 unable to display requested frame rate.", L"Error", MB_OK);
		btStopClick(this);
		return;
	}

}
//---------------------------------------------------------------------------




void __fastcall TMainCaptureForm::Timer1Timer(TObject *Sender)
{
	if(frameThread) {
		BacklogEdit->Text = bmpQueue.size();
		droppedFramesEdit->Text = FormatFloat("0", CaptureWorkerForm->dropped);
		measuredFrameRateEdit->Text = FormatFloat("0.00", CaptureWorkerForm->measuredFrameRate);
	}
	if(frameThread1) {
		BacklogEdit1->Text = bmpQueue1.size();
		droppedFramesEdit1->Text = FormatFloat("0", CaptureWorkerForm1->dropped);
		measuredFrameRateEdit1->Text = FormatFloat("0.00", CaptureWorkerForm1->measuredFrameRate);
	}
	if(frameThread2) {
		BacklogEdit2->Text = bmpQueue2.size();
		droppedFramesEdit2->Text = FormatFloat("0", CaptureWorkerForm2->dropped);
		measuredFrameRateEdit2->Text = FormatFloat("0.00", CaptureWorkerForm2->measuredFrameRate);
	}
	if(frameThread3) {
		BacklogEdit3->Text = bmpQueue3.size();
		droppedFramesEdit3->Text = FormatFloat("0", CaptureWorkerForm3->dropped);
	   	measuredFrameRateEdit3->Text = FormatFloat("0.00", CaptureWorkerForm3->measuredFrameRate);
	}
}
//---------------------------------------------------------------------------










void TMainCaptureForm::SetToVideoMode() {

		Caption = "Four Channel Video stream: video camera";
		MainCaptureForm->Caption = UnicodeString("Video Stream, version ") + getVersion();

		allegro_init();
		set_gdi_color_format();
		set_color_depth(CDEPTH);
		bmpCanvas = create_bitmap_ex(CDEPTH,tPanel->Width,tPanel->Height);
		bmpCanvas1 = create_bitmap_ex(CDEPTH,tPanel1->Width,tPanel1->Height);
		bmpCanvas2 = create_bitmap_ex(CDEPTH,tPanel2->Width,tPanel2->Height);
		bmpCanvas3 = create_bitmap_ex(CDEPTH,tPanel3->Width,tPanel3->Height);

		clear_bitmap(bmpCanvas);
		clear_bitmap(bmpCanvas1);
		clear_bitmap(bmpCanvas2);
		clear_bitmap(bmpCanvas3);
}
//---------------------------------------------------------------------------

void __fastcall TMainCaptureForm::FormClose(TObject *Sender, TCloseAction &Action)
{

	if(BitBtnStop->Enabled) btStopClick(this);
	if(frameThread) {
		frameThread->Terminate();
		delete frameThread; //will not delete till terminated, by VCL design.
		frameThread = NULL;
	}

	if(frameThread1) {
		frameThread1->Terminate();
		delete frameThread1; //will not delete till terminated, by VCL design.
		frameThread1 = NULL;
	}

	delete pBmpRec;
	delete pBmpRecGr;
	delete pBmpPau;
	delete pBmpPauGr;
	if(bmpCanvas) {
		destroy_bitmap(bmpCanvas);
		bmpCanvas = NULL;
	}

	if(bmpCanvas1) {
		destroy_bitmap(bmpCanvas1);
		bmpCanvas1 = NULL;
	}
	if(bmpCanvas2) {
		destroy_bitmap(bmpCanvas2);
		bmpCanvas1 = NULL;
	}
	if(bmpCanvas3) {
		destroy_bitmap(bmpCanvas3);
		bmpCanvas1 = NULL;
	}

}
//---------------------------------------------------------------------------







void __fastcall TMainCaptureForm::FrameDivisorEditChange(TObject *Sender)
{
	bool ex = false;
	int temp;
	try {
		temp = FrameDivisorEdit->Text.ToInt();

	} catch (...) {
		ex = true;
	}
	if(!ex && temp > 0) {
		frameDivisor = temp;
		CaptureWorkerForm->frameDivisor = frameDivisor;
		CaptureWorkerForm1->frameDivisor = frameDivisor;
		CaptureWorkerForm2->frameDivisor = frameDivisor;
		CaptureWorkerForm3->frameDivisor = frameDivisor;
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainCaptureForm::SpatialDivisorEditChange(TObject *Sender)
{
	bool ex = false;
	int temp;
	try {
		temp = SpatialDivisorEdit->Text.ToInt();

	} catch (...) {
		ex = true;
	}
	if(!ex && temp > 0) {
		spatialDivisor = temp;
		cbVideoInputFormatChange(this);
	}
}





void __fastcall TMainCaptureForm::BitBtnPlay1Click(TObject *Sender)
{

	if(outlet1) {
		lsl_destroy_outlet(outlet1);
		outlet1 = NULL;
	}
	double requestedFrameRate = edtRequestedFrameRate->Text.ToDouble();
	lsl_streaminfo info = lsl_create_streaminfo("VideoStream_1","VideoStream_1",1,requestedFrameRate,cft_int32,"");
	lsl_xml_ptr desc = lsl_get_desc(info);
	lsl_xml_ptr chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name","frame");
	lsl_append_child_value(chn,"unit","number");

	lsl_xml_ptr sync = lsl_append_child(desc, "synchronization");
	lsl_append_child_value(sync, "can_drop_samples", "true");

	outlet1 = lsl_create_outlet(info,0,360);


	cbVideoInputDevice1->Enabled = false;
	cbVideoInput->Enabled = false;
	cbVideoInputFormat->Enabled = false;
	SpatialDivisorEdit->Enabled = false;
	cbRecord->Enabled =false;

	Start(CaptureWorkerForm1, edOutput1->Text, bmpQueue1, hMutex1, outlet1, requestedFrameRate);
	frameThread1 = new TFrameThread(this, bmpQueue1, hMutex1, false);

	BitBtnStop->Enabled=true;
	BitBtnPlay1->Enabled=false;

	edtFrameRate->Text = FormatFloat ("0.00", CaptureWorkerForm1->VideoGrabber->CurrentFrameRate);


	double ratio = requestedFrameRate/CaptureWorkerForm1->VideoGrabber->CurrentFrameRate;
	if(ratio > 1.02 || ratio < .98) {
		edtFrameRate->Text = FormatFloat ("0.00", CaptureWorkerForm1->VideoGrabber->CurrentFrameRate);
		Application->MessageBoxA(L"Camera 0 unable to display requested frame rate.", L"Error", MB_OK);
		btStopClick(this);
		return;
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainCaptureForm::BitBtnPlay2Click(TObject *Sender)
{

	if(outlet2) {
		lsl_destroy_outlet(outlet2);
		outlet2 = NULL;
	}
	double requestedFrameRate = edtRequestedFrameRate->Text.ToDouble();
	lsl_streaminfo info = lsl_create_streaminfo("VideoStream_2","VideoStream_2",1,requestedFrameRate,cft_int32,"");
	lsl_xml_ptr desc = lsl_get_desc(info);
	lsl_xml_ptr chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name","frame");
	lsl_append_child_value(chn,"unit","number");

	lsl_xml_ptr sync = lsl_append_child(desc, "synchronization");
	lsl_append_child_value(sync, "can_drop_samples", "true");

	outlet2 = lsl_create_outlet(info,0,360);


	cbVideoInputDevice2->Enabled = false;
	cbVideoInput->Enabled = false;
	cbVideoInputFormat->Enabled = false;
	SpatialDivisorEdit->Enabled = false;
	cbRecord->Enabled =false;

	Start(CaptureWorkerForm2, edOutput2->Text, bmpQueue2, hMutex2, outlet2,requestedFrameRate);
	frameThread2 = new TFrameThread(this, bmpQueue2, hMutex2, false);

	BitBtnStop->Enabled=true;
	BitBtnPlay2->Enabled=false;

	edtFrameRate->Text = FormatFloat ("0.00", CaptureWorkerForm1->VideoGrabber->CurrentFrameRate);


	double ratio = requestedFrameRate/CaptureWorkerForm2->VideoGrabber->CurrentFrameRate;
	if(ratio > 1.02 || ratio < .98) {
		edtFrameRate->Text = FormatFloat ("0.00", CaptureWorkerForm2->VideoGrabber->CurrentFrameRate);
		Application->MessageBoxA(L"Camera 0 unable to display requested frame rate.", L"Error", MB_OK);
		btStopClick(this);
		return;
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainCaptureForm::BitBtnPlay3Click(TObject *Sender)
{
	if(outlet3) {
		lsl_destroy_outlet(outlet3);
		outlet3 = NULL;
	}
	double requestedFrameRate = edtRequestedFrameRate->Text.ToDouble();
	lsl_streaminfo info = lsl_create_streaminfo("VideoStream_3","VideoStream_3",1,requestedFrameRate,cft_int32,"");
	lsl_xml_ptr desc = lsl_get_desc(info);
	lsl_xml_ptr chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name","frame");
	lsl_append_child_value(chn,"unit","number");

	lsl_xml_ptr sync = lsl_append_child(desc, "synchronization");
	lsl_append_child_value(sync, "can_drop_samples", "true");

	outlet3 = lsl_create_outlet(info,0,360);


	cbVideoInputDevice3->Enabled = false;
	cbVideoInput->Enabled = false;
	cbVideoInputFormat->Enabled = false;
	SpatialDivisorEdit->Enabled = false;
	cbRecord->Enabled =false;

	Start(CaptureWorkerForm3, edOutput3->Text, bmpQueue3, hMutex3, outlet3,requestedFrameRate);
	frameThread3 = new TFrameThread(this, bmpQueue3, hMutex3, false);

	BitBtnStop->Enabled=true;
	BitBtnPlay3->Enabled=false;

	edtFrameRate->Text = FormatFloat ("0.00", CaptureWorkerForm3->VideoGrabber->CurrentFrameRate);


	double ratio = requestedFrameRate/CaptureWorkerForm3->VideoGrabber->CurrentFrameRate;
	if(ratio > 1.02 || ratio < .98) {
		edtFrameRate->Text = FormatFloat ("0.00", CaptureWorkerForm3->VideoGrabber->CurrentFrameRate);
		Application->MessageBoxA(L"Camera 0 unable to display requested frame rate.", L"Error", MB_OK);
		btStopClick(this);
		return;
	}
}
//---------------------------------------------------------------------------



