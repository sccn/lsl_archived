#include <iostream>
#include <lsl_cpp.h>
#include <memory>
#include <string>

/**
 * This example program demonstrates how a Marker string stream on the network (here of any name)
 * can be resolved, and how the marker strings (assumed to be delivered at irregular rate) can be
 * pulled from their source
 */

int main(int, char**) {
	// resolve the stream of interet
	std::vector<std::unique_ptr<lsl::stream_inlet>> inlets;

	for (auto& stream_info : lsl::resolve_stream("type", "Markers")) {
		if (stream_info.channel_count() != 1)
			std::cerr << "Skipping stream " << stream_info.name()
			          << " because it has more than one channel" << std::endl;
		else {
			inlets.emplace_back(new lsl::stream_inlet(stream_info));
			std::cout << "Listening to " << stream_info.name() << std::endl;
		}
	}
	if (inlets.empty()) {
		std::cerr << "No marker stream found" << std::endl;
		return 0;
	}

	std::string sample;
	double starttime = lsl::local_clock();
	while (true) {
		for (auto& inlet : inlets)
			if (double ts = inlet->pull_sample(&sample, 1, .2))
				std::cout << (ts - inlet->time_correction(1) - starttime) << '\t' << sample
				          << std::endl;
	}
	return 0;
}
