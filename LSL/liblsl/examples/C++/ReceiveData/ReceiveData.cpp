#include "../../../include/lsl_cpp.h"
#include <iostream>
using namespace std;

/**
* Example program that demonstrates how to resolve a specific stream on the lab network and how to connect to it in order to receive data.
*/

int main(int argc, char* argv[]) {
	string field, value;
	if (argc != 3) {
		cout << "This connects to a stream which has a particular value for a given field and receives data." << endl;
		cout << "Please enter a field name and the desired value (e.g. \"type EEG\" (without the quotes)):" << endl;
		cin >> field >> value;
	} else {
		field = argv[1];
		value = argv[2];
	}

	//try {

		// resolve the stream of interet
		cout << "Now resolving streams..." << endl;
		vector<lsl::stream_info> results = lsl::resolve_stream(field,value);

		cout << "Here is what was resolved: " << endl;
		cout << results[0].as_xml() << endl;

		// make an inlet to get data from it
		cout << "Now creating the inlet..." << endl;
		lsl::stream_inlet inlet(results[0]);

		// start receiving & displaying the data
		cout << "Now pulling samples..." << endl;
		while (true) {
			// get the sample
			float sample[8];
			double ts = inlet.pull_sample(sample,8);

			// display
			for (unsigned c=0;c<8;c++)
				cout << "\t" << sample[c];
			cout << endl;
		}

	//} catch(std::exception &e) {
	//	cerr << "Got an exception: " << e.what() << endl;
	//}
	cout << "Press any key to exit. " << endl; cin.get();
	return 0;
}
