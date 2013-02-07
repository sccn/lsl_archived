

// ---------------------------------------------------------------------------
#include "LinearAlgebra.h"
#include "useallegro4.h"
#include "MonitorDrawer.h"

#include <vcl.h>

#include "stdio.h"
#include "math_util.h"
#include "Math.hpp"
#include "EyeCalibratorForm.h"
#include "list.h"
#include "vector.h"
#include "Minimizer.h"
#include <iostream>
#include <fstream>
#include <float.h>
#include "map.h"
#include "HotspotTypes.h"


// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
#define CDEPTH 24
#define MAX_STREAMS 16

TForm4 *Form4;



//void *hGazeStream = 0, *hSceneStream = 0 , *hPhasespace = 0;

double monitorWidth=0.0 ,monitorHeight=0.0, cameraWidth=0.0,cameraHeight=0.0;
double sceneCameraWidth=0.0, sceneCameraHeight=0.0;

double r=0.0, x0=0.0, y0=0.0, z0=0.0;
double a=0.0, b=0.0, g=0.0, bx=0.0, by=0.0, bz=0.0, fc=0.0;

double aScene=0.0, bScene=0.0, gScene=0.0, bxScene=0.0, byScene=0.0, bzScene=0.0, fcScene=0.0;
double kScene=0.0, ycScene=0.0, zcScene=0.0;

int channelsPerMarker = 4;

BITMAP * bmpCanvas;
BITMAP * sceneCanvas;

std::vector<int> monitors;
std::vector<double> markerXs, markerYs; //true marker position
std::vector<double> meanEyeXs, meanEyeYs, stddevEyeXs, stddevEyeYs;//pupil position in camera
std::vector<double> meanSceneXs, meanSceneYs, stddevSceneXs,stddevSceneYs;//marker position in scene camera

std::vector<double> scaledMarkerXs, scaledMarkerYs;
std::vector<double> scaledMeanEyeXs, scaledMeanEyeYs, scaledStddevEyeXs, scaledStddevEyeYs;

std::vector<double> modeledXs, modeledYs, modeledSceneXs, modeledSceneYs;

int nHeadPoints = 7; //four for the head, three for the calibration wand
vector< list<double> > headXsList(nHeadPoints, list<double>());
vector< list<double> > headYsList(nHeadPoints, list<double>());
vector< list<double> > headZsList(nHeadPoints, list<double>());

ublas::matrix<double> headRef(3,4);
ublas::matrix<double> wand0Ref(3,3);
ublas::matrix<double> wand1Ref(3,3);
ublas::matrix<double> displayRef(3,4);


enum TimerMode {isStandard,isHead, isWand0, isWand1, isEyeTest} ;
TimerMode timerMode = isStandard;

int **tot = NULL;






int readCount = 0;
int progress = 0;

double pupilX = 0.0, pupilY = 0.0, sceneX = 0.0, sceneY = 0.0;
std::vector<double> headXs(nHeadPoints,0.0), headYs(nHeadPoints,0.0), headZs(nHeadPoints,0.0);
double markerX = -1, markerY = -1;
int monitor = -1;
double oldMarkerX = -1;
double oldMarkerY = -1;
std::list<double>pupilXs;
std::list<double>pupilYs;

std::list<double> sceneXs;
std::list<double> sceneYs;

int headCount = 0;
bool wand0RefAvailable = false;
bool wand1RefAvailable = false;
ublas::vector<double> eyePosition(3,0.0);
ublas::vector<double> displayMarkerPosition(3,0.0);

lsl_inlet gazeInlet = NULL;
int gazeChannels = 0;

lsl_inlet sceneInlet = NULL;
int sceneChannels = 0;

lsl_inlet phasespaceInlet = NULL;
int phasespaceChannels = 0;


__fastcall TForm4::TForm4(TComponent* Owner) : TForm(Owner) {

}



