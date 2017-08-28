#include "../include/lsl_c.h"
#include "stream_inlet_impl.h"



// === implementation of the stream_inlet class ===

using namespace lsl;

/**
* Construct a new stream inlet from a resolved stream info.
* @param info A resolved stream info object (as coming from one of the resolver functions).
* @param max_buflen Optionally the maximum amount of data to buffer (in seconds if there is a nominal 
*					sampling rate, otherwise x100 in samples). Recording applications want to use a fairly 
*					large buffer size here, while real-time applications would only buffer as much as 
*					they need to perform their next calculation.
* @param max_chunklen Optionally the maximum size, in samples, at which chunks are transmitted 
*					  (the default corresponds to the chunk sizes used by the sender).
*				      Recording applications can use a generous size here (leaving it to the network how 
*					  to pack things), while real-time applications may want a finer (perhaps 1-sample) granularity.
* @param recover Try to silently recover lost streams that are recoverable (=those that that have a source_id set). 
*				 In all other cases (recover is false or the stream is not recoverable) functions may throw an 
*				 lsl_lost_error if the stream's source is lost (e.g., due to an app or computer crash).
*/
LIBLSL_C_API lsl_inlet lsl_create_inlet(lsl_streaminfo info, int max_buflen, int max_chunklen, int recover) {
	try {
		stream_info_impl *infoimpl = (stream_info_impl*)info;
		lsl_inlet result = (lsl_inlet)new stream_inlet_impl(*infoimpl, infoimpl->nominal_srate()?(int)(infoimpl->nominal_srate()*max_buflen):max_buflen*100, max_chunklen, recover!=0);
		return result;
	} 
	catch(std::invalid_argument &e) {
		std::cerr << "Error during construction of a stream_inlet: " << e.what() << std::endl;
		return NULL;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during construction of a stream_inlet: " << e.what() << std::endl;
		return NULL;
	}
}

/** 
* Destructor.
* The inlet will automatically disconnect if destroyed.
*/
LIBLSL_C_API void lsl_destroy_inlet(lsl_inlet in) { 
	try {
		delete (stream_inlet_impl*)in; 
	} catch(std::exception &e) {
		std::cerr << "Unexpected during destruction of a stream_inlet: " << e.what() << std::endl;
	}
}

/**
* Retrieve the complete information of the given stream, including the extended description.
* Can be invoked at any time of the stream's lifetime.
* @param timeout Timeout of the operation.
* @param ec Error code: if nonzero, can be either lsl_timeout_error (if the timeout has expired) or lsl_lost_error (if the stream source has been lost).
*/
LIBLSL_C_API lsl_streaminfo lsl_get_fullinfo(lsl_inlet in, double timeout, int *ec) {
	if (ec)
		*ec = lsl_no_error;
	try {
		return (lsl_streaminfo)new stream_info_impl(((stream_inlet_impl*)in)->info(timeout));
	}
	catch(timeout_error &) { 
		if (ec)
			*ec = lsl_timeout_error; 
	}
	catch(lost_error &) { 
		if (ec)
			*ec = lsl_lost_error; 
	}
	catch(std::exception &) { 
		if (ec)
			*ec = lsl_internal_error; 
	}
	return NULL;
}

/**
* Subscribe to the data stream.
* All samples pushed in at the other end from this moment onwards will be queued and 
* eventually be delivered in response to pull_sample() calls.
* Pulling a sample without some preceding lsl_open_stream() is permitted (the stream will then be opened implicitly).
* @param timeout Optional timeout of the operation.
* @param ec Error code: if nonzero, can be either lsl_timeout_error (if the timeout has expired) or lsl_lost_error (if the stream source has been lost).
*/
LIBLSL_C_API void lsl_open_stream(lsl_inlet in, double timeout, int *ec) {
	if (ec)
		*ec = lsl_no_error;
	try {
		((stream_inlet_impl*)in)->open_stream(timeout);
	}
	catch(timeout_error &) { 
		if (ec)
			*ec = lsl_timeout_error; 
	}
	catch(lost_error &) { 
		if (ec)
			*ec = lsl_lost_error; 
	}
	catch(std::exception &) { 
		if (ec)
			*ec = lsl_internal_error; 
	}
}

