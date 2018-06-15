#include <limits>
#include <utility>
#include <iostream>
#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

/// Processing options for the time_postprocessor. 
enum processing_options_t {
	post_none = 0,			// No automatic post-processing; return the ground-truth time stamps for manual post-processing
	// (this is the default behavior of the inlet).
	post_clocksync = 1,		// Perform automatic clock synchronization; equivalent to manually adding the time_correction() value
	// to the received time stamps.
	post_dejitter = 2,		// Remove jitter from time stamps. This will apply a smoothing algorithm to the received time stamps;
	// the smoothing needs to see a minimum number of samples (1-2 minutes) until the remaining jitter 
	// is consistently below 1ms.
	post_monotonize = 4,	// Force the time-stamps to be monotonically ascending (only makes sense if timestamps are dejittered).
	post_threadsafe = 8,    // Post-processing is thread-safe (same inlet can be read from by multiple threads); uses somewhat more CPU.
	post_ALL = 1|2|4|8		// The combination of all possible post-processing options.
};


/// Internal class of an inlet that is responsible for post-processing time stamps.
/// (This is a version of time_postprocessor.cpp with the callback functions/hooks removed for easier testing)
class time_postprocessor {
public:
	/// Construct a new time post-processor given a callback function that 
	/// returns the time-correction offset for the current data point.
	time_postprocessor(double nominal_rate): 
		nominal_rate_(nominal_rate), next_query_time_(0.0), last_offset_(0.0), samples_seen_(0.0), options_(post_none), halftime_(90.0), smoothing_initialized_(false),
		last_value_(-std::numeric_limits<double>::infinity())
	{
	}

	// dummy implementations
	double query_correction_() { return 0.0; }
	double query_srate_() { return nominal_rate_; }
	double lsl_clock() { return 0.0; }

	/**
	* Set post-processing options to use. By default, this class performs NO post-processing and returns the 
	* ground-truth time stamps, which can then be manually synchronized using time_correction(), and then 
	* smoothed/dejittered if desired. This function allows automating these two and possibly more operations.
	* @param flags An integer that is the result of bitwise OR'ing one or more options from processing_options_t 
	*        together (e.g., post_clocksync|post_dejitter); the default is to enable all options.
	*/
	void set_options(unsigned options=post_ALL) { options_ = options; }

	/// Post-process the given time stamp and return the new time-stamp.
	double process_timestamp(double value) {
		if (options_ & post_threadsafe) {
			boost::lock_guard<boost::mutex> lock(processing_mut_);
			return process_internal(value);
		} else
			return process_internal(value);
	}

	/**
	* Override the half-time (forget factor) of the time-stamp smoothing.
	* The default is 30 seconds unless a different value is set in the config file.
	* Using a longer window will yield lower jitter in the time stamps, but very
	* long windows (>2 minutes) will not track rapid changes in the true clock rate
	* (e.g., due to room temperature changes); a rule of thumb is that an n second 
	* window will be able to track changes happening within 10*n seconds or longer.
	*/
	void smoothing_halftime(float value) { halftime_ = value; }

private:
	// Internal function to process a time stamp.
	double process_internal(double value) {
		// --- clock synchronization ---
		if (options_ & post_clocksync) {
			// update last correction value if needed (we do this every 100 samples and at most once per second)
			if ((fmod(samples_seen_,100.0) == 0.0) && lsl_clock() > next_query_time_) {
				last_offset_ = query_correction_();
				next_query_time_ = lsl_clock()+1.0;
			}
			// perform clock synchronization; this is done by adding the last-measured clock offset value
			// (typically this is used to map the value from the sender's clock to our local clock)
			value += last_offset_;
		}

		// --- jitter removal ---
		if (options_ & post_dejitter) {
			// initialize the smoothing state if not yet done so
			if (!smoothing_initialized_) {
				double srate = query_srate_();
				smoothing_applicable_ = (srate > 0.0);
				if (smoothing_applicable_) {
					// linear regression model coefficients (intercept, slope)
					w0_ = 0.0;
					w1_ = 1.0/srate;
					// forget factor lambda in RLS calculation & its inverse
					lam_ = pow(2.0, -1.0/(srate*halftime_));
					il_ = 1.0/lam_;
					// inverse autocovariance matrix of predictors u
					P00_ = P11_ = 1e10; P01_ = P10_ = 0.0;
					// numeric baseline
					baseline_value_ = value;
				}
				smoothing_initialized_ = true;
			}
			if (smoothing_applicable_) {
				value -= baseline_value_;

				// RLS update
				double u1 = samples_seen_;				// u = np.matrix([[1.0], [samples_seen]])
				double pi0 = P00_ + u1*P10_;			// pi = u.T * P
				double pi1 = P01_ + u1*P11_;			// ... (ct'd)
				double al = value - w0_ - w1_ * u1;		// al = value - w.T * u (prediction error)
				double gam = lam_ + pi0 + pi1 * u1;		// gam = lam + pi * u
				P00_ = il_ * (P00_ - ((pi0*pi0)/gam));  // Pp = k * pi; P = il * (P - Pp)
				P01_ = il_ * (P01_ - ((pi0*pi1)/gam));	// ...
				P10_ = il_ * (P10_ - ((pi1*pi0)/gam));	// ...
				P11_ = il_ * (P11_ - ((pi1*pi1)/gam));	// ...
				w0_ += al * (P00_ + P10_ * u1);         // w = w + k*al
				w1_ += al * (P01_ + P11_ * u1);			// ...
				value = w0_ + w1_*u1;					// value = float(w.T * u)

				value += baseline_value_;
			}
		}

		// --- force monotonic timestamps ---
		if (options_ & post_monotonize) {
			if (value < last_value_)
				value = last_value_;
		}

		samples_seen_ += 1.0;
		last_value_ = value;
		return value;
	}

