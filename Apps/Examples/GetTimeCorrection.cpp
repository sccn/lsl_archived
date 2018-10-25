#include <iostream>
#include <lsl_cpp.h>

/**
 * This example demonstrates how a time-correction value can be obtained on demand for a particular
 * stream on the net. This time-correction value, when added to the time stamp of an obtained
 * sample, remaps the sample's time stamp into the local clock domain (so it is in the same domain
 * as what lsl::local_clock() would return). For streams coming from the same computer, this value
 * should be approx. 0 (up to some tolerance).
 */

int main(int argc, char* argv[]) {
	std::string field, value;
	if (argc != 3) {
		std::cout << "This connects to a stream which has a particular value for a given field and "
		             "gets the time-synchronization information for it."
		          << std::endl;
		std::cout << "Please enter a field name and the desired value (e.g. \"type EEG\" (without "
		             "the quotes)):"
		          << std::endl;
		std::cin >> field >> value;
	} else {
		field = argv[1];
		value = argv[2];
	}

	try {
		// resolve the stream of interet
		std::cout << "Now resolving streams..." << std::endl;
		std::vector<lsl::stream_info> results = lsl::resolve_stream(field, value);

		std::cout << "Here is what was resolved: " << std::endl;
		std::cout << results[0].as_xml() << std::endl;

		// make an inlet to get data from it
		std::cout << "Now creating the inlet..." << std::endl;
		lsl::stream_inlet inlet(results[0]);

		// start receiving & displaying the data
		std::cout << "Press [Enter] to query a new correction value (clocks may drift)..."
		          << std::endl;
		while (true) {
			std::cout << inlet.time_correction() << std::endl;
			std::cin.get();
		}

	} catch (std::exception& e) { std::cerr << "Got an exception: " << e.what() << std::endl; }
	std::cout << "Press any key to exit. " << std::endl;
	std::cin.get();
	return 0;
}
