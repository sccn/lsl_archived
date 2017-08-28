/*
Main form for GazeStream.

Written by Matthew Grivich, Swartz Center for Computational Neuroscience, UCSD, 2011-2014
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
#include <memory>


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


int nFrames = 0;
#define MAX_STREAMS 200



std::unique_ptr<TFrameThread> frameThread;

HANDLE hMutex = 0;


lsl_outlet outlet = 0;
lsl_outlet videoFrameOutlet = 0;
std::unique_ptr<TStreamThread> gazestreamThread;

BITMAP * bmpCanvas = NULL;		// User declarations

int frameDivisor = 1;
int spatialDivisor = 1;
double maxEccentricity = 1.0;
double xParallaxCorrection = 0.0;
double yParallaxCorrection = 0.0;
int numberOfMarkers = -1;

lsl_inlet frameInlet = NULL;


#define CDEPTH 24
 void __fastcall TMainCaptureForm::CheckBox1Click(TObject *Sender)
{
	allegro_init();
	set_gdi_color_format();
	set_color_depth(CDEPTH);
	bmpCanvas = create_bitmap_ex(CDEPTH,tPanel->Width,tPanel->Height);

	clear_bitmap(bmpCanvas);

}


void AssignListToComboBox (TComboBox* ComboBox, String List, int Index)
{
   ComboBox->Items->Text = List;
   if (ComboBox->Items->Count > 0) {
	  ComboBox->ItemIndex = Index;
   }
}


void __fastcall TMainCaptureForm::FormCreate(TObject *Sender)
{

     MainCaptureForm->Caption = UnicodeString("Gaze Stream, version ") + getVersion();
	//There has to be some option other than silent failure and dialog box, but I haven't found it yet.
	//	_control87(MCW_EM, MCW_EM);  //Turn off FPU generated floating point exceptions. Threads still fail however.
	_control87( 0x1372, 0x137F ); //turns on dialog exceptions, but not in TThread. Exceptions in TThread cause the thread to die.

	pBmpRec = boost::shared_ptr<Graphics::TBitmap>(new Graphics::TBitmap);
	pBmpRec->LoadFromFile("Rec.bmp");

	pBmpRecGr = boost::shared_ptr<Graphics::TBitmap>(new Graphics::TBitmap);
	pBmpRecGr-> LoadFromFile("Rec-gr.bmp");

	//set default output folder to c:\Users\currentUser\Desktop\capture
	char pathC[MAX_PATH];
	SHGetSpecialFolderPath(NULL, pathC, CSIDL_DESKTOP, 1);
	edOutput->Text = UnicodeString(pathC) + "\\capture";

	hMutex = CreateMutex(0,false,0);
	BitBtnStop->Enabled = false;
	nOutlinesEditChange(this);
	crRadiusMultiplierEditChange(this);
	crRadiusMaxEditChange(this);
	eyeRadiusMaxEditChange(this);
	FrameDivisorEditChange(this);
	SpatialDivisorEditChange(this);
	maxEccentricityEditChange(this);
	xParallaxCorrectionEditChange(this);
	yParallaxCorrectionEditChange(this);
	cbSceneCalibColorChange(this);
	cbReferenceCalibColorChange(this);
	numberOfMarkersEditChange(this);

	gu = boost::shared_ptr<TGazeUtil>(new TGazeUtil());

	allegro_init();
	set_gdi_color_format();
	set_color_depth(CDEPTH);

	nFrames=0;
}


int acqWidth=0, acqHeight=0;

void __fastcall TMainCaptureForm::cbVideoInputDeviceChange(TObject *Sender)
{
	if(BitBtnStop->Enabled) btStopClick(this);
	if(cbVideoInputDevice->ItemIndex != -1) {
		CaptureWorkerForm->VideoGrabber->VideoDevice = cbVideoInputDevice->ItemIndex;
		AssignListToComboBox (cbVideoInputFormat, CaptureWorkerForm->VideoGrabber->VideoSizes, CaptureWorkerForm->VideoGrabber->VideoSize);
		AssignListToComboBox (cbVideoInput, CaptureWorkerForm->VideoGrabber->VideoInputs, CaptureWorkerForm->VideoGrabber->VideoInput);
		cbVideoInputFormatChange(this);
		cbVideoInputChange(this);
	}
}

void __fastcall TMainCaptureForm::cbAudioInputDeviceChange(TObject *Sender)
{
	if(BitBtnStop->Enabled) btStopClick(this);
	if(cbAudioInputDevice->ItemIndex != -1) {
		CaptureWorkerForm->VideoGrabber->AudioDevice = cbAudioInputDevice->ItemIndex;
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainCaptureForm::cbVideoInputChange(TObject *Sender)
{
	if(cbVideoInput->ItemIndex != -1) {
		CaptureWorkerForm->VideoGrabber->VideoInput = cbVideoInput->ItemIndex;
	}
}
//---------------------------------------------------------------------------

void __fastcall TMainCaptureForm::cbVideoInputFormatChange(TObject *Sender)
{
	if(BitBtnStop->Enabled) btStopClick(this);
	if (cbVideoInputFormat->ItemIndex != -1) {
		CaptureWorkerForm->VideoGrabber->VideoSize = cbVideoInputFormat->ItemIndex;
		acqWidth = CaptureWorkerForm->VideoGrabber->Display_Width;
		acqHeight = CaptureWorkerForm->VideoGrabber->Display_Height;


		tPanel->Width = acqWidth/spatialDivisor;

		crRoiRight->Max = acqWidth;
		if(crRoiRight->Position > acqWidth) crRoiRight->Position = acqWidth;

		tbRight->Max = acqWidth;
		if(tbRight->Position > acqWidth) tbRight->Position = acqWidth;

		crRoiLeft->Max = acqWidth;
		if(crRoiLeft->Position > acqWidth) crRoiLeft->Position = acqWidth;

		tbLeft->Max = acqWidth;
		if(tbLeft->Position > acqWidth) tbLeft->Position = acqWidth;

		tPanel->Height = acqHeight/spatialDivisor;

		crRoiBottom->Max = acqHeight;
		if(crRoiBottom->Position > acqHeight) crRoiBottom->Position = acqHeight;

		tbLower->Max = acqHeight;
		if(tbLower->Position > acqHeight) tbLower->Position = acqHeight;

		crRoiTop->Max = acqHeight;
		if(crRoiTop->Position > acqHeight) crRoiTop->Position = acqHeight;

		tbUpper->Max = acqHeight;
		if(tbUpper->Position > acqHeight) tbUpper->Position = acqHeight;

		if(bmpCanvas) {
			destroy_bitmap(bmpCanvas);
			bmpCanvas = NULL;
		}
		bmpCanvas = create_bitmap_ex(CDEPTH,acqWidth/spatialDivisor,acqHeight/spatialDivisor);
		clear_bitmap(bmpCanvas);

		if(videoFrameOutlet) {
			lsl_destroy_outlet(videoFrameOutlet);
			videoFrameOutlet = NULL;
		}
		if(enableFrameSending->Checked == true) {
			char * streamName = (AnsiString("VideoFrames_") + AnsiString(IdentifierEdit->Text)).c_str();
			lsl_streaminfo info = lsl_create_streaminfo(streamName,"VideoRaw",acqWidth*acqHeight*3,0,cft_int8,"1e5f8b95-68cf-418c-8538-45e05ad791df");
			char s[20];
			lsl_xml_ptr desc = lsl_get_desc(info);
			lsl_xml_ptr chn = lsl_append_child(desc, "encoding");
			sprintf(s, "%d", acqWidth);
			lsl_append_child_value(chn,"width",s);
			sprintf(s, "%d", acqHeight);
			lsl_append_child_value(chn,"height",s);
			lsl_append_child_value(chn,"color_channels","3");
			lsl_append_child_value(chn,"color_format","RGB");
			lsl_append_child_value(chn,"color_space","sRGB");
			lsl_append_child_value(chn,"codec","RAW");

			videoFrameOutlet = lsl_create_outlet(info,0,360);
		}
	}

}
//---------------------------------------------------------------------------

void __fastcall TMainCaptureForm::btStopClick(TObject *Sender)
{

	if(cbRecord->Checked == true) {
		CaptureWorkerForm->VideoGrabber->StopRecording();
	} else {
		CaptureWorkerForm->VideoGrabber->StopPreview();
	}

	BitBtnStop->Enabled=false;
	BitBtnPlay->Enabled=true;

	cbRecord->Enabled =true;

	frameThread->Terminate();
	frameThread.reset(); //will not delete till terminated, by VCL design.

	nFrames = 0;

	cbVideoInputDevice->Enabled = true;
	cbVideoInput->Enabled = true;
	cbVideoInputFormat->Enabled = true;
	SpatialDivisorEdit->Enabled = true;
	cbAudioInputDevice->Enabled = true;

}

//---------------------------------------------------------------------------

void __fastcall TMainCaptureForm::Start()
{
		double requestedFrameRate = edtRequestedFrameRate->Text.ToDouble();
 		CaptureWorkerForm->VideoGrabber->VideoProcessing_FlipVertical = flipVertCheckbox->Checked;
		CaptureWorkerForm->VideoGrabber->AnalogVideoStandard = CaptureWorkerForm->VideoGrabber->AnalogVideoStandardIndex ("NTSC M");
		cbRecord->Enabled =false;
		CaptureWorkerForm->VideoGrabber->RecordingInNativeFormat = false;
		CaptureWorkerForm->VideoGrabber->AudioDeviceRendering = cbRecordAudio->Checked;

		if(cbRecord->Checked == true) {
			CaptureWorkerForm->SetQueue(bmpQueue, hMutex);

			//save to specified filename
			int n=0;
			UnicodeString outputFileName;
			while(true) {
				if(cbCompress->Checked)
					outputFileName =  edOutput->Text+ IntToStr(n)+".asf";
				else
					outputFileName =  edOutput->Text+ IntToStr(n)+".avi";

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

			//record audio?
			CaptureWorkerForm->VideoGrabber->AudioRecording = cbRecordAudio->Checked;

			//turn off some unused features
			CaptureWorkerForm->VideoGrabber->FrameGrabber = TFrameGrabber (0);

			//save as compressed asf
			CaptureWorkerForm->VideoGrabber->VideoCompressor = cbVideoCodecs->ItemIndex;

			if(cbCompress->Checked) {
				CaptureWorkerForm->VideoGrabber->CompressionMode = cm_CompressOnTheFly;
				CaptureWorkerForm->VideoGrabber->RecordingMethod = rm_ASF;
			} else {

				CaptureWorkerForm->VideoGrabber->CompressionMode = cm_NoCompression;
				CaptureWorkerForm->VideoGrabber->RecordingMethod = rm_AVI;
			}


			CaptureWorkerForm->VideoGrabber->HoldRecording = false;
		//	CaptureWorkerForm->VideoGrabber->VideoDevice = CaptureWorkerForm->VideoGrabber->VideoDeviceIndex("Dazzle DVC100 Video Device");
			CaptureWorkerForm->VideoGrabber->StartRecording();


		} else {
			CaptureWorkerForm->SetQueue(bmpQueue, hMutex);
			CaptureWorkerForm->VideoGrabber->StartPreview();
		}

}


enum TKind {isEye,isSceneCalib,isScene, isVideo, isSceneMultiCalib} ;
TKind isKind = -1;
enum SceneCalibColor {RED, GREEN, BLUE, WHITE};
SceneCalibColor sceneCalibColor = -1;
SceneCalibColor referenceCalibColor = -1;


void drawEllipse(BITMAP *bmp, double x0, double y0, double rA, double rB, double theta, int color) {
	int nPoints = 20;
	double tempX, tempY;
	int x = 0;
	int y = 0;
	tempX = rA;
	tempY = 0;
	double oldX = rA*cos(theta)+x0;
	double oldY = rA*sin(theta)+y0;
		 //	circlefill(bmpCanvas, oldX, oldY, 3, makecol(0,0,0));
	for(int i=0; i<nPoints; i++) {
		tempX = rA*cos(2*PI*i/nPoints);
		tempY = rB*sin(2*PI*i/nPoints);
		x = (int) (tempX*cos(theta)-tempY*sin(theta)+x0);
		y = (int) (tempX*sin(theta)+tempY*cos(theta)+y0);
		line(bmp, oldX, oldY, x, y, color);
		oldX = x;
		oldY = y;
	}
	line(bmp, oldX, oldY, rA*cos(theta)+x0, rA*sin(theta)+y0, color);

}

list<boost::shared_ptr<TOutline> >  TMainCaptureForm::findLargestOutlines(BITMAP *aBmp, boolean above,
	int tbLeftPosition, int tbRightPosition, int tbLowerPosition, int tbUpperPosition, bool paint, int nOutlinesToFind) {

		int subsampling = SubsamplingEdit->Text.ToIntDef(10);
		int subWidth = (tbRightPosition - tbLeftPosition) /subsampling;
		int subHeight = (tbLowerPosition - tbUpperPosition)/subsampling;
		if(subWidth <=0 || subHeight <=0) {
			list<boost::shared_ptr<TOutline> > emptyList;
			return emptyList;
		}
		unsigned char ** subsampled = new2D<unsigned char>(subWidth, subHeight, 0);
		signed char ** mask = new2D<signed char>(subWidth, subHeight, 0);
		int xOffset = subsampling/2+tbLeftPosition;
		int yOffset =  subsampling/2+tbUpperPosition;
		int totalPoints = 0;

		boolean withinThresh = false;

		//subsample and threshold the image.
		for(int y=0; y<subHeight; y++) {
			for(int x=0; x<subWidth; x++) {
				int blue = aBmp->line[y*subsampling+yOffset]
					[(x*subsampling+xOffset)*(CDEPTH/8)];
				int green = aBmp->line[y*subsampling+yOffset]
					[(x*subsampling+xOffset)*(CDEPTH/8)+1];
				int red = aBmp->line[y*subsampling+yOffset]
					[(x*subsampling+xOffset)*(CDEPTH/8)+2];
				//if scene calib, look explicitly for a positive colored spot.

				bool isGood;
				switch(referenceCalibColor) {
					case RED:
						isGood = (255*red)/(blue+green+red+1) >= crThreshold->Position && red >= crThreshold->Position;
						break;
					case GREEN:
						isGood = (255*green)/(blue+green+red+1) >= crThreshold->Position && green >= crThreshold->Position;
						break;
					case BLUE:
						isGood = (255*blue)/(blue+green+red+1) >= crThreshold->Position && blue >= crThreshold->Position;
						break;
					case WHITE:
						isGood = (blue+green+red) / 3 >= crThreshold->Position;
						break;

				}
				if(isGood) {
					subsampled[x][y] = 1;
					if(paint) {
						bmpCanvas->line[(y*subsampling+yOffset)/spatialDivisor]
							[(x*subsampling+xOffset)/spatialDivisor*(CDEPTH/8)] = 0;
						bmpCanvas->line[(y*subsampling+yOffset)/spatialDivisor]
							[(x*subsampling+xOffset)/spatialDivisor*(CDEPTH/8)+1] = 0;
						bmpCanvas->line[(y*subsampling+yOffset)/spatialDivisor]
							[(x*subsampling+xOffset)/spatialDivisor*(CDEPTH/8)+2] = 255;
					}
					totalPoints++;
				} else {
					subsampled[x][y] = 0;
				}


			}
		}

		//find outlines

		list<boost::shared_ptr<TOutline> > outlines;
	   //	TOutline *largestOutline = new TOutline(subWidth, subHeight);
		for(int y=0; y<subHeight; y++) {
			int inSpot = 0;
			for(int x=0; x<subWidth; x++) {
				//inSpot and mask together keep track of boundary transitions.
				//mask[x][y] = +1 when a spot is entered, -1 when it is exited.
				//Donut holes are removed/ignored, because interior boundaries are never tested.
				inSpot = mask[x][y] + inSpot;
				if(inSpot == 0) {
					if(subsampled[x][y] == 1) {
						boost::shared_ptr<TOutline> outline(new TOutline(subWidth, subHeight));
						int nPointsInSpot = outline->findOutline((const unsigned char **) subsampled, mask,x, y, bmpCanvas->line);
				//		outline->drawSpot(bmpCanvas->line, mask); for testing purposes
						list<boost::shared_ptr<TOutline> >::iterator i = outlines.begin();
						int position = 0;

						while(i!= outlines.end() && ((boost::shared_ptr<TOutline>) (*i))->getNumberOfPoints() > nPointsInSpot) {
								++i;
								++position;
							}

						//if new spot is in the largest nOutlinesToFind
						if (position < nOutlinesToFind) {

							outline->estimateParams();

							//if eccentricity is somewhat circle like. This cuts long edges is the data.
							if(!_isnan(outline->eccentricity) && outline->eccentricity < maxEccentricity
									&& !_isnan(outline->r)) {
								outlines.insert(i, outline);
							}

						}
						inSpot++;
					}
				}

			}
		}

		while(outlines.size() > nOutlinesToFind) {
			outlines.pop_back();
		}

		delete2D(mask, subWidth);
		delete2D(subsampled, subWidth);
		return outlines;
}

boost::shared_ptr<TOutline> TMainCaptureForm::findLargestOutline(BITMAP *aBmp, boolean above,
	int tbLeftPosition, int tbRightPosition, int tbLowerPosition, int tbUpperPosition, bool paint) {

		int subsampling = SubsamplingEdit->Text.ToIntDef(10);
		int subWidth = (tbRightPosition - tbLeftPosition) /subsampling;
		int subHeight = (tbLowerPosition - tbUpperPosition)/subsampling;
		if(subWidth <=0 || subHeight <=0) {
			return boost::shared_ptr<TOutline>(new TOutline(subWidth, subHeight));
		}
		unsigned char ** subsampled = new2D<unsigned char>(subWidth, subHeight, 0);
		signed char ** mask = new2D<signed char>(subWidth, subHeight, 0);
		int xOffset = subsampling/2+tbLeftPosition;
		int yOffset =  subsampling/2+tbUpperPosition;
		int totalPoints = 0;

		boolean withinThresh = false;

		//subsample and threshold the image.
		for(int y=0; y<subHeight; y++) {
			for(int x=0; x<subWidth; x++) {
				int blue = aBmp->line[y*subsampling+yOffset]
					[(x*subsampling+xOffset)*(CDEPTH/8)];
				int green = aBmp->line[y*subsampling+yOffset]
					[(x*subsampling+xOffset)*(CDEPTH/8)+1];
				int red = aBmp->line[y*subsampling+yOffset]
					[(x*subsampling+xOffset)*(CDEPTH/8)+2];
				//if scene calib, look explicitly for a positive colored spot.
				if(isKind==isSceneCalib || isKind==isSceneMultiCalib) {
					bool isGood;
					switch(sceneCalibColor) {
						case RED:
							isGood = (255*red)/(blue+green+red+1) >= tbThreshold->Position && red >= tbThreshold->Position;
							break;
						case GREEN:
							isGood = (255*green)/(blue+green+red+1) >= tbThreshold->Position && green >= tbThreshold->Position;
							break;
						case BLUE:
							isGood = (255*blue)/(blue+green+red+1) >= tbThreshold->Position && blue >= tbThreshold->Position;
							break;
						case WHITE:
							isGood = (blue+green+red) / 3 >= tbThreshold->Position;
							break;

					}
					if(isGood) {
						subsampled[x][y] = 1;
						if(paint) {
							bmpCanvas->line[(y*subsampling+yOffset)/spatialDivisor]
								[(x*subsampling+xOffset)/spatialDivisor*(CDEPTH/8)] = 0;
							bmpCanvas->line[(y*subsampling+yOffset)/spatialDivisor]
								[(x*subsampling+xOffset)/spatialDivisor*(CDEPTH/8)+1] = 0;
							bmpCanvas->line[(y*subsampling+yOffset)/spatialDivisor]
								[(x*subsampling+xOffset)/spatialDivisor*(CDEPTH/8)+2] = 255;
						}
						totalPoints++;
					} else {
						subsampled[x][y] = 0;
					}
				} else if(isKind==isEye) {
					//used to find the cornea reflection. Points near the cornea reflection
					//are not included in the pupil fit.
					if(above) {
						//above "blue" but image is black and white.
						if(blue >=crThreshold->Position) {
							subsampled[x][y] = 1;
							if(paint) {
								bmpCanvas->line[(y*subsampling+yOffset)/spatialDivisor]
									[(x*subsampling+xOffset)/spatialDivisor*(CDEPTH/8)] = 0;
								bmpCanvas->line[(y*subsampling+yOffset)/spatialDivisor]
									[(x*subsampling+xOffset)/spatialDivisor*(CDEPTH/8)+1] = 0;
								bmpCanvas->line[(y*subsampling+yOffset)/spatialDivisor]
									[(x*subsampling+xOffset)/spatialDivisor*(CDEPTH/8)+2] = 255;
							}
							totalPoints++;
						} else {
							subsampled[x][y] = 0;
						}
					//used to find the pupil
					} else {
						//below "blue" but image is black and white.
						if(blue <=tbThreshold->Position) {
							subsampled[x][y] = 1;
							if(paint) {
								bmpCanvas->line[(y*subsampling+yOffset)/spatialDivisor]
									[(x*subsampling+xOffset)/spatialDivisor*(CDEPTH/8)] = 0;
								bmpCanvas->line[(y*subsampling+yOffset)/spatialDivisor]
									[(x*subsampling+xOffset)/spatialDivisor*(CDEPTH/8)+1] = 255;
								bmpCanvas->line[(y*subsampling+yOffset)/spatialDivisor]
									[(x*subsampling+xOffset)/spatialDivisor*(CDEPTH/8)+2] = 0;
							}
							totalPoints++;


						} else {
							subsampled[x][y] = 0;
						}
					}
				}



			}
		}
		//find outlines
		int pointsInLargest = 0;
		int nOutlines = 0;
		boost::shared_ptr<TOutline> largestOutline(new TOutline(subWidth, subHeight));
		for(int y=0; y<subHeight; y++) {
			int inSpot = 0;
			for(int x=0; x<subWidth; x++) {
				//inSpot and mask together keep track of boundary transitions.
				//mask[x][y] = +1 when a spot is entered, -1 when it is exited.
				//Donut holes are removed/ignored, because interior boundaries are never tested.
				inSpot = mask[x][y] + inSpot;
				if(inSpot == 0) {
					if(subsampled[x][y] == 1) {
						boost::shared_ptr<TOutline> outline(new TOutline(subWidth, subHeight));
						nOutlines++;
						int nPointsInSpot = outline->findOutline((const unsigned char **) subsampled, mask,x, y, bmpCanvas->line);
				//		outline->drawSpot(bmpCanvas->line, mask); for testing purposes

						//if new spot is larger than old spot
						if (nPointsInSpot > pointsInLargest) {
							outline->estimateParams();

							//if eccentricity is somewhat circle like. This cuts long edges is the data.
							if(!_isnan(outline->eccentricity) && outline->eccentricity < maxEccentricity
									&& !_isnan(outline->r)) {
								pointsInLargest = nPointsInSpot;
								largestOutline = outline;
							}
						}
						inSpot++;
					}
				}

			}
		}

		//If autothreshold is checked, try to keep the number of spots constant.
		//This is fairly robust to contrast and gain changes.
		if(autoThresholdBox->Checked == true && !above) {
			if(nOutlines >= nOutlinesDesired && tbThreshold->Position > 10) tbThreshold->Position--;
			if(nOutlines < nOutlinesDesired) tbThreshold->Position++;
		}
		delete2D(mask, subWidth);
		delete2D(subsampled, subWidth);
		return largestOutline;
}

 /*
void TMainCaptureForm::fitCircle(BITMAP *aBmp, boost::shared_ptr<TOutline> largestOutline, double *x0, double *y0, double *radius, double crX0, double crY0, double crRadius, boolean above,
	int tbLeftPosition, int tbRightPosition, int tbLowerPosition, int tbUpperPosition, bool paint) {

		int subsampling = SubsamplingEdit->Text.ToIntDef(10);
		int xOffset = subsampling/2+tbLeftPosition;
		int yOffset =  subsampling/2+tbUpperPosition;

		double ** x = largestOutline->createDataForFit((crX0-xOffset)/subsampling, (crY0-yOffset)/subsampling, crRadius/subsampling);
		double * y = new1D<double>(largestOutline->nPointsMinusCR, 0.0);
		double * sig = new1D<double>(largestOutline->nPointsMinusCR, 1.0);
 //		largestOutline->drawOutline(bmpCanvas->line,x,largestOutline->getNumberOfPoints()); for testing purposes
		boost::shared_ptr<Fitter> fit(new Fitter());
		//reduce dropped frames due to perverse distibutions.  Typically less than 10 iterations are needed.
		fit->maxIterations = 50;
		boost::shared_ptr<CircleFunction> f;
	   //	CircleFunction *f = NULL;

		*x0 = *y0 = *radius = NAN; // will be returned if fit fails
		try {
			if(largestOutline->nPointsMinusCR > 5) {

				f= boost::shared_ptr<CircleFunction> (new CircleFunction(largestOutline->x0, largestOutline->y0, largestOutline->r));
				fit->fit(f.get(),x,y,sig,2,largestOutline->nPointsMinusCR);

				double *pOut = fit->getParameters();
				*x0 = pOut[0]*subsampling+xOffset;
				*y0 = pOut[1]*subsampling+yOffset;
				*radius = pOut[2]*subsampling;

				if(above) {
					*radius *= crRadiusMultiplier;
					if(*radius > crRadiusMax) *radius = crRadiusMax;
				}
				if(paint) {
					if(above)
						circle(bmpCanvas, *x0/spatialDivisor, *y0/spatialDivisor, *radius/spatialDivisor, makecol(0,255,0));
					else
						circle(bmpCanvas,*x0/spatialDivisor, *y0/spatialDivisor, *radius/spatialDivisor, makecol(255,0,0));
				}
			}
		} catch (exception& ex) {
 //			printf("caught umaincapture\n");
 //			printf("exception: %s\n", ex.what());

		}
		delete2D(x, 2);
		delete1D(y);
		delete1D(sig);


	}
 */
