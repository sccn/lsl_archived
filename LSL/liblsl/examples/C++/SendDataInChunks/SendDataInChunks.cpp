#include "../../../include/lsl_cpp.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>
using namespace std;

// define a packed sample struct (here: a 16 bit stereo sample).
#pragma pack(1)
struct stereo_sample { 
	short l,r; 
};

int main(int argc, char* argv[]) {
	try {

		// make a new stream_info (44Khz, 16bit, audio, 2 channels) and open an outlet with it
		lsl::stream_info info("MyAudioStream","Audio",2,44100,lsl::cf_int16);
		lsl::stream_outlet outlet(info);

		cout << "Now sending data...";
		double starttime = ((double)clock())/CLOCKS_PER_SEC;
		for (unsigned c=0;;c++) {

			// wait a bit and generate a chunk of random data
			while (((double)clock())/CLOCKS_PER_SEC < starttime + c*0.01);
			vector<stereo_sample> mychunk(441);
			for (unsigned t=0;t<mychunk.size();t++) {
				mychunk[t].l = rand();
				mychunk[t].r = rand();
			}

			// send it
			outlet.push_chunk_numeric_structs(mychunk);
		}

	} catch(std::exception &e) {
		cerr << "Got an exception: " << e.what() << endl;
	}
	cout << "Press any key to exit. " << endl; cin.get();
	return 0;
}
