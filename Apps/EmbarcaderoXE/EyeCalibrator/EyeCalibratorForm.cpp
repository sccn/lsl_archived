 /*

 Application to allow the calibrating of a head-mounted, one scene camera, one eye camera
 eye tracker.

 Written by Matthew Grivich, Swartz Center for Computational Neuroscience, UCSD

 */

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

#include "cvWrapper.h"


// ---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
#define CDEPTH 24
#define MAX_STREAMS 50

TForm4 *Form4;



double monitorWidth=0.0 ,monitorHeight=0.0, cameraWidth=0.0,cameraHeight=0.0;

double r=0.0, x0=0.0, y0=0.0, z0=0.0;
double a=0.0, b=0.0, g=0.0, bx=0.0, by=0.0, bz=0.0, fc=0.0;
double distToTarget=0.0;

int channelsPerMarker = 4;

int head0, head1, head2, head3, phase0, phase1;
int backgroundRed, backgroundGreen,backgroundBlue;

BITMAP * bmpCanvas;
BITMAP * sceneCanvas;

std::vector<int> monitors;
std::vector<double> markerXs, markerYs; //true marker position
std::vector<double> markerIndices, meanEyeXs, meanEyeYs, stddevEyeXs, stddevEyeYs;//pupil position in camera
std::vector<double> meanSceneXs, meanSceneYs, meanSceneZs, stddevSceneXs,stddevSceneYs,stddevSceneZs;//marker position in scene camera

std::vector<double> scaledMarkerXs, scaledMarkerYs, scaledMarkerZs;
std::vector<double> scaledMeanEyeXs, scaledMeanEyeYs, scaledStddevEyeXs, scaledStddevEyeYs;

std::vector<double> modeledXs, modeledYs;

int nHeadPoints = 4; //four for the head
vector< list<double> > headXsList(nHeadPoints, list<double>());
vector< list<double> > headYsList(nHeadPoints, list<double>());
vector< list<double> > headZsList(nHeadPoints, list<double>());

int nSceneCamPoints = 4;
vector< list<double> > sceneCamXsList(nHeadPoints, list<double>());
vector< list<double> > sceneCamYsList(nHeadPoints, list<double>());
vector< list<double> > sceneCamZsList(nHeadPoints, list<double>());

ublas::matrix<double> sceneCamCur(3,nSceneCamPoints);

ublas::matrix<double> headRef(3,nHeadPoints);
ublas::matrix<double> displayRef(3,4);

ublas::matrix<double> sceneCamRef(3,nSceneCamPoints);

ublas::vector<double> phasespace0(3);
ublas::vector<double> phasespace1(3);

ublas::matrix<double> sceneCameraMatrix(3,3);
ublas::vector<double> sceneDistortionCoeffs(5);

std::map<int, THotspotScreen*> hotspotScreens;

HWND mainHwnd;

/**************************************************************************

 Begin marker control code.

***************************************************************************/

std::map<int, MonitorDrawer*> monitorDrawers;

std::vector<int> devicesTodo;
std::vector<double> markerXsTodoComplete; //includes all devices
std::vector<double> markerYsTodoComplete; //includes all devices
std::vector<double> markerXsTodo; //current device only
std::vector<double> markerYsTodo; //current device only

int currentSpot = -1;
int currentSpotComplete = -1;

/**************************************************************************

 End marker control code.

***************************************************************************/


enum TimerMode {isStandard,isHead, isEyeTest, isPhasespaceTest} ;
TimerMode timerMode = isStandard;

int **tot = NULL;


int readCount = 0;
int progress = 0;

double pupilX = 0.0, pupilY = 0.0, sceneX = 0.0, sceneY = 0.0, sceneZ = 0.0;
std::vector<double> headXs(nHeadPoints,0.0), headYs(nHeadPoints,0.0), headZs(nHeadPoints,0.0);
double markerX = -1, markerY = -1;

double oldMarkerX = -1;
double oldMarkerY = -1;
std::list<double>pupilXs;
std::list<double>pupilYs;

std::list<double> sceneXs;
std::list<double> sceneYs;
std::list<double> sceneZs;

int headCount = 0;

