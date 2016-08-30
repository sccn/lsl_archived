#include "../../../include/lsl_c.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/**
 * This example program offers an 8-channel stream, float-formatted, that resembles EEG data.
 * The example demonstrates also how per-channel meta-data can be specified using the description field of the streaminfo object.
 *
 * Note that the timer used in the send loop of this program is not particularly accurate.
 */

char *channels[] = {"C3","C4","Cz","FPz","POz","CPz","O1","O2"};

int main(int argc, char* argv[]) {
	int t,c;					/* time point and channel index */
	lsl_streaminfo info;		/* out stream declaration object */
	lsl_xml_ptr desc, chn, chns;/* some xml element pointers */ 
	lsl_outlet outlet;			/* stream outlet */
	double starttime;			/* used for send timing */
	float cursample[8];			/* the current sample */

	/* declare a new streaminfo (name: BioSemi, content type: EEG, 8 channels, 100 Hz, float values, some made-up device id (can also be empty) */
	info = lsl_create_streaminfo("BioSemi","EEG",8,100,cft_float32,"325wqer4354");

	/* add some meta-data fields to it */
	/* (for more standard fields, see https://github.com/sccn/xdf/wiki/Meta-Data) */
	desc = lsl_get_desc(info);
	lsl_append_child_value(desc,"manufacturer","BioSemi");
	chns = lsl_append_child(desc,"channels");
	for (c=0;c<8;c++) {
		chn = lsl_append_child(chns,"channel");
		lsl_append_child_value(chn,"label",channels[c]);
		lsl_append_child_value(chn,"unit","microvolts");
		lsl_append_child_value(chn,"type","EEG");
	}

	/* make a new outlet (chunking: default, buffering: 360 seconds) */
	outlet = lsl_create_outlet(info,0,360);

	/* send data forever (note: this loop is keeping the CPU busy, normally one would sleep or yield here) */
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
