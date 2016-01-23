#include "time_postprocessor.h"
#include "api_config.h"

#include <cmath>

// === implementation of the time_postprocessor class ===

using namespace lsl;

/**
* Post-process the given time-stamp and return the new time-stamp.
*/
double time_postprocessor::process_timestamp(double value) {

	// --- clock synchronization ---
	if (options_ & post_clocksync) {
		// update last correction value if needed (we do this every 100 samples and at most once per second)
		if ((fmod(samples_seen_,100.0) == 0.0) && lsl_clock() > (last_query_time_ + 1.0)) {
			last_offset_ = query_correction_();
			last_query_time_ = lsl_clock();
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
				// linear regression model coefficients
				w0_ = 0.0;
				w1_ = 1.0/srate;
				// inverse covariance matrix
				P00_ = 1.0e10;
				P01_ = 0.0;
				P10_ = 0.0;
				P11_ = 1.0e10;
				// forget factor lambda in RLS calculation
				lam_ = pow(2.0, (-1.0/(srate*halftime_)));
				il_ = 1.0/lam_;
				// numeric baseline
				baseline_value_ = value;
			}
			smoothing_initialized_ = true;
		}
		if (smoothing_applicable_) {
			value -= baseline_value_;

			// RLS update
			double u1 = samples_seen_+1;			// u = np.matrix([[1.0], [samples_seen+1]])
			double pi0 = P00_ + u1*P10_;			// pi = u.T * P
			double pi1 = P01_ + u1*P11_;			// ... (ct'd)
			double al = value - w0_ - w1_ * u1;		// al = value - w.T * u
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
}