/**
* Drop the current data stream.
* All samples that are still buffered or in flight will be dropped and the source will halt its buffering of data for this inlet.
* If an application stops being interested in data from a source (temporarily or not) but keeps the outlet alive, it should 
* call lsl_close_stream() to not pressure the source outlet to buffer unnecessarily large amounts of data.
*/
LIBLSL_C_API void lsl_close_stream(lsl_inlet in) { 
	try {
		((stream_inlet_impl*)in)->close_stream();
	} catch(std::exception &e) {
		std::cerr << "Unexpected error during close_stream(): " << e.what() << std::endl;
	}
}

/**
* Retrieve an estimated time correction offset for the given stream.
* The first call to this function takes several milliseconds until a reliable first estimate is obtained.
* Subsequent calls are instantaneous (and rely on periodic background updates).
* The precision of these estimates should be below 1 ms (empirically within +/-0.2 ms).
* @timeout Timeout to acquire the first time-correction estimate.
* @return The time correction estimate. If the first estimate cannot within the alloted time, the result is NaN.
* @param ec Error code: if nonzero, can be either lsl_timeout_error (if the timeout has expired) or lsl_lost_error (if the stream source has been lost).
*/
LIBLSL_C_API double lsl_time_correction(lsl_inlet in, double timeout, int *ec) {
	if (ec)
		*ec = lsl_no_error;
	try {
		return ((stream_inlet_impl*)in)->time_correction(timeout);
	}
	catch(timeout_error &) { 
		if (ec)
			*ec = lsl_timeout_error; 
	}
	catch(lost_error &) { 
		if (ec)
			*ec = lsl_lost_error; 
	}
	catch(std::exception &) { 
		if (ec)
			*ec = lsl_internal_error; 
	}
	return 0.0;
}

LIBLSL_C_API double lsl_time_correction_ex(lsl_inlet in, double *remote_time, double *uncertainty, double timeout, int *ec) {
	if (ec)
		*ec = lsl_no_error;
	try {
        double correction = ((stream_inlet_impl*)in)->time_correction(remote_time, uncertainty, timeout);
		return correction;
	}
	catch(timeout_error &) {
		if (ec)
			*ec = lsl_timeout_error;
	}
	catch(lost_error &) {
		if (ec)
			*ec = lsl_lost_error;
	}
	catch(std::exception &) {
		if (ec)
			*ec = lsl_internal_error;
	}
	return 0.0;
}

/**
* Set post-processing flags to use. 
*/
LIBLSL_C_API int lsl_set_postprocessing(lsl_inlet in, unsigned flags) {
	try {
		((stream_inlet_impl*)in)->set_postprocessing(flags);
		return lsl_no_error;
	}
	catch(std::invalid_argument &) { 
		return lsl_argument_error; 
	}
	catch(std::exception &) {
		return lsl_internal_error;
	}
}


/* === Pulling a sample from the inlet === */

/**
* Pull a sample from the inlet and read it into a pointer to values.
* Handles type checking & conversion.
* @param buffer A pointer to hold the resulting values. 
* @param buffer_elements The number of samples allocated in the buffer. Note: it is the responsibility of the user to allocate enough memory.
* @param timeout The timeout for this operation, if any.
* @return The capture time of the sample on the remote machine, or 0.0 if no new sample was available. 
*		  To remap this time stamp to the local clock, add the value returned by lsl_time_correction() to it. 
* @param ec Error code: if nonzero, can be either lsl_timeout_error (if the timeout has expired) or lsl_lost_error (if the stream source has been lost).
*/
LIBLSL_C_API double lsl_pull_sample_f(lsl_inlet in, float *buffer, int buffer_elements, double timeout, int *ec) {
	if (ec)
		*ec = lsl_no_error;
	try {
		return ((stream_inlet_impl*)in)->pull_sample(buffer,buffer_elements,timeout);
	}
	catch(timeout_error &) { 
		if (ec)
			*ec = lsl_timeout_error; 
	}
	catch(lost_error &) { 
		if (ec)
			*ec = lsl_lost_error; 
	}
	catch(std::invalid_argument &) { 
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::range_error &) {
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::exception &) { 
		if (ec)
			*ec = lsl_internal_error; 
	}
	return 0.0;
}