ublas::vector<double> eyePosition(3,0.0);
ublas::vector<double> phaseMarkerPosition(3,0.0);

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

		//Convert from x,y in pixels to x,y,z in mm with respect to camera
		float *buf = new1D<float>(sceneChannels,0);
		while(lsl_pull_sample_f(sceneInlet,buf,sceneChannels,0.0,&errcode)) {
			static int sceneSamples = 0;
			sceneSamples++;
			Form4->SceneProgressBar->Position = sceneSamples % 120;
			std::list<double> refMarkerXs;
			std::list<double> refMarkerYs;

			int nSpots = (int) buf[1];

			for(int i=2; i<sceneChannels;) {
				refMarkerXs.push_back(buf[i++]);
				refMarkerYs.push_back( sceneCameraMatrix(1,2)*2+1/*sceneCameraHeight*/ - buf[i++]);
			}

			if(refMarkerXs.size() != markerXsTodo.size()) {
				printf("There are %d markers from scenestream and %d markers displayed.\n", refMarkerXs.size(), markerXsTodo.size());
				continue;
			}


			//rescale markerXsTodo to scene camera pixels (markerXsTodoInPixels)

			double oldMean, oldStddev, newMean, newStddev;

			calculateStats(markerXsTodo, &oldMean, &oldStddev);
			calculateStats(refMarkerXs, &newMean, &newStddev);
 //	  printf("oldMean: %g, oldStddev: %g, newMean: %g, newStddev
			if(_isnan(oldMean) || _isnan(oldStddev) || _isnan(newMean) || _isnan(newStddev)) {
				sceneX = NAN;
				sceneY = NAN;
				sceneZ = NAN;
				printf("Unable to calculate stats for markers.\n");
				CalibDistanceEdit->Text = "Unknown";
				continue;
			}

			std::vector<double> markerXsTodoInPixels;
			for(int i=0; i<markerXsTodo.size(); i++) {

				markerXsTodoInPixels.push_back(newStddev*(markerXsTodo[i]-oldMean)/oldStddev + newMean);
			}

			//rescale markerYsTodo to scene camera pixels (markerYsTodoInPixels)
			calculateStats(markerYsTodo, &oldMean, &oldStddev);
			calculateStats(refMarkerYs, &newMean, &newStddev);

			if(_isnan(oldMean) || _isnan(oldStddev) || _isnan(newMean) || _isnan(newStddev)) {
				sceneX = NAN;
				sceneY = NAN;
				sceneZ = NAN;
				static int qq = 0;
				if(qq++%30 == 0) {
					CalibDistanceEdit->Text = "Unknown";
					printf("Unable to calculate stats for markers.\n");
				}
				continue;
			}

			std::vector<double> markerYsTodoInPixels;
			for(int i=0; i<markerYsTodo.size(); i++) {
				markerYsTodoInPixels.push_back(newStddev*(markerYsTodo[i]-oldMean)/oldStddev + newMean);
			//	printf("mx: %g, my: %g  px: %g, py %g\n", markerXsTodo[i], markerYsTodo[i], markerXsTodoInPixels[i], markerYsTodoInPixels[i]);

			}


			//remove refMarkers, one by one. put in sorted refMarkers
			//remove the refMarker that is closest to the monitor markers
			std::vector<double> sortedRefMarkerXs;
			std::vector<double> sortedRefMarkerYs;

			for(int i=0; i<markerXsTodoInPixels.size(); i++) {
				double mx = markerXsTodoInPixels[i];
				double my = markerYsTodoInPixels[i];

				std::list<double>::iterator minXIndex;
				std::list<double>::iterator minYIndex;
				double minX;
				double minY;
				double storedX;
				double storedY;

				double minDist = INF;
				std::list<double>::iterator xIndex = refMarkerXs.end();
				std::list<double>::iterator yIndex = refMarkerYs.end();

				while(xIndex != refMarkerXs.begin() && yIndex != refMarkerYs.begin()) {
					--xIndex;
					--yIndex;

					double x = *xIndex;
					double y = *yIndex;

					double curDist =  (x-mx)*(x-mx)+(y-my)*(y-my);
					if(curDist < minDist) {
						minDist = curDist;
						minXIndex = xIndex;
						minYIndex = yIndex;
						minX = x;
						minY = y;
						storedX = mx;
						storedY = my;
					}

				}

				sortedRefMarkerXs.push_back(minX);
				sortedRefMarkerYs.push_back(minY);
			//	printf("minX:%g      minY: %g    storedX:%g   storedY:%g\n", minX, minY,storedX,storedY);
				refMarkerXs.erase(minXIndex);
				refMarkerYs.erase(minYIndex);


			}
		   //	static q = 0;
		   //	if((q++)%30 == 0)

			//sets sceneX, sceneY, sceneZ
			poseFinder(sortedRefMarkerXs, sortedRefMarkerYs);
			static int v = 0;
			if(v++%30 == 0) {
				if(CONSOLE) printf("%g %g %g ",sceneX, sceneY, sceneZ);
				if(CONSOLE && !_isnan(sceneX)) {
					printf("distance: %g\n",
					sqrt(
					(sceneX)*(sceneX)+
					(sceneY)*(sceneY)+
					(sceneZ)*(sceneZ)));
				}
				if(_isnan(sceneX)) {
					CalibDistanceEdit->Text = "Unknown";
				} else {
					CalibDistanceEdit->Text = FormatFloat ("0.00", 	sqrt(
						(sceneX)*(sceneX)+
						(sceneY)*(sceneY)+
						(sceneZ)*(sceneZ)));
				}
			}

 // printf("\n\n");


		}
		delete1D(buf);

	}



	//if phasespace data is available, load the head and display marker positions.
	if(phasespaceInlet) {
		float *buf = new1D<float>(phasespaceChannels,0);
		while(lsl_pull_sample_f(phasespaceInlet,buf,phasespaceChannels,0.0,&errcode)) {
			Form4->PhasespaceProgressBar->Position = (headCount/16) % 120;
			headCount++;


			phasespace0(0) = buf[phase0*channelsPerMarker] == 0 ? NAN : 1000.0*buf[phase0*channelsPerMarker] ;
			phasespace0(1) = buf[phase0*channelsPerMarker+1] == 0 ? NAN : 1000.0*buf[phase0*channelsPerMarker+1] ;
			phasespace0(2) = buf[phase0*channelsPerMarker+2] == 0 ? NAN : 1000.0*buf[phase0*channelsPerMarker+2] ;

			phasespace1(0) = buf[phase1*channelsPerMarker] == 0 ? NAN : 1000.0*buf[phase1*channelsPerMarker] ;
			phasespace1(1) = buf[phase1*channelsPerMarker+1] == 0 ? NAN : 1000.0*buf[phase1*channelsPerMarker+1] ;
			phasespace1(2) = buf[phase1*channelsPerMarker+2] == 0 ? NAN : 1000.0*buf[phase1*channelsPerMarker+2] ;

			bool headIsNan = false;
			headXs[0] = buf[head0*channelsPerMarker] == 0 ? NAN : 1000.0*buf[head0*channelsPerMarker] ;
			headYs[0] = buf[head0*channelsPerMarker+1]== 0 ? NAN : 1000.0*buf[head0*channelsPerMarker+1];
			headZs[0] = buf[head0*channelsPerMarker+2]== 0 ? NAN : 1000.0*buf[head0*channelsPerMarker+2];

			headXs[1] = buf[head1*channelsPerMarker]== 0 ? NAN : 1000.0*buf[head1*channelsPerMarker];
			headYs[1] = buf[head1*channelsPerMarker+1]== 0 ? NAN : 1000.0*buf[head1*channelsPerMarker+1];
			headZs[1] = buf[head1*channelsPerMarker+2]== 0 ? NAN : 1000.0*buf[head1*channelsPerMarker+2];

			headXs[2] = buf[head2*channelsPerMarker]== 0 ? NAN : 1000.0*buf[head2*channelsPerMarker];
			headYs[2] = buf[head2*channelsPerMarker+1]== 0 ? NAN : 1000.0*buf[head2*channelsPerMarker+1];
			headZs[2] = buf[head2*channelsPerMarker+2]== 0 ? NAN : 1000.0*buf[head2*channelsPerMarker+2];

			headXs[3] = buf[head3*channelsPerMarker]== 0 ? NAN : 1000.0*buf[head3*channelsPerMarker];
			headYs[3] = buf[head3*channelsPerMarker+1]== 0 ? NAN : 1000.0*buf[head3*channelsPerMarker+1];
			headZs[3] = buf[head3*channelsPerMarker+2]== 0 ? NAN : 1000.0*buf[head3*channelsPerMarker+2];

			for(int i=0; i<4; i++) {
				if(_isnan(headXs[i]) || _isnan(headYs[i]) || _isnan(headZs[i])) {
					headIsNan = true;
				}
			}


			phaseMarkerPosition(0) = buf[phase0*channelsPerMarker]== 0 ? NAN :1000.0*buf[phase0*channelsPerMarker];
			phaseMarkerPosition(1) = buf[phase0*channelsPerMarker+1]== 0 ? NAN :1000.0*buf[phase0*channelsPerMarker+1];
			phaseMarkerPosition(2) = buf[phase0*channelsPerMarker+2]== 0 ? NAN :1000.0*buf[phase0*channelsPerMarker+2];


			if(timerMode == isPhasespaceTest) {
				if(headCount % 480 ==0) {
					bool nans = false;
					if(hasNan(phasespace0)) {
						StatusMemo->Lines->Add("Phasespace marker 0 is not visible.");
						nans = true;
					}

					if(hasNan(phasespace1)) {
						StatusMemo->Lines->Add("Phasespace marker 1 is not visible.");

						nans = true;
					}
					if(!nans) {
						double dist = length(phasespace0-phasespace1);
						StatusMemo->Lines->Add(UnicodeString("Distance: ") + FormatFloat ("0.00", dist));
					}

				}




			}

			if(timerMode == isHead) {
				//if tracking head, store one second (480 samples) of data
				if(headCount < 480) {
					for(int i=0; i<nHeadPoints; i++) {
						//don't add if phasespace could not see the point
						if(!_isnan(headXs[i]) && !_isnan(headYs[i]) && !_isnan(headZs[i])) {
							headXsList[i].push_back(headXs[i]);
							headYsList[i].push_back(headYs[i]);
							headZsList[i].push_back(headZs[i]);
						}


					}

					for(int i=0; i<nSceneCamPoints; i++) {
						if(!hasNan(sceneCamCur)) {
						static v = 0;
						if(v++%30==0) print("sceneCamCurAdding", sceneCamCur);
							sceneCamXsList[i].push_back(sceneCamCur(0,i));
							sceneCamYsList[i].push_back(sceneCamCur(1,i));
							sceneCamZsList[i].push_back(sceneCamCur(2,i));
						}
					}

				//done getting head data.  average and store.
				} else {
					ReferenceHeadButton->Caption = "Reference Head Position";

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


							headRef(0,i) = meanX;
							headRef(1,i) = meanY;
							headRef(2,i) = meanZ;


						}
					}

					for(int i=0; i<nSceneCamPoints; i++) {
						boolean outliersRemain = true;
						double meanX, stddevX, meanY, stddevY, meanZ, stddevZ;
						while (outliersRemain) {

							outliersRemain = false;
							calculateStats(sceneCamXsList[i], &meanX, &stddevX);
							calculateStats(sceneCamYsList[i], &meanY, &stddevY);
							calculateStats(sceneCamZsList[i], &meanZ, &stddevZ);


							std::list<double>::iterator ix = sceneCamXsList[i].begin();
							std::list<double>::iterator iy = sceneCamYsList[i].begin();
							std::list<double>::iterator iz = sceneCamZsList[i].begin();
							while( ix != sceneCamXsList[i].end() ) {
								if(_isnan(meanX) || _isnan(meanY) || _isnan(meanZ) ||
								   _isnan(stddevX) || _isnan(stddevY) || _isnan(stddevZ) ||
								   fabs(*ix - meanX) > stddevX * 5.0 ||
								   fabs(*iy - meanY) > stddevY * 5.0 ||
								   fabs(*iz - meanZ) > stddevZ * 5.0 ) {

									ix = sceneCamXsList[i].erase(ix);
									iy = sceneCamYsList[i].erase(iy);
									iz = sceneCamZsList[i].erase(iz);
									outliersRemain = true;
								} else {
									++ix;
									++iy;
									++iz;
								}
							}


							sceneCamRef(0,i) = meanX;
							sceneCamRef(1,i) = meanY;
							sceneCamRef(2,i) = meanZ;

						}
					}


					if(hasNan(headRef) || hasNan(sceneCamRef)) {
						StatusMemo->Lines->Add("Head position measurement failed.");
						if(_isnan(headRef(0,0))) StatusMemo->Lines->Add("Head marker A is not visible.");
						if(_isnan(headRef(0,1))) StatusMemo->Lines->Add("Head marker B is not visible.");
						if(_isnan(headRef(0,2))) StatusMemo->Lines->Add("Head marker C is not visible.");
						if(_isnan(headRef(0,3))) StatusMemo->Lines->Add("Head marker D is not visible.");
						if(hasNan(sceneCamRef)) StatusMemo->Lines->Add("Scene Camera location not found.");
						StatusMemo->Color = clRed;
					} else {
						if(_isnan(sceneX) || _isnan(sceneY) || _isnan(sceneZ))
							StatusMemo->Lines->Add(AnsiString("Distance from scene camera to active spot is NAN."));
						else
							StatusMemo->Lines->Add(AnsiString("Distance from scene camera to active spot is: ") + FormatFloat("0", sqrt(sceneX*sceneX + sceneY*sceneY + sceneZ*sceneZ)) + AnsiString(" mm."));
						double averageHeadX = (headRef(0,0) + headRef(0,1) + headRef(0,2) + headRef(0,3))/4.0;
						double averageHeadY = (headRef(1,0) + headRef(1,1) + headRef(1,2) + headRef(1,3))/4.0;
						double averageHeadZ = (headRef(2,0) + headRef(2,1) + headRef(2,2) + headRef(2,3))/4.0;
						double dist =  sqrt((sceneCamRef(0,3) - averageHeadX) * (sceneCamRef(0,3) - averageHeadX) +
									   (sceneCamRef(1,3) - averageHeadY) * (sceneCamRef(1,3) - averageHeadY) +
									   (sceneCamRef(2,3) - averageHeadZ) * (sceneCamRef(2,3) - averageHeadZ));
							StatusMemo->Lines->Add(AnsiString("Distance between scene camera and center of head markers: ") + FormatFloat ("0", dist) + AnsiString(" mm"));

					   if (dist > 40.0 && dist < 300.0) {
							StatusMemo->Lines->Add("Head position measurement succeeded.");
							StatusMemo->Color = clWindow;

						} else {
							StatusMemo->Lines->Add("Scene camera has not been properly located.");
							StatusMemo->Color = clRed;
						}

					}
					if(CONSOLE) print("headRef", headRef);
					if(CONSOLE) print("sceneCamRef", sceneCamRef);

					timerMode = isStandard;
					break;
				}

			}

		}

		delete1D(buf);
	}

	//if gaze changed
	if(gazeChanged && timerMode == isStandard) {
		if(readCount%10==0 && CONSOLE) printf("%d %g %g %g %g %g %g %g\n", readCount, markerX, markerY, sceneX, sceneY, sceneZ, pupilX, pupilY);
		readCount++;
		//and marker moved, average data for this location, and update plots
		if (markerX != oldMarkerX || markerY != oldMarkerY) {


			if(oldMarkerX >= 0.0) {
				double meanX = 0.0, stddevX = 0.0;
				double meanY = 0.0, stddevY = 0.0;
				double meanZ = 0.0, stddevZ = 0.0;
				double meanSceneX = 0.0, stddevSceneX = 0.0;
				double meanSceneY = 0.0, stddevSceneY = 0.0;
				double meanSceneZ = 0.0, stddevSceneZ = 0.0;

				int count=0;
				std::list<double>::iterator ix = pupilXs.begin();
				std::list<double>::iterator iy = pupilYs.begin();
				std::list<double>::iterator iz;
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
				iz = sceneZs.begin();
			 //	while(sceneXs.size() > 30) {
				while (count++ < 15) {
					ix = sceneXs.erase(ix);
					iy = sceneYs.erase(iy);
					iz = sceneZs.erase(iz);
				}


				outliersRemain = true;
				while (outliersRemain) {

					outliersRemain = false;
					calculateStats(sceneXs, &meanSceneX, &stddevSceneX);
					calculateStats(sceneYs, &meanSceneY, &stddevSceneY);
					calculateStats(sceneZs, &meanSceneZ, &stddevSceneZ);

					std::list<double>::iterator ix = sceneXs.begin();
					std::list<double>::iterator iy = sceneYs.begin();
					std::list<double>::iterator iz = sceneZs.begin();


					ix = sceneXs.begin();
					while( ix != sceneXs.end() ) {
						if(_isnan(meanSceneX) || _isnan(meanSceneY) || _isnan(stddevSceneX) || _isnan(stddevSceneY) || _isnan(stddevSceneZ) ||
						   fabs(*ix - meanSceneX) > stddevSceneX * 5.0 || fabs(*iy - meanSceneY) > stddevSceneY * 5.0 || fabs(*iz - meanSceneZ) > stddevSceneZ * 5.0) {
							ix = sceneXs.erase(ix);
							iy = sceneYs.erase(iy);
							iz = sceneZs.erase(iz);
							outliersRemain = true;
						} else {
							++ix;
							++iy;
							++iz;
						}
					}

					if(CONSOLE) printf("Scene: X: %g +/- %g, Y: %g +/- %g, Z: %g +/- %g\n", meanSceneX, stddevSceneX, meanSceneY, stddevSceneY, meanSceneZ, stddevSceneZ);
				} //end while(outliersRemain)
			bool replaced = false;
			//replace the position if it has already been measured.
			for(unsigned int i=0; i<markerXs.size();i++) {
				if(markerIndices[i] == currentSpotComplete) {
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

					meanSceneZs[i] = meanSceneZ;
					//prevent 0 standard deviation
					stddevSceneZs[i] = (stddevSceneZ < .1) ? .1 : stddevSceneZ;

					replaced = true;
					break;
				}
			}
			//otherwise, add it to the end
			if(!replaced) {
			printf("oldMarkerX: %g   oldMarkerY: %g\n\n\n", oldMarkerX, oldMarkerY);
				markerIndices.push_back(currentSpotComplete);
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

				meanSceneZs.push_back(meanSceneZ);
				stddevSceneZs.push_back(stddevSceneZ);
			}

			drawCalibration();

			}
			pupilXs.clear();
			pupilYs.clear();


			sceneXs.clear();
			sceneYs.clear();
			sceneZs.clear();

			oldMarkerX = markerX;
			oldMarkerY = markerY;
		} //end if (markerX != oldMarkerX || markerY != oldMarkerY)

		if(pupilX > 0.0 && pupilY > 0.0) {
			pupilXs.push_back(pupilX);
			pupilYs.push_back(pupilY);
		}
		if(!_isnan(sceneX) && !_isnan(sceneY) && !_isnan(sceneZ)) {
			sceneXs.push_back(sceneX);
			sceneYs.push_back(sceneY);
			sceneZs.push_back(sceneZ);
		}
   }


}



