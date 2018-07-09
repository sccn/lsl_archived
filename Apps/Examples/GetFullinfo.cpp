#include <iostream>
#include <lsl_cpp.h>

/**
 * This example program demonstrates how the full version of the stream info (i.e. including the
 * potentially large .desc() field) can be obtained from an inlet. Note that the output of the
 * resolve functions only includes the core information otherwise.
 */

int main(int argc, char* argv[]) {
	std::string field, value;
	if (argc != 3) {
		std::cout << "This connects to a stream which has a particular value for a given field and "
		             "displays its full stream_info contentss."
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

		// get & display the info
		std::cout << "The information about this stream is displayed in the following: "
		          << std::endl;
		lsl::stream_info info = inlet.info();
		std::cout << info.as_xml() << std::endl;

	} catch (std::exception& e) { std::cerr << "Got an exception: " << e.what() << std::endl; }
	std::cout << "Press any key to exit. " << std::endl;
	std::cin.get();
	return 0;
}
