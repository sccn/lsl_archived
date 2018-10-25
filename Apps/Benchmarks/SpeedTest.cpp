#include <algorithm>
#include <atomic>
#include <chrono>
#include <cmath>
#include <iostream>
#include <lsl_cpp.h>
#include <map>
#include <string>
#include <thread>

// set to true by the main program when we're ready to quit
static std::atomic<bool> stop_inlet(false), stop_outlet(false), start_outlet(false);

// get the current time
double now() { return lsl::local_clock(); }

// initialize a sample with data
template <class T> std::vector<T> init_sample(int numchan) { return std::vector<T>(numchan, 17.3); }

// initialize a sample with data
template <class T> void init_sample(int numchan, std::vector<T>& sample) {
	sample.resize(numchan);
	std::fill(sample.begin(), sample.end(), 17.3f);
}

// run an outlet
template <class T>
void run_outlet(const lsl::stream_info& info, int chunk_len, unsigned int max_samples) {
	try {
		const auto numchan = (uint) info.channel_count();
		const double srate = info.nominal_srate();
		// create a new outlet
		lsl::stream_outlet outlet(info);
		std::cout << "outlet started." << std::endl;

		while (!start_outlet) std::this_thread::sleep_for(std::chrono::milliseconds(1));

		// initialize data to send
		std::vector<T> chunk((int)(numchan * (chunk_len * srate / 1000 * 5)), 17.3f);

		// send in bursts
		double start_time = lsl::local_clock();
		for (unsigned int target, written = 0; written < max_samples && !stop_outlet;) {
			std::this_thread::sleep_for(std::chrono::milliseconds(chunk_len));
			target = static_cast<unsigned int>((lsl::local_clock() - start_time) * srate);
			auto num_elements = std::min((target - written) * numchan, (uint) chunk.size());
			outlet.push_chunk_multiplexed(&chunk[0], num_elements);
			written += num_elements / numchan;
		}
		std::cout << "outlet finished." << std::endl;
	} catch (std::exception& e) {
		std::cerr << "ERROR during run_outlet() Stress-test function: " << e.what() << std::endl;
	}
}

// run an outlet
template <class T>
void run_singlesample_outlet(const lsl::stream_info& info, unsigned int max_samples) {
	try {
		// create a new streaminfo and outlet
		const auto numchan = (uint) info.channel_count();
		const double srate = info.nominal_srate();
		lsl::stream_outlet outlet(info);
		std::cout << "outlet started." << std::endl;

		while (!start_outlet) std::this_thread::sleep_for(std::chrono::milliseconds(1));

		// initialize data to send
		std::vector<T> sample(numchan, 17.3f);

		// send in bursts
		double start_time = lsl::local_clock();
		for (unsigned int target, written = 0; written < max_samples && !stop_outlet;) {
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			target = static_cast<unsigned int>((lsl::local_clock() - start_time) * srate);
			while(written++ < target) outlet.push_sample(sample);
		}
		std::cout << "outlet finished." << std::endl;
	} catch (std::exception& e) {
		std::cerr << "ERROR during run_outlet() Stress-test function: " << e.what() << std::endl;
	}
}


// run an inlet for some time (optionally with sporadic interruptions in between)
template <class T> void run_inlet(const std::string& type, bool in_chunks, int buffer_len) {
	try {
		std::vector<std::vector<T>> chunk;
		std::vector<T> sample;

		// resolve by type and create inlet
		std::vector<lsl::stream_info> results = lsl::resolve_stream("type", type);
		lsl::stream_inlet inlet(results[0], buffer_len);
		std::cout << "inlet started." << std::endl;
		start_outlet = true;

		// variables for data rate testing
		double starttime = lsl::local_clock(), next_display = starttime + 1;
		double ts, last_ts;

		// run
		for (uint k = 0, num_samples = 0; !stop_inlet; k++) {
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
			if (in_chunks) {
				inlet.pull_chunk(chunk);
				num_samples += static_cast<uint>(chunk.size());
			} else {
				while ((ts = inlet.pull_sample(sample, 0.0)) > 0) {
					last_ts = ts;
					num_samples++;
				}
			}
			// display code
			if (k % 50 == 0) {
				double now = lsl::local_clock();
				if (now > next_display) {
					std::cout << static_cast<uint64_t>(num_samples / (now - starttime)) << " samples/sec"
					     << std::endl; // " samples/sec; latency = " << 1000*(now-last_ts) << " ms." <<
					              // std::endl; // NOTE: testing latency requires that data is sent with
					              // a burst length of 0
					next_display = now + 1;
				}
			}
		}
		std::cout << "inlet finished." << std::endl;
	} catch (std::exception& e) {
		std::cerr << "ERROR during run_inlet() Stress-test function: " << e.what() << std::endl;
	}
}

int main(int argc, char* argv[]) {
	const double srate = argc > 1 ? std::stod(argv[1]) : 5000000;
	const int numchans = argc > 2 ? std::stoi(argv[2]) : 1;
	const std::map<std::string, lsl::channel_format_t> m{
	    {"int8", lsl::cf_int8},       {"int16", lsl::cf_int16},
	    {"int32", lsl::cf_int32},     {"int64", lsl::cf_int64},
	    {"float", lsl::cf_float32},   {"double", lsl::cf_double64},
	    {"float32", lsl::cf_float32}, {"double64", lsl::cf_double64},
	    {"string", lsl::cf_string}};
	const auto format_str = argc > 3 ? argv[3] : "float";
	const auto format = m.at(format_str);
	const int burstlen = argc > 4 ? std::stoi(argv[4]): 10;
	const int maxsamples =  argc > 5 ? std::stoi(argv[5]) : int(srate * 10);
	const int bufferlen = argc > 6 ? std::stoi(argv[6]) : 10;
	const bool chunked = argc > 7 ? std::stoi(argv[7]) != 0 : false;
	{
		const auto name = std::string(argv[0]) + '_' + format_str + 'x' + std::to_string(numchans);

		std::thread inlet(&run_inlet<float>, "Serial", chunked, bufferlen);
		lsl::stream_info info(name, "Serial", numchans, srate, format);
		if (chunked)
			std::thread(&run_outlet<float>, info, burstlen, maxsamples).join();
		else
			std::thread(&run_singlesample_outlet<float>, info, maxsamples).join();

		// take'em down
		stop_inlet = true;
		inlet.join();
	}
	if(argc == 1) {
		std::cout << "Press [Enter] to exit" << std::endl;
		std::cin.get();
	}
	return 0;
}