void TMainCaptureForm::fitCircle(BITMAP *aBmp, boost::shared_ptr<TOutline> largestOutline, double *x0, double *y0, double *radius, double crX0, double crY0, double crRadius, boolean above,
	int tbLeftPosition, int tbRightPosition, int tbLowerPosition, int tbUpperPosition, bool paint) {

		int subsampling = SubsamplingEdit->Text.ToIntDef(10);
		int xOffset = subsampling/2+tbLeftPosition;
		int yOffset =  subsampling/2+tbUpperPosition;

		ublas::matrix<double> x = largestOutline->createMatrixForFit((crX0-xOffset)/subsampling, (crY0-yOffset)/subsampling, crRadius/subsampling);

		*x0 = *y0 = *radius = NAN; // will be returned if fit fails
		try {
			if(largestOutline->nPointsMinusCR > 5) {
				double xc, yc, r;
				FitCircle(x, largestOutline->nPointsMinusCR, &xc,&yc,&r);
				xc = xc*subsampling + xOffset;
				yc = yc*subsampling + yOffset;
				r = r*subsampling;

				*x0 = xc;
				*y0 = yc;
				*radius = r;

				if(above) {
					*radius *= crRadiusMultiplier;
					if(*radius > crRadiusMax) *radius = crRadiusMax;
				}
				if(paint) {
					if(above)
						circle(bmpCanvas, *x0/spatialDivisor, *y0/spatialDivisor, *radius/spatialDivisor, makecol(0,255,0));
					else
						circle(bmpCanvas,*x0/spatialDivisor, *y0/spatialDivisor, *radius/spatialDivisor, makecol(255,0,0));
				}
			}
		} catch (exception& ex) {
 //			printf("caught umaincapture\n");
 //			printf("exception: %s\n", ex.what());

		}


	}

 void TMainCaptureForm::fitEllipse(BITMAP *aBmp, boost::shared_ptr<TOutline> largestOutline, double *x0, double *y0, double *radiusA, double *radiusB, double *angle, double crX0, double crY0, double crRadius, boolean above,
	int tbLeftPosition, int tbRightPosition, int tbLowerPosition, int tbUpperPosition, bool paint) {

		int subsampling = SubsamplingEdit->Text.ToIntDef(10);
		int xOffset = subsampling/2+tbLeftPosition;
		int yOffset =  subsampling/2+tbUpperPosition;

		ublas::matrix<double> x = largestOutline->createMatrixForFit((crX0-xOffset)/subsampling, (crY0-yOffset)/subsampling, crRadius/subsampling);

 //		largestOutline->drawOutline(bmpCanvas->line,x,largestOutline->getNumberOfPoints()); for testing purposes

		*x0 = *y0 = *radiusA = *radiusB = *angle = NAN; //will be returned if fit fails
		try {
			if(largestOutline->nPointsMinusCR > 5) {
				double xc,yc,rA,rB,ang;
				FitEllipse(x, largestOutline->nPointsMinusCR,&xc,&yc,&rA,&rB,&ang);
				xc = xc*subsampling + xOffset;
				yc = yc*subsampling + yOffset;
				rA = rA*subsampling;
				rB = rB*subsampling;
				ang = ang;
				//Begin transform to standard rotation

				int nPoints =4;
				double distanceSq = 0.0;
				double maxDistanceSq = 0.0;
				int maxI = -1;

				for(int i=0; i<nPoints; i++) {
					double tempX = rA*cos(2*PI*i/nPoints);
					double tempY = rB*sin(2*PI*i/nPoints);
					double x =  tempX*cos(ang)-tempY*sin(ang)+xc;
					double y =  tempX*sin(ang)+tempY*cos(ang)+yc;
				//	distanceSq = (x-380)*(x-380) + (y-350)*(y-350);
				   	distanceSq = (x-crX0)*(x-crX0) + (y-crY0)*(y-crY0);
					if(distanceSq > maxDistanceSq) {
						maxI = i;
						maxDistanceSq = distanceSq;
					}
				}
				//if maxI == 0 do nothing
				if(maxI == 1) {
					ang += PI/2.0;
					double temp = rA;
					rA = rB;
					rB = temp;
				}
				if(maxI == 2) {
					ang += PI;
				}
				if(maxI == 3) {
					ang += 3.0*PI/2.0;
					double temp = rA;
					rA = rB;
					rB = temp;
				}
				while(ang < 0.0) ang += 2*PI;
				while(ang >= 2*PI) ang -= 2*PI;
				//end transform to standard rotation


				*x0 = xc;
				*y0 = yc;
				*radiusA = rA;
				*radiusB = rB;
				*angle = ang;
				/*
				*x0 = xc*subsampling + xOffset;
				*y0 = yc*subsampling + yOffset;
				*radiusA = rA*subsampling;
				*radiusB = rB*subsampling;
				*angle = ang;
				*/


				if(above) {
					*radiusA *= crRadiusMultiplier;
					*radiusB *= crRadiusMultiplier;
					if(*radiusA > crRadiusMax) *radiusA = crRadiusMax;
					if(*radiusB > crRadiusMax) *radiusB = crRadiusMax;
				}
				if(paint) {
					if(above)
						drawEllipse(bmpCanvas, *x0/spatialDivisor, *y0/spatialDivisor, *radiusA/spatialDivisor, *radiusB/spatialDivisor, *angle, makecol(0,255,0));
					else
						drawEllipse(bmpCanvas, *x0/spatialDivisor, *y0/spatialDivisor, *radiusA/spatialDivisor, *radiusB/spatialDivisor, *angle, makecol(255,0,0));

				}
			}
		} catch (exception& ex) {
 //			printf("caught umaincapture\n");
 //			printf("exception: %s\n", ex.what());

		}
 }