LIBLSL_C_API double lsl_pull_sample_d(lsl_inlet in, double *buffer, int buffer_elements, double timeout, int *ec) {
	if (ec)
		*ec = lsl_no_error;
	try {
		return ((stream_inlet_impl*)in)->pull_sample(buffer,buffer_elements,timeout);
	}
	catch(timeout_error &) { 
		if (ec)
			*ec = lsl_timeout_error; 
	}
	catch(lost_error &) { 
		if (ec)
			*ec = lsl_lost_error; 
	}
	catch(std::invalid_argument &) { 
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::range_error &) {
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::exception &) { 
		if (ec)
			*ec = lsl_internal_error; 
	}
	return 0.0;
}

LIBLSL_C_API double lsl_pull_sample_l(lsl_inlet in, long *buffer, int buffer_elements, double timeout, int *ec) {
	if (ec)
		*ec = lsl_no_error;
	try {
		return ((stream_inlet_impl*)in)->pull_sample(buffer,buffer_elements,timeout);
	}
	catch(timeout_error &) { 
		if (ec)
			*ec = lsl_timeout_error; 
	}
	catch(lost_error &) { 
		if (ec)
			*ec = lsl_lost_error; 
	}
	catch(std::invalid_argument &) { 
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::range_error &) {
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::exception &) { 
		if (ec)
			*ec = lsl_internal_error; 
	}
	return 0.0;
}

LIBLSL_C_API double lsl_pull_sample_i(lsl_inlet in, int *buffer, int buffer_elements, double timeout, int *ec) {
	if (ec)
		*ec = lsl_no_error;
	try {
		return ((stream_inlet_impl*)in)->pull_sample(buffer,buffer_elements,timeout);
	}
	catch(timeout_error &) { 
		if (ec)
			*ec = lsl_timeout_error; 
	}
	catch(lost_error &) { 
		if (ec)
			*ec = lsl_lost_error; 
	}
	catch(std::invalid_argument &) { 
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::range_error &) {
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::exception &) { 
		if (ec)
			*ec = lsl_internal_error; 
	}
	return 0.0;
}

LIBLSL_C_API double lsl_pull_sample_s(lsl_inlet in, short *buffer, int buffer_elements, double timeout, int *ec) {
	if (ec)
		*ec = lsl_no_error;
	try {
		return ((stream_inlet_impl*)in)->pull_sample(buffer,buffer_elements,timeout);
	}
	catch(timeout_error &) { 
		if (ec)
			*ec = lsl_timeout_error; 
	}
	catch(lost_error &) { 
		if (ec)
			*ec = lsl_lost_error; 
	}
	catch(std::invalid_argument &) { 
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::range_error &) {
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::exception &) { 
		if (ec)
			*ec = lsl_internal_error; 
	}
	return 0.0;
}

LIBLSL_C_API double lsl_pull_sample_c(lsl_inlet in, char *buffer, int buffer_elements, double timeout, int *ec) {
	if (ec)
		*ec = lsl_no_error;
	try {
		return ((stream_inlet_impl*)in)->pull_sample(buffer,buffer_elements,timeout);
	}
	catch(timeout_error &) { 
		if (ec)
			*ec = lsl_timeout_error; 
	}
	catch(lost_error &) { 
		if (ec)
			*ec = lsl_lost_error; 
	}
	catch(std::invalid_argument &) { 
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::range_error &) {
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::exception &) { 
		if (ec)
			*ec = lsl_internal_error; 
	}
	return 0.0;
}

LIBLSL_C_API double lsl_pull_sample_str(lsl_inlet in, char **buffer, int buffer_elements, double timeout, int *ec) {
	if (ec)
		*ec = lsl_no_error;
	try {
		// capture output in a temporary string buffer
		std::vector<std::string> tmp;
		double result = ((stream_inlet_impl*)in)->pull_sample(tmp,timeout);
        if (buffer_elements < (int)tmp.size())
            throw std::range_error("The provided buffer has fewer elements than the stream's number of channels.");
		// allocate memory and copy over into buffer
		for (unsigned k=0;k<tmp.size();k++) {
			buffer[k] = (char*)malloc(tmp[k].size()+1);
			if (buffer[k] == NULL) {
				for (unsigned k2=0;k2<k;k2++)
					free(buffer[k2]);
				*ec = lsl_internal_error;
				return 0.0;
			}
			strcpy(buffer[k],tmp[k].c_str());
		}
		return result;
	}
	catch(timeout_error &) { 
		if (ec)
			*ec = lsl_timeout_error; 
	}
	catch(lost_error &) { 
		if (ec)
			*ec = lsl_lost_error; 
	}
	catch(std::invalid_argument &) { 
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::range_error &) {
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::exception &) { 
		if (ec)
			*ec = lsl_internal_error; 
	}
	return 0.0;
}

