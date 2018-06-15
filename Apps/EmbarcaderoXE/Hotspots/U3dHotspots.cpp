//---------------------------------------------------------------------------


#include "useallegro4.h"
#include "MonitorDrawer.h"
#pragma hdrstop
#include "U3dHotspots.h"
#include <vcl.h>
#include "UParseLine.h"
#include "GridDialog.h"
#include "ScreenDialog.h"
//---------------------------------------------------------------------------

#include "GazeUtil.h"

#include "stdio.h"
#include "math_util.h"


#pragma package(smart_init)
#pragma resource "*.dfm"

#define CDEPTH 24



TForm11 *Form11;
//---------------------------------------------------------------------------
__fastcall TForm11::TForm11(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

#include "UEdit3D.h"
#include <math.h>
HANDLE handleRd1 = 0;
HANDLE handleWr1 = 0;
HANDLE gazeHandleRd = 0;
HANDLE hotGazeHandleWr = 0;

extern AnsiString par;

std::list<THotspot*> hotspots;
std::list<THotspotGrid*> hotspotGrids;
std::list<THotspotScreen*> hotspotScreens;
std::map<int, TPoint3D*> p3Ds;

std::map<int, MonitorDrawer*> monitorDrawers;

double xOffset, yOffset;

int nId=1;

HANDLE hMutex = 0, hGazeMutex = 0;

int dataChannels = 0;
int channelsPerMarker = 4;
double distanceScaleFactor = 1.0;

enum HS_TYPE {IS_UNDEF,IS_TOUCH,IS_DIR,IS_POINTTO};

//enum for virtual phasespace point, positive numbers are real phasespace points.
enum VIRTUAL_PS {IS_NONE=-1, IS_EYE=-2,IS_GAZE=-3};

lsl_outlet eventOutlet = NULL;
lsl_outlet hotGazeOutlet = NULL;
lsl_outlet dataOutlet = NULL;
#define GAZE_CHANNELS 16
#define MAX_STREAMS 50

TGazeUtil *gu;

void  TForm11::GenerateXMLHeader()
{
	doc.reset();
	pugi::xml_node rootNode = doc.append_child("Configuration");
	pugi::xml_node groupNode = rootNode.append_child("Locations");

	char  s[1024];

	for(std::map<int, TPoint3D*>::iterator itr=p3Ds.begin();itr !=p3Ds.end();++itr)
	{

		pugi::xml_node nodeElement = groupNode.append_child("Location");
		nodeElement.append_attribute("id") = itr->first;
		nodeElement.append_attribute("x") = itr->second->x;
		nodeElement.append_attribute("y") = itr->second->y;
		nodeElement.append_attribute("z") = itr->second->z;

	}

	groupNode = rootNode.append_child("Hotspots");

	for(std::list<THotspot*>::iterator hs = hotspots.begin(); hs != hotspots.end(); ++hs){
		THotspot * pHs = *hs;
		HS_TYPE hsTemp = (HS_TYPE) -1;
		if (pHs->sensor[1] == IS_NONE)  //no "to" sensor
			hsTemp = IS_TOUCH;
		else // is point-to or dist
		{
			if (pHs->dist_mm > 0)
				hsTemp = IS_DIR;
			else
				hsTemp = IS_POINTTO;
		}

		switch (hsTemp)
		{
			case IS_TOUCH:
			{
				pugi::xml_node nodeElement = groupNode.append_child("Touch");
				nodeElement.append_attribute("id") = pHs->name.c_str();
				sprintf(s,"%g",pHs->x);
				nodeElement.append_attribute("x") = s;
				sprintf(s,"%g",pHs->y);
				nodeElement.append_attribute("y") = s;
				sprintf(s,"%g",pHs->z);
				nodeElement.append_attribute("z") = s;
				sprintf(s,"%g",pHs->r);
				nodeElement.append_attribute("size") = s;
				sprintf(s,"%d",pHs->sensor[0]);
				nodeElement.append_attribute("from") = s;
				sprintf(s,"%d",pHs->value);
				nodeElement.append_attribute("device") = s;
			}
			break;

			case IS_POINTTO:
			{
				pugi::xml_node nodeElement = groupNode.append_child("Point_to");
				nodeElement.append_attribute("id") = pHs->name.c_str();
				sprintf(s,"%g",pHs->x);
				nodeElement.append_attribute("x") = s;
				sprintf(s,"%g",pHs->y);
				nodeElement.append_attribute("y") = s;
				sprintf(s,"%g",pHs->z);
				nodeElement.append_attribute("z") = s;
				sprintf(s,"%g",pHs->r);
				nodeElement.append_attribute("size") = s;
				sprintf(s,"%d",pHs->sensor[0]);
				nodeElement.append_attribute("from") = s;
				sprintf(s,"%d",pHs->sensor[1]);
				nodeElement.append_attribute("to") = s;
				sprintf(s,"%d",pHs->value);
				nodeElement.append_attribute("device") = s;

			}
			break;

			case IS_DIR:
			{
				pugi::xml_node nodeElement = groupNode.append_child("Direction");
				nodeElement.append_attribute("id") = pHs->name.c_str();
				sprintf(s,"%g",pHs->x/pHs->dist_mm);
				nodeElement.append_attribute("x") = s;
				sprintf(s,"%g",pHs->y/pHs->dist_mm);
				nodeElement.append_attribute("y") = s;
				sprintf(s,"%g",pHs->z/pHs->dist_mm);
				nodeElement.append_attribute("z") = s;
				sprintf(s,"%g",pHs->r);
				nodeElement.append_attribute("size") = s;
				sprintf(s,"%d",pHs->sensor[0]);
				nodeElement.append_attribute("from") = s;
				sprintf(s,"%d",pHs->sensor[1]);
				nodeElement.append_attribute("to") = s;
				sprintf(s,"%d",pHs->dist_mm);
				nodeElement.append_attribute("dist") = s;
				sprintf(s,"%d",pHs->value);
				nodeElement.append_attribute("device") = s;
			}
			break;
		}

	}



	for(std::list<THotspotGrid*>::iterator hsg = hotspotGrids.begin(); hsg != hotspotGrids.end(); ++hsg){
		THotspotGrid *hotspotGrid = *hsg;
		pugi::xml_node gridElement = groupNode.append_child("Grid");
		sprintf(s,"%d", hotspotGrid->location1);
		gridElement.append_attribute("id1") = s;
		sprintf(s,"%d", hotspotGrid->location2);
		gridElement.append_attribute("id2") = s;
		sprintf(s,"%d", hotspotGrid->location3);
		gridElement.append_attribute("id3") = s;
		sprintf(s,"%d", hotspotGrid->Xdiv);
		gridElement.append_attribute("Xdivisions") = s;
		sprintf(s,"%d", hotspotGrid->Zdiv);
		gridElement.append_attribute("Zdivisions") = s;
		sprintf(s,"%g", hotspotGrid->Xthickness);
		gridElement.append_attribute("Xthickness") = s;
		sprintf(s,"%g", hotspotGrid->Ythickness);
		gridElement.append_attribute("Ythickness") = s;
		sprintf(s,"%g", hotspotGrid->Zthickness);
		gridElement.append_attribute("Zthickness") = s;

		for(std::list<TRectHotspot*>::iterator hs = hotspotGrid->rectHotspots.begin(); hs != hotspotGrid->rectHotspots.end(); ++hs){
			TRectHotspot * pHs = *hs;

			pugi::xml_node nodeElement = gridElement.append_child("Rectangular");
			sprintf(s,"%g",pHs->x);
			nodeElement.append_attribute("x") = s;
			sprintf(s,"%g",pHs->y);
			nodeElement.append_attribute("y") = s;
			sprintf(s,"%g",pHs->z);
			nodeElement.append_attribute("z") = s;
			sprintf(s,"%d",pHs->sensor);
			nodeElement.append_attribute("sensor") = s;
			sprintf(s,"%d",pHs->value);
			nodeElement.append_attribute("device") = s;


		}
	}

	for(std::list<THotspotScreen*>::iterator hss = hotspotScreens.begin(); hss != hotspotScreens.end(); ++ hss) {
		THotspotScreen *hotspotScreen = *hss;
		pugi::xml_node gridElement = groupNode.append_child("Screen");
		sprintf(s,"%d", hotspotScreen->topLeftID);
		gridElement.append_attribute("topLeft") = s;
		sprintf(s,"%d", hotspotScreen->topRightID);
		gridElement.append_attribute("topRight") = s;
		sprintf(s,"%d", hotspotScreen->bottomLeftID);
		gridElement.append_attribute("bottomLeft") = s;
		sprintf(s,"%d", hotspotScreen->bottomRightID);
		gridElement.append_attribute("bottomRight") = s;
		sprintf(s,"%d", hotspotScreen->sensor0);
		gridElement.append_attribute("sensor0") = s;
		sprintf(s,"%d", hotspotScreen->sensor1);
		gridElement.append_attribute("sensor1") = s;
		sprintf(s,"%d", hotspotScreen->device);
		gridElement.append_attribute("device") = s;
		sprintf(s,"%g", hotspotScreen->monitorDepth);
		gridElement.append_attribute("monitorDepth") = s;
		sprintf(s,"%d", hotspotScreen->x);
		gridElement.append_attribute("x") = s;
		sprintf(s,"%d", hotspotScreen->y);
		gridElement.append_attribute("y") = s;
		sprintf(s,"%d", hotspotScreen->width);
		gridElement.append_attribute("width") = s;
		sprintf(s,"%d", hotspotScreen->height);
		gridElement.append_attribute("height") = s;

	}

 //			printf("here\n");
 //	 std::cout << "doc:\n";
 //	doc.save(std::cout);
 //
}



void TForm11::LoadConfig(const System::UnicodeString FileName)
{


	Form11->xdoc_out->LoadFromFile(FileName);

	_di_IXMLNode nodeElement =
		Form11->xdoc_out->ChildNodes->FindNode("Configuration")->ChildNodes->FindNode("Locations");

	if (nodeElement != NULL) {

		for (int i = 0; i < nodeElement->ChildNodes->Count; i++) {
			const _di_IXMLNode node = nodeElement->ChildNodes->Get(i);

			TPoint3D *    p3D =  new TPoint3D(node->Attributes["x"],node->Attributes["y"],node->Attributes["z"], node->Attributes["id"]);
			int id = node->Attributes["id"];
			AddHotspot(p3D, id);
			delete p3D;
		}
	} else {
				Application->MessageBoxA(L"Unable to find Locations group.", L"Error", MB_OK);
	}

	nodeElement = Form11->xdoc_out->ChildNodes->FindNode("Configuration")->ChildNodes->FindNode("Hotspots");

	if (nodeElement != NULL) {

		for (int i = 0; i < nodeElement->ChildNodes->Count; i++) {
			const _di_IXMLNode node = nodeElement->ChildNodes->Get(i);

			if(node->NodeName == UnicodeString("Touch")) {
				TPoint3D *    p3D =  new TPoint3D(node->Attributes["x"],node->Attributes["y"],node->Attributes["z"], node->Attributes["id"]);
				int id = node->Attributes["id"];
				nId = (id+1 > nId) ? id+1 : nId;
				addTouch(p3D, node->Attributes["size"], node->Attributes["device"], node->Attributes["from"]);
				delete p3D;
			} else if(node->NodeName == UnicodeString("Point_to")) {
				TPoint3D *    p3D =  new TPoint3D(node->Attributes["x"],node->Attributes["y"],node->Attributes["z"], node->Attributes["id"]);
				int id = node->Attributes["id"];
				nId = (id+1 > nId) ? id+1 : nId;
				addPointTo(p3D, node->Attributes["size"], node->Attributes["device"], node->Attributes["from"], node->Attributes["to"]);
				delete p3D;
			} else if(node->NodeName == UnicodeString("Direction")) {
				TPoint3D *    p3D =  new TPoint3D(node->Attributes["x"],node->Attributes["y"],node->Attributes["z"], node->Attributes["id"]);
				int id = node->Attributes["id"];
				nId = (id+1 > nId) ? id+1 : nId;
				addDirection(p3D, node->Attributes["size"], node->Attributes["device"], node->Attributes["from"], node->Attributes["to"], node->Attributes["dist"]);
				delete p3D;
			} else if(node->NodeName == UnicodeString("Grid")) {
				THotspotGrid *hotspotGrid = new THotspotGrid(
					node->Attributes["id1"],
					node->Attributes["id2"],
					node->Attributes["id3"],
					node->Attributes["Xdivisions"],
					node->Attributes["Zdivisions"],
					node->Attributes["Xthickness"],
					node->Attributes["Ythickness"],
					node->Attributes["Zthickness"]);
				for (int j = 0; j < node->ChildNodes->Count; j++) {
					const _di_IXMLNode gridNode = node->ChildNodes->Get(j);

					if(gridNode->NodeName == UnicodeString("Rectangular")) {
						addRectangular(
							hotspotGrid,
							gridNode->Attributes["x"],
							gridNode->Attributes["y"],
							gridNode->Attributes["z"],
							node->Attributes["Xthickness"],
							node->Attributes["Ythickness"],
							node->Attributes["Zthickness"],
							gridNode->Attributes["sensor"],
							gridNode->Attributes["device"]);
					}

				}

				hotspotGrids.push_back(hotspotGrid);

			} else if (node->NodeName == UnicodeString("Screen")) {

			  addScreen(
				node->Attributes["topLeft"],
				node->Attributes["topRight"],
				node->Attributes["bottomLeft"],
				node->Attributes["bottomRight"],
				node->Attributes["sensor0"],
				node->Attributes["sensor1"],
				node->Attributes["device"],
				node->Attributes["monitorDepth"],
				node->Attributes["x"],
				node->Attributes["y"],
				node->Attributes["width"],
				node->Attributes["height"]);


			} else {
				Application->MessageBoxA((UnicodeString(L"Node: ") + node->NodeName + UnicodeString(" not recognized.")).w_str(), L"Error", MB_OK);
			}

		}
	} else {
			Application->MessageBoxA(L"Unable to find Hotspots group.", L"Error", MB_OK);
	}
}





#include "StreamThread.h"
TStreamThread *phasespaceThread = NULL;
TStreamThread *gazeListenThread = NULL;


#include <XMLDoc.hpp>


void TForm11::UpdateInfo()
{
	GenerateXMLHeader();

	if(eventOutlet) {
		lsl_destroy_outlet(eventOutlet);
		eventOutlet = NULL;
	}

	if(dataOutlet) {
		lsl_destroy_outlet(dataOutlet);
		dataOutlet = NULL;
	}
	lsl_streaminfo eventInfo;
	if(streamIdentifier == NULL) {
		eventInfo = lsl_create_streaminfo("HotspotEvents", "HotspotEvents", 1,0, cft_string,generateGUID());
	} else {
		char * fullStreamName = (AnsiString("HotspotEvents_") + streamIdentifier).c_str();
		eventInfo = lsl_create_streaminfo(fullStreamName, fullStreamName, 1,0, cft_string,generateGUID());
	}
	//see liblsl/C API/lsl_xml_element_c.cpp,  http://pugixml.googlecode.com/svn/tags/release-0.9/docs/manual/modify.html
	//lsl_xml_ptr is binary equivalent to pugi::xml_node_struct*
	pugi::xml_node_struct* desc = (pugi::xml_node_struct*) lsl_get_desc(eventInfo);

	pugi::xml_node descNode = pugi::xml_node(desc);
	descNode.append_copy(doc.document_element());

	eventOutlet = lsl_create_outlet(eventInfo,0,360);

	char *xmlHeader = lsl_get_xml(eventInfo);
 //	printf("here\n%s\n", xmlHeader);

	dataChannels = hotspots.size();

	for(std::list<THotspotGrid*>::iterator hsg = hotspotGrids.begin(); hsg != hotspotGrids.end(); ++hsg){
		THotspotGrid *hotspotGrid = *hsg;
		dataChannels += hotspotGrid->nSquares;
	}
	for(std::list<THotspotScreen*>::iterator hss = hotspotScreens.begin(); hss != hotspotScreens.end(); ++ hss) {
		dataChannels += 2;
	}

	lsl_streaminfo dataInfo;
	if(streamIdentifier == NULL) {
		dataInfo = lsl_create_streaminfo("HotspotData", "HotspotData", dataChannels, (phasespaceThread) ? phasespaceThread->GetResamplingRate() : 0.0, cft_float32, generateGUID());
	} else {
		char * fullStreamName = (AnsiString("HotspotData_") + streamIdentifier).c_str();
		dataInfo = lsl_create_streaminfo(fullStreamName, fullStreamName, dataChannels, (phasespaceThread) ? phasespaceThread->GetResamplingRate() : 0.0, cft_float32, generateGUID());
	}
	dataOutlet = lsl_create_outlet(dataInfo,0,360);


}

int gazeFrame = 0;
ublas::vector<double> gazePoint(3), sceneCameraPoint(3);

void ProcessGazeData(float *data, int size, double samplingRate) {

	gazeFrame++;
	bool pr = (gazeFrame%30 == 0);

  //	TMaxArray gazeArray;
  //	memcpy(&MaxArray, data,size);
	double x0 = data[1];//MaxArray.Data[0]/100000.0;
	double y0 = data[2];//MaxArray.Data[1]/100000.0;
	double radiusA = data[3];//MaxArray.Data[2]/100000.0;
	double radiusB = data[4];//MaxArray.Data[3]/100000.0;
	double angle = data[5];//MaxArray.Data[4]/100000.0;
   //	if(pr) printf("x0: %g\n", x0);
   //	if(pr) printf("y0: %g\n", y0);
	double xMonitor = x0;
	double yMonitor = y0;
	double xScene = NAN;
	double yScene = NAN;

	unsigned int res = WaitForSingleObject(hGazeMutex,INFINITE);
	if (res!=WAIT_OBJECT_0) return;

	__try {
		if(gu->rEye != 0.0 && !_isnan(x0)) {

			gu->inverseEyeMap(&xMonitor,&yMonitor);

//			if(pr) printf("xMonitor: %g\n", xMonitor);
//			if(pr) printf("yMonitor: %g\n", yMonitor);
			xMonitor+=xOffset;
			yMonitor+=yOffset;
			if(pr) print("headCur", gu->headCur);
			if(pr) print("headRef", gu->headRef);
			if(!_isnan(xMonitor) && !_isnan(yMonitor)/* && !hasNan(gu->headCur)*/) {
				//determine the motion of the head
				ublas::vector<double> priorTranslation(3);
				ublas::vector<double> postTranslation(3);

				ublas::matrix<double> rotation(3,3);

				findRigidBody(gu->headRef, gu->headCur, priorTranslation, rotation, postTranslation);
				//if(!hasNan(rotation))  {
	  //				if(pr) print("rotation", rotation);
					 //	if(pr) print("priorTrans", priorTranslation);
					 //	if(pr) print("postTrans", postTranslation);

					 //move scene camera to line up with current head position
					ublas::matrix<double> sceneCameraCur = moveRigidBody(gu->sceneCameraRef, priorTranslation, rotation, postTranslation);
				   if(pr) print("sceneCameraCur", sceneCameraCur);
					sceneCameraPoint = ublas::column(sceneCameraCur,3);

					//use unit vectors to determine current gaze point
					gazePoint =
						(-ublas::column(sceneCameraCur,0) + sceneCameraPoint)*xMonitor +//scene camera xhat * magnitude
						(ublas::column(sceneCameraCur,1) - sceneCameraPoint)*yMonitor +//scene camera yhat * magnitude
						(ublas::column(sceneCameraCur,2) - sceneCameraPoint)*gu->distToTarget +//scene camera zhat * magnitude
						sceneCameraPoint; //scene camera origin
					if(pr) print("sceneCameraPoint", sceneCameraPoint);
					if(pr) print("gazePoint", gazePoint);
		   //		}

			}
		}

		float sample [GAZE_CHANNELS];

		sample[0] = x0;
		sample[1] = y0;
		sample[2] = xMonitor; //mm x distance to target, from scene camera
		sample[3] = yMonitor; //mm y distance to target, from scene camera
		sample[4] = radiusA; //pixels
		sample[5] = radiusB; //pixels
		sample[6] = angle;
		sample[7] = xOffset; //mm x offset in scene camera space
		sample[8] = yOffset; //mm z offset in scene camera space
		sample[9] = sceneCameraPoint(0)/1000.0;  //m
		sample[10] = sceneCameraPoint(1)/1000.0; //m
		sample[11] = sceneCameraPoint(2)/1000.0; //m
		sample[12] = gazePoint(0)/1000.0; //m
		sample[13] = gazePoint(1)/1000.0; //m
		sample[14] = gazePoint(2)/1000.0; //m
		sample[15] = gu->distToTarget;   //mm, assumed distance to target, from scene camera


		lsl_push_sample_ftp(hotGazeOutlet, sample, lsl_local_clock(), 1);

	} __finally {
		ReleaseMutex(hGazeMutex);
	}

//	Form11->GazeProgressBar->Position = ds_TailPos(handleRd1)/40.96;
	Form11->GazeProgressBar->Position = gazeFrame % 125;
}

float *phaseData = NULL; //for reading in UEdit3D.cpp
/**
 * Process phasespace data. Checks to see if any hotspots have been triggered.
 * Sends the event code of the last hotspot found in this sample.
 * Sends event code of zero for no hotspot.
 * Also sends distance between phase space indicicator of interest and hotspot.
 *
 */


void ProcessData(float * data, int nChannels, double samplingRate)
{


	int progress = -1;
	std::list<int> changedNames;
	static int count =0;


	unsigned int res = WaitForSingleObject(hMutex,INFINITE);
	if (res!=WAIT_OBJECT_0) return;

	if(hGazeMutex && gu->initialized) {
		res = WaitForSingleObject(hGazeMutex,INFINITE);
		if (res!=WAIT_OBJECT_0) return;
	}
	__try
	{
		if(phaseData) delete1D(phaseData);
		phaseData = new1D<float>(nChannels, 0.0);
		for(int i=0; i<nChannels; i++) {
			data[i]*=distanceScaleFactor; //to mm

			phaseData[i] = data[i];
		}

		count++;
		if(hGazeMutex && gu->initialized) {
			for(int marker = 0; marker < gu->headMarkers.size(); marker++) {
				for(int dim = 0; dim < gu->headCur.size1(); dim++) {
					gu->headCur(dim,marker) = data[gu->headMarkers(marker)*channelsPerMarker+dim] == 0 ? NAN :data[gu->headMarkers(marker)*channelsPerMarker+dim] ;
				}
			}
		}

		float *buffer = new1D<float>(dataChannels, 0.0);
		int i=0;
		for(std::list<THotspot*>::iterator hs = hotspots.begin(); hs != hotspots.end(); ++hs){
			THotspot * pHs = *hs;
			double x=0.0, y=0.0, z=0.0;
			int ch1 = pHs->sensor[0];
			if(ch1 == IS_NONE) {
				x = 0.0;
				y = 0.0;
				z = 0.0;
			} else if(ch1 == IS_EYE) {
			   x = sceneCameraPoint(0);
			   y = sceneCameraPoint(1);
			   z = sceneCameraPoint(2);
			} else if (ch1 == IS_GAZE) {
			   x = gazePoint(0);
			   y = gazePoint(1);
			   z = gazePoint(2);
			} else {
				x = data[channelsPerMarker*ch1];
				y = data[channelsPerMarker*ch1+1];
				z = data[channelsPerMarker*ch1+2];
			}
			TPoint3D pt1(x,y,z);

			int ch2 = pHs->sensor[1];
			bool isTouch = false;
			if(ch2 == IS_NONE) {
				x = 0.0;
				y = 0.0;
				z = 0.0;
				isTouch = true;  // no "to" sensor
			} else if(ch2 == IS_EYE) {
			   x = sceneCameraPoint(0);
			   y = sceneCameraPoint(1);
			   z = sceneCameraPoint(2);
			} else if (ch2 == IS_GAZE) {
			   x = gazePoint(0);
			   y = gazePoint(1);
			   z = gazePoint(2);
			} else {
				x = data[channelsPerMarker*ch2];
				y = data[channelsPerMarker*ch2+1];
				z = data[channelsPerMarker*ch2+2];
			}
			TPoint3D pt2(x,y,z);

			int code;
			bool in;
			if (isTouch)
			{
				buffer[i] = pHs->Distance(pt1);
				in = pHs->Includes(pt1);
			}
			else // is point-to or dist
			{
				bool isDir = pHs->dist_mm > 0;
				if (isDir)
				{
					int sz = pHs->r;
					TVectorToHotspot PointingTo(pt1,pt2,sz );
					TVectorToHotspot TargetIs(pt1,*pHs,sz);
					buffer[i]=PointingTo.Distance(TargetIs);
					in =   PointingTo.Includes(TargetIs);
				}
				else
				{	// isPointTo:
					int sz = pHs->r;
					TVectorToHotspot PointingTo(pt1,pt2,sz );
					TVectorToHotspot TargetIs(pt1,*pHs,sz);
					double deg = PointingTo.Distance(TargetIs);
					buffer[i]=deg;
					in =   PointingTo.Includes(TargetIs);
				}
			}
			if (in)
			{
				pHs->activeShape->Brush->Color = clGreen;
				code = pHs->value;
			}
			else  {
				pHs->activeShape->Brush->Color = clRed;
				code = -pHs->value;
			}
			pHs->isChanged = pHs->isIn !=in;
			pHs->isIn = in;

			if (pHs->isChanged)
			{
				char *codeStr = (char *) malloc(sizeof(char)*256);

				sprintf(codeStr, "%d", code);

				lsl_push_sample_strtp(eventOutlet, &codeStr, lsl_local_clock(), 1);
				changedNames.push_back(code);
				free(codeStr);

			}

			++i;
		}



		for(std::list<THotspotGrid*>::iterator hsg = hotspotGrids.begin(); hsg != hotspotGrids.end(); ++hsg){
		THotspotGrid *hotspotGrid = *hsg;
			for(std::list<TRectHotspot*>::iterator hs = hotspotGrid->rectHotspots.begin(); hs != hotspotGrid->rectHotspots.end(); ++hs){
				TRectHotspot * pHs = *hs;
				double x=0.0, y=0.0, z=0.0;
				int ch1 = pHs->sensor;
				if(ch1 == IS_NONE) {
					x = 0.0;
					y = 0.0;
					z = 0.0;
				} else if(ch1 == IS_EYE) {
				   x = sceneCameraPoint(0);
				   y = sceneCameraPoint(1);
				   z = sceneCameraPoint(2);
				} else if (ch1 == IS_GAZE) {
				   x = gazePoint(0);
				   y = gazePoint(1);
				   z = gazePoint(2);
				} else {
					x = data[channelsPerMarker*ch1];
					y = data[channelsPerMarker*ch1+1];
					z = data[channelsPerMarker*ch1+2];
				}
				TPoint3D pt3D(x,y,z);

				int code;
				bool in;

				buffer[i] = pHs->Distance(pt3D);
				in = pHs->Includes(pt3D);

				if (in)
				{
					pHs->activeShape->Brush->Color = clGreen;
					code = pHs->value;
				}
				else  {
					pHs->activeShape->Brush->Color = clRed;
					code = -pHs->value;
				}
				pHs->isChanged = pHs->isIn !=in;
				pHs->isIn = in;

				if (pHs->isChanged)
				{

					char *codeStr = (char *) malloc(sizeof(char)*256);
					sprintf(codeStr, "%d", code);

					lsl_push_sample_strtp(eventOutlet, &codeStr, lsl_local_clock(), 1);
					changedNames.push_back(code);
					free(codeStr);
				}
				++i;
			}

		}


		for(std::list<THotspotScreen*>::iterator hss = hotspotScreens.begin(); hss != hotspotScreens.end(); ++ hss) {
			THotspotScreen *hotspotScreen = *hss;

			int ch1 = hotspotScreen->sensor0;
			int ch2 = hotspotScreen->sensor1;
			ublas::vector<double> ch1vect(3), ch2vect(3);
			if(ch1 == IS_NONE) {
				ch1vect(0) = NAN;
				ch1vect(1) = NAN;
				ch1vect(2) = NAN;
			} else if(ch1 == IS_EYE) {
				if(sceneCameraPoint(0) == 0) {
					ch1vect(0) = NAN;
					ch1vect(1) = NAN;
					ch1vect(2) = NAN;
				} else {
					ch1vect = sceneCameraPoint;
				}

			} else if (ch1 == IS_GAZE) {
				if(gazePoint(0) == 0) {
					ch1vect(0) = NAN;
					ch1vect(1) = NAN;
					ch1vect(2) = NAN;
				} else {
					ch1vect = gazePoint;
				}
			} else {
				ch1vect(0) = data[channelsPerMarker*ch1] == 0 ? NAN : data[channelsPerMarker*ch1];
				ch1vect(1) = data[channelsPerMarker*ch1+1] == 0 ? NAN : data[channelsPerMarker*ch1+1];
				ch1vect(2) = data[channelsPerMarker*ch1+2] == 0 ? NAN : data[channelsPerMarker*ch1+2];
			}

			if(ch2 == IS_NONE) {
				ch2vect(0) = NAN;
				ch2vect(1) = NAN;
				ch2vect(2) = NAN;
			} else if(ch2 == IS_EYE) {

				if(sceneCameraPoint(0) == 0) {
					ch2vect(0) = NAN;
					ch2vect(1) = NAN;
					ch2vect(2) = NAN;
				} else {
					ch2vect = sceneCameraPoint;
				}

			} else if (ch2 == IS_GAZE) {
				if(gazePoint(0) == 0) {
					ch2vect(0) = NAN;
					ch2vect(1) = NAN;
					ch2vect(2) = NAN;
				} else {
					ch2vect = gazePoint;
				}

			} else {
				ch2vect(0) = data[channelsPerMarker*ch2] == 0 ? NAN : data[channelsPerMarker*ch2];
				ch2vect(1) = data[channelsPerMarker*ch2+1] == 0 ? NAN : data[channelsPerMarker*ch2+1];
				ch2vect(2) = data[channelsPerMarker*ch2+2] == 0 ? NAN : data[channelsPerMarker*ch2+2];
			}

					  static int count = 0;
  bool pr = (count%100 == 0);

  count++;

	   if(pr) print("fromPos", ch1vect);
	   if(pr) print("targetPos", ch2vect);

			double monitorPosX = 0, monitorPosY = 0;
			hotspotScreen->pointToScreen(ch1vect, ch2vect, &monitorPosX, &monitorPosY);

			monitorDrawers[hotspotScreen->device]->drawMarkerSmooth( monitorPosX, monitorPosY);

			buffer[i++] = monitorPosX;
			buffer[i++] = monitorPosY;

			bool in = hotspotScreen->Includes(monitorPosX, monitorPosY);

			int code = 0;
			if (in)
			{
				hotspotScreen->activeShape->Brush->Color = clGreen;
				code = hotspotScreen->device;
			}
			else  {
				hotspotScreen->activeShape->Brush->Color = clRed;
				code = -hotspotScreen->device;
			}
			hotspotScreen->isChanged = hotspotScreen->isIn !=in;
			hotspotScreen->isIn = in;

			if (hotspotScreen->isChanged)
			{

				char *codeStr = (char *) malloc(sizeof(char)*256);

				sprintf(codeStr, "%d", code);

				lsl_push_sample_strtp(eventOutlet, &codeStr, lsl_local_clock(), 1);
				changedNames.push_back(code);
				free(codeStr);

			}


		}

		lsl_push_sample_ftp(dataOutlet,buffer, lsl_local_clock(), 1);
		delete1D(buffer);
		progress =  count % 1250;

	}

	__finally
	{
		if(hGazeMutex && gu->initialized) {
			ReleaseMutex(hGazeMutex);
		}
		ReleaseMutex(hMutex);
	}

 	if(progress >= 0) Form11->ProgressBar1->Position = progress;

	for(std::list<int>::iterator i=changedNames.begin(); i != changedNames.end(); i++)
		Form11->Memo1->Lines->Add(*i) ;



}


static bool justFocused = false;
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
		for(std::map<int, MonitorDrawer*>::iterator iterator = monitorDrawers.begin(); iterator != monitorDrawers.end(); ++ iterator) {
			MonitorDrawer *monitorDrawer = iterator->second;
			monitorDrawer->blackenWindow();
		}



		return 0;
	case WM_KILLFOCUS:
		for(std::map<int, MonitorDrawer*>::iterator iterator = monitorDrawers.begin(); iterator != monitorDrawers.end(); ++ iterator) {
			//if window is a different monitordrawer, do not restore task bar
			if(iterator->second->hwnd == GetForegroundWindow()) {
				return 0;
			}
		}
		//otherwise restore task bar
		for(std::map<int, MonitorDrawer*>::iterator iterator = monitorDrawers.begin(); iterator != monitorDrawers.end(); ++ iterator) {
			SetWindowPos(iterator->second->hwnd,HWND_NOTOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		}
		return 0;

	case WM_SETFOCUS:
		//remove task bar
		for(std::map<int, MonitorDrawer*>::iterator iterator = monitorDrawers.begin(); iterator != monitorDrawers.end(); ++ iterator) {
			SetWindowPos(iterator->second->hwnd,HWND_TOPMOST,0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
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
//Uses a different numbering than control panel
//http://social.msdn.microsoft.com/Forums/en-US/windowsgeneraldevelopmentissues/thread/668e3cf9-4e00-4b40-a6f8-c7d2fc1afd39
//Does not appear to be correctable, in general.
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
	 if(!hInstance) printf("no hInstance\n");

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

 //	monitorDrawers.push_back(new MonitorDrawer(hWnd));


   return TRUE;
}


	MonitorDrawer *createMonitorDrawer(  long x, long y, long width, long height)  {
			HMODULE hInstance = GetModuleHandle(NULL);
		 if(!hInstance) printf("no hInstance\n");

			 // The variable that will define the window
		WNDCLASSEX  WndClsEx;
		// The window's name
		static char szAppName[] = "FirstClass";

		// Filling out the structure that builds the window
		WndClsEx.cbSize = sizeof(WndClsEx);
		WndClsEx.style = CS_OWNDC | CS_VREDRAW | CS_HREDRAW;
		WndClsEx.lpfnWndProc = &WindowPrc;
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
						WS_POPUP /* | WS_VISIBLE */ ,
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

		return new MonitorDrawer(hWnd);
	}



	   int oldMarkerX;
	   int oldMarkerY;


//#include "drlib.h"
void __fastcall TForm11::FormCreate(TObject *Sender)
{
	streamIdentifier = NULL;
  	Form11->Caption = UnicodeString("Hotspots, version ") + getVersion();
	allegro_init();
	set_gdi_color_format();
	set_color_depth(CDEPTH);




 //	InitDRlib();

   //	hMutex = CreateMutex(0,false,0);

	DISPLAY_DEVICE dd;
	dd.cb = sizeof(dd);
	DWORD dev = 0;

	// monitor number, as used by Display Properties > Settings

/*	while (EnumDisplayDevices(0, dev, &dd, 0))
	{

			if(dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) {
				monitors.push_back(new MonitorDrawer(dd));

			}


//			DISPLAY_DEVICE ddMon;
//			ZeroMemory(&ddMon, sizeof(ddMon));
//			ddMon.cb = sizeof(ddMon);
//
//			DWORD devMon = 0;
//			while (EnumDisplayDevices(dd.DeviceName, devMon, &ddMon, 0))
//			{
//					if (ddMon.StateFlags & DISPLAY_DEVICE_ACTIVE &&
//						 !(ddMon.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
//					{
//				 //	printf("%s\n", ddMon.DeviceName);
//				 //	printf("%s\n", ddMon.DeviceString);
//				 //	printf("%s\n", ddMon.DeviceKey);
//				 //printf("%s\n", ddMon.DeviceID);
//
//					}
//					devMon++;
//
//					ZeroMemory(&ddMon, sizeof(ddMon));
//					ddMon.cb = sizeof(ddMon);
//			}

			ZeroMemory(&dd, sizeof(dd));
			dd.cb = sizeof(dd);
			dev++;
	} */
	EnumDisplayMonitors(NULL,NULL,MonitorEnumProc,0);


	PageControl1->ActivePageIndex=0;
	PageControl1->Visible = true;





  //	lsl_streaminfo eventInfo = lsl_create_streaminfo("HotspotEvents", "HotspotEvents", 1,0, cft_int32,generateGUID());
  //	lsl_xml_ptr desc = lsl_get_desc(eventInfo);
  //	eventOutlet = lsl_create_outlet(eventInfo,0,360);

	UpdateInfo();

	if(gu) {
		delete gu;
		gu = 0;
	}
	gu = new TGazeUtil();
   hMutex = CreateMutex(0,false,0);
   hGazeMutex = CreateMutex(0,false,0);
   xOffsetEditChange(this);
   yOffsetEditChange(this);
   RefreshStreamsButtonClick(this);

}







void TForm11::clear() {
	for(std::map<int, TPoint3D*>::iterator itr=p3Ds.begin();itr !=p3Ds.end();)
	{
		delete itr->second;
		p3Ds.erase(itr++); //itr, post erase is not defined in old C++.  It is defined in C++0X.
	}

	for(std::map<int, MonitorDrawer*>::iterator itr=monitorDrawers.begin();itr !=monitorDrawers.end();)
	{
		delete itr->second;
		monitorDrawers.erase(itr++); //itr, post erase is not defined in old C++.  It is defined in C++0X.
	}
	while(!hotspots.empty()) {
		delete hotspots.back();
		hotspots.pop_back();
	}

	while(!hotspotGrids.empty()) {
		delete hotspotGrids.back();
		hotspotGrids.pop_back();
	}

	while(!hotspotScreens.empty()) {
		delete hotspotScreens.back();
		hotspotScreens.pop_back();
	}

	while(TrashPanel->ControlCount > 0) {
		TControl* cont = TrashPanel->Controls[0];
	  //    TrashPanel->RemoveComponent(cont);  //Grid Panels need this, straight panels don't.
		TrashPanel->RemoveControl(cont);
		delete cont;
	}

	while(GridPanel1->ControlCount > 10) {
		TControl* cont = GridPanel1->Controls[10];
		GridPanel1->RemoveComponent(cont);
		GridPanel1->RemoveControl(cont);
		delete cont;
	}
	GridPanel1->Height = GridPanel1->Controls[1]->Height*2;

	while(GridPanel2->ControlCount > 5) {
		TControl* cont = GridPanel2->Controls[5];
		GridPanel2->RemoveComponent(cont);
		GridPanel2->RemoveControl(cont);
		delete cont;
	}
	GridPanel2->Height = GridPanel2->Controls[1]->Height*2;

	while(GridPanel3->ControlCount > 6) {
		TControl* cont = GridPanel3->Controls[6];
		GridPanel3->RemoveComponent(cont);
		GridPanel3->RemoveControl(cont);
		delete cont;
	}
	GridPanel3->Height = GridPanel3->Controls[1]->Height*2;

	while(GridPanel4->ControlCount > 6) {
		TControl* cont = GridPanel4->Controls[6];
		GridPanel4->RemoveComponent(cont);
		GridPanel4->RemoveControl(cont);
		delete cont;
	}
	GridPanel4->Height = GridPanel4->Controls[1]->Height*2;

   while(GridPanel5->ControlCount > 9) {
	//	TControl* cont = GridPanel5->Controls[9];
	TControl* cont = GridPanel5->Controls[GridPanel5->ControlCount-1];
		GridPanel5->RemoveComponent(cont);
		GridPanel5->RemoveControl(cont);
		delete cont;
	}
	GridPanel5->Height = GridPanel5->Controls[1]->Height*2;


	while(GridPanel6->ControlCount > 8) {
		TControl* cont = GridPanel6->Controls[GridPanel6->ControlCount-1];
		GridPanel6->RemoveComponent(cont);
		GridPanel6->RemoveControl(cont);
		delete cont;
	}
	GridPanel6->Height = GridPanel6->Controls[1]->Height*2;
}


void TForm11::addTouch(TPoint3D * p3D, int size, int device, int sensor) {

		THotspot * pHs = new THotspot(
				p3D->x,
				p3D->y,
				p3D->z,
				p3D->name,
				size,
				device,
				sensor
				);


		static int buttonNumber = 0;

		TShape * pSh = new TShape(GridPanel2);
		pSh->Name = "Tsh" + p3D->name + "_" + buttonNumber;
  //		pSh->p[0] = pHs;
		pSh->Shape = stCircle;
		pSh->Brush->Color=clRed;
		pSh->Height = 12;
		pSh->Parent =  GridPanel2;
		pHs->activeShape = pSh;

		TEdit * pE =new TEdit(GridPanel2);
		pE->Name = "Te1" + p3D->name + "_" + buttonNumber;
		GridPanel2->Height += pE->Height+2;
		pE->Text = pHs->name;
		pE->Parent = GridPanel2;
		pHs->nameEdit = pE;
		pE->ReadOnly = true;

		pE =new TEdit(GridPanel2);
		pE->Name = "Te2" + p3D->name + "_" + buttonNumber;
		pE->Text = pHs->r;
		pE->Parent = GridPanel2;
		pHs->rEdit = pE;
		pE->OnChange = &ApplyChange;

		pE =new TEdit(GridPanel2);
		pE->Name = "Te3" + p3D->name + "_" + buttonNumber;
		pE->Text = pHs->sensor[0];
		pE->Parent = GridPanel2;
		pHs->sensor0Edit = pE;
		pE->OnChange = &ApplyChange;

		pE =new TEdit(GridPanel2);
		pE->Name = "Te4" + p3D->name + "_" + buttonNumber++;
		pE->Text = pHs->value;
		pE->Parent = GridPanel2;
		pHs->valueEdit = pE;
		pE->OnChange = &ApplyChange;

		unsigned int res = WaitForSingleObject(hMutex,INFINITE);
		if (res!=WAIT_OBJECT_0) return;
		__try {
			hotspots.push_back(pHs);
		} __finally {
			ReleaseMutex(hMutex);
		}

}


void TForm11::addPointTo(TPoint3D * p3D, int size, int device, int sensor1, int sensor2) {
		THotspot * pHs = new THotspot(
				p3D->x,
				p3D->y,
				p3D->z,
				p3D->name,
				size,
				device,
				sensor1,
				sensor2
				);

		static int buttonNumber = 0;

		TShape * pSh = new TShape(GridPanel3);
   //     pSh->p[0] = pHs;
		pSh->Name = "Psh" + p3D->name + "_" + buttonNumber;
		pSh->Shape = stCircle;
		pSh->Brush->Color=clRed;
		pSh->Height = 12;
		pSh->Parent =  GridPanel3;
		pHs->activeShape = pSh;

		TEdit * pE =new TEdit(GridPanel3);
		GridPanel3->Height += pE->Height+2;
		pE->Name = "Pe1"  + p3D->name + "_" + buttonNumber;
		pE->Text = pHs->name;
		pE->Parent = GridPanel3;
		pHs->nameEdit = pE;
		pE->ReadOnly = true;

		pE =new TEdit(GridPanel3);
		pE->Name = "Pe2"  + p3D->name + "_" + buttonNumber;
		pE->Text = pHs->r;
		pE->Parent = GridPanel3;
		pHs->rEdit = pE;
		pE->OnChange = &ApplyChange;

		pE =new TEdit(GridPanel3);
		pE->Name = "Pe3"  + p3D->name + "_" + buttonNumber;
		pE->Text = pHs->sensor[0];
		pE->Parent = GridPanel3;
		pHs->sensor0Edit = pE;
		pE->OnChange = &ApplyChange;

		pE =new TEdit(GridPanel3);
		pE->Name = "Pe4"  + p3D->name + "_" + buttonNumber;
		pE->Text = pHs->sensor[1];
		pE->Parent = GridPanel3;
		pHs->sensor1Edit = pE;
		pE->OnChange = &ApplyChange;

		pE =new TEdit(GridPanel3);
		pE->Name = "Pe5"  + p3D->name + "_" + buttonNumber++;
		pE->Text = pHs->value;
		pE->Parent = GridPanel3;
		pHs->valueEdit = pE;
		pE->OnChange = &ApplyChange;

		unsigned int res = WaitForSingleObject(hMutex,INFINITE);
		if (res!=WAIT_OBJECT_0) return;
		__try {
			hotspots.push_back(pHs);
		} __finally {
			ReleaseMutex(hMutex);
		}
}


void TForm11::addDirection(TPoint3D * p3D,  int r, int device, int sensor1, int sensor2, int dist_mm) {
		THotspot * pHs = new THotspot(
				p3D->x,
				p3D->y,
				p3D->z,
				p3D->name,
				r,
				device,
				sensor1,
				sensor2,
				dist_mm
				);

		pHs->x*=pHs->dist_mm;
		pHs->y*=pHs->dist_mm;
		pHs->z*=pHs->dist_mm;

		static int buttonNumber = 0;

		TShape * pSh = new TShape(GridPanel4);
		pSh->Name = "Dsh"  + p3D->name + "_" + buttonNumber;
		pSh->Shape = stCircle;
		pSh->Brush->Color=clRed;
		pSh->Height = 12;
		pSh->Parent =  GridPanel4;
		pHs->activeShape = pSh;

		TEdit * pE =new TEdit(GridPanel4);
		GridPanel4->Height += pE->Height+2;
		pE->Name = "De1"  + p3D->name + "_" + buttonNumber;
		pE->Text = pHs->name;
		pE->Parent = GridPanel4;
		pHs->nameEdit = pE;
		pE->ReadOnly = true;

		pE =new TEdit(GridPanel4);
		pE->Name = "De2"  + p3D->name + "_" + buttonNumber;
		pE->Text = pHs->r;
		pE->Parent = GridPanel4;
		pHs->rEdit = pE;
		pE->OnChange = &ApplyChange;

		pE =new TEdit(GridPanel4);
		pE->Name = "De3"  + p3D->name + "_" + buttonNumber;
		pE->Text = pHs->sensor[0];
		pE->Parent = GridPanel4;
		pHs->sensor0Edit = pE;
		pE->OnChange = &ApplyChange;

		pE =new TEdit(GridPanel4);
		pE->Name = "De4"  + p3D->name + "_" + buttonNumber;
		pE->Text = pHs->sensor[1];
		pE->Parent = GridPanel4;
		pHs->sensor1Edit = pE;
		pE->OnChange = &ApplyChange;

		pE =new TEdit(GridPanel4);
		pE->Name = "De5"  + p3D->name + "_" + buttonNumber++;
		pE->Text = pHs->value;
		pE->Parent = GridPanel4;
		pHs->valueEdit = pE;
		pE->OnChange = &ApplyChange;

		unsigned int res = WaitForSingleObject(hMutex,INFINITE);
		if (res!=WAIT_OBJECT_0) return;
		__try {
			hotspots.push_back(pHs);
		} __finally {
			ReleaseMutex(hMutex);
		}
}


void TForm11::addRectangular(THotspotGrid *hotspotGrid, double x, double y, double z, double Xthickness,
			double Ythickness, double Zthickness, int sensor, int device) {

		TRectHotspot * pHs = new TRectHotspot(
				x,
				y,
				z,
				Xthickness,
				Ythickness,
				Zthickness,
				sensor,
				device
				);



		TShape * pSh = new TShape(GridPanel5);
		pSh->Shape = stCircle;
		pSh->Brush->Color=clRed;
		pSh->Height = 12;
		pSh->Parent =  GridPanel5;
		pHs->activeShape = pSh;

		TEdit * pE =new TEdit(GridPanel5);
		GridPanel5->Height += pE->Height+2;


  /*		pE->Text = pHs->x;
		pE->Parent = GridPanel5;
		pHs->xEdit = pE;
		pE->ReadOnly = true;


		pE =new TEdit(GridPanel5);
		pE->Text = pHs->y;
		pE->Parent = GridPanel5;
		pHs->yEdit = pE;
		pE->ReadOnly = true;

		pE =new TEdit(GridPanel5);
		pE->Text = pHs->z;
		pE->Parent = GridPanel5;
		pHs->zEdit = pE;
		pE->ReadOnly = true;


		pE =new TEdit(GridPanel5);
		pE->Text = pHs->Xthickness;
		pE->Parent = GridPanel5;
		pHs->xtEdit = pE;
		pE->ReadOnly = true;

		pE =new TEdit(GridPanel5);
		pE->Text = pHs->Ythickness;
		pE->Parent = GridPanel5;
		pHs->xtEdit = pE;
		pE->ReadOnly = true;;

		pE =new TEdit(GridPanel5);
		pE->Text = pHs->Zthickness;
		pE->Parent = GridPanel5;
		pHs->ztEdit = pE;
		pE->ReadOnly = true;

		pE =new TEdit(GridPanel5);
		pE->Text = pHs->sensor;
		pE->Parent = GridPanel5;
		pHs->sensorEdit = pE;
		pE->ReadOnly = true;
      */
		pE =new TEdit(GridPanel5);
		pE->Text = pHs->value;
		pE->Parent = GridPanel5;
		pHs->valueEdit = pE;
		pE->ReadOnly = true;


		unsigned int res = WaitForSingleObject(hMutex,INFINITE);
		if (res!=WAIT_OBJECT_0) return;
		__try {
			hotspotGrid->rectHotspots.push_back(pHs);
			printf("%d\n", device);
		} __finally {
			ReleaseMutex(hMutex);
		}
}


void TForm11::addScreen(int topLeft, int topRight, int bottomLeft, int bottomRight,
			int sensor0, int sensor1, int device, double monitorDepth, int x, int y, int width, int height) {

		THotspotScreen * pHs = new THotspotScreen(
			p3Ds, topLeft, topRight, bottomLeft, bottomRight,
			sensor0, sensor1, device, monitorDepth, x, y, width, height);

		monitorDrawers[device] =  createMonitorDrawer(x,y,width,height);

		TShape * pSh = new TShape(GridPanel6);
		pSh->Shape = stCircle;
		pSh->Brush->Color=clRed;
		pSh->Height = 12;
		pSh->Parent =  GridPanel6;
		pHs->activeShape = pSh;

		TEdit * pE =new TEdit(GridPanel6);
		GridPanel6->Height += pE->Height+2;
		pE->Text = pHs->topLeftID;
		pE->Parent = GridPanel6;
		pHs->topLeftEdit = pE;
		pE->ReadOnly = true;



		pE =new TEdit(GridPanel6);
		pE->Text = pHs->topRightID;
		pE->Parent = GridPanel6;
		pHs->topRightEdit = pE;
		pE->ReadOnly = true;

		pE =new TEdit(GridPanel6);
		pE->Text = pHs->bottomLeftID;
		pE->Parent = GridPanel6;
		pHs->bottomLeftEdit = pE;
		pE->ReadOnly = true;


		pE =new TEdit(GridPanel6);
		pE->Text = pHs->bottomRightID;
		pE->Parent = GridPanel6;
		pHs->bottomRightEdit = pE;
		pE->ReadOnly = true;

		pE =new TEdit(GridPanel6);
		pE->Text = pHs->sensor0;
		pE->Parent = GridPanel6;
		pHs->sensor0Edit = pE;
		pE->OnChange = &ApplyChange;

		pE =new TEdit(GridPanel6);
		pE->Text = pHs->sensor1;
		pE->Parent = GridPanel6;
		pHs->sensor1Edit = pE;
		pE->OnChange = &ApplyChange;

		pE =new TEdit(GridPanel6);
		pE->Text = pHs->device;
		pE->Parent = GridPanel6;
		pHs->deviceEdit = pE;
		pE->OnChange = &ApplyChange;

		unsigned int res = WaitForSingleObject(hMutex,INFINITE);
		if (res!=WAIT_OBJECT_0) return;
		__try {
			hotspotScreens.push_back(pHs);
		} __finally {
			ReleaseMutex(hMutex);
		}


}


void __fastcall TForm11::GridPanel1Click(TObject *Sender)
{


	TButton * pC = (TButton *) Sender;
	UnicodeString txt = pC->Name;
	bool remove = txt.SubString(0,1)=="r";
	bool touch  = txt.SubString(0,1)=="t";
	bool point  = txt.SubString(0,1)=="p";
	bool dir    = txt.SubString(0,1)=="d";
	bool grid = txt.SubString(0,1)=="g";
	bool screen = txt.SubString(0,1)=="s";


	if (remove) {
	   int id = (txt.SubString(2, txt.Length())).ToInt();
		int cc1 = GridPanel1->ControlCollection->Count;
		int z;
		for (z = 0; z < cc1; z++) {
			TControl * pC = GridPanel1->Controls[z];
			if (pC->Name == txt)
				break;
		}
		GridPanel1->Height -= GridPanel1->Controls[1]->Height*1.5;
		for (int i = 0; i < GridPanel1->ColumnCollection->Count; i++) {
			TControl * pC = GridPanel1->Controls[z];
			//GridPanel1->RemoveControl(pC); //hides.  does not remove from form.
			pC->Parent = TrashPanel;
		}
		delete p3Ds[id];
		p3Ds.erase(id);

	}

	else
	if (touch)
	{
		int id = (txt.SubString(2, txt.Length())).ToInt();
		addTouch(p3Ds[id], 10, 1, 1);
	}
	else
	if (point)
	{
		int id = (txt.SubString(2, txt.Length())).ToInt();
		addPointTo(p3Ds[id], 10, 1, 1, 2);
	}
	else
	if (dir)
	{
		int id = (txt.SubString(2, txt.Length())).ToInt();
		addDirection(p3Ds[id], 10, 1, 1, 2,1000);

	}

	if(grid) {
		int id = (txt.SubString(2, txt.Length())).ToInt();
		GridForm->Location1Edit->Text = UnicodeString(id);
		GridForm->Location2Edit->Text = UnicodeString(id+1);
		GridForm->Location3Edit->Text = UnicodeString(id+2);
		int res = GridForm->ShowModal();
		if (res == mrOk){
			int location1 = GridForm->location1;
			int location2 = GridForm->location2;
			int location3 = GridForm->location3;
			int sensor = GridForm->sensor;
			int device = GridForm->device;
			int nColumns = GridForm->nColumns;
			int nRows = GridForm->nRows;
			double Ythickness= GridForm->heightThickness;
			double maxColumnThickness = GridForm->columnThickness;
			double maxRowThickness = GridForm->rowThickness;

			if(p3Ds.find(location1) == p3Ds.end())  {
				Application->MessageBoxA(L"Location 1 is not valid.", L"Error", MB_OK);
				return;
			}
			if(p3Ds.find(location2) == p3Ds.end())  {
				Application->MessageBoxA(L"Location 2 is not valid.", L"Error", MB_OK);
				return;
			}
			if(p3Ds.find(location3) == p3Ds.end())  {
				Application->MessageBoxA(L"Location 3 is not valid.", L"Error", MB_OK);
				return;
			}
			if(location1 == location2 || location1 == location3 || location2==location3) {
				Application->MessageBoxA(L"Locations must be different.", L"Error", MB_OK);
				return;
			}
			if(sensor < 0) {
				Application->MessageBoxA(L"Sensor must not be negative.", L"Error", MB_OK);
				return;
			}

			TPoint3D * p3D1 = p3Ds[location1];
			TPoint3D * p3D2 = p3Ds[location2];
			TPoint3D * p3D3 = p3Ds[location3];


			double columnStepX, columnStepZ, rowStepX, rowStepZ;
			int Xdiv, Zdiv;
			double Xthickness, Zthickness;
			double y = ((double) p3D3->y +(double) p3D2->y + (double) p3D1->y) / 3.0;
			double x,z;
			//if step through x first.  square 2 is at different x, same z as 1.
			if(abs(p3D2->x - p3D1->x) > abs(p3D2->z - p3D1->z)) {
				columnStepX = (double)(p3D2->x - p3D1->x)/ (double)nColumns;
				columnStepZ = 0.0;
				rowStepX = 0.0;
				rowStepZ = (double) (p3D3->z - (p3D1->z+p3D2->z)/2.0)/(double)nRows;
				Xdiv = nColumns;
				Zdiv = nRows;
				Xthickness = std::min( fabs(columnStepX), maxColumnThickness);
				Zthickness = std::min( fabs(rowStepZ), maxRowThickness);
				x = columnStepX/2.0 + p3D1->x;
				z = rowStepZ/2.0 + p3D1->z;
			//else if step through z first
			} else {
				columnStepX = 0.0;
				columnStepZ = (double)(p3D2->z - p3D1->z)/ (double)nColumns;
				rowStepX = (double) (p3D3->x - (p3D1->x+p3D2->x)/2.0)/(double)nRows;
				rowStepZ = 0.0;
				Xdiv = nRows;
				Zdiv = nColumns;
				Xthickness = std::min(fabs(rowStepX), maxRowThickness);
				Zthickness = std::min(fabs(columnStepZ), maxColumnThickness);
				x = rowStepX/2.0 + p3D1->x;
				z = columnStepZ/2.0 + p3D1->z;
			}




			unsigned int res = WaitForSingleObject(hMutex,INFINITE);
			if (res!=WAIT_OBJECT_0) return;
			THotspotGrid *hotspotGrid;
			__try {
				hotspotGrid =
					new THotspotGrid(location1, location2, location3, Xdiv, Zdiv,
							Xthickness, Ythickness, Zthickness);
						hotspotGrids.push_back(hotspotGrid);
				for(int i=0; i<nRows; i++) {
					for(int j=0; j<nColumns; j++) {
						addRectangular(hotspotGrid, x, y,
						  z,Xthickness, Ythickness, Zthickness, sensor, device++);
						x+= columnStepX;
						z+= columnStepZ;
					}
					x-= columnStepX * nColumns;
					z-= columnStepZ * nColumns;
					x += rowStepX;
					z += rowStepZ;
				}
			} __finally {
				ReleaseMutex(hMutex);
			}

		}
	}

	else if(screen) {
		int id = (txt.SubString(2, txt.Length())).ToInt();
		ScreenForm->TopLeftEdit->Text = UnicodeString(id);
		ScreenForm->TopRightEdit->Text = UnicodeString(id+1);
		ScreenForm->BottomLeftEdit->Text = UnicodeString(id+2);
		ScreenForm->BottomRightEdit->Text = UnicodeString(id+3);


		int res = ScreenForm->ShowModal();
		if (res == mrOk){
			int topLeftID = ScreenForm->topLeftID;
			int topRightID = ScreenForm->topRightID;
			int bottomLeftID = ScreenForm->bottomLeftID;
			int bottomRightID = ScreenForm->bottomRightID;
			int sensor0 = ScreenForm->sensor0;
			int sensor1 = ScreenForm->sensor1;
			int device = ScreenForm->device;
			double monitorDepth = ScreenForm->monitorDepth;
			int xMin = ScreenForm->x;
			int yMin = ScreenForm->y;
			int width = ScreenForm->width;
			int height = ScreenForm->height;

			if(p3Ds.find(topLeftID) == p3Ds.end())  {
				Application->MessageBoxA(L"Top Left ID is not valid.", L"Error", MB_OK);
				return;
			}
			if(p3Ds.find(topRightID) == p3Ds.end())  {
				Application->MessageBoxA(L"Top Right ID is not valid.", L"Error", MB_OK);
				return;
			}
			if(p3Ds.find(bottomLeftID) == p3Ds.end())  {
				Application->MessageBoxA(L"Bottom Left ID is not valid.", L"Error", MB_OK);
				return;
			}
			if(p3Ds.find(bottomRightID) == p3Ds.end())  {
				Application->MessageBoxA(L"Bottom Right ID is not valid.", L"Error", MB_OK);
				return;
			}

			if(topLeftID == topRightID || topLeftID == bottomLeftID || topLeftID==bottomRightID ||
			   topRightID == bottomLeftID || topRightID == bottomRightID ||
			   bottomLeftID == bottomRightID) {
				Application->MessageBoxA(L"IDs must be different.", L"Error", MB_OK);
				return;
			}

			addScreen(topLeftID, topRightID, bottomLeftID, bottomRightID, sensor0, sensor1, device, monitorDepth, xMin, yMin, width, height);
		}

	}
   	ApplyChanges();
}
//---------------------------------------------------------------------------

void TForm11::AddHotspot(TPoint3D * p3D, int id)
{


		while(TrashPanel->ControlCount > 0) {
			TControl* cont = TrashPanel->Controls[0];
			TrashPanel->RemoveControl(cont);
			delete cont;
		}

		Form3->StringGrid1->Objects[0][0]=0;
		Form3->ClientOrigin.x = Left + Width;

		TButton * pB = new TButton(GridPanel1);
		pB->Caption = "Remove";


		UnicodeString s = UnicodeString("r") + id;

		pB->Name = s;
		pB->OnClick = GridPanel1->OnClick;
		pB->Width = 200;
		pB->Height = 20;
		p3D->name = id;

		pB->Parent = GridPanel1;

		GridPanel1->Height += pB->Height+3;


		for (int i=0; i < 4;i++)
		{
			TEdit * pE =new TEdit(GridPanel1);
			pE->Parent = GridPanel1;
			switch (i)
			{
				case 0: pE->Text = p3D->x; pE->ReadOnly = true; break;
				case 1: pE->Text = p3D->y; pE->ReadOnly = true; break;
				case 2: pE->Text = p3D->z; pE->ReadOnly = true; break;
				case 3: pE->Text = p3D->name; pE->ReadOnly = true; break;
			}
		}

		pB = new TButton(GridPanel1);
		pB->Caption = "Touch";
		s = UnicodeString("t") + id;
		pB->Name = s;
		pB->OnClick = GridPanel1->OnClick;
		pB->Width = 80;
		pB->Height = 20;
		pB->Parent = GridPanel1;

		pB = new TButton(GridPanel1);
		pB->Caption = "Point to";
		s = UnicodeString("p") + id;
		pB->Name = s;
		pB->OnClick = GridPanel1->OnClick;
		pB->Width = 80;
		pB->Height = 20;
		pB->Parent = GridPanel1;

		pB = new TButton(GridPanel1);
		pB->Caption = "Direction";
		s = UnicodeString("d") + id;
		pB->Name = s;
		pB->OnClick = GridPanel1->OnClick;
		pB->Width = 80;
		pB->Height = 20;
		pB->Parent = GridPanel1;

		pB = new TButton(GridPanel1);
		pB->Caption = "Grid";
		s = UnicodeString("g") + id;
		pB->Name = s;
		pB->OnClick = GridPanel1->OnClick;
		pB->Width = 80;
		pB->Height = 20;
		pB->Parent = GridPanel1;

		pB = new TButton(GridPanel1);
		pB->Caption = "Screen";
		s = UnicodeString("s") + id;
		pB->Name = s;
		pB->OnClick = GridPanel1->OnClick;
		pB->Width = 80;
		pB->Height = 20;
		pB->Parent = GridPanel1;

		p3Ds[id] = new TPoint3D(*p3D);

}


void __fastcall TForm11::Button5Click(TObject *Sender)
{
	int res = Form3->ShowModal();
	if (res == mrOk)
	{
		TPoint3D * p3D = Form3->p3D;
		AnsiString s;
		s.sprintf( "%g,%g,%g",p3D->x,p3D->y,p3D->z);
		Memo2->Lines->Add(s);
		return;
	}


}


 void TForm11::ApplyChanges() {
	unsigned int res = WaitForSingleObject(hMutex,INFINITE);
	if (res!=WAIT_OBJECT_0) return;
	__try
	{

		for(std::list<THotspot*>::iterator i = hotspots.begin(); i != hotspots.end(); ++i){
			THotspot* pHs = *i;
			int hsTemp = -1;
			if (pHs->sensor[1] == IS_NONE)  //no "to" sensor
				hsTemp = IS_TOUCH;
			else // is point-to or dist
			{
			if (pHs->dist_mm > 0)
				hsTemp = IS_DIR;
			else
				hsTemp = IS_POINTTO;
			}

			int sensor0 = 0, sensor1 = 0;
			if(pHs->sensor0Edit->Text == "e") {
				sensor0 = IS_EYE;
			} else if(pHs->sensor0Edit->Text == "g") {
				sensor0 = IS_GAZE;
			} else {
				sensor0 = pHs->sensor0Edit->Text.ToInt();
			}

			if(pHs->sensor1Edit->Text == "e") {
				sensor1 = IS_EYE;
			} else if(pHs->sensor1Edit->Text == "g") {
				sensor1 = IS_GAZE;
			} else if(pHs->sensor[1] != IS_NONE) {
				sensor1 = pHs->sensor1Edit->Text.ToInt();
			}
			switch(hsTemp)
			{
				case IS_TOUCH:
				{
					pHs->r = pHs->rEdit->Text.ToInt();  //size
					pHs->sensor[0] = sensor0; //sensor
					pHs->value = pHs->valueEdit->Text.ToInt(); //device

				}
				break;
				case IS_POINTTO:
				{
					pHs->r = pHs->rEdit->Text.ToInt();  //size
					pHs->sensor[0] = sensor0; //sensor 0
					pHs->sensor[1] = sensor1; //sensor 1
					pHs->value = pHs->valueEdit->Text.ToInt(); //device
				}
				break;
				case IS_DIR:
				{
					pHs->r = pHs->rEdit->Text.ToInt();  //size
					pHs->sensor[0] = sensor0; //sensor 0
					pHs->sensor[1] = sensor1; //sensor 1
					pHs->value = pHs->valueEdit->Text.ToInt(); //device
				}
				break;
			}

		}

		for(std::list<THotspotScreen*>::iterator hss = hotspotScreens.begin(); hss != hotspotScreens.end(); ++hss) {
			THotspotScreen *pHs = *hss;
			int sensor0 = 0, sensor1 = 0;
			if(pHs->sensor0Edit->Text == "e") {
				sensor0 = IS_EYE;
			} else if(pHs->sensor0Edit->Text == "g") {
				sensor0 = IS_GAZE;
			} else {
				sensor0 = pHs->sensor0Edit->Text.ToInt();
			}

			if(pHs->sensor1Edit->Text == "e") {
				sensor1 = IS_EYE;
			} else if(pHs->sensor1Edit->Text == "g") {
				sensor1 = IS_GAZE;
			} else {
				sensor1 = pHs->sensor1Edit->Text.ToInt();
			}

			pHs->sensor0 = sensor0; //sensor 0
			pHs->sensor1 = sensor1; //sensor 1
			pHs->device = pHs->deviceEdit->Text.ToInt();
		}

		UpdateInfo();
	}

	__finally
	{
		ReleaseMutex(hMutex);
	}
}
//---------------------------------------------------------------------------



void __fastcall TForm11::SaveLocClick(TObject *Sender)
{
	if (SaveDialog1->Execute())
	{
		Memo2->Lines->SaveToFile(SaveDialog1->FileName);
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm11::LoadLocClick(TObject *Sender)
{
	if (OpenDialog1->Execute())
	{
		Memo2->Lines->LoadFromFile(OpenDialog1->FileName);
	}

}
//---------------------------------------------------------------------------



void __fastcall TForm11::SaveConfigButtonClick(TObject *Sender)
{
	if (SaveDialog2->Execute())
	{
		unsigned int res = WaitForSingleObject(hMutex,INFINITE);

		if (res!=WAIT_OBJECT_0) return;
		__try
		{

		   	bool succeeded = doc.save_file(AnsiString(SaveDialog2->FileName).c_str());

		}
		__finally
		{
			ReleaseMutex(hMutex);
		}
	}

}
//---------------------------------------------------------------------------


void __fastcall TForm11::LoadConfigButtonClick(TObject *Sender)
{

	if (OpenDialog2->Execute())
	{
		unsigned int res = WaitForSingleObject(hMutex,INFINITE);

		if (res!=WAIT_OBJECT_0) return;
		__try
		{
			clear();
			LoadConfig(OpenDialog2->FileName);
			ApplyChanges();
			//Button1Click(this);
		}
		__finally
		{

			ReleaseMutex(hMutex);
		}
	}

}
//---------------------------------------------------------------------------


void __fastcall TForm11::Button4Click(TObject *Sender)
{
	char buf[40][101];
	TPoint3D * p3D;
	memset(buf,0,sizeof buf);
	int x,y,z;

	for (int i = 0; i < Memo2->Lines->Count; i++)
	{
		UnicodeString s = Memo2->Lines->Strings[i];
		std::vector<UnicodeString> strs = splitString(s, L',');
		p3D =  new TPoint3D(strs[0].ToDouble(),strs[1].ToDouble(),strs[2].ToDouble());

		AddHotspot(p3D, nId);
		delete p3D;
		nId++;
	}
	UpdateInfo();
}
//---------------------------------------------------------------------------

void __fastcall TForm11::PhaseComboBoxChange(TObject *Sender)
{
	if(hMutex) {
		CloseHandle (hMutex);
		hMutex = NULL;
	}
	hMutex = CreateMutex(0,false,0);

	if(phasespaceThread) {
		phasespaceThread->Terminate();
		delete phasespaceThread;
		phasespaceThread = NULL;
	}

	char temp[512];
	sprintf(temp, "name='%s'", ((AnsiString) PhaseComboBox->Text).c_str());
	if(strcmp( ((AnsiString) PhaseComboBox->Text).c_str(), "PhaseSpace") == 0) {
		channelsPerMarker = 4;
		distanceScaleFactor = 1000.0;
	} else if (strcmp( ((AnsiString) PhaseComboBox->Text).c_str(), "OptiTrack") == 0) {
		channelsPerMarker = 3;
		distanceScaleFactor = 1.0;
	} else {
		Application->MessageBoxA(L"Motion data type not recognized. Assuming three channels per marker and distances measured in millimeters.", L"Warning", MB_OK);
		channelsPerMarker = 3;
		distanceScaleFactor = 1.0;
	}
	phasespaceThread = new TStreamThread(temp,ProcessData);
	phasespaceThread->Resume();
}
//---------------------------------------------------------------------------



void __fastcall TForm11::GazeComboBoxChange(TObject *Sender)
{

	if(hGazeMutex) {
		CloseHandle(hGazeMutex);
		hGazeMutex = 0;
	}

	hGazeMutex = CreateMutex(0,false,0);



	if(hotGazeOutlet) {
		lsl_destroy_outlet(hotGazeOutlet);
		hotGazeOutlet = NULL;
	}

	std::vector<UnicodeString> strings =  splitString(GazeComboBox->Text, L'_');

	char * streamName;
	if(strings.size() < 2)
		streamName = AnsiString("HotGazeStream_").c_str();
	else  {
		streamIdentifier = AnsiString(strings[1]);
		streamName = (AnsiString("HotGazeStream_") + streamIdentifier).c_str();
	}

	lsl_streaminfo gazeInfo = lsl_create_streaminfo(streamName, streamName, GAZE_CHANNELS, 30, cft_float32,generateGUID());
	lsl_xml_ptr desc = lsl_get_desc(gazeInfo);
	lsl_xml_ptr chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name","pupil position x");
	lsl_append_child_value(chn,"unit","pixels");

	chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name","pupil position y");
	lsl_append_child_value(chn,"unit","pixels");

	chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name","target position x");
	lsl_append_child_value(chn,"unit","mm");

	chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name","target position y");
	lsl_append_child_value(chn,"unit","mm");


	chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name","pupil radius A");
	lsl_append_child_value(chn,"unit","pixels");

	chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name","pupil radius B");
	lsl_append_child_value(chn,"unit","pixels");

	chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name","pupil angle");
	lsl_append_child_value(chn,"unit","radians");

	chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name","x offset, scene camera");
	lsl_append_child_value(chn,"unit","mm");

	chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name","y offset, scene camera");
	lsl_append_child_value(chn,"unit","mm");

	chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name","camera position x");
	lsl_append_child_value(chn,"unit","m");

	chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name","camera position y");
	lsl_append_child_value(chn,"unit","m");

	chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name","camera position z");
	lsl_append_child_value(chn,"unit","m");

	chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name","gaze position x");
	lsl_append_child_value(chn,"unit","m");

	chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name","gaze position y");
	lsl_append_child_value(chn,"unit","m");

	chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name","gaze position z");
	lsl_append_child_value(chn,"unit","m");

	chn = lsl_append_child(desc, "channels");
	lsl_append_child_value(chn, "name","assumed target distance, scene camera");
	lsl_append_child_value(chn,"unit","mm");


	hotGazeOutlet = lsl_create_outlet(gazeInfo,0,360);

	if(gazeListenThread) {
		gazeListenThread->Terminate();
		delete gazeListenThread; //will not delete till terminated, by VCL design.
		gazeListenThread = NULL;
	}

	char * temp = (char *) malloc(512);
	sprintf(temp, "name='%s'", ((AnsiString) GazeComboBox->Text).c_str());
	gazeListenThread = new TStreamThread(temp,ProcessGazeData);
	gazeListenThread->Resume();
    free(temp);

	UpdateInfo(); //restart the hotspots streams with the new name.

}
//---------------------------------------------------------------------------



  void __fastcall TForm11::ApplyChange(TObject *Sender)
{
	TEdit *tEdit = (TEdit *) Sender;
	bool ex = false;
	try {
		if(tEdit->Text != "e" && tEdit->Text !="g")  tEdit->Text.ToInt();
	} catch (...) {
		ex = true;
	}
	if(!ex) ApplyChanges();
}




void __fastcall TForm11::LoadCalibrationClick(TObject *Sender)
{

	if(OpenDialog1->Execute()) {
		gu->LoadGazeCalibration(OpenDialog1->FileName, xdoc_in);

	}
}
//---------------------------------------------------------------------------





void __fastcall TForm11::FormDestroy(TObject *Sender)
{
	clear();

	if(hMutex) CloseHandle(hMutex);
	if(hGazeMutex) CloseHandle(hGazeMutex);

	if(gu) delete gu;

	if(gazeListenThread) {
		gazeListenThread->Terminate();
		delete gazeListenThread; //will not delete till terminated, by VCL design.
		gazeListenThread = NULL;
	}


	if(phasespaceThread) {
		phasespaceThread->Terminate();
		delete phasespaceThread;
		phasespaceThread = NULL;
	}
	if(eventOutlet) {
		lsl_destroy_outlet(eventOutlet);
		eventOutlet = NULL;
	}

	if(dataOutlet) {
		lsl_destroy_outlet(dataOutlet);
		dataOutlet = NULL;

	}
	if(phaseData) {
		delete1D(phaseData);
    	phaseData = NULL;
	}


}
void __fastcall TForm11::OpenDisplaysButtonClick(TObject *Sender)
{
	for(std::list<THotspotScreen*>::iterator hss = hotspotScreens.begin(); hss != hotspotScreens.end(); ++ hss) {
		THotspotScreen *hotspotScreen = *hss;
		if(!monitorDrawers[hotspotScreen->device]->visible) monitorDrawers[hotspotScreen->device]->setVisible(SW_SHOW);

	}
}
//---------------------------------------------------------------------------

void __fastcall TForm11::CloseDisplaysButtonClick(TObject *Sender)
{
	for(std::map<int, MonitorDrawer*>::iterator iterator = monitorDrawers.begin(); iterator != monitorDrawers.end(); ++ iterator) {
		MonitorDrawer *monitorDrawer = iterator->second;
		if(monitorDrawer->visible) monitorDrawer->setVisible(SW_HIDE);
	}
}
//---------------------------------------------------------------------------


void __fastcall TForm11::xOffsetEditChange(TObject *Sender)
{
	bool ex = false;
	try {
		xOffsetEdit->Text.ToDouble();
	} catch (...) {
		ex = true;
	}

	if(!ex) xOffset = xOffsetEdit->Text.ToDouble();
}
//---------------------------------------------------------------------------

void __fastcall TForm11::yOffsetEditChange(TObject *Sender)
{
	bool ex = false;
	try {
		yOffsetEdit->Text.ToDouble();
	} catch (...) {
		ex = true;
	}

	if(!ex) yOffset = yOffsetEdit->Text.ToDouble();
}
//---------------------------------------------------------------------------


void __fastcall TForm11::RefreshStreamsButtonClick(TObject *Sender)
{
	lsl_streaminfo infos[MAX_STREAMS];
	int streamsFound = lsl_resolve_all(infos, MAX_STREAMS, 0.1);

	PhaseComboBox->Items->Clear();
	for (int i = 0; i < streamsFound; i++) {
		PhaseComboBox->Items->Append(lsl_get_name(infos[i]));
	}

	GazeComboBox->Items->Clear();
	for (int i = 0; i < streamsFound; i++) {
		GazeComboBox->Items->Append(lsl_get_name(infos[i]));
		lsl_destroy_streaminfo(infos[i]);
	}

}
//---------------------------------------------------------------------------


