#include "../../../include/lsl_cpp.h"
#include <iostream>
#include <time.h>
#include <stdlib.h>
using namespace std;

/**
* This example program offers an 8-channel stream, float-formatted, that resembles EEG data.
* The example demonstrates also how per-channel meta-data can be specified using the .desc() field of the stream information object.
*
* Note that the timer used in the send loop of this program is not particularly accurate.
*/


const char *channels[] = {"C3","C4","Cz","FPz","POz","CPz","O1","O2"};

int main(int argc, char* argv[]) {
	string name, type;
	if (argc != 3) {
		cout << "This opens a stream under some user-defined name and with a user-defined content type." << endl;
		cout << "Please enter the stream name and the stream type (e.g. \"BioSemi EEG\" (without the quotes)):" << endl;
		cin >> name >> type;
	} else {
		name = argv[1];
		type = argv[2];
	}

	try {

		// make a new stream_info (100 Hz)
		lsl::stream_info info(name,type,8,100,lsl::cf_float32,string(name)+=type);

		// add some description fields
		info.desc().append_child_value("manufacturer", "BioSemi");
		lsl::xml_element chns = info.desc().append_child("channels");
		for (int k=0;k<8;k++)
			chns.append_child("channel")
			.append_child_value("label",channels[k])
			.append_child_value("unit","microvolts")
			.append_child_value("type","EEG");

		// make a new outlet
		lsl::stream_outlet outlet(info);

		// send data forever
		cout << "Now sending data... " << endl;
		double starttime = ((double)clock())/CLOCKS_PER_SEC;
		for(unsigned t=0;;t++) {
			
			// wait a bit and create random data
			while (((double)clock())/CLOCKS_PER_SEC < starttime + t*0.01);
			float sample[8];
			for (int c=0;c<8;c++)
				sample[c] = (float)((rand()%1500)/500.0-1.5);

			// send the sample
			outlet.push_sample(sample);
		}

	} catch(std::exception &e) {
		cerr << "Got an exception: " << e.what() << endl;
	}
	cout << "Press any key to exit. " << endl; cin.get();
	return 0;
}