LIBLSL_C_API double lsl_pull_sample_buf(lsl_inlet in, char **buffer, unsigned *buffer_lengths, int buffer_elements, double timeout, int *ec) {
	if (ec)
		*ec = lsl_no_error;
	try {
		// capture output in a temporary string buffer
		std::vector<std::string> tmp;
		double result = ((stream_inlet_impl*)in)->pull_sample(tmp,timeout);
        if (buffer_elements < (int)tmp.size())
            throw std::range_error("The provided buffer has fewer elements than the stream's number of channels.");
		// allocate memory and copy over into buffer
		for (unsigned k=0;k<tmp.size();k++) {
			buffer[k] = (char*)malloc(tmp[k].size());
			if (buffer[k] == NULL) {
				for (unsigned k2=0;k2<k;k++)
					free(buffer[k2]);
				*ec = lsl_internal_error;
				return 0.0;
			}
			buffer_lengths[k] = (unsigned)tmp[k].size();
			memcpy(buffer[k],&tmp[k][0],tmp[k].size());
		}
		return result;
	}
	catch(timeout_error &) { 
		if (ec)
			*ec = lsl_timeout_error; 
	}
	catch(lost_error &) { 
		if (ec)
			*ec = lsl_lost_error; 
	}
	catch(std::invalid_argument &) { 
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::range_error &) {
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::exception &) { 
		if (ec)
			*ec = lsl_internal_error; 
	}
	return 0.0;
}


/**
* Pull a sample from the inlet and read it into a custom struct or buffer. 
* Overall size checking but no type checking or conversion are done. Do not use for variable-size/string-formatted streams.
* @param sample Pointer to hold the sample data. Search for #pragma pack for information on how to pack structs appropriately.
* @param timeout The timeout for this operation, if any.
* @return The capture time of the sample on the remote machine, or 0.0 if no new sample was available. 
*		   To remap this time stamp to the local clock, add the value returned by .time_correction() to it. 
* @param ec Error code: if nonzero, can be either lsl_timeout_error (if the timeout has expired) or lsl_lost_error (if the stream source has been lost).
*/
LIBLSL_C_API double lsl_pull_sample_v(lsl_inlet in, void *buffer, int buffer_bytes, double timeout, int *ec) {
	if (ec)
		*ec = lsl_no_error;
	try {
		return ((stream_inlet_impl*)in)->pull_numeric_raw(buffer,buffer_bytes,timeout);
	}
	catch(timeout_error &) { 
		if (ec)
			*ec = lsl_timeout_error; 
	}
	catch(lost_error &) { 
		if (ec)
			*ec = lsl_lost_error; 
	}
	catch(std::invalid_argument &) { 
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::range_error &) {
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::exception &) { 
		if (ec)
			*ec = lsl_internal_error; 
	}
	return 0.0;
}

LIBLSL_C_API unsigned long lsl_pull_chunk_f(lsl_inlet in, float *data_buffer, double *timestamp_buffer, unsigned long data_buffer_elements, unsigned long timestamp_buffer_elements, double timeout, int *ec) {
	if (ec)
		*ec = lsl_no_error;
	try {
		return ((stream_inlet_impl*)in)->pull_chunk_multiplexed(data_buffer,timestamp_buffer,data_buffer_elements,timestamp_buffer_elements,timeout);
	}
	catch(timeout_error &) { 
		if (ec)
			*ec = lsl_timeout_error; 
	}
	catch(lost_error &) { 
		if (ec)
			*ec = lsl_lost_error; 
	}
	catch(std::invalid_argument &) { 
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::range_error &) {
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::exception &) { 
		if (ec)
			*ec = lsl_internal_error; 
	}
	return 0;
}

