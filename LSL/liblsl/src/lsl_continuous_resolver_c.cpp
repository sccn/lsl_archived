#include "../include/lsl_c.h"
#include "resolver_impl.h"
#include "common.h"
#include <iostream>


// === implementation of the continuous_resolver class ===

using namespace lsl;

/**
* Construct a new continuous_resolver that resolves all streams on the network. 
* This is analogous to the functionality offered by the free function resolve_streams().
* @param forget_after When a stream is no longer visible on the network (e.g., because it was shut down),
*				      this is the time in seconds after which it is no longer reported by the resolver.
*					  The recommended default value is 5.0.
*/
LIBLSL_C_API lsl_continuous_resolver lsl_create_continuous_resolver(double forget_after) {
	try {
		// create a new resolver
		resolver_impl *resolver = new resolver_impl();
		// start it with the given query
		std::ostringstream os; os << "session_id='" << api_config::get_instance()->session_id() << "'";
		resolver->resolve_continuous(os.str(),forget_after);
		return (lsl_continuous_resolver)resolver;
	} catch(std::exception &e) {
		std::cerr << "Error while creating a continuous_resolver: " << e.what() << std::endl;
		return NULL;
	}
}

/**
* Construct a new continuous_resolver that resolves all streams with a specific value for a given property.
* This is analogous to the functionality provided by the free function resolve_stream(prop,value).
* @param prop The stream_info property that should have a specific value (e.g., "name", "type", "source_id", or "desc/manufaturer").
* @param value The string value that the property should have (e.g., "EEG" as the type property).
* @param forget_after When a stream is no longer visible on the network (e.g., because it was shut down),
*				      this is the time in seconds after which it is no longer reported by the resolver.
*					  The recommended default value is 5.0.
*/
LIBLSL_C_API lsl_continuous_resolver lsl_create_continuous_resolver_byprop(char *prop, char *value, double forget_after) {
	try {
		// create a new resolver
		resolver_impl *resolver = new resolver_impl();
		// start it with the given query
		std::ostringstream os; os << "session_id='" << api_config::get_instance()->session_id() << "' and " << prop << "='" << value << "'";
		resolver->resolve_continuous(os.str(),forget_after);
		return (lsl_continuous_resolver)resolver;
	} catch(std::exception &e) {
		std::cerr << "Error while creating a continuous_resolver: " << e.what() << std::endl;
		return NULL;
	}
}

/**
* Construct a new continuous_resolver that resolves all streams that match a given predicate.
* This is analogous to the functionality provided by the free function resolve_stream(pred).
* @param pred The predicate string, e.g. "name='BioSemi'" or "type='EEG' and starts-with(name,'BioSemi') and count(info/desc/channel)=32"
* @param forget_after When a stream is no longer visible on the network (e.g., because it was shut down),
*				      this is the time in seconds after which it is no longer reported by the resolver.
*					  The recommended default value is 5.0.
*/
LIBLSL_C_API lsl_continuous_resolver lsl_create_continuous_resolver_bypred(char *pred, double forget_after) {
	try {
		// create a new resolver
		resolver_impl *resolver = new resolver_impl();
		// start it with the given query
		std::ostringstream os; os << "session_id='" << api_config::get_instance()->session_id() << "' and " << pred;
		resolver->resolve_continuous(os.str(),forget_after);
		return (lsl_continuous_resolver)resolver;
	} catch(std::exception &e) {
		std::cerr << "Error while creating a continuous_resolver: " << e.what() << std::endl;
		return NULL;
	}
}

/**
* Obtain the set of currently present streams on the network (i.e. resolve result).
* @param buffer A user-allocated buffer to hold the resolve results.
*				Note: it is the user's responsibility to either destroy the resulting streaminfo 
*				objects or to pass them back to the LSL during during creation of an inlet.
* @param buffer_elements The user-provided buffer length.
* @return The number of results written into the buffer (never more than the provided # of slots) 
*		  or a negative number if an error has occurred (values corresponding to lsl_error_code_t).
*/
LIBLSL_C_API int lsl_resolver_results(lsl_continuous_resolver res, lsl_streaminfo *buffer, unsigned buffer_elements) {
	try {
		// query it
		resolver_impl *resolver = (resolver_impl*)res;
		std::vector<stream_info_impl> tmp = resolver->results();
		// allocate new stream_info_impl's and assign to the buffer
		unsigned result = buffer_elements<tmp.size() ? buffer_elements : (unsigned)tmp.size();
		for (unsigned k=0;k<result;k++)
			buffer[k] = (lsl_streaminfo)new stream_info_impl(tmp[k]);
		return result;
	} catch(std::exception &e) {
		std::cerr << "Unexpected error querying lsl_resolver_results: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

/** 
* Destructor for the continuous resolver.
*/
LIBLSL_C_API void lsl_destroy_continuous_resolver(lsl_continuous_resolver res) {
	try {
		delete (resolver_impl*)res; 
	} catch(std::exception &e) {
		std::cerr << "Unexpected during destruction of a continuous_resolver: " << e.what() << std::endl;
	}
}

