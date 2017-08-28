#include <lsl_cpp.h>
#include <stdlib.h>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds
using namespace lsl;

/**
 * This is an example of how a simple data stream can be offered on the network. 
 * Here, the stream is named SimpleStream, has content-type EEG, and 8 channels.
 * The transmitted samples contain random numbers (and the sampling rate is irregular)
 */

constexpr int nchannels = 8;

int main(int argc, char* argv[]) {
	// make a new stream_info (nchannelsch) and open an outlet with it
	stream_info info(argc > 1 ? argv[1] : "SimpleStream", "EEG", nchannels);
	stream_outlet outlet(info);

	// send data forever
	float sample[nchannels];
	while(true) {
		// generate random data
		for (int c=0;c<nchannels;c++) sample[c] = (rand()%1500)/500.0-1.5;
		// send it
		outlet.push_sample(sample);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	return 0;
}
