#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <lsl_cpp.h>
#include <numeric>
#include <string>

/* Test the latency between sending a sample and receiving it*/

int main(int, char**) {
	constexpr int numBounces = 10000;
	double timestamps[numBounces][2];

	auto streaminfo = lsl::stream_info("Sender", "Bounce", 1, lsl::IRREGULAR_RATE, lsl::cf_int32);
	lsl::stream_outlet outlet(streaminfo);

	auto found_stream_info = lsl::resolve_stream("name", "Sender");
	if (found_stream_info.empty()) throw std::runtime_error("Sender outlet not found!");
	lsl::stream_info si = found_stream_info[0];
	std::cout << "Found " << si.name() << '@' << si.hostname() << std::endl;

	lsl::stream_inlet inlet(found_stream_info[0]);

	// push a single sample first
	int dummy = 0;
	outlet.push_sample(&dummy, 0, true);
	inlet.pull_sample(&dummy, 1, 2);

	std::cout << "Starting bounce loop" << std::endl;
	for (int32_t counter = 0; counter < numBounces; counter++) {
		int32_t received_counter;
		timestamps[counter][0] = lsl::local_clock();
		outlet.push_sample(&counter, timestamps[counter][0], true);
		inlet.pull_sample(&received_counter, 1, 2);
		timestamps[counter][1] = lsl::local_clock();
		if (received_counter != counter) throw std::runtime_error("Got the wrong sample!");
	}

	double latencies[numBounces];
	double meanLatency = 0;
	for (int i = 1; i < numBounces; i++) {
		latencies[i] = (timestamps[i][1] - timestamps[i][0]);
		meanLatency += latencies[i] / numBounces;
	}
	double sdLatency = 0;
	for (double x_i : latencies)
		sdLatency += ((x_i - meanLatency) * (x_i - meanLatency)) / numBounces;
	sdLatency = std::sqrt(sdLatency);

	std::cout << "Bounced " << numBounces << " samples, latency M=" << (meanLatency * 1000)
	          << "ms, SD=" << (sdLatency * 1000) << "ms" << std::endl;
	std::ofstream tscsv("bounce.csv");
	if (tscsv.is_open()) {
		tscsv << "t1\tt2\n" << std::setprecision(15) << std::fixed;
		for (auto& timestamp : timestamps) tscsv << timestamp[0] << '\t' << timestamp[1] << '\n';
		std::cout << "Wrote timestamps to bounce.csv" << std::endl;
	}

	return 0;
}
