/**********************************
 Java Native Interface library
**********************************/
#include <jni.h>
#include <android/log.h>
#include "lsl_cpp.h"
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
using namespace std;

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "SendStringMarkers", __VA_ARGS__))

char *markertypes[] = { (char*)"Test", (char*)"Blah", (char*)"Marker", (char*)"XXX", (char*)"Testtest", (char*)"Test-1-2-3" };

void send_markers() {

}

extern "C"
JNIEXPORT void JNICALL
Java_edu_ucsd_sccn_SendStringMarkers_sendstringmarkersNative( JNIEnv* env, jobject thiz )
{
	try {
		// make a new stream_info
		lsl::stream_info info("MyEventStream", "Markers", 1, lsl::IRREGULAR_RATE, lsl::cf_string, "myuniquesourceid23443");

		// make a new outlet
		lsl::stream_outlet outlet(info);

		// send random marker strings
		LOGI("Now sending markers... ");
		while (true) {
			// wait for a random period of time
			double endtime = ((double)clock()) / CLOCKS_PER_SEC + (rand() % 1000) / 1000.0;
			while (((double)clock()) / CLOCKS_PER_SEC < endtime);
			// and choose the marker to send
			string mrk = markertypes[rand() % (sizeof(markertypes) / sizeof(markertypes[0]))];
			LOGI("now sending: %s", mrk.c_str());

			// now send it (note the &)
			outlet.push_sample(&mrk);
		}

	}
	catch (std::exception &e) {
		LOGI(e.what());
	}
}
