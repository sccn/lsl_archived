#include "../include/lsl_c.h"
#define NO_EXPLICIT_TEMPLATE_INSTANTIATION // a convention that applies when including portable_oarchive.h in multiple .cpp files.
#include "stream_outlet_impl.h"


// === implementation of the lsl_outlet functions of the C API ===

using namespace lsl;

// boilerplate wrapper code
LIBLSL_C_API lsl_outlet lsl_create_outlet(lsl_streaminfo info, int chunk_size, int max_buffered) { 
	try {
		stream_info_impl *infoimpl = (stream_info_impl*)info;
		lsl_outlet result = (lsl_outlet)new stream_outlet_impl(*infoimpl, chunk_size, infoimpl->nominal_srate()?(int)(infoimpl->nominal_srate()*max_buffered):max_buffered*100);
		return result;
	} catch(std::exception &e) {
		std::cerr << "Unexpected error during construction of stream outlet: " << e.what() << std::endl;
		return NULL;
	}
}

LIBLSL_C_API void lsl_destroy_outlet(lsl_outlet out) { 
	try {
		delete (stream_outlet_impl*)out; 
	} catch(std::exception &e) {
		std::cerr << "Unexpected error during deletion of stream outlet: " << e.what() << std::endl;
	}
}

LIBLSL_C_API int lsl_push_sample_f(lsl_outlet out, float *data) { 
	try {
		((stream_outlet_impl*)out)->push_sample(data);
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_ft(lsl_outlet out, float *data, double timestamp) {
	try {
		((stream_outlet_impl*)out)->push_sample(data,timestamp); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_ftp(lsl_outlet out, float *data, double timestamp, int pushthrough) { 
	try {
		((stream_outlet_impl*)out)->push_sample(data,timestamp,pushthrough!=0); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_d(lsl_outlet out, double *data) { 
	try {
		((stream_outlet_impl*)out)->push_sample(data); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_dt(lsl_outlet out, double *data, double timestamp) { 
	try {
		((stream_outlet_impl*)out)->push_sample(data,timestamp); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_dtp(lsl_outlet out, double *data, double timestamp, int pushthrough) { 
	try {
		((stream_outlet_impl*)out)->push_sample(data,timestamp,pushthrough!=0); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_l(lsl_outlet out, long *data) { 
	try {
		((stream_outlet_impl*)out)->push_sample(data); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_lt(lsl_outlet out, long *data, double timestamp) { 
	try {
		((stream_outlet_impl*)out)->push_sample(data,timestamp); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_ltp(lsl_outlet out, long *data, double timestamp, int pushthrough) { 
	try {
		((stream_outlet_impl*)out)->push_sample(data,timestamp,pushthrough!=0); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_i(lsl_outlet out, int *data) { 
	try {
		((stream_outlet_impl*)out)->push_sample(data); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_it(lsl_outlet out, int *data, double timestamp) { 
	try {
		((stream_outlet_impl*)out)->push_sample(data,timestamp); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_itp(lsl_outlet out, int *data, double timestamp, int pushthrough) { 
	try {
		((stream_outlet_impl*)out)->push_sample(data,timestamp,pushthrough!=0); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_s(lsl_outlet out, short *data) { 
	try {
		((stream_outlet_impl*)out)->push_sample(data); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_st(lsl_outlet out, short *data, double timestamp) { 
	try {
		((stream_outlet_impl*)out)->push_sample(data,timestamp); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_stp(lsl_outlet out, short *data, double timestamp, int pushthrough) { 
	try {
		((stream_outlet_impl*)out)->push_sample(data,timestamp,pushthrough!=0); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_c(lsl_outlet out, char *data) { 
	try {
		((stream_outlet_impl*)out)->push_sample(data); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_ct(lsl_outlet out, char *data, double timestamp) { 
	try {
		((stream_outlet_impl*)out)->push_sample(data,timestamp); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_ctp(lsl_outlet out, char *data, double timestamp, int pushthrough) { 
	try {
		((stream_outlet_impl*)out)->push_sample(data,timestamp,pushthrough!=0); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_v(lsl_outlet out, void *data) { 
	try {
		((stream_outlet_impl*)out)->push_numeric_raw(data); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_vt(lsl_outlet out, void *data, double timestamp) { 
	try {
		((stream_outlet_impl*)out)->push_numeric_raw(data,timestamp); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_vtp(lsl_outlet out, void *data, double timestamp, int pushthrough) { 
	try {
		((stream_outlet_impl*)out)->push_numeric_raw(data,timestamp,pushthrough!=0); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_str(lsl_outlet out, char **data) {
	try {
		stream_outlet_impl* outimpl = (stream_outlet_impl*)out;
		std::vector<std::string> tmp;
		for (unsigned k=0;k<(unsigned)outimpl->info().channel_count();k++)
			tmp.push_back(std::string(data[k]));
		outimpl->push_sample(&tmp[0]);
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_strt(lsl_outlet out, char **data, double timestamp) {
	try {
		stream_outlet_impl* outimpl = (stream_outlet_impl*)out;
		std::vector<std::string> tmp;
		for (unsigned k=0;k<(unsigned)outimpl->info().channel_count();k++)
			tmp.push_back(std::string(data[k]));
		outimpl->push_sample(&tmp[0],timestamp);
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_strtp(lsl_outlet out, char **data, double timestamp, int pushthrough) {
	try {
		stream_outlet_impl* outimpl = (stream_outlet_impl*)out;
		std::vector<std::string> tmp;
		for (unsigned k=0;k<(unsigned)outimpl->info().channel_count();k++)
			tmp.push_back(std::string(data[k]));
		outimpl->push_sample(&tmp[0],timestamp,pushthrough!=0);
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_buf(lsl_outlet out, char **data, unsigned *lengths) {
	try {
		stream_outlet_impl* outimpl = (stream_outlet_impl*)out;
		std::vector<std::string> tmp;
		for (unsigned k=0;k<(unsigned)outimpl->info().channel_count();k++)
			tmp.push_back(std::string(data[k],lengths[k]));
		outimpl->push_sample(&tmp[0]);
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_buft(lsl_outlet out, char **data, unsigned *lengths, double timestamp) {
	try {
		stream_outlet_impl* outimpl = (stream_outlet_impl*)out;
		std::vector<std::string> tmp;
		for (unsigned k=0;k<(unsigned)outimpl->info().channel_count();k++)
			tmp.push_back(std::string(data[k],lengths[k]));
		outimpl->push_sample(&tmp[0],timestamp);
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_sample_buftp(lsl_outlet out, char **data, unsigned *lengths, double timestamp, int pushthrough) {
	try {
		stream_outlet_impl* outimpl = (stream_outlet_impl*)out;
		std::vector<std::string> tmp;
		for (unsigned k=0;k<(unsigned)outimpl->info().channel_count();k++)
			tmp.push_back(std::string(data[k],lengths[k]));
		outimpl->push_sample(&tmp[0],timestamp,pushthrough!=0);
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_sample: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_f(lsl_outlet out, float *data, unsigned long data_elements) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,data_elements); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_d(lsl_outlet out, double *data, unsigned long data_elements) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,data_elements); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_l(lsl_outlet out, long *data, unsigned long data_elements) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,data_elements); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_i(lsl_outlet out, int *data, unsigned long data_elements) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,data_elements); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_s(lsl_outlet out, short *data, unsigned long data_elements) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,data_elements); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_c(lsl_outlet out, char *data, unsigned long data_elements) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,data_elements); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_ft(lsl_outlet out, float *data, unsigned long data_elements, double timestamp) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,data_elements,timestamp); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_dt(lsl_outlet out, double *data, unsigned long data_elements, double timestamp) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,data_elements,timestamp); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_lt(lsl_outlet out, long *data, unsigned long data_elements, double timestamp) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,data_elements,timestamp); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_it(lsl_outlet out, int *data, unsigned long data_elements, double timestamp) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,data_elements,timestamp); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_st(lsl_outlet out, short *data, unsigned long data_elements, double timestamp) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,data_elements,timestamp); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_ct(lsl_outlet out, char *data, unsigned long data_elements, double timestamp) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,data_elements,timestamp); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_ftp(lsl_outlet out, float *data, unsigned long data_elements, double timestamp, int pushthrough) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,data_elements,timestamp,pushthrough); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_dtp(lsl_outlet out, double *data, unsigned long data_elements, double timestamp, int pushthrough) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,data_elements,timestamp,pushthrough); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_ltp(lsl_outlet out, long *data, unsigned long data_elements, double timestamp, int pushthrough) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,data_elements,timestamp,pushthrough); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_itp(lsl_outlet out, int *data, unsigned long data_elements, double timestamp, int pushthrough) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,data_elements,timestamp,pushthrough); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_stp(lsl_outlet out, short *data, unsigned long data_elements, double timestamp, int pushthrough) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,data_elements,timestamp,pushthrough); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_ctp(lsl_outlet out, char *data, unsigned long data_elements, double timestamp, int pushthrough) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,data_elements,timestamp,pushthrough); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_ftn(lsl_outlet out, float *data, unsigned long data_elements, double *timestamps) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,timestamps,data_elements); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_dtn(lsl_outlet out, double *data, unsigned long data_elements, double *timestamps) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,timestamps,data_elements); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_ltn(lsl_outlet out, long *data, unsigned long data_elements, double *timestamps) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,timestamps,data_elements); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_itn(lsl_outlet out, int *data, unsigned long data_elements, double *timestamps) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,timestamps,data_elements); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_stn(lsl_outlet out, short *data, unsigned long data_elements, double *timestamps) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,timestamps,data_elements); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_ctn(lsl_outlet out, char *data, unsigned long data_elements, double *timestamps) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,timestamps,data_elements); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_ftnp(lsl_outlet out, float *data, unsigned long data_elements, double *timestamps, int pushthrough) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,timestamps,data_elements,pushthrough); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_dtnp(lsl_outlet out, double *data, unsigned long data_elements, double *timestamps, int pushthrough) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,timestamps,data_elements,pushthrough); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_ltnp(lsl_outlet out, long *data, unsigned long data_elements, double *timestamps, int pushthrough) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,timestamps,data_elements,pushthrough); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_itnp(lsl_outlet out, int *data, unsigned long data_elements, double *timestamps, int pushthrough) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,timestamps,data_elements,pushthrough); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_stnp(lsl_outlet out, short *data, unsigned long data_elements, double *timestamps, int pushthrough) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,timestamps,data_elements,pushthrough); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_ctnp(lsl_outlet out, char *data, unsigned long data_elements, double *timestamps, int pushthrough) {
	try {
		((stream_outlet_impl*)out)->push_chunk_multiplexed(data,timestamps,data_elements,pushthrough); 
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_str(lsl_outlet out, char **data, unsigned long data_elements) {
	try {
		std::vector<std::string> tmp;
		for (unsigned long k=0;k<data_elements;k++)
			tmp.push_back(std::string(data[k]));
		if (data_elements)
			((stream_outlet_impl*)out)->push_chunk_multiplexed(&tmp[0],tmp.size());
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_strt(lsl_outlet out, char **data, unsigned long data_elements, double timestamp) {
	try {
		std::vector<std::string> tmp;
		for (unsigned long k=0;k<data_elements;k++)
			tmp.push_back(std::string(data[k]));
		if (data_elements)
			((stream_outlet_impl*)out)->push_chunk_multiplexed(&tmp[0],tmp.size(),timestamp);
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_strtp(lsl_outlet out, char **data, unsigned long data_elements, double timestamp, int pushthrough) {
	try {
		std::vector<std::string> tmp;
		for (unsigned long k=0;k<data_elements;k++)
			tmp.push_back(std::string(data[k]));
		if (data_elements)
			((stream_outlet_impl*)out)->push_chunk_multiplexed(&tmp[0],tmp.size(),timestamp,pushthrough);
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_strtn(lsl_outlet out, char **data, unsigned long data_elements, double *timestamps) {
	try {
		if (data_elements) {
			std::vector<std::string> tmp;
			for (unsigned long k=0;k<data_elements;k++)
				tmp.push_back(std::string(data[k]));
			((stream_outlet_impl*)out)->push_chunk_multiplexed(&tmp[0],timestamps,(std::size_t)data_elements);
		}
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_strtnp(lsl_outlet out, char **data, unsigned long data_elements, double *timestamps, int pushthrough) {
	try {
		if (data_elements) {
			std::vector<std::string> tmp;
			for (unsigned long k=0;k<data_elements;k++)
				tmp.push_back(std::string(data[k]));
			((stream_outlet_impl*)out)->push_chunk_multiplexed(&tmp[0],timestamps,(std::size_t)data_elements,pushthrough);
		}
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_buf(lsl_outlet out, char **data, unsigned *lengths, unsigned long data_elements) {
	try {
		std::vector<std::string> tmp;
		for (unsigned long k=0;k<data_elements;k++)
			tmp.push_back(std::string(data[k],lengths[k]));
		if (data_elements)
			((stream_outlet_impl*)out)->push_chunk_multiplexed(&tmp[0],tmp.size());
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_buft(lsl_outlet out, char **data, unsigned *lengths, unsigned long data_elements, double timestamp) {
	try {
		std::vector<std::string> tmp;
		for (unsigned long k=0;k<data_elements;k++)
			tmp.push_back(std::string(data[k],lengths[k]));
		if (data_elements)
			((stream_outlet_impl*)out)->push_chunk_multiplexed(&tmp[0],tmp.size(),timestamp);
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_buftp(lsl_outlet out, char **data, unsigned *lengths, unsigned long data_elements, double timestamp, int pushthrough) {
	try {
		std::vector<std::string> tmp;
		for (unsigned long k=0;k<data_elements;k++)
			tmp.push_back(std::string(data[k],lengths[k]));
		if (data_elements)
			((stream_outlet_impl*)out)->push_chunk_multiplexed(&tmp[0],tmp.size(),timestamp,pushthrough);
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_buftn(lsl_outlet out, char **data, unsigned *lengths, unsigned long data_elements, double *timestamps) {
	try {
		if (data_elements) {
			std::vector<std::string> tmp;
			for (unsigned long k=0;k<data_elements;k++)
				tmp.push_back(std::string(data[k],lengths[k]));
			((stream_outlet_impl*)out)->push_chunk_multiplexed(&tmp[0],timestamps,(std::size_t)data_elements);
		}
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_push_chunk_buftnp(lsl_outlet out, char **data, unsigned *lengths, unsigned long data_elements, double *timestamps, int pushthrough) {
	try {
		if (data_elements) {
			std::vector<std::string> tmp;
			for (unsigned long k=0;k<data_elements;k++)
				tmp.push_back(std::string(data[k],lengths[k]));
			((stream_outlet_impl*)out)->push_chunk_multiplexed(&tmp[0],timestamps,(std::size_t)data_elements,pushthrough);
		}
		return lsl_no_error;
	} 
	catch(std::range_error &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::invalid_argument &e) {
		std::cerr << "Error during push_chunk: " << e.what() << std::endl;
		return lsl_argument_error;
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_chunk: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int lsl_have_consumers(lsl_outlet out) { 
	try {
		return ((stream_outlet_impl*)out)->have_consumers(); 
	} 
	catch(std::exception &e) {
		std::cerr << "Unexpected error in have_consumers: " << e.what() << std::endl;
		return 1;
	}
}

LIBLSL_C_API int lsl_wait_for_consumers(lsl_outlet out, double timeout) {
	try {
		return ((stream_outlet_impl*)out)->wait_for_consumers(timeout);
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error in wait_for_consumers: " << e.what() << std::endl;
		return 1;
	}
}

LIBLSL_C_API lsl_streaminfo lsl_get_info(lsl_outlet out) { 
	try {
		return (lsl_streaminfo)new stream_info_impl(((stream_outlet_impl*)out)->info()); 
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error in lsl_get_info: " << e.what() << std::endl;
		return NULL;
	}
}