void __fastcall TForm4::Timer1Timer(TObject *Sender) {
 //	TMaxArray fr;

	bool gazeChanged = false;
	int errcode;

	if(gazeInlet) {
		float *buf = new1D<float>(gazeChannels, 0);
		while(lsl_pull_sample_f(gazeInlet,buf, gazeChannels, 0.0, &errcode)) {
			static int gazeSamples = 0;
			gazeChanged = true;
			pupilX = buf[1];
			pupilY = buf[2];
		  //	progress =  ds_TailPos(hGazeStream)/40.96;
			Form4->GazeProgressBar->Position = gazeSamples % 120;

			gazeSamples++;
		}
		delete1D(buf);
	}
	if(sceneInlet) {
		float *buf = new1D<float>(sceneChannels,0);
		while(lsl_pull_sample_f(sceneInlet,buf,sceneChannels,0.0,&errcode)) {
			static int sceneSamples = 0;
			sceneX = buf[1];
			sceneY = buf[2];
			Form4->SceneProgressBar->Position = sceneSamples % 120;

			sceneSamples++;
		}
		delete1D(buf);
	}



	//if phasespace data is available, load the head, wand, and display marker positions.
	if(phasespaceInlet) {
		float *buf = new1D<float>(phasespaceChannels,0);
		while(lsl_pull_sample_f(phasespaceInlet,buf,phasespaceChannels,0.0,&errcode)) {
			Form4->PhasespaceProgressBar->Position = (headCount/16) % 120;
			headCount++;
			bool headIsNan = false;
			int head0 =  HeadMarker0Edit->Text.ToInt();
			headXs[0] = buf[head0*channelsPerMarker] == 0 ? NAN : 1000.0*buf[head0*channelsPerMarker] ;
			headYs[0] = buf[head0*channelsPerMarker+1]== 0 ? NAN : 1000.0*buf[head0*channelsPerMarker+1];
			headZs[0] = buf[head0*channelsPerMarker+2]== 0 ? NAN : 1000.0*buf[head0*channelsPerMarker+2];

			int head1 = HeadMarker1Edit->Text.ToInt();
			headXs[1] = buf[head1*channelsPerMarker]== 0 ? NAN : 1000.0*buf[head1*channelsPerMarker];
			headYs[1] = buf[head1*channelsPerMarker+1]== 0 ? NAN : 1000.0*buf[head1*channelsPerMarker+1];
			headZs[1] = buf[head1*channelsPerMarker+2]== 0 ? NAN : 1000.0*buf[head1*channelsPerMarker+2];

			int head2 = HeadMarker2Edit->Text.ToInt();
			headXs[2] = buf[head2*channelsPerMarker]== 0 ? NAN : 1000.0*buf[head2*channelsPerMarker];
			headYs[2] = buf[head2*channelsPerMarker+1]== 0 ? NAN : 1000.0*buf[head2*channelsPerMarker+1];
			headZs[2] = buf[head2*channelsPerMarker+2]== 0 ? NAN : 1000.0*buf[head2*channelsPerMarker+2];

			int head3 = HeadMarker3Edit->Text.ToInt();
			headXs[3] = buf[head3*channelsPerMarker]== 0 ? NAN : 1000.0*buf[head3*channelsPerMarker];
			headYs[3] = buf[head3*channelsPerMarker+1]== 0 ? NAN : 1000.0*buf[head3*channelsPerMarker+1];
			headZs[3] = buf[head3*channelsPerMarker+2]== 0 ? NAN : 1000.0*buf[head3*channelsPerMarker+2];

			for(int i=0; i<4; i++) {
				if(_isnan(headXs[i]) || _isnan(headYs[i]) || _isnan(headZs[i])) {
					headIsNan = true;
				}
			}

			int head4 = WandNear0Edit->Text.ToInt();
			headXs[4] = buf[head4*channelsPerMarker]== 0 ? NAN : 1000.0*buf[head4*channelsPerMarker];
			headYs[4] = buf[head4*channelsPerMarker+1]== 0 ? NAN : 1000.0*buf[head4*channelsPerMarker+1];
			headZs[4] = buf[head4*channelsPerMarker+2]== 0 ? NAN : 1000.0*buf[head4*channelsPerMarker+2];

			int head5 = WandNear1Edit->Text.ToInt();
			headXs[5] = buf[head5*channelsPerMarker]== 0 ? NAN : 1000.0*buf[head5*channelsPerMarker];
			headYs[5] = buf[head5*channelsPerMarker+1]== 0 ? NAN : 1000.0*buf[head5*channelsPerMarker+1];
			headZs[5] = buf[head5*channelsPerMarker+2]== 0 ? NAN : 1000.0*buf[head5*channelsPerMarker+2];

			int head6 = WandFarEdit->Text.ToInt();
			headXs[6] = buf[head6*channelsPerMarker]== 0 ? NAN : 1000.0*buf[head6*channelsPerMarker];
			headYs[6] = buf[head6*channelsPerMarker+1]== 0 ? NAN : 1000.0*buf[head6*channelsPerMarker+1];
			headZs[6] = buf[head6*channelsPerMarker+2]== 0 ? NAN : 1000.0*buf[head6*channelsPerMarker+2];

			int displayMarker = DisplayMarkerEdit->Text.ToInt();
			displayMarkerPosition(0) = buf[displayMarker*channelsPerMarker]== 0 ? NAN :1000.0*buf[displayMarker*channelsPerMarker];
			displayMarkerPosition(1) = buf[displayMarker*channelsPerMarker+1]== 0 ? NAN :1000.0*buf[displayMarker*channelsPerMarker+1];
			displayMarkerPosition(2) = buf[displayMarker*channelsPerMarker+2]== 0 ? NAN :1000.0*buf[displayMarker*channelsPerMarker+2];


			if(timerMode == isEyeTest) {

				ublas::matrix<double> headCur(3,4);

				for(int i=0; i<4; i++) {
					headCur(0, i) = headXs[i];
					headCur(1, i) = headYs[i];
					headCur(2, i) = headZs[i];
				}

				ublas::vector<double> priorTranslation(3);
				ublas::vector<double> postTranslation(3);
				ublas::matrix<double> rotation(3,3);


				if(!headIsNan) {
					findRigidBody(headRef, headCur, priorTranslation, rotation, postTranslation);
					ublas::vector<double> rotatedEye = moveRigidVector(eyePosition, priorTranslation, rotation, postTranslation);
					if(headCount % 500 == 0) {
					//	print("priorTranslation", priorTranslation);
					//	print("postTranslation", postTranslation);
						print("marker", displayMarkerPosition);
						print("rotatedEye", rotatedEye);
					}
				}


				else {
					if(headCount % 500 == 0 && CONSOLE) printf("findRigidBody inputs have NANs.\n");
				}
			}

			if(timerMode == isHead || timerMode == isWand0 || timerMode == isWand1) {
				//if tracking head or wand, store one second (480 samples) of data
				if(headCount < 480) {
					for(int i=0; i<nHeadPoints; i++) {
						//don't add if phasespace could not see the point
						if(headXs[i] != 0 && headYs[i] != 0 && headZs[i] != 0) {
							headXsList[i].push_back(headXs[i]);
							headYsList[i].push_back(headYs[i]);
							headZsList[i].push_back(headZs[i]);
						}
					}
				//done getting head and wand data.  average and store.
				} else {
					ublas::matrix<double> wandCur(3,3);
					ublas::matrix<double> headCur(3,4);
					for(int i=0; i<nHeadPoints; i++) {
						boolean outliersRemain = true;
						double meanX, stddevX, meanY, stddevY, meanZ, stddevZ;
						while (outliersRemain) {

							outliersRemain = false;
							calculateStats(headXsList[i], &meanX, &stddevX);
							calculateStats(headYsList[i], &meanY, &stddevY);
							calculateStats(headZsList[i], &meanZ, &stddevZ);

							std::list<double>::iterator ix = headXsList[i].begin();
							std::list<double>::iterator iy = headYsList[i].begin();
							std::list<double>::iterator iz = headZsList[i].begin();
							while( ix != headXsList[i].end() ) {
								if(_isnan(meanX) || _isnan(meanY) || _isnan(meanZ) ||
								   _isnan(stddevX) || _isnan(stddevY) || _isnan(stddevZ) ||
								   fabs(*ix - meanX) > stddevX * 5.0 ||
								   fabs(*iy - meanY) > stddevY * 5.0 ||
								   fabs(*iz - meanZ) > stddevZ * 5.0 ) {

									ix = headXsList[i].erase(ix);
									iy = headYsList[i].erase(iy);
									iz = headZsList[i].erase(iz);
									outliersRemain = true;
								} else {
									++ix;
									++iy;
									++iz;
								}
							}

		//					printf("Head Point: %d :: X: %g +/- %g, Y: %g +/- %g, Z: %g +/- %g\n", i, meanX, stddevX, meanY, stddevY, meanZ, stddevZ);
							if(timerMode == isHead) {
								ReferenceHeadButton->Caption = "Reference Head Position";

								if(i<4) {
									headRef(0,i) = meanX;
									headRef(1,i) = meanY;
									headRef(2,i) = meanZ;
								}

							}

							if(timerMode == isWand0 || timerMode == isWand1) {
								WandPosition0Button->Caption = "Wand Position 0";
								WandPosition1Button->Caption = "Wand Position 1";

								if(i<4) {
									headCur(0,i) = meanX;
									headCur(1,i) = meanY;
									headCur(2,i) = meanZ;

								} else {
									wandCur(0, i-4) = meanX;
									wandCur(1, i-4) = meanY;
									wandCur(2, i-4) = meanZ;

								}

							}

						}
					}

					if(timerMode == isHead) {
						if(hasNan(headRef)) {
							StatusMemo->Lines->Add("Head position measurement failed. At least one phasespace marker is not visible.");
							StatusMemo->Color = clRed;
						}
						else {
							StatusMemo->Lines->Add("Head position measurement succeeded.");
							StatusMemo->Color = clWindow;
						}
						print("headRef", headRef);
					}


					if(timerMode == isWand0 || timerMode == isWand1) {
						if(hasNan(headCur)) {
							StatusMemo->Lines->Add("Head position measurement failed. At least one phasespace marker is not visible.");
							StatusMemo->Color = clRed;
							timerMode = isStandard;
							return;
						}
						else {
							StatusMemo->Lines->Add("Head position measurement succeeded.");
							StatusMemo->Color = clWindow;
						}

						if(hasNan(wandCur)) {
							StatusMemo->Lines->Add("Wand position measurement failed. At least one phasespace marker is not visible.");
							StatusMemo->Color = clRed;
							timerMode = isStandard;
							return;
						}
						else {
							StatusMemo->Lines->Add("Wand position measurement succeeded.");
							StatusMemo->Color = clWindow;
						}

					  //	print("headCur", headCur);
					  //	print("wandCur", wandCur);
						ublas::vector<double> priorTranslation(3);
						ublas::vector<double> postTranslation(3);
						ublas::matrix<double> rotation(3,3);

						try {
							findRigidBody(headRef, headCur, priorTranslation, rotation, postTranslation);
						} catch (IllegalArgumentException &ex) {
							//shouldn't get here. Nans are caught earlier.
							StatusMemo->Lines->Add("Find Rigid Body failed.");
							StatusMemo->Color = clRed;
							timerMode = isStandard;
							return;
						}
					//	print("rotation", rotation);
					//	print("priorTrans", priorTranslation);
					//	print("postTrans", postTranslation);

						//move the wand to the head origin
						if(timerMode == isWand0) {
							//wand0Ref = moveRigidBody(wandCur, priorTranslation, rotation, postTranslation);
							wand0Ref = moveRigidBody(wandCur, postTranslation, trans(rotation), priorTranslation);
							print("wand0Cur", wandCur);
							print("wand0Ref", wand0Ref);
							wand0RefAvailable = true;
						} else {

							//wand1Ref = moveRigidBody(wandCur, priorTranslation, rotation, postTranslation);
							wand1Ref = moveRigidBody(wandCur, postTranslation, trans(rotation), priorTranslation);
							print("wand1Cur", wandCur);
							print("wand1Ref", wand1Ref);
							wand1RefAvailable = true;
						}
						//find the centerpoint between the two wand positions
						if(wand0RefAvailable && wand1RefAvailable) {
							ublas::vector<double> wand0Near(3);
							wand0Near(0) = (wand0Ref(0,0) + wand0Ref(0,1))/2.0;
							wand0Near(1) = (wand0Ref(1,0) + wand0Ref(1,1))/2.0;
							wand0Near(2) = (wand0Ref(2,0) + wand0Ref(2,1))/2.0;

							ublas::vector<double> wand0Far(3);
							wand0Far(0) = wand0Ref(0,2);
							wand0Far(1) = wand0Ref(1,2);
							wand0Far(2) = wand0Ref(2,2);

							ublas::vector<double> wand1Near(3);
							wand1Near(0) = (wand1Ref(0,0) + wand1Ref(0,1))/2.0;
							wand1Near(1) = (wand1Ref(1,0) + wand1Ref(1,1))/2.0;
							wand1Near(2) = (wand1Ref(2,0) + wand1Ref(2,1))/2.0;

							ublas::vector<double> wand1Far(3);
							wand1Far(0) = wand1Ref(0,2);
							wand1Far(1) = wand1Ref(1,2);
							wand1Far(2) = wand1Ref(2,2);

							eyePosition = findMidpoint(wand0Near,wand0Far,wand1Near, wand1Far);
							print("eyePosition", eyePosition);
							StatusMemo->Lines->Add("Eye position calculated.");

						}

					}

					timerMode = isStandard;
					break;
				}
			}
		}
		delete1D(buf);
	}

	//if gaze changed
	if(gazeChanged && timerMode == isStandard) {
		if(readCount%10==0 && CONSOLE) printf("%d %g %g %g %g %g %g\n", readCount, markerX, markerY, sceneX, sceneY, pupilX, pupilY);
		readCount++;
		//and marker moved, average data for this location, and update plots
		if (markerX != oldMarkerX || markerY != oldMarkerY) {


			if(oldMarkerX >= 0.0) {
				double meanX = 0.0, stddevX = 0.0;
				double meanY = 0.0, stddevY = 0.0;
				double meanSceneX = 0.0, stddevSceneX = 0.0;
				double meanSceneY = 0.0, stddevSceneY = 0.0;

				int count=0;
				std::list<double>::iterator ix = pupilXs.begin();
				std::list<double>::iterator iy = pupilYs.begin();
				//remove points from before settling time.
			  //	while(pupilXs.size() > 30) {
				while (count++ < 15) {
					ix = pupilXs.erase(ix);
					iy = pupilYs.erase(iy);
				}

				boolean outliersRemain = true;
				while (outliersRemain) {

					outliersRemain = false;
					calculateStats(pupilXs, &meanX, &stddevX);
					calculateStats(pupilYs, &meanY, &stddevY);

					ix = pupilXs.begin();
					iy = pupilYs.begin();
					while( ix != pupilXs.end() ) {
						if(_isnan(meanX) || _isnan(meanY) || _isnan(stddevX) || _isnan(stddevY) ||
						   fabs(*ix - meanX) > stddevX * 5.0 || fabs(*iy - meanY) > stddevY * 5.0) {
							ix = pupilXs.erase(ix);
							iy = pupilYs.erase(iy);
							outliersRemain = true;
						} else {
							++ix;
							++iy;
						}
					}

					if(CONSOLE) printf("Eye: X: %g +/- %g, Y: %g +/- %g\n", meanX, stddevX, meanY, stddevY);
				}  //end while(outliersRemain)

				count=0;
				ix = sceneXs.begin();
				iy = sceneYs.begin();
			 //	while(sceneXs.size() > 30) {
				while (count++ < 15) {
					ix = sceneXs.erase(ix);
					iy = sceneYs.erase(iy);
				}


				outliersRemain = true;
				while (outliersRemain) {

					outliersRemain = false;
					calculateStats(sceneXs, &meanSceneX, &stddevSceneX);
					calculateStats(sceneYs, &meanSceneY, &stddevSceneY);

					std::list<double>::iterator ix = sceneXs.begin();
					std::list<double>::iterator iy = sceneYs.begin();


					ix = sceneXs.begin();
					while( ix != sceneXs.end() ) {
						if(_isnan(meanSceneX) || _isnan(meanSceneY) || _isnan(stddevSceneX) || _isnan(stddevSceneY) ||
						   fabs(*ix - meanSceneX) > stddevSceneX * 5.0 || fabs(*iy - meanSceneY) > stddevSceneY * 5.0) {
							ix = sceneXs.erase(ix);
							iy = sceneYs.erase(iy);
							outliersRemain = true;
						} else {
							++ix;
							++iy;
						}
					}

					if(CONSOLE) printf("Scene: X: %g +/- %g, Y: %g +/- %g\n", meanSceneX, stddevSceneX, meanSceneY, stddevSceneY);
				} //end while(outliersRemain)
			bool replaced = false;
			//replace the position if it has already been measured.
			for(unsigned int i=0; i<markerXs.size();i++) {
				if(markerXs[i] == oldMarkerX && markerYs[i] == oldMarkerY) {
					markerXs[i] = oldMarkerX;
					meanEyeXs[i] = meanX;
					stddevEyeXs[i] = stddevX;
					meanSceneXs[i] = meanSceneX;
					//prevent 0 standard deviation
					stddevSceneXs[i] = (stddevSceneX < .1) ? .1 : stddevSceneX;

					markerYs[i] = oldMarkerY;
					meanEyeYs[i] = meanY;
					stddevEyeYs[i] = stddevY;
					meanSceneYs[i] = meanSceneY;
					//prevent 0 standard deviation
					stddevSceneYs[i] = (stddevSceneY < .1) ? .1 : stddevSceneY;
					replaced = true;
					break;
				}
			}
			//otherwise, add it to the end
			if(!replaced) {
				markerXs.push_back(oldMarkerX);
				meanEyeXs.push_back(meanX);
				stddevEyeXs.push_back(stddevX);
				meanSceneXs.push_back(meanSceneX);
				stddevSceneXs.push_back(stddevSceneX);

				markerYs.push_back(oldMarkerY);
				meanEyeYs.push_back(meanY);
				stddevEyeYs.push_back(stddevY);
				meanSceneYs.push_back(meanSceneY);
				stddevSceneYs.push_back(stddevSceneY);
			}
			drawCalibration();

			}
			pupilXs.clear();
			pupilYs.clear();


			sceneXs.clear();
			sceneYs.clear();

			oldMarkerX = markerX;
			oldMarkerY = markerY;
		} //end if (markerX != oldMarkerX || markerY != oldMarkerY)

		if(pupilX > 0.0 && pupilY > 0.0) {
			pupilXs.push_back(pupilX);
			pupilYs.push_back(pupilY);
		}
		if(sceneX > 0.0 && sceneY > 0.0) {
			sceneXs.push_back(sceneX);
			sceneYs.push_back(sceneY);
		}
   }


}



