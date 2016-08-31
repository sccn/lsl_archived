#include "../../../include/lsl_c.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
* This example program offers a 1-channel stream which contains strings. 
* The stream has the "Marker" content type and irregular rate.
 */

char *markertypes[] = {"Test", "Blah", "Marker", "XXX", "Testtest", "Test-1-2-3"};

int main(int argc, char* argv[]) {
	lsl_streaminfo info;		/* out stream declaration object */
	lsl_outlet outlet;			/* stream outlet */
	double endtime;				/* used for send timing */
	char *mrk;					/* marker to send next */

	/* declare a new streaminfo (name: "MyEventStream", content type: "Markers", 1 channel, irregular rate, ... */
	/* ... string values, some made-up source id (can also be empty) */
	info = lsl_create_streaminfo("MyEventStream","Markers",1,LSL_IRREGULAR_RATE,cft_string,"myuniquesourceid23443");

	/* make a new outlet (chunking: default, buffering: 360k markers) */
	outlet = lsl_create_outlet(info,0,360);

	/* send random marker streams (note: this loop is keeping the CPU busy, normally one would sleep or yield here) */
	printf("Now sending markers...\n");

	while(1) {
		/* wait for a random period of time */
		endtime = ((double)clock())/CLOCKS_PER_SEC + (rand()%1000)/1000.0;
		while (((double)clock())/CLOCKS_PER_SEC < endtime);
		/* and choose the marker to send */
		mrk = markertypes[rand() % (sizeof(markertypes)/sizeof(markertypes[0]))];
		printf("now sending: %s\n",mrk);
		/* now send it (note the &, since this function takes an array of char*) */
		lsl_push_sample_str(outlet,&mrk);
	}

	/* we never get here, buy anyway */
	lsl_destroy_outlet(outlet);

	return 0;
}
