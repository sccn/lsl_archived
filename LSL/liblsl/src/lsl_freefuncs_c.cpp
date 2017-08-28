#include "../include/lsl_c.h"
#include "common.h"
#include "resolver_impl.h"
#include <boost/chrono/system_clocks.hpp>


// === Implementation of the free-standing functions in lsl_c.h ===

using namespace lsl;
using std::string;
using std::vector;


/**
* Get the protocol version.
*/
LIBLSL_C_API int lsl_protocol_version() { return api_config::get_instance()->use_protocol_version(); }

/**
* Get the library version.
*/
LIBLSL_C_API int lsl_library_version() { return LSL_LIBRARY_VERSION; }

/**
* Obtain a local system time stamp in seconds. The resolution is better than a millisecond.
* This reading can be used to assign time stamps to samples as they are being acquired. 
* If the "age" of a sample is known at a particular time (e.g., from USB transmission 
* delays), it can be used as an offset to local_clock() to obtain a better estimate of 
* when a sample was actually captured.
*/
LIBLSL_C_API double lsl_local_clock() { 
	return boost::chrono::nanoseconds(boost::chrono::high_resolution_clock::now().time_since_epoch()).count()/1000000000.0; 
} 

/**
* Resolve all streams on the network.
* This function returns all currently available streams from any outlet on the network.
* The network is usually the subnet specified at the local router, but may also include 
* a multicast group of machines (given that the network supports it), or list of hostnames.
* These details may optionally be customized by the experimenter in a configuration file 
* (see Network Connectivity in the LSL wiki).
* This is the default mechanism used by the browsing programs and the recording program.
* @param buffer A user-allocated buffer to hold the resolve results.
*				Note: it is the user's responsibility to either destroy the resulting streaminfo 
*				objects or to pass them back to the LSL during during creation of an inlet.
* @param buffer_elements The user-provided buffer length.
* @param wait_time The waiting time for the operation, in seconds, to search for streams.
*				   Warning: If this is too short (<0.5s) only a subset (or none) of the 
*							outlets that are present on the network may be returned.
* @return The number of results written into the buffer (never more than the provided # of slots) or a negative number if an
*		  error has occurred (values corresponding to lsl_error_code_t).
*/
LIBLSL_C_API int lsl_resolve_all(lsl_streaminfo *buffer, unsigned buffer_elements, double wait_time) {
	try {
		// create a new resolver
		resolver_impl resolver;
		// invoke it (our only constraint is that the session id shall be the same as ours)
		string sess_id = api_config::get_instance()->session_id();
		std::vector<stream_info_impl> tmp = resolver.resolve_oneshot((string("session_id='") += sess_id) += "'",0,wait_time);
		// allocate new stream_info_impl's and assign to the buffer
		unsigned result = buffer_elements<tmp.size() ? buffer_elements : (unsigned)tmp.size();
		for (unsigned k=0;k<result;k++)
			buffer[k] = (lsl_streaminfo)new stream_info_impl(tmp[k]);
		return result;
	} catch(std::exception &e) {
		std::cerr << "Error during resolve_all: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

/**
* Resolve all streams with a given value for a property.
* If the goal is to resolve a specific stream, this method is preferred over resolving all streams and then selecting the desired one.
* @param buffer A user-allocated buffer to hold the resolve results.
*				Note: it is the user's responsibility to either destroy the resulting streaminfo 
*				objects or to pass them back to the LSL during during creation of an inlet.
* @param buffer_elements The user-provided buffer length.
* @param prop The streaminfo property that should have a specific value (e.g., "name", "type", "source_id", or "desc/manufaturer").
* @param value The string value that the property should have (e.g., "EEG" as the type property).
* @param minimum Return at least this number of streams.
* @param timeout Optionally a timeout of the operation, in seconds (default: no timeout).
*				  If the timeout expires, less than the desired number of streams (possibly none) will be returned.
* @return The number of results written into the buffer (never more than the provided # of slots) or a negative number if an
*		  error has occurred (values corresponding to lsl_error_code_t).
*/
LIBLSL_C_API int lsl_resolve_byprop(lsl_streaminfo *buffer, unsigned buffer_elements, char *prop, char *value, int minimum, double timeout) {
	try {
		// create a new resolver
		resolver_impl resolver;
		// build a new query.
		std::ostringstream os; os << "session_id='" << api_config::get_instance()->session_id() << "' and " << prop << "='" << value << "'";
		// invoke it
		std::vector<stream_info_impl> tmp = resolver.resolve_oneshot(os.str(),minimum,timeout);
		// allocate new stream_info_impl's and assign to the buffer
		unsigned result = buffer_elements<tmp.size() ? buffer_elements : (unsigned)tmp.size();
		for (unsigned k=0;k<result;k++)
			buffer[k] = (lsl_streaminfo)new stream_info_impl(tmp[k]);
		return result;
	} catch(std::exception &e) {
		std::cerr << "Error during resolve_byprop: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

/**
* Resolve all streams that match a given predicate.
* Advanced query that allows to impose more conditions on the retrieved streams; the given string is an XPath 1.0 
* predicate for the <info> node (omitting the surrounding []'s), see also http://en.wikipedia.org/w/index.php?title=XPath_1.0&oldid=474981951.
* @param buffer A user-allocated buffer to hold the resolve results.
*				Note: it is the user's responsibility to either destroy the resulting streaminfo 
*				objects or to pass them back to the LSL during during creation of an inlet.
* @param buffer_elements The user-provided buffer length.
* @param pred The predicate string, e.g. "name='BioSemi'" or "type='EEG' and starts-with(name,'BioSemi') and count(info/desc/channel)=32"
* @param minimum Return at least this number of streams.
* @param timeout Optionally a timeout of the operation, in seconds (default: no timeout).
*				  If the timeout expires, less than the desired number of streams (possibly none) will be returned.
* @return The number of results written into the buffer (never more than the provided # of slots) or a negative number if an
*		  error has occurred (values corresponding to lsl_error_code_t).
*/
LIBLSL_C_API int lsl_resolve_bypred(lsl_streaminfo *buffer, unsigned buffer_elements, char *pred, int minimum, double timeout) {
	try {
		// create a new resolver
		resolver_impl resolver;
		// build a new query.
		std::ostringstream os; os << "session_id='" << api_config::get_instance()->session_id() << "' and " << pred;
		// invoke it
		std::vector<stream_info_impl> tmp = resolver.resolve_oneshot(os.str(),minimum,timeout);
		// allocate new stream_info_impl's and assign to the buffer
		unsigned result = buffer_elements<tmp.size() ? buffer_elements : (unsigned)tmp.size();
		for (unsigned k=0;k<result;k++)
			buffer[k] = (lsl_streaminfo)new stream_info_impl(tmp[k]);
		return result;
	} catch(std::exception &e) {
		std::cerr << "Error during resolve_bypred: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

/** 
* Deallocate a string that has been transferred to the application.
* The only use case is to deallocate the contents of string-valued samples
* received from LSL in an application where no free() method is available 
* (e.g., in some scripting languages).
*/
LIBLSL_C_API void lsl_destroy_string(char *s) {
	if (s)
		free(s);
}