void __fastcall TForm4::eyeCalibration() {

	double **coords = new2D<double>(markerXs.size(), 2, 0.0);
	double **data = new2D<double>(meanEyeXs.size(), 2, 0.0);
	double **error = new2D<double>(stddevEyeXs.size(),2,0.0);

	for(unsigned int i=0; i<markerXs.size(); i++) {
		if(_isnan(scaledMarkerXs[i]) || _isnan(scaledMarkerYs[i]) ||
		   _isnan(scaledMeanEyeXs[i]) || _isnan(scaledMeanEyeYs[i]) ||
		   _isnan(scaledStddevEyeXs[i]) || _isnan(scaledStddevEyeYs[i])) {
		   Application->MessageBoxA(L"Eye data is missing points.", L"Error", MB_OK);
		   return;
		}
		coords[i][0] = scaledMarkerXs[i];
		coords[i][1] = scaledMarkerYs[i];
		data[i][0] = scaledMeanEyeXs[i];
		data[i][1] = scaledMeanEyeYs[i];
		error[i][0] = scaledStddevEyeXs[i];
		error[i][1] = scaledStddevEyeYs[i];
	}

	double *params = new1D<double>(11, 0.0);
	params[0] = rInEdit->Text.ToDouble();
	params[1] = x0InEdit->Text.ToDouble();
	params[2] = y0InEdit->Text.ToDouble();
	params[3] = z0InEdit->Text.ToDouble();
	params[4] = aInEdit->Text.ToDouble();
	params[5] = bInEdit->Text.ToDouble();
	params[6] = gInEdit->Text.ToDouble();
	params[7] = bxInEdit->Text.ToDouble();
	params[8] = byInEdit->Text.ToDouble();
	params[9] = bzInEdit->Text.ToDouble();
	params[10] = fcInEdit->Text.ToDouble();

	double *spread = new1D<double>(11, 0.0);
	spread[4] = aRangeEdit->Text.ToDouble();
	spread[5] = bRangeEdit->Text.ToDouble();
	spread[6] = gRangeEdit->Text.ToDouble();
	spread[7] = bxRangeEdit->Text.ToDouble();
	spread[8] = byRangeEdit->Text.ToDouble();
	spread[9] = bzRangeEdit->Text.ToDouble();

	EyeDeltaFunction *df = new EyeDeltaFunction(coords, data, error, markerXs.size(), 2, params, 11);
	df->setStartingSpread(spread);
	Minimizer *m = new Minimizer(df);

	m->DoMinimize();

	double* pOut = df->getParameters();
	if(CONSOLE) {
		printf("R: %g\n", pOut[0]);
		printf("x0: %g\n", pOut[1]);
		printf("y0: %g\n", pOut[2]);
		printf("z0: %g\n", pOut[3]);
		printf("a: %g\n", pOut[4]);
		printf("b: %g\n", pOut[5]);
		printf("g: %g\n", pOut[6]);
		printf("bx: %g\n", pOut[7]);
		printf("by: %g\n", pOut[8]);
		printf("bz: %g\n", pOut[9]);
		printf("z: %g\n", pOut[10]);
	}

	spread[1] = x0RangeEdit->Text.ToDouble();
	spread[2] = y0RangeEdit->Text.ToDouble();
	spread[3] = z0RangeEdit->Text.ToDouble();
	df->setStartingSpread(spread);
	m->DoMinimize();
	pOut = df->getParameters();

	if(CONSOLE) {
		printf("R: %g\n", pOut[0]);
		printf("x0: %g\n", pOut[1]);
		printf("y0: %g\n", pOut[2]);
		printf("z0: %g\n", pOut[3]);
		printf("a: %g\n", pOut[4]);
		printf("b: %g\n", pOut[5]);
		printf("g: %g\n", pOut[6]);
		printf("bx: %g\n", pOut[7]);
		printf("by: %g\n", pOut[8]);
		printf("bz: %g\n", pOut[9]);
		printf("z: %g\n", pOut[10]);
	}

	spread[0] = rRangeEdit->Text.ToDouble();

	spread[10] = fcRangeEdit->Text.ToDouble();

	df->setStartingSpread(spread);
	m->DoMinimize();
	pOut = df->getParameters();

	r = pOut[0];
	x0 = pOut[1];
	y0 = pOut[2];
	z0 = pOut[3];
	a = pOut[4];
	b = pOut[5];
	g = pOut[6];
	bx = pOut[7];
	by = pOut[8];
	bz = pOut[9];
	fc = pOut[10];

	rOutEdit->Text = FormatFloat ("0.00", pOut[0]);
	x0OutEdit->Text = FormatFloat("0.00", pOut[1]);
	y0OutEdit->Text = FormatFloat("0.00", pOut[2]);
	z0OutEdit->Text = FormatFloat("0.00", pOut[3]);
	aOutEdit->Text = FormatFloat("0.00", pOut[4]);
	bOutEdit->Text = FormatFloat("0.00", pOut[5]);
	gOutEdit->Text = FormatFloat("0.00", pOut[6]);
	bxOutEdit->Text = FormatFloat("0.00", pOut[7]);
	byOutEdit->Text = FormatFloat("0.00", pOut[8]);
	bzOutEdit->Text = FormatFloat("0.00", pOut[9]);
	fcOutEdit->Text = FormatFloat("0.00", pOut[10]);

	if(CONSOLE) {
		printf("R: %g\n", pOut[0]);
		printf("x0: %g\n", pOut[1]);
		printf("y0: %g\n", pOut[2]);
		printf("z0: %g\n", pOut[3]);
		printf("a: %g\n", pOut[4]);
		printf("b: %g\n", pOut[5]);
		printf("g: %g\n", pOut[6]);
		printf("bx: %g\n", pOut[7]);
		printf("by: %g\n", pOut[8]);
		printf("bz: %g\n", pOut[9]);
		printf("z: %g\n", pOut[10]);
	}

	modeledXs.clear();
	modeledYs.clear();
	for(unsigned int i=0; i<meanEyeXs.size(); i++) {
		modeledXs.push_back(df->modeled[i][0]);
		modeledYs.push_back(df->modeled[i][1]);
	}
	delete m;
	delete df;
	delete1D(spread);
	delete1D(params);
	delete2D(coords, markerXs.size());
	delete2D(data, meanEyeXs.size());
	delete2D(error, stddevEyeXs.size());
}



