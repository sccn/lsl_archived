#include <cmath>
#include <iostream>
#include <lsl_cpp.h>
#include <thread>

// define a packed sample struct (here: a 16 bit stereo sample).
#pragma pack(1)
struct stereo_sample {
	int16_t l, r;
};

int main(int, char* []) {
	try {
		// make a new stream_info (44.1Khz, 16bit, audio, 2 channels) and open an outlet with it
		lsl::stream_info info("MyAudioStream", "Audio", 2, 44100, lsl::cf_int16);
		lsl::stream_outlet outlet(info);

		std::cout << "Now sending data..." << std::endl;
		auto nextsample = std::chrono::high_resolution_clock::now();
		std::vector<stereo_sample> mychunk(4410);
		int phase = 0;
		for (unsigned c = 0;; c++) {
			// wait a bit and generate a chunk of random data
			nextsample += std::chrono::milliseconds(100);
			std::this_thread::sleep_until(nextsample);

			for (stereo_sample& sample : mychunk) {
				sample.l = static_cast<int16_t>(1000 * sin(phase / 200.));
				sample.r = static_cast<int16_t>(1000 * sin(phase / 400.));
				phase++;
			}

			// send it
			outlet.push_chunk_numeric_structs(mychunk);
		}

	} catch (std::exception& e) { std::cerr << "Got an exception: " << e.what() << std::endl; }
	std::cout << "Press any key to exit. " << std::endl;
	std::cin.get();
	return 0;
}
