//---------------------------------------------------------------------------

#pragma hdrstop

#include <tchar.h>

#ifdef _DEBUG
	#undef _DEBUG  //prevent loading of debug version of library.
	#include "lsl_c.h" //must be "", not <>, otherwise get data corruption in console apps when using dll functions
	#define _DEBUG
#else
	#include "lsl_c.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <math.h>

#pragma argsused
//---------------------------------------------------------------------------

/**
 * This example program offers an 8-channel stream, float-formatted, that resembles EEG data.
 * The example demonstrates also how per-channel meta-data can be specified using the description field of the streaminfo object.
 *
 * Note that the timer used in the send loop of this program is not particularly accurate.
 */



int _tmainx(int argc, _TCHAR* argv[])
{
	char *channels[] = {"C3","C4","Cz","FPz","POz","CPz","O1","O2"};
	int t,c;					/* time point and channel index */
	lsl_streaminfo info;		/* out stream declaration object */
	lsl_xml_ptr desc, chn;		/* some xml element pointers */
	lsl_outlet outlet;			/* stream outlet */
	double starttime;			/* used for send timing */
	float cursample[8];			/* the current sample */

	/* declare a new streaminfo (8 channels, 100 Hz, float-formatted, some pseudo device id */
	info = lsl_create_streaminfo("BioSemi","EEG",8,100,cft_float32,"325wqer4354");

	/* add some meta-data fields to it */
	desc = lsl_get_desc(info);
	lsl_append_child_value(desc,"manufacturer","BioSemi");
	for (c=0;c<8;c++) {
		chn = lsl_append_child(desc,"channels");
		lsl_append_child_value(chn,"name",channels[c]);
		lsl_append_child_value(chn,"unit","microvolts");
		lsl_append_child_value(chn,"type","EEG");
	}

	/* make a new outlet */
	outlet = lsl_create_outlet(info,0,360);

	/* send data forever */
	printf("Now sending data...\n");
	starttime = ((double)clock())/CLOCKS_PER_SEC;
	for(t=0;;t++) {
		for (c=0;c<8;c++)
			cursample[c] = (float)((rand()%1500)/500.0-1.5);
		lsl_push_sample_f(outlet,cursample);
		while (((double)clock())/CLOCKS_PER_SEC < starttime + t*0.01);
	}

	/* we never get here, buy anyway */
	lsl_destroy_outlet(outlet);
	return 0;
}

int _tmainxx(int argc, _TCHAR* argv[])
{
	int nChannels = 9;
   //	char *channels[] = {"C3","C4","Cz","FPz","POz","CPz","O1","O2"};
	int t,c;					/* time point and channel index */
	lsl_streaminfo info;		/* out stream declaration object */
	lsl_xml_ptr desc, chn;		/* some xml element pointers */
	lsl_outlet outlet;			/* stream outlet */
	double starttime;			/* used for send timing */
	float cursample[9];			/* the current sample */

	/* declare a new streaminfo (8 channels, 100 Hz, float-formatted, some pseudo device id */
	info = lsl_create_streaminfo("BioSemi","EEG",nChannels,100,cft_float32,"325wqer4354");

	/* add some meta-data fields to it */
	desc = lsl_get_desc(info);
	lsl_append_child_value(desc,"manufacturer","BioSemi");
 /*	for (c=0;c<8;c++) {
		chn = lsl_append_child(desc,"channels");
		lsl_append_child_value(chn,"name",channels[c]);
		lsl_append_child_value(chn,"unit","microvolts");
		lsl_append_child_value(chn,"type","EEG");
	}
   */
	/* make a new outlet */
	outlet = lsl_create_outlet(info,0,360);

	/* send data forever */
	printf("Now sending data...\n");
	starttime = ((double)clock())/CLOCKS_PER_SEC;
	for(t=0;;t++) {
		for (c=0;c<nChannels;c++)
			cursample[c] = (float)((rand()%1500)/500.0-1.5);
		lsl_push_sample_f(outlet,cursample);
		while (((double)clock())/CLOCKS_PER_SEC < starttime + t*0.01);
	}

	/* we never get here, buy anyway */
	lsl_destroy_outlet(outlet);
	return 0;
}

inline float degToRad(float degrees) {
	return degrees*0.0174532925;
}
int _tmain(int argc, _TCHAR* argv[])
{
	int nChannels = 40;
	int nMarkers = nChannels/4;
	double samplesPerSec = 440;
	char *channels[40];


	int t,c,m;					/* time point and channel index */
	lsl_streaminfo info;		/* out stream declaration object */
	lsl_xml_ptr desc, chn, chns;		/* some xml element pointers */
	lsl_outlet outlet;			/* stream outlet */
	double starttime;			/* used for send timing */
	float cursample[40];			/* the current sample */
	char chanName[256];
	/* declare a new streaminfo (8 channels, 100 Hz, float-formatted, some pseudo device id */
	info = lsl_create_streaminfo("PhaseSpace","Mocap",nChannels,samplesPerSec,cft_float32,"76sfsfdsdfa67");

	/* add some meta-data fields to it */
	desc = lsl_get_desc(info);
	lsl_append_child_value(desc,"manufacturer","Phasespace");
	chns = lsl_append_child(desc,"channels");
	for (c=0;c<nMarkers;c++) {

		chn = lsl_append_child(chns,"channel");
		sprintf(chanName, "Marker%d_X", c);
		lsl_append_child_value(chn, "label", chanName);
		lsl_append_child_value(chn,"type","PositionX");
		lsl_append_child_value(chn,"unit","meters");


		chn = lsl_append_child(chns,"channel");
		sprintf(chanName, "Marker%d_Y", c);
		lsl_append_child_value(chn, "label", chanName);
		lsl_append_child_value(chn,"type","PositionY");
		lsl_append_child_value(chn,"unit","meters");


		chn = lsl_append_child(chns,"channel");
		sprintf(chanName, "Marker%d_Z", c);
		lsl_append_child_value(chn, "label", chanName);
		lsl_append_child_value(chn,"type","PositionY");
		lsl_append_child_value(chn,"unit","meters");

	}

	/* make a new outlet */
	outlet = lsl_create_outlet(info,0,360);

	/* send data forever */
	printf("Now sending data...\n");
	starttime = ((double)clock())/CLOCKS_PER_SEC;

	float x = 0;
	float y = 0;
	float z = 0;

	for(t=0;;t++) {
		for(m=0, c=0; m<nMarkers; m++) {
			x++;
			if(t%3 == 0) y++;
			if(t%5 == 0) z++;


			cursample[c++] = sin(degToRad((x-360*m)/10.0))*.1;

			cursample[c++] = sin(degToRad((y-360*m)/10.0))*.1;

			cursample[c++] = sin(degToRad((z-360*m)/10.0))*.1;

			cursample[c++] =  (float)((rand()%1500)/500.0-1.5)*.001;


		}

		lsl_push_sample_f(outlet,cursample);

		while (((double)clock())/CLOCKS_PER_SEC < starttime + t/samplesPerSec) {
			Sleep(1);
		}
	   //	printf("here %d\n", t);
	}

	/* we never get here, but anyway */
	lsl_destroy_outlet(outlet);
	return 0;
}
//---------------------------------------------------------------------------
