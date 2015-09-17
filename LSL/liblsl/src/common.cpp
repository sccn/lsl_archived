#include "common.h"
#include "api_config.h"
#include <boost/chrono/system_clocks.hpp>

#ifdef _WIN32
#include <windows.h>
#pragma comment (lib,"winmm.lib")
#endif

// === implementation of misc functions ===

/// Implementation of the clock facility.
double lsl::lsl_clock() { 
	return boost::chrono::nanoseconds(boost::chrono::high_resolution_clock::now().time_since_epoch()).count()/1000000000.0; 
}

/// Ensure that LSL is initialized. Performs initialization tasks
void lsl::ensure_lsl_initialized() {
	static bool is_initialized = false;
	if (!is_initialized) {
		is_initialized = true;
#ifdef _WIN32
		// if a timer resolution other than 0 is requested (0 means don't override)...
		if (int desired_timer_resolution = lsl::api_config::get_instance()->timer_resolution()) {			
			// then override it for the lifetime of this program
			struct override_timer_resolution_until_exit {
				override_timer_resolution_until_exit(int res): res_(res) { timeBeginPeriod(res_); }
				~override_timer_resolution_until_exit() { timeEndPeriod(res_); }
				int res_;
			};
			static override_timer_resolution_until_exit overrider(desired_timer_resolution);
		}
#endif
	}
}