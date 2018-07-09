#include <iostream>
#include <lsl_cpp.h>

/**
 * This example program shows how all streams that are currently present on the lab network can be
 * resolved and displayed. This is useful for browsing applications.
 */

int main(int argc, char* argv[]) {
	try {
		std::cout << "Here is a one-shot resolve of all current streams:" << std::endl;

		// discover all streams on the network
		std::vector<lsl::stream_info> results = lsl::resolve_streams();

		// display them
		for (auto& stream: results) std::cout << stream.as_xml() << "\n\n";

		std::cout << "Press any key to switch to the continuous resolver test: " << std::endl;
		std::cin.get();

		lsl::continuous_resolver r;
		while (true) {
			std::vector<lsl::stream_info> tmp = r.results();
			for (auto& val : tmp) std::cout << val.name() << " ";
			std::cout << std::endl;
		}

	} catch (std::exception& e) { std::cerr << "Got an exception: " << e.what() << std::endl; }
	std::cout << "Press any key to exit. " << std::endl;
	std::cin.get();
	return 0;
}