void __fastcall TForm4::eyeCalibration() {
	if(markerXs.size() == 0) {
		Application->MessageBoxA(L"There is no calibration data.", L"Error", MB_OK);
		return;
	}

	double **coords = new2D<double>(markerXs.size(), 3, 0.0);
	double **data = new2D<double>(meanEyeXs.size(), 2, 0.0);
	double **error = new2D<double>(stddevEyeXs.size(),2,0.0);

	for(unsigned int i=0; i<markerXs.size(); i++) {
		if(_isnan(scaledMarkerXs[i]) || _isnan(scaledMarkerYs[i]) || _isnan(scaledMarkerZs[i]) ||
		   _isnan(scaledMeanEyeXs[i]) || _isnan(scaledMeanEyeYs[i]) ||
		   _isnan(scaledStddevEyeXs[i]) || _isnan(scaledStddevEyeYs[i])) {
		   Application->MessageBoxA(L"Eye or scene data is missing points.", L"Error", MB_OK);
		   return;
		}
		coords[i][0] = scaledMarkerXs[i];
		coords[i][1] = scaledMarkerYs[i];
		coords[i][2] = scaledMarkerZs[i];
		data[i][0] = scaledMeanEyeXs[i];
		data[i][1] = scaledMeanEyeYs[i];
		error[i][0] = scaledStddevEyeXs[i];
		error[i][1] = scaledStddevEyeYs[i];
	}
	distToTarget = scaledMarkerXs[0]; //distance from scene camera to center target along center axis

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

	spread[2] = y0RangeEdit->Text.ToDouble();
	spread[3] = z0RangeEdit->Text.ToDouble();
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
			printf("sceneX: %g, sceneY: %g, sceneZ: %g modeledX: %g modeledY: %g\n", coords[i][0], coords[i][1], coords[i][2], df->modeled[i][0], df->modeled[i][1]);
	}


	delete m;
	delete df;
	delete1D(spread);
	delete1D(params);
	delete2D(coords, markerXs.size());
	delete2D(data, meanEyeXs.size());
	delete2D(error, stddevEyeXs.size());
}


