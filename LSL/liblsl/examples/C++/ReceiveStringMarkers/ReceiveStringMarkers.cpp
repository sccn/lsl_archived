#include "../../../include/lsl_cpp.h"
#include <iostream>
using namespace std;

/**
 * This example program demonstrates how a Marker string stream on the network (here of any name) can be resolved, 
 * and how the marker strings (assumed to be delivered at irregular rate) can be pulled from their source
 */

int main(int argc, char* argv[]) {
	try {

		// resolve the stream of interet
		cout << "Now resolving a marker stream..." << endl;
		vector<lsl::stream_info> results = lsl::resolve_stream("type","Markers");

		cout << "Here is what was resolved: " << endl;
		cout << results[0].as_xml() << endl;

		// make an inlet to get data from it
		cout << "Now creating the inlet..." << endl;
		lsl::stream_inlet inlet(results[0]);

		// start receiving & displaying the data
		while (true) {
			string sample;
			inlet.pull_sample(&sample,1);
			cout << "got: " << sample << endl;
		}

	} catch(std::exception &e) {
		cerr << "Got an exception: " << e.what() << endl;
	}
	cout << "Press any key to exit. " << endl; cin.get();
	return 0;
}
