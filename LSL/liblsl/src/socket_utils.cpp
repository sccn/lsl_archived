#include "socket_utils.h"
#include "endian/conversion.hpp"
#include "common.h"


// === Implementation of the socket utils ===

using namespace lsl;

/// Measure the endian conversion performance of this machine.
double lsl::measure_endian_performance() {
	const double measure_duration = 0.01;
	double data=12335.5, t_end=lsl_clock() + measure_duration;
	double k;
	for (k=0; ((int)k & 0xFF) != 0 || lsl_clock()<t_end; k++)
		lslboost::endian::reverse_value(data);
	return k;
}