void TForm4::poseFinder(std::vector<double> xs, std::vector<double> ys) {

	double **coords = new2D<double>(markerXsTodo.size(), 3, 0.0);
	double **data = new2D<double>(xs.size(), 2, 0.0);

	double producerWidth = 1.0;
	double producerHeight = 1.0;



	for(unsigned int i=0; i<xs.size(); i++) {
		if(timerMode == isHead) {
			//centered at zero
			coords[i][0] = monitorWidth*(markerXsTodo[i]-0.5); //to mm
			coords[i][1] = monitorHeight*(markerYsTodo[i]-0.5); //to mm
			coords[i][2] = 0.0;
		} else {
			//make current marker 0,0,0. This way, rotations do not have to be understood or used.
			coords[i][0] = monitorWidth*(markerXsTodo[i]-markerXsTodo[currentSpot]); //to mm
			coords[i][1] = monitorHeight*(markerYsTodo[i]-markerYsTodo[currentSpot]); //to mm
			coords[i][2] = 0.0;
		}
   //		printf("coords[%d][0]: %g coords[%d][1]: %g", i, coords[i][0], i, coords[i][1]);
		data[i][0] = xs[i];
		data[i][1] = ys[i];
   //		printf("  data[%d][0]: %g data[%d][1]: %g\n", i, data[i][0], i, data[i][1]);

	}
	  //	printf("\n\n");

	double ** cameraM = new2D<double>(3,3,0.0);
	double * distanceC = new1D<double>(5,0.0);
	double ** rMat = new2D<double>(3,3,0.0);
	double * tVec = new1D<double>(3,0.0);
	cameraM[0][0] = sceneCameraMatrix(0,0);//557;//654.8611202347574;fcInSceneEdit->Text.ToDouble();
	cameraM[0][1] = sceneCameraMatrix(0,1);//0;
	cameraM[0][2] = sceneCameraMatrix(0,2);//(sceneCameraWidth - 1)/2.0;//319.5;
	cameraM[1][0] = sceneCameraMatrix(1,0);//0;
	cameraM[1][1] = sceneCameraMatrix(1,1);//557;//654.8611202347574;fcInSceneEdit->Text.ToDouble();
	cameraM[1][2] = sceneCameraMatrix(1,2);//(sceneCameraHeight -1)/2.0;//239.5;
	cameraM[2][0] = sceneCameraMatrix(2,0);//0;
	cameraM[2][1] = sceneCameraMatrix(2,1);//0;
	cameraM[2][2] = sceneCameraMatrix(2,2);//1;

	distanceC[0] = sceneDistortionCoeffs(0);//-0.3669624087278113;
	distanceC[1] = sceneDistortionCoeffs(1);//-0.07638290902180184;
	distanceC[2] = sceneDistortionCoeffs(2);//0;
	distanceC[3] = sceneDistortionCoeffs(3);//0;
	distanceC[4] = sceneDistortionCoeffs(4);//0.5764668364450144;

	openCV_findPose(coords, data, xs.size(), cameraM, distanceC, rMat, tVec);
	//millimiters (determined by calibration screen size and camera calibration matrix)
	//centered at center of scene camera field of view.
	//Tracked object moving up, right, and away from the camera is positive.
	sceneX = tVec[0]; //right
	sceneY = tVec[1]; //up
	sceneZ = tVec[2]; //away



 	//screen coordinates must be centered for this to be legit.
	static q=0;
	if(phasespaceInlet && (timerMode == isHead || timerMode == isEyeTest)) {

		if(q %30 == 0 && !_isnan(sceneX) && !_isnan(sceneY) && !_isnan(sceneZ)) printf("sceneX: %g sceneY: %g sceneZ: %g distance: %g\n",sceneX,sceneY,sceneZ, sqrt(sceneX*sceneX+sceneY*sceneY+sceneZ*sceneZ));

		ublas::matrix<double> sceneRotation(3,3);
		ublas::vector<double> sceneTranslation(3);

		ublas::matrix<double> cameraPositionOpenCV(3,4); //xhat,yhat,zhat, cameraPos, openCV coordinates
		ublas::matrix<double> cameraPosition(3,4); //xhat, yhat, zhat, cameraPos, phasespace coordinates

		for(int i=0; i<3; i++) {
			sceneTranslation(i) = -tVec[i];
			for(int j=0; j<3; j++) {
				sceneRotation(i,j) = rMat[j][i]; //transposed
			}
		}

		//if(q %30 == 0) print("sceneTranslation", sceneTranslation);
		//if(q %30 == 0) print("sceneRotation", sceneRotation);

		cameraPositionOpenCV(0,0) = 1;
		cameraPositionOpenCV(1,0) = 0;
		cameraPositionOpenCV(2,0) = 0;
		cameraPositionOpenCV(0,1) = 0;
		cameraPositionOpenCV(1,1) = 1;
		cameraPositionOpenCV(2,1) = 0;
		cameraPositionOpenCV(0,2) = 0;
		cameraPositionOpenCV(1,2) = 0;
		cameraPositionOpenCV(2,2) = 1;
		cameraPositionOpenCV(0,3) = 0;
		cameraPositionOpenCV(1,3) = 0;
		cameraPositionOpenCV(2,3) = 0;

		for(int i=0; i<3; i++) {
			for(int j=0; j<4; j++) {
				cameraPositionOpenCV(i,j) += sceneTranslation(i);
			}
		}

		cameraPositionOpenCV = ublas::prod(sceneRotation, cameraPositionOpenCV);

	  //	if(q%30==0)	printf("cameraPositionOpenCV: %g\n",360/2/3.14159*atan(cameraPositionOpenCV(2,0)/cameraPositionOpenCV(0,0)));

	   //	if(q%30 == 0) print("rotated foo", cameraPositionOpenCV);



	   //	if(q %30 == 0) print("cameraPositionOpenCV", cameraPositionOpenCV);

		//map from opencv coords to phasespace coords ( z -> x, x -> z, y -> y )
		cameraPosition(2,0) = cameraPositionOpenCV(0,0);
		cameraPosition(0,0) = cameraPositionOpenCV(2,0);
		cameraPosition(1,0) = cameraPositionOpenCV(1,0);

		cameraPosition(2,1) = cameraPositionOpenCV(0,1);
		cameraPosition(0,1) = cameraPositionOpenCV(2,1);
		cameraPosition(1,1) = cameraPositionOpenCV(1,1);

		cameraPosition(2,2) = cameraPositionOpenCV(0,2);
		cameraPosition(0,2) = cameraPositionOpenCV(2,2);
		cameraPosition(1,2) = cameraPositionOpenCV(1,2);

		cameraPosition(2,3) = cameraPositionOpenCV(0,3);
		cameraPosition(0,3) = cameraPositionOpenCV(2,3);
		cameraPosition(1,3) = cameraPositionOpenCV(1,3);

		 //  if(q%30==0)	printf("cameraPosition: %g\n",360/2/3.14159* atan((cameraPosition(0,0)-cameraPosition(0,3))/(cameraPosition(2,0)-cameraPosition(2,3))));

		ublas::vector<double> priorTranslation(3);
		ublas::vector<double> postTranslation(3);
		ublas::matrix<double> rotation(3,3);


		ublas::matrix<double> displayAtOrigin(3,4); //phasespace coordinates
		//Top left
		displayAtOrigin(0,0) = 0;
		displayAtOrigin(1,0) = (monitorHeight)/2;
		displayAtOrigin(2,0) = -(monitorWidth)/2;

		//top right
		displayAtOrigin(0,1) = 0;
		displayAtOrigin(1,1) = (monitorHeight)/2;
		displayAtOrigin(2,1) = (monitorWidth)/2;

		//bottom left
		displayAtOrigin(0,2) = 0;
		displayAtOrigin(1,2) = -(monitorHeight)/2;
		displayAtOrigin(2,2) = -(monitorWidth)/2;

		//bottom right
		displayAtOrigin(0,3) = 0;
		displayAtOrigin(1,3) = -(monitorHeight)/2;
		displayAtOrigin(2,3) = (monitorWidth)/2;

		double tot = 0.0;
		for(int i=0; i<3; i++) {
			for(int j=0; j<4; j++) {
				if(_isnan(displayRef(i,j))) {
					if(q%30 ==0) printf("display ref has NaNs\n");
					return;
				}
				tot += displayRef(i,j);

			}

		}
		if(tot == 0) {
			if(q%30 ==0)printf("displayRef has not been measured yet.\n");
			return;
		}

   //	if(q %30 ==0) print("displayAtOrigin", displayAtOrigin);
   //		if(q %30 ==0) print("displayRef", displayRef);
		//determine manipulation to get display from origin to actual displayRef (phasespace) position
		findRigidBody(displayAtOrigin, displayRef, priorTranslation, rotation, postTranslation);
	//	if(q%30 == 0) print("priorTranslation", priorTranslation);
	//	if(q%30 == 0) print("postTranslation", postTranslation);
		//if(q%30 == 0) print("cameraPosition", cameraPosition);
		//put scene camera pose in the same coordinates as display ref
		sceneCamCur = moveRigidBody(cameraPosition, priorTranslation, rotation, postTranslation);

  //	if(q%30==0)	printf("sceneCamCur: %g\n\n",360/2/3.14159*atan((sceneCamCur(2,0))/(sceneCamCur(0,0))));

		if(q%30==0 && !_isnan(phaseMarkerPosition(0)) && !_isnan(phaseMarkerPosition(1)) && !_isnan(phaseMarkerPosition(2))) {
		 printf("distance, marker: %g\n",
		sqrt(
		(phaseMarkerPosition(0) - postTranslation(0))*(phaseMarkerPosition(0) - postTranslation(0))+
		(phaseMarkerPosition(1) - postTranslation(1))* (phaseMarkerPosition(1) - postTranslation(1))+
		(phaseMarkerPosition(2) - postTranslation(2))*(phaseMarkerPosition(2) - postTranslation(2))));
		if(!hasNan(sceneCamCur)) printf("distance betwixt: %g\n",
			sqrt(
			(phaseMarkerPosition(0)-sceneCamCur(0,3))*(phaseMarkerPosition(0)-sceneCamCur(0,3))+
			(phaseMarkerPosition(1)-sceneCamCur(1,3))*(phaseMarkerPosition(1)-sceneCamCur(1,3))+
			(phaseMarkerPosition(2)-sceneCamCur(2,3))*(phaseMarkerPosition(2)-sceneCamCur(2,3))));
		}
		if(q % 30 ==0) printf("display marker x: %g y: %g z: %g\n", phaseMarkerPosition(0), phaseMarkerPosition(1), phaseMarkerPosition(2));
		if(q % 30 ==0) printf("scene camera x: %g y: %g z: %g\n", sceneCamCur(0,3), sceneCamCur(1,3), sceneCamCur(2,3));

		if(q %30 == 0) print("sceneCamCur", sceneCamCur);

	}
		 q++;

	delete2D(cameraM, 3);
	delete1D(distanceC);
	delete2D(rMat,3);
	delete1D(tVec);

	delete2D(coords, markerXsTodo.size());
	delete2D(data, xs.size());
	}




