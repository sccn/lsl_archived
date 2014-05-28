#include "../../../include/lsl_c.h"
#include "../../../include/cbw.h"
#include "../../../NIDAQmx.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
* This example program offers a 1-channel stream which contains strings. 
* The stream has the "Marker" content type and irregular rate.
 */

char *markertypes[] = {"4", "6"};

int main(int argc, char* argv[]) {
	lsl_streaminfo info;		/* out stream declaration object */
	lsl_outlet outlet;			/* stream outlet */
	double endtime;				/* used for send timing */
	char *mrk;					/* marker to send next */
	int count = 0;
    int BoardNum = 0;
	int Chan = 0;
    int ULStat = 0;
    int Gain = BIP5VOLTS;


	WORD DataValueZero = 0;
	WORD DataValueOne = 1000;
	DWORD DataValue32 = 0;
	
	float    RevLevel = (float)CURRENTREVNUM;

	double time = 0.0;
	double oldTime = 0.0;

  /* Declare UL Revision Level */
   ULStat = cbDeclareRevision(&RevLevel);



	/* declare a new streaminfo (name: "MyEventStream", content type: "Markers", 1 channel, irregular rate, ... */
	/* ... string values, some made-up source id (can also be empty) */
	info = lsl_create_streaminfo("MarkerStream","Markers",1,LSL_IRREGULAR_RATE,cft_string,"myuniquesourceid23443");

	/* make a new outlet (chunking: default, buffering: 360k markers) */
	outlet = lsl_create_outlet(info,0,360);

	printf("Now sending markers...\n");
   
     ULStat = cbAOut (BoardNum, Chan, Gain, DataValueZero);
	endtime = 1.0 + ((double) clock())/CLOCKS_PER_SEC;

	while (((double)clock())/CLOCKS_PER_SEC < endtime ) Sleep(1);

	while(1) {
		
		//returns once signal sent. Will wait till the next 250 kHz transition.
		ULStat = cbAOut(BoardNum, Chan, Gain, (count % 250) ? DataValueZero : DataValueOne);
//		ULStat = cbAIn32(BoardNum, Chan, Gain, &DataValue32, 0);
		if(!(count%250)) lsl_push_sample_str(outlet,&(markertypes[0]));
	//	oldTime = time;
//		time =lsl_local_clock();
//		if(count % 5000) printf("Elapsed: %g\n", time - oldTime);
	count++;
	}
/*	while(1) {

		printf("now sending: %s\n",markertypes[0]);
		lsl_push_sample_str(outlet,&(markertypes[0]));
		ULStat = cbAOut (BoardNum, Chan, Gain, DataValueOne);
		
		endtime = 1.0 + ((double)clock())/CLOCKS_PER_SEC + (rand()%1000)/1000.0;
		while (((double)clock())/CLOCKS_PER_SEC < endtime ) Sleep(1);
		ULStat = cbAOut (BoardNum, Chan, Gain, DataValueZero);
		lsl_push_sample_str(outlet,&(markertypes[1]));
		printf("now sending: %s\n",markertypes[1]);
		
		endtime = 1.0 + ((double)clock())/CLOCKS_PER_SEC + (rand()%1000)/1000.0;
		while (((double)clock())/CLOCKS_PER_SEC < endtime ) Sleep(1);
		

		
	
		
		count ++;
	}
*/
	/* we never get here, buy anyway */
	lsl_destroy_outlet(outlet);

	return 0;
}
