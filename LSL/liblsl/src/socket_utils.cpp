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

// std::vector<std::string> enum_adapters(bool is_ipv6) {
void do_32(){
   //define something for Windows (32-bit)
	std::cout << "do_32" << std::endl;	
}

void do_64(){
   //define something for Windows (32-bit)
	std::cout << "do_64" << std::endl;	
}

void do_86(){
   //define something for Windows (32-bit)
	std::cout << "do_86" << std::endl;	
}



#ifdef _WIN32
   //define something for Windows (32-bit and 64-bit, this part is common)
   #ifdef _WIN64
	  //define something for Windows (64-bit only)
		void do_64();
   #elif _WIN86
		void do_86();
   #else
	  //define something for Windows (32-bit only)
		void do_32();
   #endif
#elif __APPLE__
	#include "TargetConditionals.h"
	#if TARGET_IPHONE_SIMULATOR
		 // iOS Simulator
	#elif TARGET_OS_IPHONE
		// iOS device
	#elif TARGET_OS_MAC
		// Other kinds of Mac OS
	#else
	#   error "Unknown Apple platform"
	#endif
#elif __linux__
	// linux
#elif __unix__ // all unices not caught above
	// Unix
#elif defined(_POSIX_VERSION)
	// POSIX
#else
	void do_that();	
#endif

/*#include <boost/predef.hpp>
#if BOOST_OS_WINDOWS

#elif BOOST_OS_LINUX

#elif BOOST_OS_MACOS

#endif
*/

/*
#ifdef _WIN64
   //define something for Windows (64-bit)
#elif _WIN32{
   //define something for Windows (32-bit)
	std::cout << "here!" << endl;	
	}
#elif __APPLE__
	#include "TargetConditionals.h"
	#if TARGET_IPHONE_SIMULATOR
		 // iOS Simulator
	#elif TARGET_OS_IPHONE
		// iOS device
	#elif TARGET_OS_MAC
		// Other kinds of Mac OS
	#else
	#   error "Unknown Apple platform"
	#endif
#elif __linux__
	// linux
#elif __unix__ // all unices not caught above
	// Unix
#elif defined(_POSIX_VERSION)
	// POSIX
#else
#   error "Unknown compiler"
#endif
*/