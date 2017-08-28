#include <cv.h>
#include <highgui.h>
#include "lsl_c.h"

char default_query[] = "type='VideoRaw'";

int main(int argc, char *argv[]) {
	char *query;				/* the query used to find the video stream */
	lsl_streaminfo result,info;	/* the streaminfo returned by the resolve call, and the one associated with the inlet */
	lsl_inlet inlet;			/* a stream inlet to get samples from */
	int errcode;				/* error code (lsl_lost_error or timeouts) */
	int width,height,channels;	/* image parameters */
	IplImage* img;				/* OpenCV image struct */
	char *name;					/* name of the stream that we resolved */

	query = argc>1 ? argv[1] : default_query;
	while (1) {
		/* resolve the stream of interest (result array, array capacity: 1 element, predicate is our query, resolve at least 1 stream, wait forever if necessary) */
		printf("Now waiting for a stream with %s...",query);
		lsl_resolve_bypred(&result,1,query,1,LSL_FOREVER);
		printf("done.\n");

		/* make an inlet to read data from the stream (buffer max. 1 seconds of data, no preference regarding chunking, automatic recovery disabled) */
		inlet = lsl_create_inlet(result, 1, LSL_NO_PREFERENCE, 0);

		/* get the stream meta-data */
		info = lsl_get_fullinfo(inlet,LSL_FOREVER,&errcode);
		name = lsl_get_name(info);
		width = atoi(lsl_child_value_n(lsl_child(lsl_get_desc(info),"encoding"),"width"));
		height = atoi(lsl_child_value_n(lsl_child(lsl_get_desc(info),"encoding"),"height"));
		channels = atoi(lsl_child_value_n(lsl_child(lsl_get_desc(info),"encoding"),"color_channels"));

		/* allocate an OpenCV image*/
		img = cvCreateImage(cvSize(width,height),IPL_DEPTH_8S,channels);

		/* create a window */
		cvNamedWindow(name, CV_WINDOW_AUTOSIZE);
		cvMoveWindow(name, 100, 100);

		while (true) {
			/* get a new sample */
			lsl_pull_sample_c(inlet,img->imageData,width*height*channels,LSL_FOREVER,&errcode);
			if (errcode)
				break;
			/* display */
			cvShowImage(name, img);
			if ((char)cvWaitKey(1)==27)
				return 0;
		}
		cvReleaseImage(&img);
		cvDestroyWindow(name);
		lsl_destroy_inlet(inlet);
		lsl_destroy_streaminfo(info);
		lsl_destroy_streaminfo(result);
	}
	
    return 0;
}
