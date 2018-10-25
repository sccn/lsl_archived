#include <iostream>
#include <lsl_cpp.h>
#include <string>

/**
 * Example program that demonstrates how to resolve a specific stream on the lab
 * network and how to connect to it in order to receive data.
 */

int main(int argc, char* argv[]) {
	std::string field, value;
	const int max_samples = argc > 3 ? std::stoi(argv[3]) : 10;
	if (argc < 3) {
		std::cout << "This connects to a stream which has a particular value for a "
		             "given field and receives data.\nPlease enter a field name and the desired "
		             "value (e.g. \"type EEG\" (without the quotes)):"
		          << std::endl;
		std::cin >> field >> value;
	} else {
		field = argv[1];
		value = argv[2];
	}

	// resolve the stream of interet
	std::cout << "Now resolving streams..." << std::endl;
	std::vector<lsl::stream_info> results = lsl::resolve_stream(field, value);

	std::cout << "Here is what was resolved: " << std::endl;
	std::cout << results[0].as_xml() << std::endl;

	// make an inlet to get data from it
	std::cout << "Now creating the inlet..." << std::endl;
	lsl::stream_inlet inlet(results[0]);

	// start receiving & displaying the data
	std::cout << "Now pulling samples..." << std::endl;

	std::vector<float> sample;
	for(int i=0; i < max_samples; ++i) {
		// get the sample
		double ts = inlet.pull_sample(sample);

		// display
		std::cout << ts << ':';
		for (auto c: sample) std::cout << "\t" << c;
		std::cout << std::endl;
	}

	if(argc == 1) {
		std::cout << "Press any key to exit. " << std::endl;
		std::cin.get();
	}
	return 0;
}
