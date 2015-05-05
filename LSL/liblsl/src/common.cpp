#include "common.h"
#include <boost/chrono/system_clocks.hpp>

// === implementation of misc functions ===

/// Implementation of the clock facility.
double lsl::lsl_clock() { 
	return boost::chrono::nanoseconds(boost::chrono::high_resolution_clock::now().time_since_epoch()).count()/1000000000.0; 
}