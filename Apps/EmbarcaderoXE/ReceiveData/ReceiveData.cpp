//---------------------------------------------------------------------------


#pragma hdrstop

#ifdef _DEBUG
	#undef _DEBUG  //prevent loading of debug version of library.
	#include "lsl_c.h" //must be "", not <>, otherwise get data corruption in console apps when using dll functions
	#define _DEBUG
#else
	#include "lsl_c.h"
#endif

#include <stdio.h>
#include <tchar.h>
//---------------------------------------------------------------------------

#pragma argsused

/**
* Example program that demonstrates how to resolve a specific stream on the lab network and how to connect to it in order to receive data.
*/
#define MAX_CHANNELS 128
int _tmainx(int argc, _TCHAR* argv[])
{
	unsigned k,t;			/* channel index */
	lsl_streaminfo info;	/* the streaminfo returned by the resolve call */
	lsl_inlet inlet;		/* a stream inlet to get samples from */
	int errcode;			/* error code (lsl_lost_error or timeouts) */
	float cursample[MAX_CHANNELS];		/* array to old our current sample */
	double timestamp;		/* time stamp of the current sample (in sender time) */

	/* resolve the stream of interest (providing a 1-element results array, type shall be EEG, resolve at least 1 stream, waits forever if it has to) */
	printf("Now waiting for an Phasespace stream...\n");
	lsl_resolve_byprop(&info,1, "type","Mocap", 1, LSL_FOREVER);

	/* make an inlet to read data from the stream (buffer max. 300 seconds of data, no preference regarding chunking, automatic recovery enabled) */
	inlet = lsl_create_inlet(info, 300, LSL_NO_PREFERENCE, 1);

	/* subscribe to the stream (optional, but a nice way of getting error information) */
	lsl_open_stream(inlet,LSL_FOREVER,&errcode);
	if (errcode != 0)
		return errcode;

	int nChannels = lsl_get_channel_count(info);
	if(nChannels > MAX_CHANNELS) {
		printf("nChannels is larger than MAX_CHANNELS.\n");
	}

	printf("Displaying data...\n");
	for(t=0;;t++) {
		/* get the next sample (and a timestamp for it) */
		timestamp = lsl_pull_sample_f(inlet,cursample,nChannels,1.0,&errcode);

		if(errcode != 0) {
				printf("err: %d", errcode);
				break;
		}
		/* print the data */
		for (k=0; k<nChannels; ++k)
			printf("\t%.2f",cursample[0]);
		printf("\n");
	}

	/* we never get here, but anyway */
	lsl_destroy_inlet(inlet);

	return 0;
}
//---------------------------------------------------------------------------


//receive hotspot events.
int _tmain(int argc, _TCHAR* argv[])
{
	unsigned k,t;			/* channel index */
	lsl_streaminfo info;	/* the streaminfo returned by the resolve call */
	lsl_inlet inlet;		/* a stream inlet to get samples from */
	int errcode;			/* error code (lsl_lost_error or timeouts) */
	float cursample[MAX_CHANNELS];		/* array to old our current sample */
	double timestamp;		/* time stamp of the current sample (in sender time) */

	/* resolve the stream of interest (providing a 1-element results array, type shall be EEG, resolve at least 1 stream, waits forever if it has to) */
	printf("Now waiting for an Phasespace stream...\n");
	lsl_resolve_byprop(&info,1, "type","Mocap", 1, LSL_FOREVER);

	/* make an inlet to read data from the stream (buffer max. 300 seconds of data, no preference regarding chunking, automatic recovery enabled) */
	inlet = lsl_create_inlet(info, 300, LSL_NO_PREFERENCE, 1);

	/* subscribe to the stream (optional, but a nice way of getting error information) */
	lsl_open_stream(inlet,LSL_FOREVER,&errcode);
	if (errcode != 0)
		return errcode;

	int nChannels = lsl_get_channel_count(info);
	if(nChannels > MAX_CHANNELS) {
		printf("nChannels is larger than MAX_CHANNELS.\n");
	}

	printf("Displaying data...%d Channels.\n", nChannels);
	for(t=0;;t++) {
		/* get the next sample (and a timestamp for it) */
		timestamp = lsl_pull_sample_f(inlet,cursample,nChannels,LSL_FOREVER,&errcode);

		if(errcode != 0) {
				printf("err: %d", errcode);
				break;
		}
		/* print the data */
		if(t%480 == 0) {
		for (k=nChannels-4; k<nChannels; ++k)
			printf("%d ",(int)(cursample[k]*1000));
		printf("\n");
		}
	}

	/* we never get here, but anyway */
	lsl_destroy_inlet(inlet);

	return 0;
}