double xScene = 0.0;
double yScene = 0.0;
int goodFrames = 0;
double goodTime = 0.0;


double firstTimestamp, oldTimestamp, currentTimestamp;

double x0scene = 0.0;
double y0scene = 0.0;
double radiusAscene = 0.0;
double radiusBscene = 0.0;
double angleScene = 0.0;

void __fastcall TMainCaptureForm::DoFrame(BITMAP *aBmp)
{


	bool paint = false;
	if(nFrames % frameDivisor == 0)	paint = true;

	if(nFrames == 0) {
		firstTimestamp = lsl_local_clock();//ds_TimestampMs();
		oldTimestamp = firstTimestamp;
	 } else {
		currentTimestamp = lsl_local_clock();//ds_TimestampMs();
		double interval = (currentTimestamp - oldTimestamp);
		double localFrameRate = frameRate;
		//if frame interval is reasonable, use it to estimate frame rate.
		if(RoundTo(interval*frameRate,0) == 1) {
       // if(frameRate*interval < 1.4 && frameRate*interval > .6) {
			goodFrames++;
			goodTime += interval;
		}

		if(goodFrames > 0 && goodTime > 0) localFrameRate = goodFrames/goodTime;
		oldTimestamp = currentTimestamp;
		double dropped = (currentTimestamp - firstTimestamp)*localFrameRate  - nFrames;
		droppedFramesEdit->Text =  FormatFloat ("0", dropped);
		measuredFrameRateEdit->Text = FormatFloat("0.00", localFrameRate);

   //		printf("frame rate:%g   interval:%g   goodFrames:%d  goodTimes:%g\n", localFrameRate, interval, goodFrames, goodTime);
	}

	char *sbuf;
	int errcode;

   if(videoFrameOutlet && (sendFrameCheckbox->Checked == true || ((bool) frameInlet && (bool) lsl_pull_sample_str(frameInlet, &sbuf, 1, 0, &errcode)))) {
		sendFrameCheckbox->Checked = false;
		int height = aBmp->h;
		int width = aBmp->w;
		char *videoFrameBuffer= new1D<char>(height*width*3, (char) 0);
		for(int y=0; y<height; y++) {
			for(int x=0; x<width; x++) {
				for(int c=0; c<3; c++) {
					videoFrameBuffer[(y*width+x)*3+c]= aBmp->line[height-1-y][x*3+c];
				}
			}
		}
		lsl_push_sample_ctp(videoFrameOutlet, videoFrameBuffer, currentTimestamp, 1);
		delete1D(videoFrameBuffer);
	}

	if(paint) {
		stretch_blit(aBmp, bmpCanvas,
			0,0, aBmp->w, aBmp->h,
			0,0, bmpCanvas->w,bmpCanvas->h);
	}

	//to prevent the definition from begin changed during the calculation.
	int tbUpperPosition = tbUpper->Position;
	int tbLowerPosition = tbLower->Position;
	int tbLeftPosition = tbLeft->Position;
	int tbRightPosition = tbRight->Position;

	int crRoiLeftPosition = crRoiLeft->Position;
	int crRoiRightPosition = crRoiRight->Position;
	int crRoiTopPosition = crRoiTop->Position;
	int crRoiBottomPosition = crRoiBottom->Position;


   if (isKind == isSceneMultiCalib)
	{

		float sample [100];
		int nOutlinesToFind = numberOfMarkers-1;
		int sampleIndex = 0;
		sample[sampleIndex++] = nFrames;
		sample[sampleIndex++] = numberOfMarkers;
		double x0, y0, radius;
		boost::shared_ptr<TOutline> largestOutline =  findLargestOutline(aBmp, true,
			crRoiLeftPosition, crRoiRightPosition, crRoiBottomPosition, crRoiTopPosition, paint);

			fitCircle(aBmp, largestOutline, &x0, &y0, &radius, 0.0, 0.0, 0.0, true,
				crRoiLeftPosition, crRoiRightPosition, crRoiBottomPosition, crRoiTopPosition, paint);

		sample[sampleIndex++] = x0;
		sample[sampleIndex++] = y0;


	   	list<boost::shared_ptr<TOutline> > outlines =  findLargestOutlines(aBmp, true,
			crRoiLeftPosition, crRoiRightPosition, crRoiBottomPosition, crRoiTopPosition, paint, nOutlinesToFind);

		for(std::list<boost::shared_ptr<TOutline> >::iterator outlineIterator = outlines.begin(); outlineIterator != outlines.end(); ++outlineIterator) {

			boost::shared_ptr<TOutline> pOutline(*outlineIterator);
			fitCircle(aBmp, pOutline, &x0, &y0, &radius, 0.0, 0.0, 0.0, true,
		  		crRoiLeftPosition, crRoiRightPosition, crRoiBottomPosition, crRoiTopPosition, paint);
			sample[sampleIndex++] = x0;
			sample[sampleIndex++] = y0;

		}



		float timestamp = lsl_local_clock();
		lsl_push_sample_ftp(outlet, sample, timestamp, 1);


		nFrames++;

		if(paint) {
			if(false) {
				hline(bmpCanvas, 0,240,640,makecol(255,0,0));
				vline(bmpCanvas,320,0,480, makecol(255,0,0));
			}
			hline(bmpCanvas, crRoiLeftPosition/spatialDivisor, crRoiTopPosition/spatialDivisor, crRoiRightPosition/spatialDivisor, makecol(255,0,0));
			hline(bmpCanvas, crRoiLeftPosition/spatialDivisor, crRoiBottomPosition/spatialDivisor, crRoiRightPosition/spatialDivisor, makecol(255,0,0));
			vline(bmpCanvas, crRoiLeftPosition/spatialDivisor,crRoiTopPosition/spatialDivisor, crRoiBottomPosition/spatialDivisor, makecol(255,0,0));
			vline(bmpCanvas, crRoiRightPosition/spatialDivisor,crRoiTopPosition/spatialDivisor, crRoiBottomPosition/spatialDivisor, makecol(255,0,0));

	   //		hline(bmpCanvas, 0, y0/spatialDivisor, bmpCanvas->w, makecol(0,255,255));
	   //		vline(bmpCanvas, x0/spatialDivisor, 0, bmpCanvas->h, makecol(0,255,255));

			HWND hWnd = tPanel->Handle;
			HDC hDC = GetDC(hWnd);
			draw_to_hdc (hDC,bmpCanvas,0,0);
			ReleaseDC(hWnd,hDC);
		}
		return;

	} else if (isKind == isSceneCalib)  // Scene, Calibration mode
	{
		double x0, y0, radius;
//		findAndFitCircle(aBmp, &x0, &y0, &radius, 0.0,0.0,0.0, true,crRoiLeftPosition, crRoiRightPosition, crRoiBottomPosition, crRoiTopPosition, paint);

		boost::shared_ptr<TOutline> largestOutline =  findLargestOutline(aBmp, true,
			crRoiLeftPosition, crRoiRightPosition, crRoiBottomPosition, crRoiTopPosition, paint);

		fitCircle(aBmp, largestOutline, &x0, &y0, &radius, 0.0, 0.0, 0.0, true,
			crRoiLeftPosition, crRoiRightPosition, crRoiBottomPosition, crRoiTopPosition, paint);

		float sample [4];

		sample[0] = nFrames;
		sample[1] = x0;
		sample[2] = y0;
		sample[3] = radius;
		float timestamp = lsl_local_clock();
		lsl_push_sample_ftp(outlet, sample, timestamp, 1);


		nFrames++;

		if(paint) {
			hline(bmpCanvas, crRoiLeftPosition/spatialDivisor, crRoiTopPosition/spatialDivisor, crRoiRightPosition/spatialDivisor, makecol(255,0,0));
			hline(bmpCanvas, crRoiLeftPosition/spatialDivisor, crRoiBottomPosition/spatialDivisor, crRoiRightPosition/spatialDivisor, makecol(255,0,0));
			vline(bmpCanvas, crRoiLeftPosition/spatialDivisor,crRoiTopPosition/spatialDivisor, crRoiBottomPosition/spatialDivisor, makecol(255,0,0));
			vline(bmpCanvas, crRoiRightPosition/spatialDivisor,crRoiTopPosition/spatialDivisor, crRoiBottomPosition/spatialDivisor, makecol(255,0,0));

   //			hline(bmpCanvas, 0, y0/spatialDivisor, bmpCanvas->w, makecol(0,255,255));
   //			vline(bmpCanvas, x0/spatialDivisor, 0, bmpCanvas->h, makecol(0,255,255));

			HWND hWnd = tPanel->Handle;
			HDC hDC = GetDC(hWnd);
			draw_to_hdc (hDC,bmpCanvas,0,0);
			ReleaseDC(hWnd,hDC);
		}
		return;
	} else if (isKind == isScene) {
		float data[6];
		int errcode;
		while(lsl_pull_sample_f(gazestreamThread->inlet,data, 6, 0.0, &errcode));
		x0scene = data[1];
		y0scene = data[2];
		radiusAscene = data[3];
		radiusBscene = data[4];
		angleScene = data[5];


		double xMonitor = x0scene;
		double yMonitor = y0scene;
		double xScene = NAN;
		double yScene = NAN;

		if(gu->rEye != 0.0 && !_isnan(radiusAscene) && !_isnan(radiusBscene) ) {
				gu->inverseEyeMap(&xMonitor,&yMonitor);



				xScene = xMonitor;
				yScene = yMonitor;

				gu->sceneMap(&xScene, &yScene);
			  //	static v=0;

			   //	if(v++ % frameRate == 0) printf("x0: %g y0: %g xMonitor: %g yMonitor: %g xScene: %g yScene: %g\n", x0scene, y0scene, xMonitor, yMonitor, xScene, yScene);

		}

		xScene += xParallaxCorrection;
		yScene += yParallaxCorrection;

		CaptureWorkerForm->xScene = xScene;
		CaptureWorkerForm->yScene = yScene;

		float sample [13];


		sample[0] = nFrames;
		sample[1] = x0scene; //pixels
		sample[2] = y0scene; //pixels
		sample[3] = xMonitor; //mm x distance to target, from scene camera
		sample[4] = yMonitor;//mm  y distance to target, from scene camera
		sample[5] = radiusAscene; //pixels
		sample[6] = radiusBscene; //pixels
		sample[7] = angleScene; //radians

		sample[8] = xScene; //pixels
		sample[9] = yScene; //pixels

		sample[10] = xParallaxCorrection; //pixels
		sample[11] = yParallaxCorrection; //pixels
		sample[12] = gu->distToTarget; //mm, assumed distance to target, from scene camera

		float timestamp = lsl_local_clock();
		lsl_push_sample_ftp(outlet, sample, timestamp, 1);


		nFrames++;

		if(paint) {
			if(false){
				hline(bmpCanvas, 0,240,640,makecol(255,0,0));
				vline(bmpCanvas,320,0,480, makecol(255,0,0));
			}
			circlefill(bmpCanvas, xScene/spatialDivisor, yScene/spatialDivisor, 5, makecol(0,0,0));
			circlefill(bmpCanvas, xScene/spatialDivisor, yScene/spatialDivisor, 3, makecol(255,255,0));
			HWND hWnd = tPanel->Handle;
			HDC hDC = GetDC(hWnd);
			draw_to_hdc (hDC,bmpCanvas,0,0);
			ReleaseDC(hWnd,hDC);
		}

		return;
	} else if (isKind == isVideo)  // Video
	{



		int sample [1];
		sample[0] = nFrames;
		double timestamp = lsl_local_clock();
		char *txt = "f";


		//lsl_push_sample_strtp(outlet, &txt, timestamp, 1);
		lsl_push_sample_itp(outlet, sample, timestamp,1);
		nFrames++;


		if(paint) {

			HWND hWnd = tPanel->Handle;
			HDC  hDC  = GetDC(hWnd);
			draw_to_hdc (hDC,bmpCanvas,0,0);
			ReleaseDC(hWnd,hDC);
		}
		return;
	} else if (isKind == isEye) {

	   //	TMaxArray fr;

		bool pr = (nFrames%int(frameRate) == 0);
		double x0 = 0.0, y0 = 0.0, radius = 0.0;
		double radiusA = 0.0, radiusB = 0.0, angle = 0.0;
		double crX0 = 0.0, crY0 = 0.0, crRadius = 0.0;
		//find cornea reflection
		boost::shared_ptr<TOutline> largestOutline =  findLargestOutline(aBmp, true,
			crRoiLeftPosition, crRoiRightPosition, crRoiBottomPosition, crRoiTopPosition, paint);

		fitCircle(aBmp, largestOutline, &crX0, &crY0, &crRadius, NAN, NAN, NAN, true,
			crRoiLeftPosition, crRoiRightPosition, crRoiBottomPosition, crRoiTopPosition, paint);

		//find pupil, don't fit near cornea reflection

		largestOutline =  findLargestOutline(aBmp, false,
			tbLeftPosition, tbRightPosition, tbLowerPosition,tbUpperPosition, paint);

		fitEllipse(aBmp, largestOutline, &x0, &y0, &radiusA, &radiusB, &angle, crX0, crY0, crRadius, false,
			tbLeftPosition, tbRightPosition, tbLowerPosition, tbUpperPosition, paint);

					//If autothreshold is checked, try to keep the number of spots constant.
		//This is fairly robust to contrast and gain changes.
		if(autoThresholdBox->Checked == true && !_isnan(radiusA) && sqrt(radiusA*radiusB) > eyeRadiusMax) {
			tbThreshold->Position = tbThreshold->Position - 2;
			eyeRadiusMaxEdit->Color = clRed;
		} else {
			eyeRadiusMaxEdit->Color = clWhite;
		}

		if(storeFit) {
			ds_Ellipse ellipse;
			ellipse.x0 = x0;
			ellipse.y0 = y0;
			ellipse.a = radiusA;
			ellipse.b = radiusB;
			ellipse.angle = angle;
			storedPupils.push_back(ellipse);
			storeFit = false;
		}


		if(pr && CONSOLE) printf("x0: %g\n", x0);
		if(pr && CONSOLE) printf("y0: %g\n", y0);

		float sample [6];

		sample[0] = nFrames;
		sample[1] = x0; //pixels
		sample[2] = y0; //pixels
		sample[3] = radiusA;//pixels
		sample[4] = radiusB;//pixels
		sample[5] = angle;//radians


		double timestamp = lsl_local_clock();
		lsl_push_sample_ftp(outlet, sample, timestamp, 1);


		nFrames++;

		if(paint) {

			/////drawing ellipse lines
		  for(std::list<ds_Ellipse>::iterator ellp = storedPupils.begin(); ellp != storedPupils.end(); ++ellp){
				ds_Ellipse ell = *ellp;
				line(bmpCanvas, ell.x0-20*ell.a*cos(ell.angle), ell.y0-20*ell.a*sin(ell.angle), ell.a*cos(ell.angle)+ell.x0, ell.a*sin(ell.angle)+ell.y0, makecol(0,0,0));
			}
			/////end drawing ellipse lines



			hline(bmpCanvas, tbLeft->Position/spatialDivisor, tbUpper->Position/spatialDivisor, tbRight->Position/spatialDivisor, makecol(0,255,0));
			hline(bmpCanvas, tbLeft->Position/spatialDivisor, tbLower->Position/spatialDivisor, tbRight->Position/spatialDivisor, makecol(0,255,0));
			vline(bmpCanvas, tbLeft->Position/spatialDivisor,tbUpper->Position/spatialDivisor, tbLower->Position/spatialDivisor, makecol(0,255,0));
			vline(bmpCanvas, tbRight->Position/spatialDivisor,tbUpper->Position/spatialDivisor, tbLower->Position/spatialDivisor, makecol(0,255,0));

			hline(bmpCanvas, crRoiLeft->Position/spatialDivisor, crRoiTop->Position/spatialDivisor, crRoiRight->Position/spatialDivisor, makecol(255,0,0));
			hline(bmpCanvas, crRoiLeft->Position/spatialDivisor, crRoiBottom->Position/spatialDivisor, crRoiRight->Position/spatialDivisor, makecol(255,0,0));
			vline(bmpCanvas, crRoiLeft->Position/spatialDivisor,crRoiTop->Position/spatialDivisor, crRoiBottom->Position/spatialDivisor, makecol(255,0,0));
			vline(bmpCanvas, crRoiRight->Position/spatialDivisor,crRoiTop->Position/spatialDivisor, crRoiBottom->Position/spatialDivisor, makecol(255,0,0));

			hline(bmpCanvas, 0, y0/spatialDivisor, bmpCanvas->w, makecol(0,255,255));
			vline(bmpCanvas, x0/spatialDivisor, 0, bmpCanvas->h, makecol(0,255,255));

			circlefill(bmpCanvas, x0/spatialDivisor, y0/spatialDivisor, 3, makecol(255,255,0));

			HWND hWnd = tPanel->Handle;
			HDC hDC = GetDC(hWnd);
			draw_to_hdc (hDC,bmpCanvas,0,0);
			ReleaseDC(hWnd,hDC);
		}
		return;
	}

}