LIBLSL_C_API unsigned long lsl_pull_chunk_d(lsl_inlet in, double *data_buffer, double *timestamp_buffer, unsigned long data_buffer_elements, unsigned long timestamp_buffer_elements, double timeout, int *ec) {
	if (ec)
		*ec = lsl_no_error;
	try {
		return ((stream_inlet_impl*)in)->pull_chunk_multiplexed(data_buffer,timestamp_buffer,data_buffer_elements,timestamp_buffer_elements,timeout);
	}
	catch(timeout_error &) { 
		if (ec)
			*ec = lsl_timeout_error; 
	}
	catch(lost_error &) { 
		if (ec)
			*ec = lsl_lost_error; 
	}
	catch(std::invalid_argument &) { 
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::range_error &) {
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::exception &) { 
		if (ec)
			*ec = lsl_internal_error; 
	}
	return 0;
}

LIBLSL_C_API unsigned long lsl_pull_chunk_l(lsl_inlet in, long *data_buffer, double *timestamp_buffer, unsigned long data_buffer_elements, unsigned long timestamp_buffer_elements, double timeout, int *ec) {
	if (ec)
		*ec = lsl_no_error;
	try {
		return ((stream_inlet_impl*)in)->pull_chunk_multiplexed(data_buffer,timestamp_buffer,data_buffer_elements,timestamp_buffer_elements,timeout);
	}
	catch(timeout_error &) { 
		if (ec)
			*ec = lsl_timeout_error; 
	}
	catch(lost_error &) { 
		if (ec)
			*ec = lsl_lost_error; 
	}
	catch(std::invalid_argument &) { 
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::range_error &) {
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::exception &) { 
		if (ec)
			*ec = lsl_internal_error; 
	}
	return 0;
}

LIBLSL_C_API unsigned long lsl_pull_chunk_i(lsl_inlet in, int *data_buffer, double *timestamp_buffer, unsigned long data_buffer_elements, unsigned long timestamp_buffer_elements, double timeout, int *ec) {
	if (ec)
		*ec = lsl_no_error;
	try {
		return ((stream_inlet_impl*)in)->pull_chunk_multiplexed(data_buffer,timestamp_buffer,data_buffer_elements,timestamp_buffer_elements,timeout);
	}
	catch(timeout_error &) { 
		if (ec)
			*ec = lsl_timeout_error; 
	}
	catch(lost_error &) { 
		if (ec)
			*ec = lsl_lost_error; 
	}
	catch(std::invalid_argument &) { 
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::range_error &) {
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::exception &) { 
		if (ec)
			*ec = lsl_internal_error; 
	}
	return 0;
}

LIBLSL_C_API unsigned long lsl_pull_chunk_s(lsl_inlet in, short *data_buffer, double *timestamp_buffer, unsigned long data_buffer_elements, unsigned long timestamp_buffer_elements, double timeout, int *ec) {
	if (ec)
		*ec = lsl_no_error;
	try {
		return ((stream_inlet_impl*)in)->pull_chunk_multiplexed(data_buffer,timestamp_buffer,data_buffer_elements,timestamp_buffer_elements,timeout);
	}
	catch(timeout_error &) { 
		if (ec)
			*ec = lsl_timeout_error; 
	}
	catch(lost_error &) { 
		if (ec)
			*ec = lsl_lost_error; 
	}
	catch(std::invalid_argument &) { 
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::range_error &) {
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::exception &) { 
		if (ec)
			*ec = lsl_internal_error; 
	}
	return 0;
}

LIBLSL_C_API unsigned long lsl_pull_chunk_c(lsl_inlet in, char *data_buffer, double *timestamp_buffer, unsigned long data_buffer_elements, unsigned long timestamp_buffer_elements, double timeout, int *ec) {
	if (ec)
		*ec = lsl_no_error;
	try {
		return ((stream_inlet_impl*)in)->pull_chunk_multiplexed(data_buffer,timestamp_buffer,data_buffer_elements,timestamp_buffer_elements,timeout);
	}
	catch(timeout_error &) { 
		if (ec)
			*ec = lsl_timeout_error; 
	}
	catch(lost_error &) { 
		if (ec)
			*ec = lsl_lost_error; 
	}
	catch(std::invalid_argument &) { 
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::range_error &) {
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::exception &) { 
		if (ec)
			*ec = lsl_internal_error; 
	}
	return 0;
}

