#include <lsl_c.h>
#include <stdio.h>

/**
* Example program that demonstrates how to resolve a specific stream on the lab network and how
* to connect to it in order to receive string-formatted markers.
*/

int main(int argc, char* argv[]) {

	lsl_streaminfo info;	/* the streaminfo returned by the resolve call */
	lsl_inlet inlet;		/* a stream inlet to get samples from */
	int errcode;			/* error code (lsl_lost_error or timeouts) */
	char *cursample;		/* array to hold our current sample */
	double timestamp;		/* time stamp of the current sample (in sender time) */

	/* resolve the stream of interest (result array: info, array capacity: 1 element, type shall be EEG, resolve at least 1 stream, wait forever if necessary) */
	printf("Now waiting for a Markers stream...\n");
	lsl_resolve_byprop(&info,1, "type","Markers", 1, LSL_FOREVER);

	/* make an inlet to read data from the stream (buffer max. 300k markers, no preference regarding chunking, automatic recovery enabled) */
	inlet = lsl_create_inlet(info, 300, LSL_NO_PREFERENCE, 1);

	/* start receiving & displaying the data */
	printf("Displaying data...\n");
	while (1) {
		timestamp = lsl_pull_sample_str(inlet,&cursample,1,LSL_FOREVER,&errcode);
		printf("got: %s at time %.3f\n",cursample,timestamp);
	}

	/* we never get here, but anyway */
	lsl_destroy_inlet(inlet);

	return 0;
}