void __fastcall TMainCaptureForm::cbRecordClick(TObject *Sender)
{
	nFrames=0;
	edFrame->Text = nFrames;
 	BitBtnPlay->Glyph = cbRecord->Checked ? pBmpRec.get():pBmpRecGr.get();
}


void __fastcall TMainCaptureForm::BitBtnPlayClick(TObject *Sender)
{


	cbVideoInputDevice->Enabled = false;
	cbVideoInput->Enabled = false;
	cbVideoInputFormat->Enabled = false;
	SpatialDivisorEdit->Enabled = false;
	cbAudioInputDevice->Enabled = false;

	double requestedFrameRate = edtRequestedFrameRate->Text.ToDouble();
	CaptureWorkerForm->VideoGrabber->FrameRate = requestedFrameRate;

   	if(outlet) {
		lsl_destroy_outlet(outlet);
		outlet = NULL;
	}

 if(isKind == isEye) {
		char * streamName = (AnsiString("GazeStream_") + AnsiString(IdentifierEdit->Text)).c_str();
		lsl_streaminfo info = lsl_create_streaminfo(streamName,streamName,6,requestedFrameRate,cft_float32,generateGUID());

		lsl_xml_ptr desc = lsl_get_desc(info);
		lsl_xml_ptr chnls = lsl_append_child(desc, "channels");

		lsl_xml_ptr chn = lsl_append_child(chnls, "channel");
		lsl_append_child_value(chn, "label","frame");
		lsl_append_child_value(chn,"unit","number");

		chn = lsl_append_child(chnls, "channel");
		lsl_append_child_value(chn, "label","pupil position x");
		lsl_append_child_value(chn,"unit","pixels");

		chn = lsl_append_child(chnls, "channel");
		lsl_append_child_value(chn, "label","pupil position y");
		lsl_append_child_value(chn,"unit","pixels");

		chn = lsl_append_child(chnls, "channel");
		lsl_append_child_value(chn, "label","pupil radius A");
		lsl_append_child_value(chn,"unit","pixels");

		chn = lsl_append_child(chnls, "channel");
		lsl_append_child_value(chn, "label","pupil radius B");
		lsl_append_child_value(chn,"unit","pixels");

		chn = lsl_append_child(chnls, "channel");
		lsl_append_child_value(chn, "label","pupil angle");
		lsl_append_child_value(chn,"unit","radians");

		lsl_xml_ptr sync = lsl_append_child(desc, "synchronization");
		lsl_append_child_value(sync, "can_drop_samples", "true");

		outlet = lsl_create_outlet(info,0,360);
  } else if (isKind == isSceneMultiCalib) {
 	char * streamName = (AnsiString("SceneCalibrateStream_") + AnsiString(IdentifierEdit->Text)).c_str();
		lsl_streaminfo info = lsl_create_streaminfo(streamName,streamName,2 + numberOfMarkers*2,requestedFrameRate,cft_float32,generateGUID());
		lsl_xml_ptr desc = lsl_get_desc(info);
		lsl_xml_ptr chnls = lsl_append_child(desc, "channels");

		lsl_xml_ptr chn = lsl_append_child(chnls, "channel");
		lsl_append_child_value(chn, "label","frame");
		lsl_append_child_value(chn,"unit","number");

		chn = lsl_append_child(chnls, "channel");
		lsl_append_child_value(chn, "label","reference marker count");
		lsl_append_child_value(chn,"unit","count");

		chn = lsl_append_child(chnls, "channel");
		lsl_append_child_value(chn, "label","scene position x");
		lsl_append_child_value(chn,"unit","pixels");

		chn = lsl_append_child(chnls, "channel");
		lsl_append_child_value(chn, "label","scene position y");
		lsl_append_child_value(chn,"unit","pixels");

		for(int i=0; i<numberOfMarkers; i++) {
			chn = lsl_append_child(chnls, "channel");
			lsl_append_child_value(chn, "label","reference position x");
			lsl_append_child_value(chn,"unit","pixels");

			chn = lsl_append_child(chnls, "channel");
			lsl_append_child_value(chn, "label","reference position y");
			lsl_append_child_value(chn,"unit","pixels");
		}

		lsl_xml_ptr sync = lsl_append_child(desc, "synchronization");
		lsl_append_child_value(sync, "can_drop_samples", "true");

		outlet = lsl_create_outlet(info,0,360);


  } else if (isKind == isScene) {
	char * streamName = (AnsiString("SceneStream_") + AnsiString(IdentifierEdit->Text)).c_str();
		lsl_streaminfo info = lsl_create_streaminfo(streamName,streamName,13,requestedFrameRate,cft_float32,generateGUID());
		lsl_xml_ptr desc = lsl_get_desc(info);
		lsl_xml_ptr chnls = lsl_append_child(desc, "channels");

		lsl_xml_ptr chn = lsl_append_child(chnls, "channel");
		lsl_append_child_value(chn, "label","frame");
		lsl_append_child_value(chn,"unit","number");

		chn = lsl_append_child(chnls, "channel");
		lsl_append_child_value(chn, "label","pupil position x");
		lsl_append_child_value(chn,"unit","pixels");

		chn = lsl_append_child(chnls, "channel");
		lsl_append_child_value(chn, "label","pupil position y");
		lsl_append_child_value(chn,"unit","pixels");

		chn = lsl_append_child(chnls, "channel");
		lsl_append_child_value(chn, "label","screen position x");
		lsl_append_child_value(chn,"unit","x/monitorWidth");

		chn = lsl_append_child(chnls, "channel");
		lsl_append_child_value(chn, "label","screen position y");
		lsl_append_child_value(chn,"unit","y/monitorHeight");


		chn = lsl_append_child(chnls, "channel");
		lsl_append_child_value(chn, "label","pupil radius A");
		lsl_append_child_value(chn,"unit","pixels");

		chn = lsl_append_child(chnls, "channel");
		lsl_append_child_value(chn, "label","pupil radius B");
		lsl_append_child_value(chn,"unit","pixels");

		chn = lsl_append_child(chnls, "channel");
		lsl_append_child_value(chn, "label","pupil angle");
		lsl_append_child_value(chn,"unit","radians");

		chn = lsl_append_child(chnls, "channel");
		lsl_append_child_value(chn, "label","scene position x");
		lsl_append_child_value(chn,"unit","pixels");

		chn = lsl_append_child(chnls, "channel");
		lsl_append_child_value(chn, "label","scene position y");
		lsl_append_child_value(chn,"unit","pixels");

		chn = lsl_append_child(chnls, "channel");
		lsl_append_child_value(chn, "label","parallax correction x");
		lsl_append_child_value(chn,"unit","pixels");

		chn = lsl_append_child(chnls, "channel");
		lsl_append_child_value(chn, "label","parallax correction y");
		lsl_append_child_value(chn,"unit","pixels");

		lsl_xml_ptr sync = lsl_append_child(desc, "synchronization");
		lsl_append_child_value(sync, "can_drop_samples", "true");

		outlet = lsl_create_outlet(info,0,360);
		streamName = (AnsiString("type='GazeStream_") + AnsiString(IdentifierEdit->Text) + AnsiString("'")).c_str();

		gazestreamThread = std::unique_ptr<TStreamThread>(new TStreamThread(streamName));

  } else if (isKind == isVideo) {
   		char * streamName = (AnsiString("VideoStream_") + AnsiString(IdentifierEdit->Text)).c_str();
		lsl_streaminfo info = lsl_create_streaminfo(streamName,streamName,1,/*LSL_IRREGULAR_RATE*/requestedFrameRate,/*cft_string*/cft_int32,generateGUID());
		lsl_xml_ptr desc = lsl_get_desc(info);
		lsl_xml_ptr chnls = lsl_append_child(desc, "channels");
		lsl_xml_ptr chn = lsl_append_child(chnls, "channel");
		lsl_append_child_value(chn, "label","frame");
		lsl_append_child_value(chn,"unit","number");

		lsl_xml_ptr sync = lsl_append_child(desc, "synchronization");
		lsl_append_child_value(sync, "can_drop_samples", "true");

		outlet = lsl_create_outlet(info,0,360);

  }

  	goodFrames = 0;
	goodTime = 0;
	nFrames = 0;

	Start();

	frameThread = std::unique_ptr<TFrameThread>(new TFrameThread(this, bmpQueue, hMutex, false));
	BitBtnStop->Enabled=true;
	BitBtnPlay->Enabled=false;

	edtFrameRate->Text = FormatFloat ("0.00", CaptureWorkerForm->VideoGrabber->CurrentFrameRate);

	frameRate = CaptureWorkerForm->VideoGrabber->CurrentFrameRate;

	double ratio = requestedFrameRate/frameRate;
	if(ratio > 1.02 || ratio < .98) {
		edtFrameRate->Text = FormatFloat ("0.00", CaptureWorkerForm->VideoGrabber->CurrentFrameRate);
		Application->MessageBoxA(L"Camera unable to display at requested frame rate.", L"Error", MB_OK);
		btStopClick(this);
		return;
	}
}
//---------------------------------------------------------------------------



