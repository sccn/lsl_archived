/*
Main form for GazeStream.

Written by Matthew Grivich and Andre Vankov, Swartz Center for Computational Neuroscience, UCSD, 2011
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
	char pathC[MAX_PATH];
	SHGetSpecialFolderPath(NULL, pathC, CSIDL_DESKTOP, 1);
	edOutput->Text = UnicodeString(pathC) + "\\capture";

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

	cbRecord->Enabled =true;

	frameThread->Terminate();
	frameThread1->Terminate();
	frameThread2->Terminate();
	frameThread3->Terminate();
	delete frameThread; //will not delete till terminated, by VCL design.
	delete frameThread1;
	delete frameThread2;
	delete frameThread3;

	frameThread = NULL;
	frameThread1 = NULL;
	frameThread2 = NULL;
	frameThread3 = NULL;

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

void __fastcall TMainCaptureForm::Start()
{


		CaptureWorkerForm->VideoGrabber->AnalogVideoStandard = CaptureWorkerForm->VideoGrabber->AnalogVideoStandardIndex ("NTSC M");
		CaptureWorkerForm1->VideoGrabber->AnalogVideoStandard = CaptureWorkerForm1->VideoGrabber->AnalogVideoStandardIndex ("NTSC M");
		CaptureWorkerForm2->VideoGrabber->AnalogVideoStandard = CaptureWorkerForm2->VideoGrabber->AnalogVideoStandardIndex ("NTSC M");
		CaptureWorkerForm3->VideoGrabber->AnalogVideoStandard = CaptureWorkerForm3->VideoGrabber->AnalogVideoStandardIndex ("NTSC M");


		cbRecord->Enabled =false;
		CaptureWorkerForm->VideoGrabber->RecordingInNativeFormat = false;
		CaptureWorkerForm1->VideoGrabber->RecordingInNativeFormat = false;
		CaptureWorkerForm2->VideoGrabber->RecordingInNativeFormat = false;
		CaptureWorkerForm3->VideoGrabber->RecordingInNativeFormat = false;

		if(cbRecord->Checked == true) {
		  //	CaptureWorkerForm->VideoGrabber->VideoProcessing_FlipVertical = true;//HACK FOR POINT GREY BUG, RECORD ONLY, flipVertCheckbox->Checked;
		  //	CaptureWorkerForm1->VideoGrabber->VideoProcessing_FlipVertical = true;//HACK FOR POINT GREY BUG, RECORD ONLY, flipVertCheckbox->Checked;
		  //	CaptureWorkerForm2->VideoGrabber->VideoProcessing_FlipVertical = true;//HACK FOR POINT GREY BUG, RECORD ONLY, flipVertCheckbox->Checked;
		  //	CaptureWorkerForm3->VideoGrabber->VideoProcessing_FlipVertical = true;//HACK FOR POINT GREY BUG, RECORD ONLY, flipVertCheckbox->Checked;


			CaptureWorkerForm->SetQueue(bmpQueue, hMutex, outlet);
			CaptureWorkerForm1->SetQueue(bmpQueue1, hMutex1, outlet1);
			CaptureWorkerForm2->SetQueue(bmpQueue2, hMutex2, outlet2);
			CaptureWorkerForm3->SetQueue(bmpQueue3, hMutex3, outlet3);

			//save to specified filename
			int n=0;
			UnicodeString outputFileName;
			UnicodeString outputFileName1;
			UnicodeString outputFileName2;
			UnicodeString outputFileName3;
			while(true) {
				if(cbCompress->Checked) {
					outputFileName =  edOutput->Text+ IntToStr(n)+"_0.asf";
					outputFileName1 =  edOutput->Text+ IntToStr(n)+"_1.asf";
					outputFileName2 =  edOutput->Text+ IntToStr(n)+"_2.asf";
					outputFileName3 =  edOutput->Text+ IntToStr(n)+"_3.asf";
				} else {
					outputFileName =  edOutput->Text+ IntToStr(n)+"_0.avi";
					outputFileName1 =  edOutput->Text+ IntToStr(n)+"_1.avi";
					outputFileName2 =  edOutput->Text+ IntToStr(n)+"_2.avi";
					outputFileName3 =  edOutput->Text+ IntToStr(n)+"_3.avi";
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

			CaptureWorkerForm->VideoGrabber->RecordingFileName= outputFileName;
			CaptureWorkerForm1->VideoGrabber->RecordingFileName= outputFileName1;
			CaptureWorkerForm2->VideoGrabber->RecordingFileName= outputFileName2;
			CaptureWorkerForm3->VideoGrabber->RecordingFileName= outputFileName3;

			//record audio?
			CaptureWorkerForm->VideoGrabber->AudioRecording = cbRecordAudio->Checked;
			CaptureWorkerForm1->VideoGrabber->AudioRecording = cbRecordAudio->Checked;
			CaptureWorkerForm2->VideoGrabber->AudioRecording = cbRecordAudio->Checked;
			CaptureWorkerForm3->VideoGrabber->AudioRecording = cbRecordAudio->Checked;

			//turn off some unused features
			CaptureWorkerForm->VideoGrabber->FrameGrabber = TFrameGrabber (0);
			CaptureWorkerForm1->VideoGrabber->FrameGrabber = TFrameGrabber (0);
			CaptureWorkerForm2->VideoGrabber->FrameGrabber = TFrameGrabber (0);
			CaptureWorkerForm3->VideoGrabber->FrameGrabber = TFrameGrabber (0);

			//save as compressed asf
			CaptureWorkerForm->VideoGrabber->VideoCompressor = cbVideoCodecs->ItemIndex;
			CaptureWorkerForm1->VideoGrabber->VideoCompressor = cbVideoCodecs->ItemIndex;
			CaptureWorkerForm2->VideoGrabber->VideoCompressor = cbVideoCodecs->ItemIndex;
			CaptureWorkerForm3->VideoGrabber->VideoCompressor = cbVideoCodecs->ItemIndex;

			if(cbCompress->Checked) {
				CaptureWorkerForm->VideoGrabber->CompressionMode = cm_CompressOnTheFly;
				CaptureWorkerForm->VideoGrabber->RecordingMethod = rm_ASF;
				CaptureWorkerForm1->VideoGrabber->CompressionMode = cm_CompressOnTheFly;
				CaptureWorkerForm1->VideoGrabber->RecordingMethod = rm_ASF;
				CaptureWorkerForm2->VideoGrabber->CompressionMode = cm_CompressOnTheFly;
				CaptureWorkerForm2->VideoGrabber->RecordingMethod = rm_ASF;
				CaptureWorkerForm3->VideoGrabber->CompressionMode = cm_CompressOnTheFly;
				CaptureWorkerForm3->VideoGrabber->RecordingMethod = rm_ASF;
			} else {

				CaptureWorkerForm->VideoGrabber->CompressionMode = cm_NoCompression;
				CaptureWorkerForm->VideoGrabber->RecordingMethod = rm_AVI;
				CaptureWorkerForm1->VideoGrabber->CompressionMode = cm_NoCompression;
				CaptureWorkerForm1->VideoGrabber->RecordingMethod = rm_AVI;
				CaptureWorkerForm2->VideoGrabber->CompressionMode = cm_NoCompression;
				CaptureWorkerForm2->VideoGrabber->RecordingMethod = rm_AVI;
				CaptureWorkerForm3->VideoGrabber->CompressionMode = cm_NoCompression;
				CaptureWorkerForm3->VideoGrabber->RecordingMethod = rm_AVI;
			}


			CaptureWorkerForm->VideoGrabber->HoldRecording = false;
			CaptureWorkerForm1->VideoGrabber->HoldRecording = false;
			CaptureWorkerForm2->VideoGrabber->HoldRecording = false;
			CaptureWorkerForm3->VideoGrabber->HoldRecording = false;

			CaptureWorkerForm->VideoGrabber->StartRecording();
			CaptureWorkerForm1->VideoGrabber->StartRecording();
			CaptureWorkerForm2->VideoGrabber->StartRecording();
			CaptureWorkerForm3->VideoGrabber->StartRecording();


		} else {
			CaptureWorkerForm->VideoGrabber->VideoProcessing_FlipVertical = false;//HACK FOR POINT GREY BUG, RECORD ONLY, flipVertCheckbox->Checked;
			CaptureWorkerForm1->VideoGrabber->VideoProcessing_FlipVertical = false;//HACK FOR POINT GREY BUG, RECORD ONLY, flipVertCheckbox->Checked;
			CaptureWorkerForm2->VideoGrabber->VideoProcessing_FlipVertical = false;//HACK FOR POINT GREY BUG, RECORD ONLY, flipVertCheckbox->Checked;
			CaptureWorkerForm3->VideoGrabber->VideoProcessing_FlipVertical = false;//HACK FOR POINT GREY BUG, RECORD ONLY, flipVertCheckbox->Checked;

			CaptureWorkerForm->SetQueue(bmpQueue, hMutex, outlet);
			CaptureWorkerForm1->SetQueue(bmpQueue1, hMutex1, outlet1);
			CaptureWorkerForm2->SetQueue(bmpQueue2, hMutex2, outlet2);
			CaptureWorkerForm3->SetQueue(bmpQueue3, hMutex3, outlet3);

			CaptureWorkerForm->VideoGrabber->StartPreview();
			CaptureWorkerForm1->VideoGrabber->StartPreview();
			CaptureWorkerForm2->VideoGrabber->StartPreview();
			CaptureWorkerForm3->VideoGrabber->StartPreview();


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
				droppedFramesEdit->Text = FormatFloat("0", CaptureWorkerForm->dropped);
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
				droppedFramesEdit1->Text = FormatFloat("0", CaptureWorkerForm1->dropped);
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
				droppedFramesEdit2->Text = FormatFloat("0", CaptureWorkerForm2->dropped);
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
				droppedFramesEdit3->Text = FormatFloat("0", CaptureWorkerForm3->dropped);
			}
		}



}

void __fastcall TMainCaptureForm::cbRecordClick(TObject *Sender)
{
	nFrames=0;
	edFrame->Text = nFrames;
	BitBtnPlay->Glyph = cbRecord->Checked ? pBmpRec:pBmpRecGr;
}


void __fastcall TMainCaptureForm::BitBtnPlayClick(TObject *Sender)
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

	lsl_streaminfo info = lsl_create_streaminfo("VideoStream_0","VideoStream_0",1,30,cft_int32,"");
	lsl_xml_ptr desc = lsl_get_desc(info);
	lsl_xml_ptr chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name","frame");
	lsl_append_child_value(chn,"unit","number");

	outlet = lsl_create_outlet(info,0,360);

	info = lsl_create_streaminfo("VideoStream_1","VideoStream_1",1,30,cft_int32,"");
	desc = lsl_get_desc(info);
	chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name","frame");
	lsl_append_child_value(chn,"unit","number");

	outlet1 = lsl_create_outlet(info,0,360);

	info = lsl_create_streaminfo("VideoStream_2","VideoStream_2",1,30,cft_int32,"");
	desc = lsl_get_desc(info);
	chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name","frame");
	lsl_append_child_value(chn,"unit","number");

	outlet2 = lsl_create_outlet(info,0,360);

	info = lsl_create_streaminfo("VideoStream_3","VideoStream_3",1,30,cft_int32,"");
	desc = lsl_get_desc(info);
	chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name","frame");
	lsl_append_child_value(chn,"unit","number");

	outlet3 = lsl_create_outlet(info,0,360);


	cbVideoInputDevice->Enabled = false;
	cbVideoInputDevice1->Enabled = false;
	cbVideoInputDevice2->Enabled = false;
	cbVideoInputDevice3->Enabled = false;
	cbVideoInput->Enabled = false;
	cbVideoInputFormat->Enabled = false;
	SpatialDivisorEdit->Enabled = false;

	Start();
	frameThread = new TFrameThread(this, bmpQueue, hMutex, false);
	frameThread1 = new TFrameThread(this, bmpQueue1, hMutex1, false);
	frameThread2 = new TFrameThread(this, bmpQueue2, hMutex2, false);
	frameThread3 = new TFrameThread(this, bmpQueue3, hMutex3, false);

	BitBtnStop->Enabled=true;
	BitBtnPlay->Enabled=false;

	edtFrameRate->Text = 15.0;//HACK FOR POINTGREY BUG FormatFloat ("0.00", CaptureWorkerForm->VideoGrabber->CurrentFrameRate);

	  /*			Sleep(100);
			printf("sleep0: %g\n", frameThread->sleepTime);
			printf("sleep1: %g\n", frameThread1->sleepTime);
			printf("sleep2: %g\n", frameThread2->sleepTime);
			printf("sleep3: %g\n", frameThread3->sleepTime);  */
}
//---------------------------------------------------------------------------




void __fastcall TMainCaptureForm::Timer1Timer(TObject *Sender)
{
	if(frameThread) {
		BacklogEdit->Text = bmpQueue.size();
	}
	if(frameThread1) {
		BacklogEdit1->Text = bmpQueue1.size();
	}
	if(frameThread2) {
		BacklogEdit2->Text = bmpQueue2.size();
	}
	if(frameThread3) {
		BacklogEdit3->Text = bmpQueue3.size();
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




