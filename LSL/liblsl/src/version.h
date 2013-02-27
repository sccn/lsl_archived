#ifndef TARGETVER_H
#define TARGETVER_H

#ifdef USE_OFFICIAL_BOOST
#include <boost/version.hpp>
#else
#include <lslboost/version.hpp>
#endif 

#if BOOST_VERSION < 104500
	#error "Please do not compile this with a boost version older than 1.45 because the library would otherwise not be protocol-compatible with builds using other versions."
#endif

// the protocol version
const int LSL_PROTOCOL_VERSION = 100;

// the library version
const int LSL_LIBRARY_VERSION = 104;


// === Auto-generated target platform version defines (for Microsoft and asio) ===

#ifndef WINVER
#define WINVER 0x0501
#endif

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif

#ifndef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0501
#endif

#ifndef _WIN32_IE
#define _WIN32_IE 0x0500
#endif


#endif