double alphaX = 0;
double alphaY = 0;
int posX;
int posY;

void __fastcall TMainCaptureForm::Timer1Timer(TObject *Sender)
{
	if(frameThread) {
		BacklogEdit->Text = bmpQueue.size();
		edFrame->Text = nFrames;
		edTimestamp->Text = FormatFloat("0.00", currentTimestamp - firstTimestamp);
	}
}
//---------------------------------------------------------------------------









void __fastcall TMainCaptureForm::RadioGroup1Click(TObject *Sender)
{
	if(BitBtnStop->Enabled) btStopClick(this);


	if(gazestreamThread) {
		gazestreamThread->Terminate();
		gazestreamThread.reset();
	}
	//eye camera
	if (RadioGroup1->ItemIndex==0)
	{
		isKind = isEye;
		PageControl2->ActivePage = tsEyeTracker;
		Caption = "Video stream: eye camera";


	}

	else
	//scene camera multi-spot calibrate
	if (RadioGroup1->ItemIndex==1)
	{



		crRoiLeft->Position = 40;
		crRoiRight->Position = 600;
		crRoiTop->Position = 40;
		crRoiBottom->Position = 440;
		isKind = isSceneMultiCalib;
		PageControl2->ActivePage = tsEyeTracker;
		Caption = "Video stream: scene camera calibrate";


	}
	else

	//scene display
	if (RadioGroup1->ItemIndex==2)
	{
		isKind = isScene;
		PageControl2->ActivePage = tsScene;
		Caption = "Video stream: scene camera";


	}


}


