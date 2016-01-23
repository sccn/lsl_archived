#ifndef TIME_POSTPROCESSOR_H
#define TIME_POSTPROCESSOR_H


#include <boost/function.hpp>
#include "api_config.h"

namespace lsl {

	/**
	* Processing options for the time_postprocessor. 
	*/
	enum processing_options_t {
		post_none = 0,		// No automatic post-processing; return the ground-truth time stamps for manual post-processing
							// (this is the default behavior of the inlet).
		post_clocksync = 1, // Perform automatic clock synchronization; equivalent to manually adding the time_correction() value
							// to the received time stamps.
		post_dejitter = 2,	// Remove jitter from time stamps. This will apply a smoothing algorithm to the received time stamps;
							// the smoothing needs to see a minimum number of samples (1-2 minutes) until the remaining jitter 
							// is consistently below 1ms.
		post_monotonize = 4,	// force the time-stamps to be monotonically ascending (only makes sense if timestamps are dejittered)
		post_ALL = 1|2|4		// The combination of all possible post-processing options.
	};

	/// A callback function that allows the post-process to query time-correction values
	/// when it needs them.
	typedef boost::function<double()> postproc_callback_t;


	/// Internal class of an inlet that is responsible for post-processing time stamps.
	class time_postprocessor {
	public:
		/// Construct a new time post-processor given a callback function that 
		/// returns the time-correction offset for the current data point.
		time_postprocessor(const postproc_callback_t &query_correction, const postproc_callback_t &query_srate): query_correction_(query_correction), query_srate_(query_srate), 
			last_query_time_(0.0), last_offset_(0.0), samples_seen_(0.0), options_(post_none), halftime_(api_config::get_instance()->smoothing_halftime()), smoothing_initialized_(false),
			last_value_(-std::numeric_limits<double>::infinity())
		{}


		/**
		* Set post-processing options to use. By default, this class performs NO post-processing and returns the 
		* ground-truth time stamps, which can then be manually synchronized using time_correction(), and then 
		* smoothed/dejittered if desired. This function allows automating these two and possibly more operations.
		* @param flags An integer that is the result of bitwise OR'ing one or more options from processing_options_t 
		*        together (e.g., post_clocksync|post_dejitter); the default is to enable all options.
		*/
		void set_options(unsigned options=post_ALL) { options_ = options; }

		/// Post-process the given time-stamp and return the new time-stamp.
		double process_timestamp(double value);

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
		// machinery for obtaining time corrections
		postproc_callback_t query_correction_;	// a callback function that returns the current time-correction offset
		double last_query_time_;				// the last time when we queried the time-correction offset
		double last_offset_;					// last queried correction offset
		double samples_seen_;					// number of samples seen so far
		
		// configuration parameters
		postproc_callback_t query_srate_;		// a callback function that returns the current nominal sampling rate
		unsigned options_;						// current processing options
		float halftime_;						// smoothing half-time

		// runtime parameters for smoothing
		double baseline_value_;					// first observed time-stamp value, used as a baseline to improve numerics
		double w0_, w1_;						// linear regression model coefficients		
		double P00_, P01_, P10_, P11_;			// inverse covariance matrix		
		double lam_, il_;						// forget factor lambda in RLS calculation & its inverse
		bool smoothing_applicable_;				// whether smoothing can be applied to these data (false if irregular srate)
		bool smoothing_initialized_;			// whether the smoothing parameters have been initialized already

		// runtime parameters for monotonize
		double last_value_;						// last observed time-stamp value, to force monotonically increasing stamps
	};


}

#endif