	double nominal_rate_;					// holds the current nominal sampling rate
	double samples_seen_;					// number of samples seen so far

	// configuration parameters
	unsigned options_;						// current processing options
	float halftime_;						// smoothing half-time

	// handling of time corrections
	double next_query_time_;				// the next time when we query the time-correction offset
	double last_offset_;					// last queried correction offset

	// runtime parameters for smoothing
	double baseline_value_;					// first observed time-stamp value, used as a baseline to improve numerics
	double w0_, w1_;						// linear regression model coefficients		
	double P00_, P01_, P10_, P11_;			// inverse covariance matrix		
	double lam_, il_;						// forget factor lambda in RLS calculation & its inverse
	bool smoothing_applicable_;				// whether smoothing can be applied to these data (false if irregular srate)
	bool smoothing_initialized_;			// whether the smoothing parameters have been initialized already

	// runtime parameters for monotonize
	double last_value_;						// last observed time-stamp value, to force monotonically increasing stamps

	// mutex for thread safety
	boost::mutex processing_mut_;			// a mutex that protects the runtime data structures
};


// the scripts that regenerate these buffers (and evaluate results) can be found at: ftp://sccn.ucsd.edu/pub/christian/SyncTest
const char* const datapath = "C:\\Synched\\Misc\\JitterRemovalTestBuffers\\TestBuffer\\";
const int rates[] = {50,100,200,500,1000,2000}; //,2000,3000,4000,5000};
const int n_chunks = 100;

void main() {
	for (int k=0; k<sizeof(rates)/sizeof(rates[0]); k++) {
		double rate = rates[k];
		std::cout << "running test for srate = " << rate << std::endl;
		// get path to test cases
		std::string loadpath(datapath);
		loadpath += ("NR" + boost::lexical_cast<std::string>(rate));
		// run test
		double sum_errors = 0.0;
		int n_samples = 0;
		for (int chunk=1; chunk<=n_chunks; chunk++) {
			std::string true_data = loadpath + "_YMatrix_" + boost::lexical_cast<std::string>(chunk) + ".bin";
			std::string jittered_data = loadpath + "_ZMatrix_" + boost::lexical_cast<std::string>(chunk) + ".bin";
			std::string estimated_data = loadpath + "_WMatrix_" + boost::lexical_cast<std::string>(chunk) + ".bin";
			std::ifstream true_stream(true_data.c_str(), std::ifstream::binary);
			std::ifstream jittered_stream(jittered_data.c_str(), std::ifstream::binary);
			std::ofstream estimated_stream(estimated_data.c_str(), std::ofstream::binary | std::ofstream::trunc);
			time_postprocessor processor(rate);
			processor.set_options(post_ALL & ~post_threadsafe & ~post_clocksync);
			while (true) {
				// read new sample, process, also get ground truth
				double jittered_value = 0.0; jittered_stream.read((char*)&jittered_value, sizeof(jittered_value));
				if (!jittered_stream)
					break;
				double estimated_value = processor.process_timestamp(jittered_value);
				double true_value = 0.0; true_stream.read((char*)&true_value, sizeof(true_value));
				double error = estimated_value - true_value;
				// update statistics
				sum_errors += abs(error); n_samples++;
				// write resulting sample as well...
				estimated_stream.write((char*)&estimated_value, sizeof(estimated_value));
			}
		}
		std::cout << "average error: " << sum_errors/n_samples << std::endl;
	}
	std::cin.get();	
}