void TMainCaptureForm::SetToVideoMode() {


		if(gazestreamThread) {
			gazestreamThread->Terminate();
			gazestreamThread.reset();
		}

		isKind = isVideo;
		PageControl2->ActivePage = tsVideo;
		Caption = "Video stream: video camera";
		MainCaptureForm->Caption = UnicodeString("Video Stream, version ") + getVersion();

		RadioGroup1->Visible = false;

		allegro_init();
		set_gdi_color_format();
		set_color_depth(CDEPTH);
		bmpCanvas = create_bitmap_ex(CDEPTH,tPanel->Width,tPanel->Height);
		clear_bitmap(bmpCanvas);




}
//---------------------------------------------------------------------------

void __fastcall TMainCaptureForm::FormClose(TObject *Sender, TCloseAction &Action)
{

	if(BitBtnStop->Enabled) btStopClick(this);
	if(frameThread) {
		frameThread->Terminate();
		frameThread.reset(); //will not delete till terminated, by VCL design.

	}

	if(bmpCanvas) {
		destroy_bitmap(bmpCanvas);
		bmpCanvas = NULL;
	}

	if(outlet) {
		lsl_destroy_outlet(outlet);
		outlet = NULL;
	}
	if(gazestreamThread) {
		gazestreamThread->Terminate();
		gazestreamThread.reset();
	}

	if(videoFrameOutlet) {
		lsl_destroy_outlet(videoFrameOutlet);
		videoFrameOutlet = NULL;
	}



}
//---------------------------------------------------------------------------





