#include <lsl_c.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * This example program offers an 8-channel stream, float-formatted, that resembles EEG data.
 * The example demonstrates also how per-channel meta-data can be specified using the description field of the streaminfo object.
 *
 * Note that the app sends data as fast as it can
 * Generally, you should sleep between sending samples and send multiple samples at once (push_chunk_*-functions)
 */

int main(int argc, char* argv[]) {
	printf("SendDataC example program. Sends 8 float channels as fast as possible.\n");
	printf("Usage: %s [streamname] [streamuid]\n", argv[0]);
	printf("Using lsl %d, lsl_library_info: %s\n", lsl_library_version(), lsl_library_info());
	const char* name = argc > 1 ? argv[1] : "SendDataC";
	const char* uid = argc > 2 ? argv[2] : "325wqer4354";
	/* declare a new streaminfo (name: SendDataC / argument 1, content type: EEG, 8 channels, 500 Hz, float values, some made-up device id (can also be empty) */
	lsl_streaminfo info = lsl_create_streaminfo(name, "EEG", 8, 500, cft_float32, uid);

	/* add some meta-data fields to it */
	/* (for more standard fields, see https://github.com/sccn/xdf/wiki/Meta-Data) */
	lsl_xml_ptr desc = lsl_get_desc(info);
	lsl_append_child_value(desc, "manufacturer", "LSL");
	const char *channels[] = {"C3","C4","Cz","FPz","POz","CPz","O1","O2"};
	lsl_xml_ptr chns = lsl_append_child(desc,"channels");
	for (int c=0;c<8;c++) {
		lsl_xml_ptr chn = lsl_append_child(chns,"channel");
		lsl_append_child_value(chn,"label",channels[c]);
		lsl_append_child_value(chn,"unit","microvolts");
		lsl_append_child_value(chn,"type","EEG");
	}

	/* make a new outlet (chunking: default, buffering: 360 seconds) */
	lsl_outlet outlet = lsl_create_outlet(info,0,360);

	do printf("Waiting for consumers\n");
	while(!lsl_wait_for_consumers(outlet, 120));
	
	printf("Now sending data...\n");
	
	/* send data until the last consumer has disconnected */
	for(int t=0;lsl_have_consumers(outlet);t++) {
		float cursample[8]; /* the current sample */
		cursample[0] = (float) t;
		for (int c=1;c<8;c++)
			cursample[c] = (float)((rand()%1500)/500.0-1.5);
		lsl_push_sample_f(outlet,cursample);
	}
	printf("Lost the last consumer, shutting down\n");
	lsl_destroy_outlet(outlet);
	return 0;
}
