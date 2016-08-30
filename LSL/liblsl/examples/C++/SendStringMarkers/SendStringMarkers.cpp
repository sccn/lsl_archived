#include "../../../include/lsl_cpp.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>
using namespace std;

/**
* This example program offers a 1-channel stream which contains strings (here representing numbers). 
* The stream has the "Marker" content type and irregular rate. The name of the stream can be chosen as a startup parameter.
*/

const char *markertypes[] = {"Test", "Blah", "Marker", "XXX", "Testtest", "Test-1-2-3"};

int main(int argc, char* argv[]) {
	try {

		// make a new stream_info
		lsl::stream_info info("MyEventStream","Markers",1,lsl::IRREGULAR_RATE,lsl::cf_string,"myuniquesourceid23443");

		// make a new outlet
		lsl::stream_outlet outlet(info);

		// send random marker strings
		cout << "Now sending markers... " << endl;
		while(true) {
			// wait for a random period of time
			double endtime = ((double)clock())/CLOCKS_PER_SEC + (rand()%1000)/1000.0;
			while (((double)clock())/CLOCKS_PER_SEC < endtime);
			// and choose the marker to send
			string mrk = markertypes[rand() % (sizeof(markertypes)/sizeof(markertypes[0]))];
			cout << "now sending: " << mrk << endl;

			// now send it (note the &)
			outlet.push_sample(&mrk);
		}

	} catch(std::exception &e) {
		cerr << "Got an exception: " << e.what() << endl;
	}
	cout << "Press any key to exit. " << endl; cin.get();
	return 0;
}
