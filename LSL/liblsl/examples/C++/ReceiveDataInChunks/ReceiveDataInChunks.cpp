#include "../../../include/lsl_cpp.h"
#include <iostream>
using namespace std;

// define a packed sample struct (here a stereo sample).
#pragma pack(1)
struct stereo_sample { 
	short l,r; 
};

int main(int argc, char* argv[]) {
	try {

		// resolve the stream of interest & make an inlet
		lsl::stream_inlet inlet(lsl::resolve_stream("name","MyAudioStream")[0]);

		// and retrieve the chunks (note: this can of course also be done with pure std::vectors instead of stereo_samples)
		while (true) {
			vector<stereo_sample> result;
			if (double timestamp = inlet.pull_chunk_numeric_structs(result))
				cout << timestamp << endl; // only showing the time stamps here
		}

	} catch(std::exception &e) {
		cerr << "Got an exception: " << e.what() << endl;
	}
	cout << "Press any key to exit. " << endl; cin.get();
	return 0;
}
