#ifndef TIME_POSTPROCESSOR_H
#define TIME_POSTPROCESSOR_H

#include <boost/function.hpp>
#include <boost/thread.hpp>
#include "common.h"


namespace lsl {

	/// A callback function that allows the post-processor to query some state from other objects when needed.
	typedef boost::function<double()> postproc_callback_t;
	typedef boost::function<bool()> reset_callback_t;


	/// Internal class of an inlet that is responsible for post-processing time stamps.
	class time_postprocessor {
	public:
		/// Construct a new time post-processor given some callback functions.
		time_postprocessor(const postproc_callback_t &query_correction, const postproc_callback_t &query_srate,
			const reset_callback_t &query_reset);

		/**
		* Set post-processing options to use. By default, this class performs NO post-processing and returns the 
		* ground-truth time stamps, which can then be manually synchronized using time_correction(), and then 
		* smoothed/dejittered if desired. This function allows automating these two and possibly more operations.
		* @param flags An integer that is the result of bitwise OR'ing one or more options from processing_options_t 
		*        together (e.g., post_clocksync|post_dejitter); the default is to enable all options.
		*/
		void set_options(unsigned options=post_ALL) { options_ = options; }

		/// Post-process the given time stamp and return the new time-stamp.
		double process_timestamp(double value);

		/// Override the half-time (forget factor) of the time-stamp smoothing.
		void smoothing_halftime(float value) { halftime_ = value; }

	private:
		// Internal function to process a time stamp.
		double process_internal(double value); 

		double samples_seen_;					// number of samples seen so far

		// configuration parameters
		postproc_callback_t query_srate_;		// a callback function that returns the current nominal sampling rate
		unsigned options_;						// current processing options
		float halftime_;						// smoothing half-time

		// handling of time corrections
		postproc_callback_t query_correction_;	// a callback function that returns the current time-correction offset
		reset_callback_t query_reset_;			// a callback function that returns whether the clock was reset
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


}

#endif
