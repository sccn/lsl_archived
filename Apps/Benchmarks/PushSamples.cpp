#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <iostream>
#include <lsl_cpp.h>
#include <map>
#include <string>
#include <thread>

using std::chrono::milliseconds;

template <typename T>
void push_fn(const std::vector<T> buffer, bool push_single, uint chunksize, lsl::stream_outlet& out) {
	if (push_single)
		for (uint i = 0; i < chunksize; ++i) out.push_sample(buffer);
	else
		out.push_chunk_multiplexed(buffer, out.info().channel_count() * chunksize);
}
template <typename T>
void pull_fn(std::vector<T> buffer, bool pull_single, uint chunksize, int numchans, lsl::stream_inlet& in) {
	std::vector<std::vector<T>> buffer_;
	if (pull_single)
		for (uint i = 0; i < chunksize; ++i) in.pull_sample(buffer);
	else
		//in.pull_chunk_multiplexed(buffer.data(), nullptr, numchans * chunksize, 0);
		in.pull_chunk(buffer_);
}

int main(int argc, char* argv[]) {
	if (argc == 2 && argv[1] == std::string("-h")) {
		std::cout << "Usage: " << argv[0] << ": [numchans=1] [format_str=0] [maxsamples=5000000]"
		          << "[chunksize=10000] "
		          << "[push_single=0] "
		          << "[pull_single=0] " << std::endl;
		std::cout << "\tformat_str: 0 for float, 1 for string samples\n";
		std::cout << "\tpush_single / pull_single: 0 for chunked operation, "
		          << "1 for handling each sample on its own (likely much slower)\n\n";
		std::cout << "Example:\n\t ./push_sample 5 1\n\t"
		          << "Pushes the default number of samples in 5 string channels" << std::endl;
		return 0;
	}
	const uint numchans = argc > 1 ? std::stoul(argv[1]) : 1;
	const auto format_str = argc > 2 && *argv[2] == '1';
	const uint maxsamples = argc > 3 ? std::stoul(argv[3]) : 5000000;
	const uint chunksize = argc > 4 ? std::stoul(argv[4]) : 10000;
	const bool push_single = argc > 5 && *argv[5] == '1';
	const bool pull_single = argc > 6 && *argv[6] == '1';

	std::cout << "Starting speed test with " << numchans << " channels ("
	          << (format_str ? "string" : "float32") << "), " << maxsamples << " samples\n"
	          << "Pushing single samples: " << push_single << "\nPulling single samples: " << pull_single
	          << std::endl;
	{
		const auto name = std::string(argv[0]) + '_' + (format_str ? "str" : "float") + 'x' +
		                  std::to_string(numchans);
		std::cout << "Publishing stream " << name << std::endl;

		lsl::stream_info info(name, "Benchmark", numchans, lsl::IRREGULAR_RATE,
		                      format_str ? lsl::cf_string : lsl::cf_float32);
		lsl::stream_outlet outlet(info);

		lsl::stream_inlet inlet(lsl::resolve_stream("name", name, 1, 5)[0]);
		inlet.open_stream(2.);
		outlet.wait_for_consumers(2.);

		const uint buffersize = numchans * chunksize;
		std::vector<float> samples_float(format_str ? 0 : buffersize, 17.3f);
		std::vector<std::string> samples_str(format_str ? buffersize : 0, "test");

		if (format_str)
			push_fn<std::string>(samples_str, false, chunksize, outlet);
		else
			push_fn<float>(samples_float, false, chunksize, outlet);

		double outlettime = 0, inlettime = 0;
		for (uint chunk = 0; chunk < maxsamples / chunksize; chunk++) {
			double starttime = lsl::lsl_local_clock();
			if (format_str)
				push_fn(samples_str, push_single, chunksize, outlet);
			else
				push_fn(samples_float, push_single, chunksize, outlet);
			outlettime += lsl::lsl_local_clock() - starttime;

			starttime = lsl::lsl_local_clock();
			if (format_str)
				pull_fn(samples_str, pull_single, chunksize, numchans, inlet);
			else
				pull_fn(samples_float, pull_single, chunksize, numchans, inlet);
			inlettime += lsl::lsl_local_clock() - starttime;
		}
		std::cout << "Inlet: " << maxsamples << " in " << static_cast<int>(inlettime * 1000)
		          << " ms (" << static_cast<int>(maxsamples / inlettime) << " samples/s)"
		          << std::endl;
		std::cout << "Outlet: " << maxsamples << " in " << static_cast<int>(outlettime * 1000)
		          << " ms (" << static_cast<int>(maxsamples / outlettime) << " samples/s)"
		          << std::endl;
	}
	if (argc == 1) {
		std::cout << "Press [Enter] to exit" << std::endl;
		std::cin.get();
	}
	return 0;
}