void __fastcall TForm4::CalculateCalibrationBtnClick(TObject *Sender) {
	scaleToPhysical();
	eyeCalibration();
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
		nodeElement->Attributes["distToTarget"] = UnicodeString(distToTarget);

		nodeElement = Form4->xdoc_out->DocumentElement->AddChild("SceneCalibration", -1);
		nodeElement->Attributes["cameraMatrix"] = matrixToUnicodeString(sceneCameraMatrix);
		nodeElement->Attributes["distortionCoeffs"] = vectorToUnicodeString(sceneDistortionCoeffs);

		nodeElement = Form4->xdoc_out->DocumentElement->AddChild("Scaling", -1);
		nodeElement->Attributes["monitorWidth"] = UnicodeString(monitorWidth);
		nodeElement->Attributes["monitorHeight"] = UnicodeString(monitorHeight);
		nodeElement->Attributes["eyeCameraWidth"] = UnicodeString(cameraWidth);
		nodeElement->Attributes["eyeCameraHeight"] = UnicodeString(cameraHeight);

		nodeElement = Form4->xdoc_out->DocumentElement->AddChild("ReferencePositions", -1);
		nodeElement->Attributes["head"] = matrixToUnicodeString(headRef);
		ublas::vector<int> headMarkers(4);
		headMarkers(0) = HeadMarker0Edit->Text.ToInt();
		headMarkers(1) = HeadMarker1Edit->Text.ToInt();
		headMarkers(2) = HeadMarker2Edit->Text.ToInt();
		headMarkers(3) = HeadMarker3Edit->Text.ToInt();
		nodeElement->Attributes["headMarkers"] = vectorToUnicodeString(headMarkers);
		nodeElement->Attributes["display"] = matrixToUnicodeString(displayRef);
        nodeElement->Attributes["sceneCamera"] = matrixToUnicodeString(sceneCamCur);

		Form4->xdoc_out->SaveToFile(SaveDialog1->FileName);
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm4::LoadCalibrationPointsBtnClick(TObject *Sender)
{
	if(OpenDialog1->Execute()) {
		ClearCalibrationPointsBtnClick(this);

		int nPoints = 0;

		markerIndices.clear();
		markerXs.clear();
		markerYs.clear();
		meanEyeXs.clear();
		meanEyeYs.clear();
		stddevEyeXs.clear();
		stddevEyeYs.clear();
		meanSceneXs.clear();
		meanSceneYs.clear();
		meanSceneZs.clear();
		stddevSceneXs.clear();
		stddevSceneYs.clear();
		stddevSceneZs.clear();

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
			markerIndices.push_back(atof(value.c_str()));

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
			meanSceneZs.push_back(atof(value.c_str()));

			getline(ifs, value, ',');
			stddevSceneXs.push_back(atof(value.c_str()));

			getline(ifs, value, ',');
			stddevSceneYs.push_back(atof(value.c_str()));

			getline(ifs, value);
			stddevSceneZs.push_back(atof(value.c_str()));
			printf("%g %g %g %g %g %g %g %g %g %g %g %g %g\n\n", markerIndices[i], markerXs[i], markerYs[i], meanEyeXs[i], meanEyeYs[i], stddevEyeXs[i], stddevEyeYs[i], meanSceneXs[i], meanSceneYs[i], meanSceneZs[i], stddevSceneXs[i], stddevSceneYs[i], stddevSceneZs[i]);
		}

		ifs.close();
		scaleToPhysical();
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
	scaledMarkerZs.clear();
	scaledMeanEyeXs.clear();
	scaledMeanEyeYs.clear();
	scaledStddevEyeXs.clear();
	scaledStddevEyeYs.clear();
	for(unsigned int i=0; i<markerXs.size(); i++) {

	 //	scaledMarkerXs.push_back(monitorWidth*((producerWidth - markerXs[i])/producerWidth));  //to mm
	 //	scaledMarkerYs.push_back(monitorHeight*((producerHeight - markerYs[i])/producerHeight)); // to mm
		scaledMarkerXs.push_back(meanSceneZs[i]); //covert from monitor centric to eye centric x,y,z still mm, still centered at zero
		scaledMarkerYs.push_back(-meanSceneXs[i]); //covert from monitor centric to eye centric x,y,z still mm, still centered at zero
		scaledMarkerZs.push_back(meanSceneYs[i]); //covert from monitor centric to eye centric x,y,z still mm, still centered at zero
		scaledMeanEyeXs.push_back(meanEyeXs[i]-(cameraWidth-1)/2.0); //to camera pixels, centered at zero
		scaledMeanEyeYs.push_back((cameraHeight-1)/2.0 - meanEyeYs[i]); //to camera pixels, centered at zero
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


	minMax(meanSceneYs, &minY, &maxY);




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

//sets defaults only
void setCameraParams() {
	sceneCameraMatrix(0,0) = 557;//654.8611202347574;
	sceneCameraMatrix(0,1) = 0;
	sceneCameraMatrix(0,2) = (640.0 - 1.0)/2.0;//319.5;
	sceneCameraMatrix(1,0) = 0;
	sceneCameraMatrix(1,1) = 557;//654.8611202347574;
	sceneCameraMatrix(1,2) = (480.0 -1.0)/2.0;//239.5;
	sceneCameraMatrix(2,0) = 0;
	sceneCameraMatrix(2,1) = 0;
	sceneCameraMatrix(2,2) = 1;

	sceneDistortionCoeffs[0] = -0.3669624087278113;
	sceneDistortionCoeffs[1] = -0.07638290902180184;
	sceneDistortionCoeffs[2] = 0;
	sceneDistortionCoeffs[3] = 0;
	sceneDistortionCoeffs[4] = 0.5764668364450144;


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
	HeadMarker0EditChange(this);
	HeadMarker1EditChange(this);
	HeadMarker2EditChange(this);
	HeadMarker3EditChange(this);
    phasespaceMarker0EditChange(this);
	phasespaceMarker1EditChange(this);
	BackgroundRedEditChange(this);
	BackgroundGreenEditChange(this);
	BackgroundBlueEditChange(this);



	Form4->Caption = UnicodeString("Eye Calibrator, version ") + getVersion();

	setCameraParams();
	RefreshStreamsButtonClick(this);


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




void __fastcall TForm4::SaveCalibrationPointsBtnClick(TObject *Sender)
{
	if(SaveDialog2->Execute()) {
		FILE *fp = fopen(AnsiString(SaveDialog2->FileName).c_str(), "w");
		if(fp) {
			fprintf(fp, "markerIndices,markerXs,markerYs,meanEyeXs,meanEyeYs,stddevEyeXs,stddevEyeYs,meanSceneXs,meanSceneYs,meanSceneZs,stddevSceneXs,stddevSceneYs,stddevSceneZs\n");
			for(unsigned int i=0; i < markerXs.size(); ++i) {
				fprintf(fp, "%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g,%g\n", markerIndices[i],markerXs[i], markerYs[i],
					meanEyeXs[i], meanEyeYs[i], stddevEyeXs[i], stddevEyeYs[i],
					meanSceneXs[i], meanSceneYs[i], meanSceneZs[i],stddevSceneXs[i], stddevSceneYs[i],stddevSceneZs[i]);
			}
			fclose(fp);
		}
	}
}
//---------------------------------------------------------------------------

void __fastcall TForm4::ClearCalibrationPointsBtnClick(TObject *Sender)
{
	markerIndices.clear();
	markerXs.clear();
	markerYs.clear();
	meanEyeXs.clear();
	meanEyeYs.clear();
	stddevEyeXs.clear();
	stddevEyeYs.clear();
	meanSceneXs.clear();
	meanSceneYs.clear();
	meanSceneZs.clear();
	stddevSceneXs.clear();
	stddevSceneYs.clear();
	stddevSceneZs.clear();
	modeledXs.clear();
	modeledYs.clear();
	drawCalibration();
	Timer3->Enabled == false;
}
//---------------------------------------------------------------------------




void __fastcall TForm4::FormClose(TObject *Sender, TCloseAction &Action)
{
	if(gazeInlet) lsl_destroy_inlet(gazeInlet);
	if(sceneInlet) lsl_destroy_inlet(sceneInlet);
	if(phasespaceInlet) lsl_destroy_inlet(phasespaceInlet);




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
	for(int i=0; i<nSceneCamPoints; i++) {
		sceneCamXsList[i].clear();
		sceneCamYsList[i].clear();
		sceneCamZsList[i].clear();
	}
	timerMode = isHead;
	headCount = 0;

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

	if(sceneInlet) lsl_destroy_inlet(sceneInlet);

	lsl_streaminfo info;
	lsl_resolve_byprop(&info,1, "name", ((AnsiString) SceneComboBox->Text).c_str(), 1,1.0);
	sceneInlet = lsl_create_inlet(info, 300, LSL_NO_PREFERENCE,1);

	sceneChannels = lsl_get_channel_count(info);
}
//---------------------------------------------------------------------------



void __fastcall TForm4::PhaseComboBoxChange(TObject *Sender)
{

	if(phasespaceInlet) lsl_destroy_inlet(phasespaceInlet);

	lsl_streaminfo info;
	lsl_resolve_byprop(&info,1, "name", ((AnsiString) PhaseComboBox->Text).c_str(), 1,1.0);
	phasespaceInlet = lsl_create_inlet(info, 300, LSL_NO_PREFERENCE,1);

	phasespaceChannels = lsl_get_channel_count(info);
}
//---------------------------------------------------------------------------


 LRESULT CALLBACK WindowPrc(HWND hWnd, UINT Msg,
							WPARAM wParam, LPARAM lParam)
{
	switch(Msg)
	{
	case WM_DESTROY:
	 //   PostQuitMessage(0); Exit app on window close.
		return 0;
	case WM_ERASEBKGND:
		Form4->UpdateMarkers();

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

  //	monitorDrawers.push_back(new MonitorDrawer(hWnd,makecol(backgroundRed,backgroundGreen,backgroundBlue), 1));


   return TRUE;
}

	MonitorDrawer *createMonitorDrawer(  long x, long y, long width, long height, double monitorWidthMM, double monitorHeightMM)  {
			HMODULE hInstance = GetModuleHandle(NULL);
		 if(!hInstance) printf("no hInstance\n");

			 // The variable that will define the window
		WNDCLASSEX  WndClsEx;
		// The window's name
		static char szAppName[] = "FirstClass";

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
						WS_EX_OVERLAPPEDWINDOW | WS_EX_TOPMOST,
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

		return new MonitorDrawer(hWnd,makecol(backgroundRed,backgroundGreen,backgroundBlue), monitorWidthMM, monitorHeightMM);
	}


void TForm4::LoadHotspotsConfig(const System::UnicodeString FileName)
{
	for(std::map<int, THotspotScreen*>::iterator itr=hotspotScreens.begin();itr !=hotspotScreens.end();)
	{
		delete itr->second;
		hotspotScreens.erase(itr++); //itr, post erase is not defined in old C++.  It is defined in C++0X.
	}

	std::map<int, TPoint3D*> p3Ds;
	Form4->xdoc_out->LoadFromFile(FileName);


	_di_IXMLNode nodeElement =
		Form4->xdoc_out->ChildNodes->FindNode("Configuration")->ChildNodes->FindNode("Locations");

	if (nodeElement != NULL) {

		for (int i = 0; i < nodeElement->ChildNodes->Count; i++) {
			const _di_IXMLNode node = nodeElement->ChildNodes->Get(i);

			int id = node->Attributes["id"];
			p3Ds[id] =  new TPoint3D(node->Attributes["x"],node->Attributes["y"],node->Attributes["z"], node->Attributes["id"]);

		}
	} //else {
	 //	Application->MessageBoxA(L"Unable to find Locations group.", L"Error", MB_OK);
   //	}
   	//if not head-tracking
	if(p3Ds.size() == 0) {
		nodeElement = Form4->xdoc_out->ChildNodes->FindNode("Configuration")->ChildNodes->FindNode("Devices");
	} else {
		nodeElement = Form4->xdoc_out->ChildNodes->FindNode("Configuration")->ChildNodes->FindNode("Hotspots");
	}

	if (nodeElement != NULL) {
		for(std::map<int, MonitorDrawer*>::iterator iterator = monitorDrawers.begin(); iterator != monitorDrawers.end();) {
			MonitorDrawer *monitorDrawer = iterator->second;
			if(monitorDrawer->visible) monitorDrawer->setVisible(SW_HIDE);
			delete iterator->second;
			monitorDrawers.erase(iterator++);

		}

		for (int i = 0; i < nodeElement->ChildNodes->Count; i++) {
			const _di_IXMLNode node = nodeElement->ChildNodes->Get(i);

			 if (node->NodeName == UnicodeString("Screen")) {
				int device = node->Attributes["device"];
				int x = node->Attributes["x"];
				int y = node->Attributes["y"];
				int width = node->Attributes["width"];
				int height = node->Attributes["height"];

				if(p3Ds.size() == 0) {
					monitorWidth= node->Attributes["width_mm"];
					MonitorWidthEdit->Text = monitorWidth;
					monitorHeight = node->Attributes["height_mm"];
					MonitorHeightEdit->Text = monitorHeight;
				}
				//if in phasespace headtracking mode.
				if(p3Ds.size() !=0) {
					int topLeft = node->Attributes["topLeft"];
					int topRight = node->Attributes["topRight"];
					int bottomLeft = node->Attributes["bottomLeft"];
					int bottomRight = node->Attributes["bottomRight"];
					int sensor0 = node->Attributes["sensor0"];
					int sensor1 = node->Attributes["sensor1"];
					double monitorDepth = node->Attributes["monitorDepth"];
					hotspotScreens[device] = new THotspotScreen(
						p3Ds, topLeft, topRight, bottomLeft, bottomRight,
						sensor0, sensor1, device, monitorDepth, x,y,width,height);
					monitorWidth = hotspotScreens[device]->monitorWidth;
					monitorHeight = hotspotScreens[device]->monitorHeight;
				}
				monitorDrawers[device] = createMonitorDrawer(x,y,width,height,monitorWidth,monitorHeight);
				StatusMemo->Lines->Add(UnicodeString("Device ") + UnicodeString(device) + UnicodeString(" measurements loaded."));
			}

		}
	} else {
			Application->MessageBoxA(L"Unable to find Hotspots group.", L"Error", MB_OK);
	}
}


/**************************************************************************

 Begin marker control code.

***************************************************************************/



int timerCount = 0;
void __fastcall TForm4::FormKeyPress(TObject *Sender, wchar_t &Key)
{
	if(currentSpot >= 0) {
		if(Timer3->Enabled == false) {
			switch(Key)
			{
			case ' ':
				timerCount = 0;
				Timer3->Enabled = true;
				break;
			case 'b':

				currentSpotComplete--;
				if(currentSpotComplete < 0) currentSpotComplete = 0;
				markerX = -1.0;
				markerY = -1.0;
				UpdateMarkers();
				break;
			case VK_ESCAPE:
				AbortCalibrationButtonClick(this);
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





THotspotScreen * TForm4::getHotspotScreen(int deviceNumber) {
	for(std::map<int, THotspotScreen*>::iterator itr=hotspotScreens.begin();itr !=hotspotScreens.end();++itr)
	{
		if(itr->first == deviceNumber) {
			return itr->second;
		}
	}
	return NULL;
}

void __fastcall TForm4::UpdateMarkers() {
   		markerXsTodo.clear();
		markerYsTodo.clear();
		currentSpot = 0;
		bool currentSpotFound = false;
		for(int i=0; i<markerXsTodoComplete.size(); i++) {
			double x = markerXsTodoComplete[i];
			double y = markerYsTodoComplete[i];
			int device = devicesTodo[i];
			if(i==currentSpotComplete) {
				monitorDrawers[device]->drawMarkers(x,y,1); //active
				markerXsTodo.push_back(x);
				markerYsTodo.push_back(y);
				currentSpotFound = true;
			} else if(device == devicesTodo[currentSpotComplete]){
				monitorDrawers[device]->drawMarkers(x,y,0); //inactive
				markerXsTodo.push_back(x);
				markerYsTodo.push_back(y);
				if(!currentSpotFound) currentSpot++;
			} else {
				monitorDrawers[device]->drawMarkers(x,y,-1); //invisible, different screen
			}
		}

		monitorWidth = monitorDrawers[devicesTodo[currentSpot]]->monitorWidthMM;
		MonitorWidthEdit->Text = monitorWidth;
		monitorHeight = monitorDrawers[devicesTodo[currentSpot]]->monitorHeightMM;
		MonitorHeightEdit->Text = monitorHeight;

		if(phasespaceInlet) {
			THotspotScreen * hs = getHotspotScreen(devicesTodo[currentSpot]);
		
			for(int i=0; i<3; i++) {
				displayRef(i,0) = hs->monitorTL(i);
				displayRef(i,1) = hs->monitorTR(i);
				displayRef(i,2) = hs->monitorBL(i);
				displayRef(i,3) = hs->monitorBR(i);
			}
		//	StatusMemo->Lines->Add("Monitor measurement updated.");
		} else {


		}
		 //	printf("%d %g %g\n\n", devicesTodo[i], markerXsTodo[i], markerYsTodo[i]);


}
void __fastcall TForm4::CalibrationWindowButtonClick(TObject *Sender)
{
	currentSpot = 0;
	currentSpotComplete = 0;
	markerX = -1;
	markerY = -1;


 //	EnumDisplayMonitors(NULL, NULL, MonitorEnumProc, 0);
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

		devicesTodo.clear();
		markerXsTodoComplete.clear();
		markerYsTodoComplete.clear();
		markerXsTodo.clear();
		markerYsTodo.clear();
		ifs.clear();
		ifs.seekg(0);
		getline(ifs,value);  //get header line.

		if(strcmp("MonitorIndex,MarkerX,MarkerY", value.c_str())) {
			Application->MessageBoxA(L"Calibration Window File should start with: MonitorIndex,MarkerX,MarkerY.", L"Error", MB_OK);
			return;
		}

		for(int i=0; i<nPoints; i++) {
			getline(ifs, value, ',');
			int device = atoi(value.c_str());
			std::map<int, MonitorDrawer*>::iterator itr = monitorDrawers.find(device);
			if(itr==monitorDrawers.end()) {
				Application->MessageBoxA(L"Monitor position file not loaded or monitor position file and calibration window do not agree with respect to monitor numbers.", L"Error", MB_OK);
                return;
			}
			if(!monitorDrawers[device]->visible) monitorDrawers[device]->setVisible(SW_SHOW);
			devicesTodo.push_back(device);

			getline(ifs, value, ',');
			double x = atof(value.c_str());
			markerXsTodoComplete.push_back(x);

			getline(ifs, value);
			double y = atof(value.c_str());
			markerYsTodoComplete.push_back(y);


		 //	printf("%d %g %g\n\n", devicesTodo[i], markerXsTodo[i], markerYsTodo[i]);
		}

		ifs.close();

		UpdateMarkers();

	}
	Form4->SetFocus();
}




void __fastcall TForm4::Timer3Timer(TObject *Sender)
{
	if(timerCount == 0 && currentSpotComplete >= 0 && currentSpotComplete < markerXsTodoComplete.size()) {

		markerX = markerXsTodo[currentSpot];
		markerY = markerYsTodo[currentSpot];

	}
	timerCount++;
	if(timerCount == 150) {
		timerCount = 0;
		Timer3->Enabled = false;
		markerX = -1.0;
		markerY = -1.0;

	   //	if(currentSpot >= 0 && currentSpot < markerXsTodo.size())
	   //		monitorDrawers[devicesTodo[currentSpot]]->drawMarkers(markerXsTodo[currentSpot],markerYsTodo[currentSpot],false);
		currentSpotComplete++;
		if(currentSpotComplete == (int) markerXsTodoComplete.size()) {
			for(std::map<int, MonitorDrawer*>::iterator iterator = monitorDrawers.begin(); iterator != monitorDrawers.end();++iterator) {
				MonitorDrawer *monitorDrawer = iterator->second;
				if(monitorDrawer->visible) monitorDrawer->setVisible(SW_HIDE);
			 //	delete iterator->second;
			 //	monitorDrawers.erase(iterator++);
			}
			currentSpot = -1;
			currentSpotComplete = -1;
		} else {
			UpdateMarkers();

		}



	}


}

/**************************************************************************

 End marker control code.

***************************************************************************/







void __fastcall TForm4::phasespaceTestStartClick(TObject *Sender)
{
	timerMode = isPhasespaceTest;
}
//---------------------------------------------------------------------------

void __fastcall TForm4::phasespaceTestStopClick(TObject *Sender)
{
	timerMode = isStandard;
}
//---------------------------------------------------------------------------





void __fastcall TForm4::MonitorPositionButtonClick(TObject *Sender)
{
	if (OpenDialog2->Execute())
	{
		LoadHotspotsConfig(OpenDialog2->FileName);
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm4::HeadMarker0EditChange(TObject *Sender)
{
	bool ex = false;
	try {
		HeadMarker0Edit->Text.ToInt();
	} catch (...) {
		ex = true;
	}

	if(!ex)  head0 =  HeadMarker0Edit->Text.ToInt();
}
//---------------------------------------------------------------------------

void __fastcall TForm4::HeadMarker1EditChange(TObject *Sender)
{
 	bool ex = false;
	try {
		HeadMarker1Edit->Text.ToInt();
	} catch (...) {
		ex = true;
	}

	if(!ex)  head1 =  HeadMarker1Edit->Text.ToInt();
}
//---------------------------------------------------------------------------

void __fastcall TForm4::HeadMarker2EditChange(TObject *Sender)
{
 	bool ex = false;
	try {
		HeadMarker2Edit->Text.ToInt();
	} catch (...) {
		ex = true;
	}

	if(!ex)  head2 =  HeadMarker2Edit->Text.ToInt();
}
//---------------------------------------------------------------------------

void __fastcall TForm4::HeadMarker3EditChange(TObject *Sender)
{
	bool ex = false;
	try {
		HeadMarker3Edit->Text.ToInt();
	} catch (...) {
		ex = true;
	}

	if(!ex)  head3 =  HeadMarker3Edit->Text.ToInt();
}
//---------------------------------------------------------------------------

void __fastcall TForm4::phasespaceMarker0EditChange(TObject *Sender)
{

	bool ex = false;
	try {
		phasespaceMarker0Edit->Text.ToInt();
	} catch (...) {
		ex = true;
	}

	if(!ex)  phase0 = phasespaceMarker0Edit->Text.ToInt();
}
//---------------------------------------------------------------------------

void __fastcall TForm4::phasespaceMarker1EditChange(TObject *Sender)
{
 	bool ex = false;
	try {
		phasespaceMarker1Edit->Text.ToInt();
	} catch (...) {
		ex = true;
	}

	if(!ex)  phase1 = phasespaceMarker1Edit->Text.ToInt();
}
//---------------------------------------------------------------------------

void __fastcall TForm4::AbortCalibrationButtonClick(TObject *Sender)
{
	for(std::map<int, MonitorDrawer*>::iterator iterator = monitorDrawers.begin(); iterator != monitorDrawers.end();++iterator) {
		MonitorDrawer *monitorDrawer = iterator->second;
		if(monitorDrawer->visible) monitorDrawer->setVisible(SW_HIDE);
	  //	delete iterator->second;
	  //	monitorDrawers.erase(iterator++);

	}
	currentSpot = -1;
	currentSpotComplete = -1;
	Timer3->Enabled == false;


}
//---------------------------------------------------------------------------

void __fastcall TForm4::LoadIntrinsicButtonClick(TObject *Sender)
{
	if(OpenDialog1->Execute()) {

		Form4->xdoc_in->LoadFromFile(OpenDialog1->FileName);


			_di_IXMLNode node =
				Form4->xdoc_in->ChildNodes->FindNode("Configuration")->ChildNodes->FindNode("SceneCalibration");
			if(node == NULL) {
				Application->MessageBoxA(L"Unable to find SceneCalibration node.", L"Error", MB_OK);
				return;
			}

			sceneCameraMatrix = unicodeStringToMatrix<double>(node->Attributes["cameraMatrix"]);
			sceneDistortionCoeffs = unicodeStringToVector<double>(node->Attributes["distortionCoeffs"]);

			node = Form4->xdoc_in->ChildNodes->FindNode("Configuration")->ChildNodes->FindNode("Scaling");
			if(node == NULL) {
				Application->MessageBoxA(L"Unable to find Scaling node.", L"Error", MB_OK);
				return;
			}
			cameraWidth = node->Attributes["eyeCameraWidth"];
			CameraWidthEdit->Text = cameraWidth;

			cameraHeight = node->Attributes["eyeCameraHeight"];
			CameraHeightEdit->Text = cameraHeight;


	}

}
//---------------------------------------------------------------------------

void __fastcall TForm4::RefreshStreamsButtonClick(TObject *Sender)
{

	lsl_streaminfo infos[MAX_STREAMS];
	int streamsFound = lsl_resolve_all(infos, MAX_STREAMS, 0.1);

	GazeComboBox->Items->Clear();
	SceneComboBox->Items->Clear();
	PhaseComboBox->Items->Clear();
	for (int i = 0; i < streamsFound; i++) {
		GazeComboBox->Items->Append(lsl_get_name(infos[i]));
		SceneComboBox->Items->Append(lsl_get_name(infos[i]));
		PhaseComboBox->Items->Append(lsl_get_name(infos[i]));
	}



}
//---------------------------------------------------------------------------

void __fastcall TForm4::BackgroundRedEditChange(TObject *Sender)
{
	bool ex = false;
	try {
		BackgroundRedEdit->Text.ToInt();
	} catch (...) {
		ex = true;
	}

	if(!ex)  backgroundRed = BackgroundRedEdit->Text.ToInt();
}
//---------------------------------------------------------------------------

void __fastcall TForm4::BackgroundGreenEditChange(TObject *Sender)
{
	bool ex = false;
	try {
		BackgroundGreenEdit->Text.ToInt();
	} catch (...) {
		ex = true;
	}

	if(!ex)  backgroundGreen = BackgroundGreenEdit->Text.ToInt();
}
//---------------------------------------------------------------------------

void __fastcall TForm4::BackgroundBlueEditChange(TObject *Sender)
{
	bool ex = false;
	try {
		BackgroundBlueEdit->Text.ToInt();
	} catch (...) {
		ex = true;
	}

	if(!ex)  backgroundBlue = BackgroundBlueEdit->Text.ToInt();
}
//---------------------------------------------------------------------------



