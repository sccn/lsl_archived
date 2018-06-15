#include "../../../include/lsl_cpp.h"
#include <iostream>
using namespace std;

/**
* This example program demonstrates how the full version of the stream info (i.e. including the potentially large .desc() field) can be 
* obtained from an inlet. Note that the output of the resolve functions only includes the core information otherwise.
*/

int main(int argc, char* argv[]) {
	string field, value;
	if (argc != 3) {
		cout << "This connects to a stream which has a particular value for a given field and displays its full stream_info contentss." << endl;
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

		// get & display the info
		cout << "The information about this stream is displayed in the following: " << endl;
		lsl::stream_info info = inlet.info();
		cout << info.as_xml() << endl;

	} catch(std::exception &e) {
		cerr << "Got an exception: " << e.what() << endl;
	}
	cout << "Press any key to exit. " << endl; cin.get();
	return 0;
}
