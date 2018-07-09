#include <iostream>
#include <lsl_cpp.h>

// define a packed sample struct (here a stereo sample).
#pragma pack(1)
struct stereo_sample {
	int16_t l, r;
};

int main(int, char* []) {
	try {

		// resolve the stream of interest & make an inlet
		lsl::stream_inlet inlet(lsl::resolve_stream("name", "MyAudioStream")[0]);

		// and retrieve the chunks (note: this can of course also be done with pure std::vectors
		// instead of stereo_samples)
		while (true) {
			std::vector<stereo_sample> result;
			if (double timestamp = inlet.pull_chunk_numeric_structs(result))
				std::cout << timestamp << std::endl; // only showing the time stamps here
		}

	} catch (std::exception& e) { std::cerr << "Got an exception: " << e.what() << std::endl; }
	std::cout << "Press any key to exit. " << std::endl;
	std::cin.get();
	return 0;
}
