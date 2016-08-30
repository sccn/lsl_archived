#include "time_postprocessor.h"
#include "api_config.h"

#include <cmath>

// === implementation of the time_postprocessor class ===

using namespace lsl;


/// Construct a new time post-processor.
time_postprocessor::time_postprocessor(const postproc_callback_t &query_correction, const postproc_callback_t &query_srate, const reset_callback_t &query_reset): 
	query_correction_(query_correction), query_srate_(query_srate), query_reset_(query_reset), next_query_time_(0.0),  
	last_offset_(0.0), samples_seen_(0.0), options_(post_none), halftime_(api_config::get_instance()->smoothing_halftime()), 
	smoothing_initialized_(false), last_value_(-std::numeric_limits<double>::infinity())
{
}

/// Post-process the given time-stamp and return the new time-stamp.

double time_postprocessor::process_timestamp(double value) {
	if (options_ & post_threadsafe) {
		boost::lock_guard<boost::mutex> lock(processing_mut_);
		return process_internal(value);
	} else
		return process_internal(value);
}

double time_postprocessor::process_internal(double value) {
	// --- clock synchronization ---
	if (options_ & post_clocksync) {
		// update last correction value if needed (we do this every 50 samples and at most twice per second)
		if ((fmod(samples_seen_,50.0) == 0.0) && lsl_clock() > next_query_time_) {
			last_offset_ = query_correction_();
			if (query_reset_()) {
				// reset state to unitialized
				last_offset_ = query_correction_();
				last_value_ = -std::numeric_limits<double>::infinity();
				samples_seen_ = 0;
				smoothing_initialized_ = false;
			}
			next_query_time_ = lsl_clock()+0.5;
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