void __fastcall TMainCaptureForm::LoadCalibrationClick(TObject *Sender)
{
	if(OpenDialog1->Execute()) {
		gu->LoadGazeCalibration(OpenDialog1->FileName, xdoc_in);
	 /*	gu->eyeMap(-2.94026, -7.25053);
		double x = 72.6125+320;
		double y =   93.5124+240;

		gu->inverseEyeMap(&x, &y);
		printf("sceneX: %g sceneY: %g, distance: %g\n",x,y,gu->distToTarget);

		gu->sceneMap(distance, &x, &y);
		printf("scenecamX: %g scenecamY: %g\n",x,y);  */
	  //	remember to
	  //	double x = 0;
	  //	double y = *yIn;// - (cameraWidth-1)/2; //from raw pixel coordinates to centered
	  //	double z = /*(cameraHeight-1)/2 - */*zIn; //from raw pixel coordinates to centered inverted
	  //in inverse eye map

	}
}
//---------------------------------------------------------------------------




void __fastcall TMainCaptureForm::crRadiusMultiplierEditChange(TObject *Sender)
{
	bool ex = false;
	try {
		crRadiusMultiplierEdit->Text.ToDouble();
	} catch (...) {
		ex = true;
	}

	if(!ex) crRadiusMultiplier = crRadiusMultiplierEdit->Text.ToDouble();
}


