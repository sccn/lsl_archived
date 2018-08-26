#include <lsl_cpp.h>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds

/**
 * This is an example of how a simple data stream can be offered on the network.
 * Here, the stream is named SimpleStream, has content-type EEG, and 8 channels.
 * The transmitted samples contain random numbers (and the sampling rate is irregular)
 */

const int nchannels = 8;

int main(int argc, char* argv[]) {
	// make a new stream_info (nchannelsch) and open an outlet with it
	lsl::stream_info info(argc > 1 ? argv[1] : "SimpleStream", "EEG", nchannels);
	lsl::stream_outlet outlet(info);

	while(!outlet.wait_for_consumers(120));

	// send data forever
	float sample[nchannels];
	while(outlet.have_consumers()) {
		// generate random data
		for (int c=0;c<nchannels;c++) sample[c] = (rand()%1500)/500.0-1.5;
		// send it
		outlet.push_sample(sample);
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}

	return 0;
}