LIBLSL_C_API unsigned long lsl_pull_chunk_str(lsl_inlet in, char **data_buffer, double *timestamp_buffer, unsigned long data_buffer_elements, unsigned long timestamp_buffer_elements, double timeout, int *ec) {
	if (ec)
		*ec = lsl_no_error;
	try {
		// capture output in a temporary string buffer
		if (data_buffer_elements) {
			std::vector<std::string> tmp(data_buffer_elements);
			unsigned long result = ((stream_inlet_impl*)in)->pull_chunk_multiplexed(&tmp[0],timestamp_buffer,data_buffer_elements,timestamp_buffer_elements,timeout);
			// allocate memory and copy over into buffer
			for (unsigned k=0;k<tmp.size();k++) {
				data_buffer[k] = (char*)malloc(tmp[k].size()+1);
				if (data_buffer[k] == NULL) {
					for (unsigned k2=0;k2<k;k2++)
						free(data_buffer[k2]);
					*ec = lsl_internal_error;
					return 0;
				}
				strcpy(data_buffer[k],tmp[k].c_str());
			}
			return result;
		} else
			return 0;
	}
	catch(timeout_error &) { 
		if (ec)
			*ec = lsl_timeout_error; 
	}
	catch(lost_error &) { 
		if (ec)
			*ec = lsl_lost_error; 
	}
	catch(std::invalid_argument &) { 
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::range_error &) {
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::exception &) { 
		if (ec)
			*ec = lsl_internal_error; 
	}
	return 0;
}

LIBLSL_C_API unsigned long lsl_pull_chunk_buf(lsl_inlet in, char **data_buffer, unsigned *lengths_buffer, double *timestamp_buffer, unsigned long data_buffer_elements, unsigned long timestamp_buffer_elements, double timeout, int *ec) {
	if (ec)
		*ec = lsl_no_error;
	try {
		// capture output in a temporary string buffer
		if (data_buffer_elements) {
			std::vector<std::string> tmp(data_buffer_elements);
			unsigned long result = ((stream_inlet_impl*)in)->pull_chunk_multiplexed(&tmp[0],timestamp_buffer,data_buffer_elements,timestamp_buffer_elements,timeout);
			// allocate memory and copy over into buffer
			for (unsigned k=0;k<tmp.size();k++) {
				data_buffer[k] = (char*)malloc(tmp[k].size()+1);
				if (data_buffer[k] == NULL) {
					for (unsigned k2=0;k2<k;k++)
						free(data_buffer[k2]);
					*ec = lsl_internal_error;
					return 0;
				}
				lengths_buffer[k] = (unsigned)tmp[k].size();
				strcpy(data_buffer[k],tmp[k].c_str());
			}
			return result;
		} else
			return 0;
	}
	catch(timeout_error &) { 
		if (ec)
			*ec = lsl_timeout_error; 
	}
	catch(lost_error &) { 
		if (ec)
			*ec = lsl_lost_error; 
	}
	catch(std::invalid_argument &) { 
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::range_error &) {
		if (ec)
			*ec = lsl_argument_error; 
	}
	catch(std::exception &) { 
		if (ec)
			*ec = lsl_internal_error; 
	}
	return 0;
}

/**
* Query the number of samples that are currently available for immediate pickup.
*/
LIBLSL_C_API unsigned lsl_samples_available(lsl_inlet in) {
	try {
		return (unsigned)((stream_inlet_impl*)in)->samples_available();
	}
	catch(std::exception &) {
		return 0;
	}
}

/**
* Query whether the clock was potentially reset since the last call to was_clock_reset().
*/
LIBLSL_C_API unsigned lsl_was_clock_reset(lsl_inlet in) {
	try {
		return (unsigned)((stream_inlet_impl*)in)->was_clock_reset();
	}
	catch(std::exception &) {
		return 0;
	}
}

/**
* Override the half-time (forget factor) of the time-stamp smoothing.
*/
LIBLSL_C_API int lsl_smoothing_halftime(lsl_inlet in, float value) {
	try {
		((stream_inlet_impl*)in)->smoothing_halftime(value);
		return lsl_no_error;
	}
	catch(std::invalid_argument &) { 
		return lsl_argument_error; 
	}
	catch(std::exception &) {
		return lsl_internal_error;
	}
}