//---------------------------------------------------------------------------

void __fastcall TMainCaptureForm::nOutlinesEditChange(TObject *Sender)
{
	bool ex = false;
	try {
		nOutlinesEdit->Text.ToInt();
	} catch (...) {
		ex = true;
	}

	if(!ex) nOutlinesDesired = nOutlinesEdit->Text.ToInt();
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------

void __fastcall TMainCaptureForm::crRadiusMaxEditChange(TObject *Sender)
{
	bool ex = false;
	try {
		crRadiusMaxEdit->Text.ToDouble();
	} catch (...) {
		ex = true;
	}

	if(!ex) crRadiusMax = crRadiusMaxEdit->Text.ToDouble();
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
	if(!ex && temp > 0) frameDivisor = temp;
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
//---------------------------------------------------------------------------

void __fastcall TMainCaptureForm::maxEccentricityEditChange(TObject *Sender)
{
	bool ex = false;
	try {
		maxEccentricityEdit->Text.ToDouble();
	} catch (...) {
		ex = true;
	}

	if(!ex) maxEccentricity = maxEccentricityEdit->Text.ToDouble();
}
//---------------------------------------------------------------------------





void __fastcall TMainCaptureForm::xParallaxCorrectionEditChange(TObject *Sender)
{
	bool ex = false;
	try {
		xParallaxCorrectionEdit->Text.ToDouble();
	} catch (...) {
		ex = true;
	}

	if(!ex) xParallaxCorrection = xParallaxCorrectionEdit->Text.ToDouble();
}
//---------------------------------------------------------------------------

void __fastcall TMainCaptureForm::yParallaxCorrectionEditChange(TObject *Sender)
{
	bool ex = false;
	try {
		yParallaxCorrectionEdit->Text.ToDouble();
	} catch (...) {
		ex = true;
	}

	if(!ex) yParallaxCorrection = -yParallaxCorrectionEdit->Text.ToDouble();
}
//---------------------------------------------------------------------------




void __fastcall TMainCaptureForm::cbSceneCalibColorChange(TObject *Sender)
{
if(cbSceneCalibColor->ItemIndex != -1) {
		sceneCalibColor = cbSceneCalibColor->ItemIndex;
	}
}
//---------------------------------------------------------------------------

 void __fastcall TMainCaptureForm::eyeRadiusMaxEditChange(TObject *Sender)
{
	bool ex = false;
	try {
		eyeRadiusMaxEdit->Text.ToDouble();
	} catch (...) {
		ex = true;
	}

	if(!ex) eyeRadiusMax = eyeRadiusMaxEdit->Text.ToDouble();
}

//---------------------------------------------------------------------------


void __fastcall TMainCaptureForm::cbReferenceCalibColorChange(TObject *Sender)
{
	if(cbReferenceCalibColor->ItemIndex != -1) {
		referenceCalibColor = cbReferenceCalibColor->ItemIndex;
	}
}
//---------------------------------------------------------------------------




void __fastcall TMainCaptureForm::IdentifierEditChange(TObject *Sender)
{
	if(BitBtnStop->Enabled) btStopClick(this);
	enableFrameSendingClick(this);
	if(isKind != isVideo) RadioGroup1Click(this);
	else SetToVideoMode();
}
//---------------------------------------------------------------------------


void __fastcall TMainCaptureForm::numberOfMarkersEditChange(TObject *Sender)
{
	bool ex = false;
	try {
		numberOfMarkersEdit->Text.ToInt();
	} catch (...) {
		ex = true;
	}

	if(!ex) {
		numberOfMarkers = numberOfMarkersEdit->Text.ToInt();
		if(	isKind != isVideo) RadioGroup1Click(this);
		else SetToVideoMode();
	}
}
//---------------------------------------------------------------------------


void __fastcall TMainCaptureForm::FrameComboBoxChange(TObject *Sender)
{
	if(frameInlet) lsl_destroy_inlet(frameInlet);

	lsl_streaminfo info;
	lsl_resolve_byprop(&info,1, "name", ((AnsiString) FrameComboBox->Text).c_str(), 1,1.0);
	frameInlet = lsl_create_inlet(info, 300, LSL_NO_PREFERENCE,1);
}
//---------------------------------------------------------------------------

void __fastcall TMainCaptureForm::RefreshStreamsButtonClick(TObject *Sender)
{
	lsl_streaminfo infos[MAX_STREAMS];
	int streamsFound = lsl_resolve_all(infos, MAX_STREAMS, 0.1);

	FrameComboBox->Items->Clear();
	for (int i = 0; i < streamsFound; i++) {
		FrameComboBox->Items->Append(lsl_get_name(infos[i]));

	}



}
//---------------------------------------------------------------------------

void __fastcall TMainCaptureForm::enableFrameSendingClick(TObject *Sender)
{

	if(videoFrameOutlet) {
		lsl_destroy_outlet(videoFrameOutlet);
		videoFrameOutlet = NULL;
	}
	if(enableFrameSending->Checked == true) {
		char * streamName = (AnsiString("VideoFrames_") + AnsiString(IdentifierEdit->Text)).c_str();
		lsl_streaminfo info = lsl_create_streaminfo(streamName,"VideoRaw",acqWidth*acqHeight*3,0,cft_int8,generateGUID());
		char s[20];
		lsl_xml_ptr desc = lsl_get_desc(info);
		lsl_xml_ptr chn = lsl_append_child(desc, "encoding");
		sprintf(s, "%d", acqWidth);
		lsl_append_child_value(chn,"width",s);
		sprintf(s, "%d", acqHeight);
		lsl_append_child_value(chn,"height",s);
		lsl_append_child_value(chn,"color_channels","3");
		lsl_append_child_value(chn,"color_format","RGB");
		lsl_append_child_value(chn,"color_space","sRGB");
		lsl_append_child_value(chn,"codec","RAW");

		videoFrameOutlet = lsl_create_outlet(info,0,360);
	}

}
//---------------------------------------------------------------------------