void __fastcall TForm4::sceneCalibration() {

	double **coords = new2D<double>(markerXs.size(), 2, 0.0);
	double **data = new2D<double>(meanSceneXs.size(), 2, 0.0);
	double **error = new2D<double>(stddevSceneXs.size(),2,0.0);

	for(unsigned int i=0; i<markerXs.size(); i++) {
		if(_isnan(scaledMarkerXs[i]) || _isnan(scaledMarkerYs[i]) ||
		   _isnan(meanSceneXs[i]) || _isnan(meanSceneYs[i]) ||
		   _isnan(stddevSceneXs[i]) || _isnan(stddevSceneYs[i])) {
		   Application->MessageBoxA(L"Scene data is missing points.", L"Error", MB_OK);
		   return;
		}
		coords[i][0] = scaledMarkerXs[i];
		coords[i][1] = scaledMarkerYs[i];
		data[i][0] = meanSceneXs[i];
		data[i][1] = meanSceneYs[i];
		error[i][0] = stddevSceneXs[i];
		error[i][1] = stddevSceneYs[i];
	}

	double *params = new1D<double>(10, 0.0);
	params[0] = aInSceneEdit->Text.ToDouble();
	params[1] = bInSceneEdit->Text.ToDouble();
	params[2] = gInSceneEdit->Text.ToDouble();
	params[3] = bxInSceneEdit->Text.ToDouble();
	params[4] = byInSceneEdit->Text.ToDouble();
	params[5] = bzInSceneEdit->Text.ToDouble();
	params[6] = fcInSceneEdit->Text.ToDouble();
	params[7] = kInSceneEdit->Text.ToDouble();
	params[8] = ydInSceneEdit->Text.ToDouble();
	params[9] = zdInSceneEdit->Text.ToDouble();

	double *spread = new1D<double>(10, 0.0);
	spread[0] = aRangeSceneEdit->Text.ToDouble();
	spread[1] = bRangeSceneEdit->Text.ToDouble();
	spread[2] = gRangeSceneEdit->Text.ToDouble();
	spread[3] = bxRangeSceneEdit->Text.ToDouble();
	spread[4] = byRangeSceneEdit->Text.ToDouble();
	spread[5] = bzRangeSceneEdit->Text.ToDouble();
	spread[6] = fcRangeSceneEdit->Text.ToDouble();

	SceneDeltaFunction *df = new SceneDeltaFunction(coords, data, error, markerXs.size(), 2, params, 10);

	df->setStartingSpread(spread);
	Minimizer *m = new Minimizer(df);

	m->DoMinimize();

	double* pOut = df->getParameters();
	if(CONSOLE) {
		printf("\n");
		printf("a: %g\n", pOut[0]);
		printf("b: %g\n", pOut[1]);
		printf("g: %g\n", pOut[2]);
		printf("bx: %g\n", pOut[3]);
		printf("by: %g\n", pOut[4]);
		printf("bz: %g\n", pOut[5]);
		printf("fc: %g\n", pOut[6]);
		printf("K: %g\n", pOut[7]);
		printf("yc: %g\n", pOut[8]);
		printf("zc: %g\n", pOut[9]);
	}

	spread[7] = kRangeSceneEdit->Text.ToDouble();
	spread[8] = ydRangeSceneEdit->Text.ToDouble();
	spread[9] = zdRangeSceneEdit->Text.ToDouble();

	df->setStartingSpread(spread);
	m->DoMinimize();

	if(CONSOLE) {
		printf("\n");
		printf("a: %g\n", pOut[0]);
		printf("b: %g\n", pOut[1]);
		printf("g: %g\n", pOut[2]);
		printf("bx: %g\n", pOut[3]);
		printf("by: %g\n", pOut[4]);
		printf("bz: %g\n", pOut[5]);
		printf("fc: %g\n", pOut[6]);
		printf("K: %g\n", pOut[7]);
		printf("yc: %g\n", pOut[8]);
		printf("zc: %g\n", pOut[9]);
	}

	aOutSceneEdit->Text = FormatFloat("0.00", pOut[0]);
	bOutSceneEdit->Text = FormatFloat("0.00", pOut[1]);
	gOutSceneEdit->Text = FormatFloat("0.00", pOut[2]);
	bxOutSceneEdit->Text = FormatFloat("0.00", pOut[3]);
	byOutSceneEdit->Text = FormatFloat("0.00", pOut[4]);
	bzOutSceneEdit->Text = FormatFloat("0.00", pOut[5]);
	fcOutSceneEdit->Text = FormatFloat("0.00", pOut[6]);
	kOutSceneEdit->Text = FormatFloat("0.0E+00", pOut[7]);
	ydOutSceneEdit->Text = FormatFloat("0.00", pOut[8]);
	zdOutSceneEdit->Text = FormatFloat("0.00", pOut[9]);

	aScene = pOut[0];
	bScene = pOut[1];
	gScene = pOut[2];
	bxScene = pOut[3];
	byScene = pOut[4];
	bzScene = pOut[5];
	fcScene = pOut[6];
	kScene = pOut[7];
	ycScene = pOut[8];
	zcScene = pOut[9];

	modeledSceneXs.clear();
	modeledSceneYs.clear();
	for(unsigned int i=0; i<meanEyeXs.size(); i++) {
		modeledSceneXs.push_back(df->modeled[i][0]);
		modeledSceneYs.push_back(df->modeled[i][1]);
	}

	delete m;
	delete df;
	delete1D(spread);
	delete1D(params);
	delete2D(coords, markerXs.size());
	delete2D(data, meanSceneXs.size());
	delete2D(error, stddevSceneXs.size());
}


void __fastcall TForm4::CalculateCalibrationBtnClick(TObject *Sender) {
	scaleToPhysical();
	eyeCalibration();
	sceneCalibration();
	drawCalibration();
}



