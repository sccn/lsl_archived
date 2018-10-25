#include <lsl_c.h>
#include <stdio.h>
#include <stdlib.h>

/**
* This example program offers a 1-channel stream which contains strings. 
* The stream has the "Marker" content type and irregular rate.
 */

int main(int argc, char* argv[]) {
	lsl_streaminfo info;		/* out stream declaration object */
	lsl_outlet outlet;			/* stream outlet */
	double endtime;				/* used for send timing */
	const char *mrk;			/* marker to send next */

	/* declare a new streaminfo (name: "MyEventStream", content type: "Markers", 1 channel, irregular rate, ... */
	/* ... string values, some made-up source id (can also be empty) */
	const char* name = argc > 1 ? argv[1] : "MyEventStream";
	info = lsl_create_streaminfo(name, "Markers", 1, LSL_IRREGULAR_RATE, cft_string, "id23443");

	/* make a new outlet (chunking: default, buffering: 360k markers) */
	outlet = lsl_create_outlet(info,0,360);

	/* send random marker streams (note: this loop is keeping the CPU busy, normally one would sleep or yield here) */
	printf("Now sending markers...\n");

	const char *markertypes[] = {"Test", "Blah", "Marker", "XXX", "Testtest", "Test-1-2-3"};

	while(1) {
		/* wait for a random period of time */
		endtime = lsl_local_clock() + (rand()%1000)/1000.0;
		while (lsl_local_clock() < endtime);
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
