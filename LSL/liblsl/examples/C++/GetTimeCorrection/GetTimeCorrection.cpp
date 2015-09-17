#include "../../../include/lsl_cpp.h"
#include <iostream>
using namespace std;

/**
* This example demonstrates how a time-correction value can be obtained on demand for a particular stream on the net.
* This time-correction value, when added to the time stamp of an obtained sample, remaps the sample's time stamp into the 
* local clock domain (so it is in the same domain as what lsl::local_clock() would return). For streams coming from the same
* computer, this value should be approx. 0 (up to some tolerance).
*/

int main(int argc, char* argv[]) {
	string field, value;
	if (argc != 3) {
		cout << "This connects to a stream which has a particular value for a given field and gets the time-synchronization information for it." << endl;
		cout << "Please enter a field name and the desired value (e.g. \"type EEG\" (without the quotes)):" << endl;
		cin >> field >> value;
	} else {
		field = argv[1];
		value = argv[2];
	}

	try {

		// resolve the stream of interet
		cout << "Now resolving streams..." << endl;
		vector<lsl::stream_info> results = lsl::resolve_stream(field,value);

		cout << "Here is what was resolved: " << endl;
		cout << results[0].as_xml() << endl;

		// make an inlet to get data from it
		cout << "Now creating the inlet..." << endl;
		lsl::stream_inlet inlet(results[0]);

		// start receiving & displaying the data
		cout << "Press [Enter] to query a new correction value (clocks may drift)..." << endl;
		while (true) {
			cout << inlet.time_correction() << endl;
			cin.get();
		}

	} catch(std::exception &e) {
		cerr << "Got an exception: " << e.what() << endl;
	}
	cout << "Press any key to exit. " << endl; cin.get();
	return 0;
}