void __fastcall TForm4::SaveCalibrationBtnClick(TObject *Sender)
{

	if(SaveDialog1->Execute()) {
		Form4->xdoc_out->Active = true;
		Form4->xdoc_out->DocumentElement = Form4->xdoc_out->CreateNode("Configuration",
		ntElement, "");

		 _di_IXMLNode nodeElement = Form4->xdoc_out->DocumentElement->AddChild("EyeCalibration", -1);
		nodeElement->Attributes["r"] = UnicodeString(r);
		nodeElement->Attributes["x0"] = UnicodeString(x0);
		nodeElement->Attributes["y0"] = UnicodeString(y0);
		nodeElement->Attributes["z0"] = UnicodeString(z0);
		nodeElement->Attributes["a"] = UnicodeString(a);
		nodeElement->Attributes["b"] = UnicodeString(b);
		nodeElement->Attributes["g"] = UnicodeString(g);
		nodeElement->Attributes["bx"] = UnicodeString(bx);
		nodeElement->Attributes["by"] = UnicodeString(by);
		nodeElement->Attributes["bz"] = UnicodeString(bz);
		nodeElement->Attributes["fc"] = UnicodeString(fc);

		nodeElement = Form4->xdoc_out->DocumentElement->AddChild("SceneCalibration", -1);
		nodeElement->Attributes["a"] = UnicodeString(aScene);
		nodeElement->Attributes["b"] = UnicodeString(bScene);
		nodeElement->Attributes["g"] = UnicodeString(gScene);
		nodeElement->Attributes["bx"] = UnicodeString(bxScene);
		nodeElement->Attributes["by"] = UnicodeString(byScene);
		nodeElement->Attributes["bz"] = UnicodeString(bzScene);
		nodeElement->Attributes["fc"] = UnicodeString(fcScene);
		nodeElement->Attributes["k"] = UnicodeString(kScene);
		nodeElement->Attributes["yc"] = UnicodeString(ycScene);
		nodeElement->Attributes["zc"] = UnicodeString(zcScene);

		nodeElement = Form4->xdoc_out->DocumentElement->AddChild("Scaling", -1);
		nodeElement->Attributes["monitorWidth"] = UnicodeString(monitorWidth);
		nodeElement->Attributes["monitorHeight"] = UnicodeString(monitorHeight);
		nodeElement->Attributes["eyeCameraWidth"] = UnicodeString(cameraWidth);
		nodeElement->Attributes["eyeCameraHeight"] = UnicodeString(cameraHeight);
		nodeElement->Attributes["sceneCameraWidth"] = UnicodeString(sceneCameraWidth);
		nodeElement->Attributes["sceneCameraHeight"] = UnicodeString(sceneCameraHeight);

		nodeElement = Form4->xdoc_out->DocumentElement->AddChild("ReferencePositions", -1);
		nodeElement->Attributes["head"] = matrixToUnicodeString(headRef);

		ublas::vector<int> headMarkers(4);
		headMarkers(0) = HeadMarker0Edit->Text.ToInt();
		headMarkers(1) = HeadMarker1Edit->Text.ToInt();
		headMarkers(2) = HeadMarker2Edit->Text.ToInt();
		headMarkers(3) = HeadMarker3Edit->Text.ToInt();
		nodeElement->Attributes["headMarkers"] = vectorToUnicodeString(headMarkers);
		nodeElement->Attributes["eye"] = vectorToUnicodeString(eyePosition);
		nodeElement->Attributes["display"] = matrixToUnicodeString(displayRef);

		Form4->xdoc_out->SaveToFile(SaveDialog1->FileName);
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm4::LoadCalibrationPointsBtnClick(TObject *Sender)
{
	if(OpenDialog1->Execute()) {
		ClearCalibrationPointsBtnClick(this);

		int nPoints = 0;

		markerXs.clear();
		markerYs.clear();
		meanEyeXs.clear();
		meanEyeYs.clear();
		stddevEyeXs.clear();
		stddevEyeYs.clear();
		meanSceneXs.clear();
		meanSceneYs.clear();
		stddevSceneXs.clear();
		stddevSceneYs.clear();

		std::ifstream ifs;
		ifs.open (AnsiString(OpenDialog1->FileName).c_str());
		string value;

		while (ifs.good()) {
			getline(ifs, value);
			nPoints++;

		}
		nPoints-=2;


		ifs.clear();
		ifs.seekg(0);
		getline(ifs,value);  //skip header line.
		for(int i=0; i<nPoints; i++) {
			getline(ifs, value, ',');
			markerXs.push_back(atof(value.c_str()));

			getline(ifs, value, ',');
			markerYs.push_back(atof(value.c_str()));

			getline(ifs, value, ',');
			meanEyeXs.push_back(atof(value.c_str()));

			getline(ifs, value, ',');
			meanEyeYs.push_back(atof(value.c_str()));

			getline(ifs, value, ',');
			stddevEyeXs.push_back(atof(value.c_str()));


			getline(ifs, value, ',');
			stddevEyeYs.push_back(atof(value.c_str()));


			getline(ifs, value, ',');
			meanSceneXs.push_back(atof(value.c_str()));

			getline(ifs, value, ',');
			meanSceneYs.push_back(atof(value.c_str()));

			getline(ifs, value, ',');
			stddevSceneXs.push_back(atof(value.c_str()));

			getline(ifs, value);
			stddevSceneYs.push_back(atof(value.c_str()));
	 //		printf("%g %g %g %g %g %g %g %g %g %g\n\n", markerXs[i], markerYs[i], meanEyeXs[i], meanEyeYs[i], stddevEyeXs[i], stddevEyeYs[i], meanSceneXs[i], meanSceneYs[i], stddevSceneXs[i], stddevSceneYs[i]);
		}

		ifs.close();
		drawCalibration();
	}
}
//---------------------------------------------------------------------------

void rescaleForPanel(double *x, double *y, double minX, double maxX, double minY, double maxY) {

	if(IsInfinite(maxX) || IsInfinite(minX) || IsInfinite(maxY) || IsInfinite(minY)) return;
	*x = (*x - minX) /(maxX-minX) * bmpCanvas->w;
	*y = bmpCanvas->h - (*y - minY)/(maxY-minY) * bmpCanvas->h;


}

void minMax(vector<double> vect, double *min, double *max) {
	*max = -INF;
	*min = INF;
	for(unsigned int i=0; i<vect.size(); i++) {
		if(vect[i] > *max) *max = vect[i];
		if(vect[i] < *min) *min = vect[i];
	}

}


void TForm4::scaleToPhysical() {

	double producerWidth = 1.0; //producer window coordinates
	double producerHeight = 1.0; //producer window coordinates
	scaledMarkerXs.clear();
	scaledMarkerYs.clear();
	scaledMeanEyeXs.clear();
	scaledMeanEyeYs.clear();
	scaledStddevEyeXs.clear();
	scaledStddevEyeYs.clear();
	for(unsigned int i=0; i<markerXs.size(); i++) {

		scaledMarkerXs.push_back(monitorWidth*((producerWidth - markerXs[i])/producerWidth));  //to mm
		scaledMarkerYs.push_back(monitorHeight*((producerHeight - markerYs[i])/producerHeight)); // to mm
		scaledMeanEyeXs.push_back(meanEyeXs[i]-cameraWidth/2.0); //to camera pixels, centered at zero
		scaledMeanEyeYs.push_back(cameraHeight/2.0 - meanEyeYs[i]); //to camera pixels, centered at zero
		scaledStddevEyeXs.push_back(stddevEyeXs[i]); //remain in camera pixels
		scaledStddevEyeYs.push_back(stddevEyeYs[i]); //remain in camera pixels
	}
}

void __fastcall TForm4::drawCalibration() {
	scaleToPhysical();
	clear_bitmap(bmpCanvas);
	double x, y, sxl, sxu, syl, syu;
	double minX, maxX, minY, maxY;
	minMax(scaledMeanEyeXs, &minX, &maxX);
	minMax(scaledMeanEyeYs, &minY, &maxY);

	//draw modeled eye Xs and Ys.
	for(unsigned int i=0; i<modeledXs.size(); i++) {
		x = modeledXs[i];
		y = modeledYs[i];

		rescaleForPanel(&x,&y, minX-10, maxX + 10, minY - 10, maxY + 10);
		circlefill(bmpCanvas,x, y, 2, makecol(255,0,0));
	}

	//draw measured eye Xs and Ys.
	for(unsigned int i=0; i<meanEyeXs.size(); i++) {
		x = scaledMeanEyeXs[i];
		y = scaledMeanEyeYs[i];
		sxl = scaledMeanEyeXs[i] - scaledStddevEyeXs[i];
		sxu = scaledMeanEyeXs[i] + scaledStddevEyeXs[i];
		syl = scaledMeanEyeYs[i] - scaledStddevEyeYs[i];
		syu = scaledMeanEyeYs[i] + scaledStddevEyeYs[i];

		rescaleForPanel(&x,&y, minX-10, maxX + 10, minY - 10, maxY + 10);
		rescaleForPanel(&sxl, &syl, minX-10, maxX+10, minY-10, maxY+10);
		rescaleForPanel(&sxu, &syu, minX-10, maxX+10, minY-10, maxY+10);

  //		circlefill(bmpCanvas,x, y, 2, makecol(0,255,0));
		hline(bmpCanvas, sxu, y, sxl, makecol(0,255,0));
		vline(bmpCanvas, x, syl, syu, makecol(0,255,0));
	}



	HWND hWnd = eyeCalibPanel->Handle;
	HDC hDC = GetDC(hWnd);
	draw_to_hdc (hDC,bmpCanvas,0,0);
	ReleaseDC(hWnd,hDC);

	double tempMin;
	double tempMax;
	clear_bitmap(sceneCanvas);
	minMax(meanSceneXs, &minX, &maxX);
	minMax(modeledSceneXs, &tempMin, &tempMax);
	minX = (tempMin < minX) ? tempMin : minX;
	maxX = (tempMax > maxX) ? tempMax : maxX;

	minMax(meanSceneYs, &minY, &maxY);
	minMax(modeledSceneYs, &tempMin, &tempMax);
	minY = (tempMin < minY) ? tempMin : minY;
	maxY = (tempMax > maxY) ? tempMax : maxY;


	//draw modeled scene Xs and Ys.
	for(unsigned int i=0; i<modeledSceneXs.size(); i++) {
		x = modeledSceneXs[i];
		y = modeledSceneYs[i];
//		printf("modeled, x: %g  y: %g\n", x, y);

		rescaleForPanel(&x,&y, minX-10, maxX + 10, minY - 10, maxY + 10);
		circlefill(sceneCanvas,x, y, 2, makecol(255,0,0));
	}

	//draw measured scene Xs and Ys.
	for(unsigned int i=0; i<meanSceneXs.size(); i++) {
		x = meanSceneXs[i];
		y = meanSceneYs[i];
 //	printf("measured, x: %g  y: %g\n", x, y);
		sxl = meanSceneXs[i] - stddevSceneXs[i];
		sxu = meanSceneXs[i] + stddevSceneXs[i];
		syl = meanSceneYs[i] - stddevSceneYs[i];
		syu = meanSceneYs[i] + stddevSceneYs[i];

		rescaleForPanel(&x,&y, minX-10, maxX + 10, minY - 10, maxY + 10);
		rescaleForPanel(&sxl, &syl, minX-10, maxX+10, minY-10, maxY+10);
		rescaleForPanel(&sxu, &syu, minX-10, maxX+10, minY-10, maxY+10);

		hline(sceneCanvas, sxu, y, sxl, makecol(0,255,0));
		vline(sceneCanvas, x, syl, syu, makecol(0,255,0));

	}

	hWnd = sceneCalibPanel->Handle;
	hDC = GetDC(hWnd);
	draw_to_hdc (hDC,sceneCanvas,0,0);
	ReleaseDC(hWnd,hDC);

}

void __fastcall TForm4::FormCreate(TObject *Sender)
{
	allegro_init();
	set_gdi_color_format();
	set_color_depth(CDEPTH);
	bmpCanvas = create_bitmap_ex(CDEPTH,eyeCalibPanel->Width,eyeCalibPanel->Height);
	sceneCanvas = create_bitmap_ex(CDEPTH, sceneCalibPanel->Width, sceneCalibPanel->Height);
	clear_bitmap(bmpCanvas);
	clear_bitmap(sceneCanvas);
	MonitorWidthEditChange(this);
	MonitorHeightEditChange(this);
	CameraWidthEditChange(this);
	CameraHeightEditChange(this);
	SceneCameraWidthEditChange(this);
	SceneCameraHeightEditChange(this);

    Form4->Caption = UnicodeString("Eye Calibrator, version ") + getVersion();

}
//---------------------------------------------------------------------------


void __fastcall TForm4::MonitorWidthEditChange(TObject *Sender)
{
	monitorWidth = MonitorWidthEdit->Text.ToDouble();
}
//---------------------------------------------------------------------------

void __fastcall TForm4::MonitorHeightEditChange(TObject *Sender)
{
	monitorHeight = MonitorHeightEdit->Text.ToDouble();
}
//---------------------------------------------------------------------------

void __fastcall TForm4::CameraWidthEditChange(TObject *Sender)
{
	cameraWidth = CameraWidthEdit->Text.ToDouble();
}
//---------------------------------------------------------------------------

void __fastcall TForm4::CameraHeightEditChange(TObject *Sender)
{
	cameraHeight = CameraHeightEdit->Text.ToDouble();
}
//---------------------------------------------------------------------------

void __fastcall TForm4::SceneCameraWidthEditChange(TObject *Sender)
{
	sceneCameraWidth = SceneCameraWidthEdit->Text.ToDouble();
}
//---------------------------------------------------------------------------

void __fastcall TForm4::SceneCameraHeightEditChange(TObject *Sender)
{
	sceneCameraHeight = SceneCameraHeightEdit->Text.ToDouble();
}
//---------------------------------------------------------------------------


void __fastcall TForm4::SaveCalibrationPointsBtnClick(TObject *Sender)
{
	if(SaveDialog2->Execute()) {
		FILE *fp = fopen(AnsiString(SaveDialog2->FileName).c_str(), "w");
		if(fp) {
			fprintf(fp, "markerXs,markerYs,meanEyeXs,meanEyeYs,stddevEyeXs,stddevEyeYs,meanSceneXs,meanSceneYs,stddevSceneXs,stddevSceneYs\n");
			for(unsigned int i=0; i < markerXs.size(); ++i) {
				fprintf(fp, "%g,%g,%g,%g,%g,%g,%g,%g,%g,%g\n", markerXs[i], markerYs[i],
					meanEyeXs[i], meanEyeYs[i], stddevEyeXs[i], stddevEyeYs[i],
					meanSceneXs[i], meanSceneYs[i], stddevSceneXs[i], stddevSceneYs[i]);
			}
			fclose(fp);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm4::ClearCalibrationPointsBtnClick(TObject *Sender)
{
	markerXs.clear();
	markerYs.clear();
	meanEyeXs.clear();
	meanEyeYs.clear();
	stddevEyeXs.clear();
	stddevEyeYs.clear();
	meanSceneXs.clear();
	meanSceneYs.clear();
	stddevSceneXs.clear();
	stddevSceneYs.clear();
	modeledXs.clear();
	modeledYs.clear();
	modeledSceneXs.clear();
	modeledSceneYs.clear();
	drawCalibration();
}
//---------------------------------------------------------------------------




void __fastcall TForm4::FormClose(TObject *Sender, TCloseAction &Action)
{
	if(gazeInlet) lsl_destroy_inlet(gazeInlet);
	if(sceneInlet) lsl_destroy_inlet(sceneInlet);
	if(phasespaceInlet) lsl_destroy_inlet(phasespaceInlet);
   /*	if(hGazeStream) ds_Close(hGazeStream);
	if(hSceneStream) ds_Close(hSceneStream);
	if(hPhasespace) ds_Close(hPhasespace);  */
}
//---------------------------------------------------------------------------


void __fastcall TForm4::ReferenceHeadButtonClick(TObject *Sender)
{

	ReferenceHeadButton->Caption = "Please Hold Still";
	for(int i=0; i<nHeadPoints; i++) {
		headXsList[i].clear();
		headYsList[i].clear();
		headZsList[i].clear();
	}
	timerMode = isHead;
	headCount = 0;

}
//---------------------------------------------------------------------------


void __fastcall TForm4::WandPosition0ButtonClick(TObject *Sender)
{
	WandPosition0Button->Caption = "Please Hold Still";
	for(int i=0; i<nHeadPoints; i++) {
		headXsList[i].clear();
		headYsList[i].clear();
		headZsList[i].clear();
	}
	timerMode = isWand0;
	headCount = 0;
}
//---------------------------------------------------------------------------


void __fastcall TForm4::WandPosition1ButtonClick(TObject *Sender)
{
	WandPosition1Button->Caption = "Please Hold Still";
	for(int i=0; i<nHeadPoints; i++) {
		headXsList[i].clear();
		headYsList[i].clear();
		headZsList[i].clear();
	}
	timerMode = isWand1;
	headCount = 0;
}
//---------------------------------------------------------------------------


void TForm4::updateMonitorMeasurement() {
	for(unsigned int i=0; i<displayRef.size1(); i++) {
		for(unsigned int j=0; j<displayRef.size2(); j++) {
			if(displayRef(i,j) == 0.0) return; //not all data available
		}
	}
	monitorWidth = (length(ublas::column(displayRef, 3) - ublas::column(displayRef,2)) +
	length(ublas::column(displayRef, 1) - ublas::column(displayRef,0)))/2;
	MonitorWidthEdit->Text = monitorWidth;

	monitorHeight = (length(ublas::column(displayRef, 2) - ublas::column(displayRef,0)) +
	length(ublas::column(displayRef, 3) - ublas::column(displayRef,1)))/2;
	MonitorHeightEdit->Text = monitorHeight;
	StatusMemo->Lines->Add("Monitor measurement updated.");
}


void __fastcall TForm4::DisplayULButtonClick(TObject *Sender)
{
	Timer1->Enabled = false;
	Sleep(2);
   //	TMaxArray fr;
	displayRef(0,0) = displayRef(1,0) = displayRef(2,0) = 0;
 //	while(ds_Read(hPhasespace, (char *) &fr)) {
	if(phasespaceInlet) {
		float *buf = new1D<float>(phasespaceChannels,0);
		int errcode;
		while(lsl_pull_sample_f(phasespaceInlet,buf,phasespaceChannels,0.0,&errcode));

		int disp =  DisplayMarkerEdit->Text.ToInt();
		displayRef(0,0) = buf[disp*channelsPerMarker]*1000.0;
		displayRef(1,0) = buf[disp*channelsPerMarker+1]*1000.0;
		displayRef(2,0) = buf[disp*channelsPerMarker+2]*1000.0;
		if(CONSOLE) printf("display marker: %g %g %g\n", buf[disp*4]*1000.0, buf[disp*4+1]*1000.0, buf[disp*4+2]*1000.0);
		delete1D(buf);
   }
	if(displayRef(0,0) == 0) {
		StatusMemo->Lines->Add("Display measurement failed. Phasespace marker is not visible.");
		StatusMemo->Color = clRed;
	}
	else {
		updateMonitorMeasurement();
		StatusMemo->Lines->Add("Display measurement succeeded.");
		StatusMemo->Color = clWindow;
	}
   Timer1->Enabled = true;
}
//---------------------------------------------------------------------------


void __fastcall TForm4::DisplayURButtonClick(TObject *Sender)
{
	Timer1->Enabled = false;
	Sleep(2);
   //	TMaxArray fr;
	displayRef(0,1) = displayRef(1,1) = displayRef(2,1) = 0;
   //	while(ds_Read(hPhasespace, (char *) &fr)) {
	if(phasespaceInlet) {
		float *buf = new1D<float>(phasespaceChannels,0);
		int errcode;
		while(lsl_pull_sample_f(phasespaceInlet,buf,phasespaceChannels,0.0,&errcode));
		int disp =  DisplayMarkerEdit->Text.ToInt();
		displayRef(0,1) = buf[disp*channelsPerMarker]*1000.0;
		displayRef(1,1) = buf[disp*channelsPerMarker+1]*1000.0;
		displayRef(2,1) = buf[disp*channelsPerMarker+2]*1000.0;
		if(CONSOLE) printf("display marker: %g %g %g\n", buf[disp*channelsPerMarker]*1000.0, buf[disp*channelsPerMarker+1]*1000.0, buf[disp*channelsPerMarker+2]*1000.0);

		delete1D(buf);
	}
	if(displayRef(0,1) == 0) {
		StatusMemo->Lines->Add("Display measurement failed. Phasespace marker is not visible.");
		StatusMemo->Color = clRed;
	}
	else {
		updateMonitorMeasurement();
		StatusMemo->Lines->Add("Display measurement succeeded.");
		StatusMemo->Color = clWindow;
	}
	Timer1->Enabled = true;
}
//---------------------------------------------------------------------------


void __fastcall TForm4::DisplayBLButtonClick(TObject *Sender)
{
	Timer1->Enabled = false;
	Sleep(2);
   //	TMaxArray fr;
	displayRef(0,2) = displayRef(1,2) = displayRef(2,2) = 0;
   //	while(ds_Read(hPhasespace, (char *) &fr)) {
	if(phasespaceInlet) {
		float *buf = new1D<float>(phasespaceChannels,0);
		int errcode;
		while(lsl_pull_sample_f(phasespaceInlet,buf,phasespaceChannels,0.0,&errcode));
		int disp =  DisplayMarkerEdit->Text.ToInt();
		displayRef(0,2) = buf[disp*channelsPerMarker]*1000.0;
		displayRef(1,2) = buf[disp*channelsPerMarker+1]*1000.0;
		displayRef(2,2) = buf[disp*channelsPerMarker+2]*1000.0;
		if(CONSOLE) printf("display marker: %g %g %g\n", buf[disp*channelsPerMarker]*1000.0, buf[disp*channelsPerMarker+1]*1000.0, buf[disp*channelsPerMarker+2]*1000.0);
		delete1D(buf);
   }
	if(displayRef(0,2) == 0) {
		StatusMemo->Lines->Add("Display measurement failed. Phasespace marker is not visible.");
		StatusMemo->Color = clRed;
	}
	else {
		updateMonitorMeasurement();
		StatusMemo->Lines->Add("Display measurement succeeded.");
		StatusMemo->Color = clWindow;
	}
	Timer1->Enabled = true;
}
//---------------------------------------------------------------------------


void __fastcall TForm4::DisplayBRButtonClick(TObject *Sender)
{
	Timer1->Enabled = false;
	Sleep(2);
  //	TMaxArray fr;
	displayRef(0,3) = displayRef(1,3) = displayRef(2,3) = 0;
  //	while(ds_Read(hPhasespace, (char *) &fr)) {
	if(phasespaceInlet) {
		float *buf = new1D<float>(phasespaceChannels,0);
		int errcode;
		while(lsl_pull_sample_f(phasespaceInlet,buf,phasespaceChannels,0.0,&errcode));
		int disp =  DisplayMarkerEdit->Text.ToInt();
		displayRef(0,3) = buf[disp*channelsPerMarker]*1000.0;
		displayRef(1,3) = buf[disp*channelsPerMarker+1]*1000.0;
		displayRef(2,3) = buf[disp*channelsPerMarker+2]*1000.0;
		if(CONSOLE) printf("display marker: %g %g %g\n", buf[disp*channelsPerMarker]*1000.0, buf[disp*channelsPerMarker+1]*1000.0, buf[disp*channelsPerMarker+2]*1000.0);
		delete1D(buf);
	}
	if(displayRef(0,3) == 0) {
		StatusMemo->Lines->Add("Display measurement failed. Phasespace marker is not visible.");
		StatusMemo->Color = clRed;
	}
	else {
		updateMonitorMeasurement();
		StatusMemo->Lines->Add("Display measurement succeeded.");
		StatusMemo->Color = clWindow;
	}
	Timer1->Enabled = true;
}
//---------------------------------------------------------------------------


void __fastcall TForm4::testEyeClick(TObject *Sender)
{

	timerMode = isEyeTest;
	headCount = 0;

}
//---------------------------------------------------------------------------


void __fastcall TForm4::eyeTestDoneClick(TObject *Sender)
{
	timerMode = isStandard;
}
//---------------------------------------------------------------------------


void __fastcall TForm4::Timer2Timer(TObject *Sender) {

	/*
	AnsiString DirMMF[100];
	AnsiString s ="/tmp/*.#";
	int Count =0;
	TSearchRec Info;
	if (FindFirst (s,faAnyFile,Info) ==0) {
		do
			DirMMF[Count++]=Info.Name;
		while (FindNext(Info)==0);
		FindClose(Info);
	}
	 */
	lsl_streaminfo infos[MAX_STREAMS];
	int streamsFound = lsl_resolve_all(infos, MAX_STREAMS, 0.1);
	if (streamsFound !=GazeComboBox->Items->Count) {
		GazeComboBox->Items->Clear();
		SceneComboBox->Items->Clear();
		PhaseComboBox->Items->Clear();
		for (int i = 0; i < streamsFound; i++) {
			GazeComboBox->Items->Append(lsl_get_name(infos[i]));
			SceneComboBox->Items->Append(lsl_get_name(infos[i]));
			PhaseComboBox->Items->Append(lsl_get_name(infos[i]));
		}

	}
	Timer2->Enabled = false;
}
//---------------------------------------------------------------------------


void __fastcall TForm4::GazeComboBoxChange(TObject *Sender)
{

	/*
	AnsiString s = GazeComboBox->Text;
	s[strlen(s.c_str())-1] = 0;
	AnsiString fn = "/tmp/" + s;

	if(hGazeStream)
		ds_Close(hGazeStream);
	hGazeStream = ds_Open(fn.c_str());

	if(hGazeStream && hSceneStream) {
		Timer1->Enabled = true;
	}
	*/

	if(gazeInlet) lsl_destroy_inlet(gazeInlet);

	lsl_streaminfo info;
	lsl_resolve_byprop(&info,1, "name", ((AnsiString) GazeComboBox->Text).c_str(), 1,1.0);
	gazeInlet = lsl_create_inlet(info, 300, LSL_NO_PREFERENCE,1);

	gazeChannels = lsl_get_channel_count(info);




}
//---------------------------------------------------------------------------



void __fastcall TForm4::SceneComboBoxChange(TObject *Sender)
{
 /*	AnsiString s = SceneComboBox->Text;
	s[strlen(s.c_str())-1] = 0;
	AnsiString fn = "/tmp/" + s;

	if(hSceneStream)
		ds_Close(hSceneStream);
	hSceneStream = ds_Open(fn.c_str());

	if(hGazeStream && hSceneStream) {
		Timer1->Enabled = true;
	}
	*/

	if(sceneInlet) lsl_destroy_inlet(sceneInlet);

	lsl_streaminfo info;
	lsl_resolve_byprop(&info,1, "name", ((AnsiString) SceneComboBox->Text).c_str(), 1,1.0);
	sceneInlet = lsl_create_inlet(info, 300, LSL_NO_PREFERENCE,1);

	sceneChannels = lsl_get_channel_count(info);
}
//---------------------------------------------------------------------------



void __fastcall TForm4::PhaseComboBoxChange(TObject *Sender)
{
/*
	AnsiString s = PhaseComboBox->Text;
	s[strlen(s.c_str())-1] = 0;
	AnsiString fn = "/tmp/" + s;

	if(hPhasespace)
		ds_Close(hPhasespace);
	hPhasespace = ds_Open(fn.c_str());

	if(hGazeStream && hSceneStream) {
		Timer1->Enabled = true;
	}
	*/

	if(phasespaceInlet) lsl_destroy_inlet(phasespaceInlet);

	lsl_streaminfo info;
	lsl_resolve_byprop(&info,1, "name", ((AnsiString) PhaseComboBox->Text).c_str(), 1,1.0);
	phasespaceInlet = lsl_create_inlet(info, 300, LSL_NO_PREFERENCE,1);

	phasespaceChannels = lsl_get_channel_count(info);
}
//---------------------------------------------------------------------------


/**************************************************************************

 Begin marker control code.

***************************************************************************/

std::vector<MonitorDrawer*> monitorDrawers;

std::vector<int> monitorsTodo;
std::vector<double> markerXsTodo;
std::vector<double> markerYsTodo;

int currentSpot = -1;

void __fastcall TForm4::FormKeyPress(TObject *Sender, wchar_t &Key)
{
	if(currentSpot >= 0) {
		if(Timer3->Enabled == false) {
			switch(Key)
			{
			case ' ':
				Timer3->Enabled = true;
				break;
			case 'b':
				if(currentSpot >= 0 && currentSpot < markerXsTodo.size())
					monitorDrawers[monitorsTodo[currentSpot]-1]->drawMarkers(markerXsTodo[currentSpot],markerYsTodo[currentSpot],false);
				currentSpot--;
				if(currentSpot >= 0 && currentSpot < markerXsTodo.size())
					monitorDrawers[monitorsTodo[currentSpot]-1]->drawMarkers(markerXsTodo[currentSpot],markerYsTodo[currentSpot],true);
				markerX = -1.0;
				markerY = -1.0;
				break;
			case VK_ESCAPE:
				currentSpot = -1;
				Timer3->Enabled = false;
				markerX = -1.0;
				markerY = -1.0;
				for(int monitor=1; monitor<=monitors.size(); monitor++) {
					if(monitorDrawers[monitor-1]->visible)  monitorDrawers[monitor-1]->setVisible(SW_HIDE);
				}
				break;
			}
		}
		Key = 0;
	}

}

void __fastcall TForm4::FormKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)

{
	if(currentSpot >= 0) {
		Key = 0;
	}
}

void repaintCalibWindow(int drawer) {
	monitorDrawers[drawer]->blackenWindow();
	for(int i=0; i<markerXsTodo.size(); i++) {
		if(monitorsTodo[i]-1==drawer) {
			if(i==currentSpot)
				monitorDrawers[drawer]->drawMarkers(markerXsTodo[i],markerYsTodo[i],true);
			else
				monitorDrawers[drawer]->drawMarkers(markerXsTodo[i],markerYsTodo[i],false);
		}
	}
}

LRESULT CALLBACK WindowPrc(HWND hWnd, UINT Msg,
							WPARAM wParam, LPARAM lParam)
{
   //	printf("here\n");
	switch(Msg)
	{
	case WM_DESTROY:
	 //   PostQuitMessage(0); Exit app on window close.
		return 0;
	case WM_ERASEBKGND:
		for(int i=0; i<monitorDrawers.size(); i++) {
			if(monitorDrawers[i]->hwnd == hWnd)
				repaintCalibWindow(i);
		}

		return 0;
	//case WM_PAINT:
	//	printf("repaint me\n");
	//	return 0;
	default:
		break;
	}

	return DefWindowProc(hWnd, Msg, wParam, lParam);
}

BOOL CALLBACK MonitorEnumProc(
  __in  HMONITOR hMonitor,
  __in  HDC hDC,
  __in  LPRECT lprcMonitor,
  __in  LPARAM dwData
)
{

	MONITORINFOEX monitorinfo;
	monitorinfo.cbSize = sizeof(MONITORINFOEX);
	GetMonitorInfo( hMonitor, &monitorinfo );
	long x, y, width, height;
	x = monitorinfo.rcMonitor.left;
	y = monitorinfo.rcMonitor.top;
	width = monitorinfo.rcMonitor.right - monitorinfo.rcMonitor.left;
	height = monitorinfo.rcMonitor.bottom - monitorinfo.rcMonitor.top;

	if(CONSOLE) printf("x: %d, y: %d, width: %d, height: %d\n", x, y,width,height);


	HMODULE hInstance = GetModuleHandle(NULL);
	 if(!hInstance && CONSOLE) printf("no hInstance\n");

		 // The variable that will define the window
	WNDCLASSEX  WndClsEx;
	// The window's name
	static char szAppName[] = "FirstClass";
	MSG         Msg;

	// Filling out the structure that builds the window
	WndClsEx.cbSize = sizeof(WndClsEx);
	WndClsEx.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
	WndClsEx.lpfnWndProc = WindowPrc;
	WndClsEx.cbClsExtra = 0;
	WndClsEx.cbWndExtra = 0;
	WndClsEx.hInstance = hInstance;
	WndClsEx.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClsEx.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClsEx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClsEx.lpszMenuName = NULL;
	WndClsEx.lpszClassName = szAppName;
	WndClsEx.hIconSm = LoadIcon(hInstance, IDI_APPLICATION);

	RegisterClassEx(&WndClsEx);

   HWND hWnd = CreateWindowEx(
					WS_EX_OVERLAPPEDWINDOW,
					szAppName,
					"Basic Win32 Application",
					WS_POPUP/* | WS_VISIBLE */ ,
				   	x-2,
					y-2,
					width+4,
					height+4,
				   //	x,y,width-1,height-1,
					NULL,
					NULL,
					hInstance,
					NULL
				  );
				  int err= GetLastError();
				  //printf("lerr: %d\n", err);

	monitorDrawers.push_back(new MonitorDrawer(hWnd,1));


   return TRUE;
}



void __fastcall TForm4::CalibrationWindowButtonClick(TObject *Sender)
{
	currentSpot = 0;
	markerX = -1;
	markerY = -1;

	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);
	int nPoints=0;

	if (OpenDialog1->Execute())
	{

		std::ifstream ifs;
		ifs.open (AnsiString(OpenDialog1->FileName).c_str());
		string value;

		while (ifs.good()) {
			std::getline(ifs, value);
			nPoints++;

		}
		nPoints-=2;

		monitorsTodo.clear();
		markerXsTodo.clear();
		markerYsTodo.clear();
		ifs.clear();
		ifs.seekg(0);
		getline(ifs,value);  //skip header line.
		for(int i=0; i<nPoints; i++) {
			getline(ifs, value, ',');
			int monitor = atoi(value.c_str());
			if(!monitorDrawers[monitor-1]->visible) monitorDrawers[monitor-1]->setVisible(SW_SHOW);
			monitorsTodo.push_back(monitor);

			getline(ifs, value, ',');
			double x = atof(value.c_str());
			markerXsTodo.push_back(x);

			getline(ifs, value);
			double y = atof(value.c_str());
			markerYsTodo.push_back(y);

			if(i==0)
				monitorDrawers[monitor-1]->drawMarkers(x,y,true);
			else
				monitorDrawers[monitor-1]->drawMarkers(x,y,false);


		 //	printf("%d %g %g\n\n", monitorsTodo[i], markerXsTodo[i], markerYsTodo[i]);
		}

		currentSpot = 0;
		ifs.close();

	}
	Form4->SetFocus();
}



int timerCount = 0;
void __fastcall TForm4::Timer3Timer(TObject *Sender)
{
	if(timerCount == 0 && currentSpot >= 0 && currentSpot < markerXsTodo.size()) {
		monitor = monitorsTodo[currentSpot]-1;
		markerX = markerXsTodo[currentSpot];
		markerY = markerYsTodo[currentSpot];
	}
	timerCount++;
	if(timerCount == 150) {
		Timer3->Enabled = false;
		markerX = -1.0;
		markerY = -1.0;
		if(currentSpot >= 0 && currentSpot < markerXsTodo.size())
			monitorDrawers[monitorsTodo[currentSpot]-1]->drawMarkers(markerXsTodo[currentSpot],markerYsTodo[currentSpot],false);
		currentSpot++;
		if(currentSpot >= 0 && currentSpot < markerXsTodo.size())
			monitorDrawers[monitorsTodo[currentSpot]-1]->drawMarkers(markerXsTodo[currentSpot],markerYsTodo[currentSpot],true);
		timerCount = 0;

	}

	if(currentSpot == (int) markerXsTodo.size()) {
		for(unsigned int monitor=1; monitor<=monitorDrawers.size(); monitor++) {
			if(monitorDrawers[monitor-1]->visible)  monitorDrawers[monitor-1]->setVisible(SW_HIDE);
		}
		currentSpot = -1;
	}
}

/**************************************************************************

 End marker control code.

***************************************************************************/



/*
//---------------------------------------------------------------------------
std::map<int, TPoint3D*> p3Ds;

void __fastcall TForm4::LoadCalibrationButtonClick(TObject *Sender)
{
	if (OpenDialog2->Execute())
	{
		LoadConfig(OpenDialog2->FileName);
	}
}
//---------------------------------------------------------------------------


void TForm4::LoadConfig(const System::UnicodeString FileName)
{


	Form4->xdoc_out->LoadFromFile(FileName);

	_di_IXMLNode nodeElement =
		Form4->xdoc_out->ChildNodes->FindNode("Configuration")->ChildNodes->FindNode("Locations");

	if (nodeElement != NULL) {

		for (int i = 0; i < nodeElement->ChildNodes->Count; i++) {
			const _di_IXMLNode node = nodeElement->ChildNodes->Get(i);

			TPoint3D *    p3D =  new TPoint3D(node->Attributes["x"],node->Attributes["y"],node->Attributes["z"], node->Attributes["id"]);
			int id = node->Attributes["id"];
			p3Ds[id] = p3D;

		}
	} else {
		Application->MessageBoxA(L"Unable to find Locations group.", L"Error", MB_OK);
	}

	nodeElement = Form4->xdoc_out->ChildNodes->FindNode("Configuration")->ChildNodes->FindNode("Hotspots");

	if (nodeElement != NULL) {

		for (int i = 0; i < nodeElement->ChildNodes->Count; i++) {
			const _di_IXMLNode node = nodeElement->ChildNodes->Get(i);

		if (node->NodeName == UnicodeString("Screen")) {


				THotspotScreen * pHs = new THotspotScreen(
				p3Ds,
				(int) node->Attributes["topLeft"],
				(int) node->Attributes["topRight"],
				(int) node->Attributes["bottomLeft"],
				(int) node->Attributes["bottomRight"],
				(int) node->Attributes["sensor0"],
				(int) node->Attributes["sensor1"],
				(int) node->Attributes["device"],
				(int) node->Attributes["monitor"]);
				hotspotScreens.push_back(pHs);
			} else {
				Application->MessageBoxA((UnicodeString(L"Node: ") + node->NodeName + UnicodeString(" not recognized.")).w_str(), L"Error", MB_OK);
			}

		}
	} else {
			Application->MessageBoxA(L"Unable to find Hotspots group.", L"Error", MB_OK);
	}
}

  */




