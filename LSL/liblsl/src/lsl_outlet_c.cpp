#include "../include/lsl_c.h"
#include "stream_outlet_impl.h"


extern "C" {
// === implementation of the lsl_outlet functions of the C API ===

using namespace lsl;

// boilerplate wrapper code
LIBLSL_C_API lsl_outlet lsl_create_outlet(lsl_streaminfo info, int32_t chunk_size, int32_t max_buffered) { 
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

LIBLSL_C_API int32_t lsl_push_sample_f(lsl_outlet out, const float *data) { 
	return ((stream_outlet_impl*)out)->push_sample_noexcept(data);
}

LIBLSL_C_API int32_t lsl_push_sample_ft(lsl_outlet out, const float *data, double timestamp) {
	return ((stream_outlet_impl*)out)->push_sample_noexcept(data, timestamp);
}

LIBLSL_C_API int32_t lsl_push_sample_ftp(lsl_outlet out, const float *data, double timestamp, int32_t pushthrough) { 
	return ((stream_outlet_impl*)out)->push_sample_noexcept(data, timestamp, pushthrough);
}

LIBLSL_C_API int32_t lsl_push_sample_d(lsl_outlet out, const double *data) { 
	return ((stream_outlet_impl*)out)->push_sample_noexcept(data);
}

LIBLSL_C_API int32_t lsl_push_sample_dt(lsl_outlet out, const double *data, double timestamp) { 
	return ((stream_outlet_impl*)out)->push_sample_noexcept(data, timestamp);
}

LIBLSL_C_API int32_t lsl_push_sample_dtp(lsl_outlet out, const double *data, double timestamp, int32_t pushthrough) { 
	return ((stream_outlet_impl*)out)->push_sample_noexcept(data, timestamp, pushthrough);
}

LIBLSL_C_API int32_t lsl_push_sample_l(lsl_outlet out, const long *data) {
	return ((stream_outlet_impl*)out)->push_sample_noexcept(data);
}

LIBLSL_C_API int32_t lsl_push_sample_lt(lsl_outlet out, const long *data, double timestamp) {
	return ((stream_outlet_impl*)out)->push_sample_noexcept(data, timestamp);
}

LIBLSL_C_API int32_t lsl_push_sample_ltp(lsl_outlet out, const long *data, double timestamp, int32_t pushthrough) {
	return ((stream_outlet_impl*)out)->push_sample_noexcept(data, timestamp, pushthrough);
}

LIBLSL_C_API int32_t lsl_push_sample_i(lsl_outlet out, const int32_t *data) { 
	return ((stream_outlet_impl*)out)->push_sample_noexcept(data);
}

LIBLSL_C_API int32_t lsl_push_sample_it(lsl_outlet out, const int32_t *data, double timestamp) { 
	return ((stream_outlet_impl*)out)->push_sample_noexcept(data, timestamp);
}

LIBLSL_C_API int32_t lsl_push_sample_itp(lsl_outlet out, const int32_t *data, double timestamp, int32_t pushthrough) { 
	return ((stream_outlet_impl*)out)->push_sample_noexcept(data, timestamp, pushthrough);
}

LIBLSL_C_API int32_t lsl_push_sample_s(lsl_outlet out, const int16_t *data) { 
	return ((stream_outlet_impl*)out)->push_sample_noexcept(data);
}

LIBLSL_C_API int32_t lsl_push_sample_st(lsl_outlet out, const int16_t *data, double timestamp) { 
	return ((stream_outlet_impl*)out)->push_sample_noexcept(data, timestamp);
}

LIBLSL_C_API int32_t lsl_push_sample_stp(lsl_outlet out, const int16_t *data, double timestamp, int32_t pushthrough) { 
	return ((stream_outlet_impl*)out)->push_sample_noexcept(data, timestamp, pushthrough);
}

LIBLSL_C_API int32_t lsl_push_sample_c(lsl_outlet out, const char *data) { 
	return ((stream_outlet_impl*)out)->push_sample_noexcept(data);
}

LIBLSL_C_API int32_t lsl_push_sample_ct(lsl_outlet out, const char *data, double timestamp) { 
	return ((stream_outlet_impl*)out)->push_sample_noexcept(data, timestamp);
}

LIBLSL_C_API int32_t lsl_push_sample_ctp(lsl_outlet out, const char *data, double timestamp, int32_t pushthrough) { 
	return ((stream_outlet_impl*)out)->push_sample_noexcept(data, timestamp, pushthrough);
}

LIBLSL_C_API int32_t lsl_push_sample_v(lsl_outlet out, const void *data) { 
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

LIBLSL_C_API int32_t lsl_push_sample_vt(lsl_outlet out, const void *data, double timestamp) { 
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

LIBLSL_C_API int32_t lsl_push_sample_vtp(lsl_outlet out, const void *data, double timestamp, int32_t pushthrough) { 
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

LIBLSL_C_API int32_t lsl_push_sample_str(lsl_outlet out, const char **data) {
	return lsl_push_sample_strtp(out, data, 0.0, true);
}

LIBLSL_C_API int32_t lsl_push_sample_strt(lsl_outlet out, const char **data, double timestamp) {
	return lsl_push_sample_strtp(out, data, timestamp, true);
}

LIBLSL_C_API int32_t lsl_push_sample_strtp(lsl_outlet out, const char **data, double timestamp, int32_t pushthrough) {
	try {
		stream_outlet_impl* outimpl = (stream_outlet_impl*)out;
		std::vector<std::string> tmp;
		for (uint32_t k=0;k<(uint32_t)outimpl->info().channel_count();k++)
			tmp.push_back(std::string(data[k]));
		return outimpl->push_sample_noexcept(&tmp[0],timestamp,pushthrough!=0);
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int32_t lsl_push_sample_buf(lsl_outlet out, const char **data, const uint32_t *lengths) {
	return lsl_push_sample_buftp(out, data, lengths, 0.0, true);
}

LIBLSL_C_API int32_t lsl_push_sample_buft(lsl_outlet out, const char **data, const uint32_t *lengths, double timestamp) {
	return lsl_push_sample_buftp(out, data, lengths, timestamp, true);
}

LIBLSL_C_API int32_t lsl_push_sample_buftp(lsl_outlet out, const char **data, const uint32_t *lengths, double timestamp, int32_t pushthrough) {
	try {
		stream_outlet_impl* outimpl = (stream_outlet_impl*)out;
		std::vector<std::string> tmp;
		for (uint32_t k=0;k<(uint32_t)outimpl->info().channel_count();k++)
			tmp.push_back(std::string(data[k],lengths[k]));
		return outimpl->push_sample_noexcept(&tmp[0], timestamp, pushthrough);
	}
	catch(std::exception &e) {
		std::cerr << "Unexpected error during push_sample: " << e.what() << std::endl;
		return lsl_internal_error;
	}
}

LIBLSL_C_API int32_t lsl_push_chunk_f(lsl_outlet out, const float *data, uint32_t data_elements) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data,data_elements);
}

LIBLSL_C_API int32_t lsl_push_chunk_d(lsl_outlet out, const double *data, uint32_t data_elements) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data,data_elements);
}

LIBLSL_C_API int32_t lsl_push_chunk_l(lsl_outlet out, const long *data, uint32_t data_elements) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data,data_elements);
}

LIBLSL_C_API int32_t lsl_push_chunk_i(lsl_outlet out, const int32_t *data, uint32_t data_elements) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data,data_elements);
}

LIBLSL_C_API int32_t lsl_push_chunk_s(lsl_outlet out, const int16_t *data, uint32_t data_elements) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data,data_elements);
}

LIBLSL_C_API int32_t lsl_push_chunk_c(lsl_outlet out, const char *data, uint32_t data_elements) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data,data_elements);
}

LIBLSL_C_API int32_t lsl_push_chunk_ft(lsl_outlet out, const float *data, uint32_t data_elements, double timestamp) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data,data_elements, timestamp);
}

LIBLSL_C_API int32_t lsl_push_chunk_dt(lsl_outlet out, const double *data, uint32_t data_elements, double timestamp) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data,data_elements, timestamp);
}

LIBLSL_C_API int32_t lsl_push_chunk_lt(lsl_outlet out, const long *data, uint32_t data_elements, double timestamp) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data,data_elements, timestamp);
}

LIBLSL_C_API int32_t lsl_push_chunk_it(lsl_outlet out, const int32_t *data, uint32_t data_elements, double timestamp) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data,data_elements, timestamp);
}

LIBLSL_C_API int32_t lsl_push_chunk_st(lsl_outlet out, const int16_t *data, uint32_t data_elements, double timestamp) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data,data_elements, timestamp);
}

LIBLSL_C_API int32_t lsl_push_chunk_ct(lsl_outlet out, const char *data, uint32_t data_elements, double timestamp) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data,data_elements, timestamp);
}

LIBLSL_C_API int32_t lsl_push_chunk_ftp(lsl_outlet out, const float *data, uint32_t data_elements, double timestamp, int32_t pushthrough) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data,data_elements, timestamp, pushthrough);
}

LIBLSL_C_API int32_t lsl_push_chunk_dtp(lsl_outlet out, const double *data, uint32_t data_elements, double timestamp, int32_t pushthrough) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data,data_elements, timestamp, pushthrough);
}

LIBLSL_C_API int32_t lsl_push_chunk_ltp(lsl_outlet out, const long *data, uint32_t data_elements, double timestamp, int32_t pushthrough) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data,data_elements, timestamp, pushthrough);
}

LIBLSL_C_API int32_t lsl_push_chunk_itp(lsl_outlet out, const int32_t *data, uint32_t data_elements, double timestamp, int32_t pushthrough) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data,data_elements, timestamp, pushthrough);
}

LIBLSL_C_API int32_t lsl_push_chunk_stp(lsl_outlet out, const int16_t *data, uint32_t data_elements, double timestamp, int32_t pushthrough) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data,data_elements, timestamp, pushthrough);
}

LIBLSL_C_API int32_t lsl_push_chunk_ctp(lsl_outlet out, const char *data, uint32_t data_elements, double timestamp, int32_t pushthrough) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data,data_elements, timestamp, pushthrough);
}

LIBLSL_C_API int32_t lsl_push_chunk_ftn(lsl_outlet out, const float *data, uint32_t data_elements, const double *timestamps) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data, timestamps, data_elements);
}

LIBLSL_C_API int32_t lsl_push_chunk_dtn(lsl_outlet out, const double *data, uint32_t data_elements, const double *timestamps) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data, timestamps, data_elements);
}

LIBLSL_C_API int32_t lsl_push_chunk_ltn(lsl_outlet out, const long *data, uint32_t data_elements, const double *timestamps) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data, timestamps, data_elements);
}

LIBLSL_C_API int32_t lsl_push_chunk_itn(lsl_outlet out, const int32_t *data, uint32_t data_elements, const double *timestamps) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data, timestamps, data_elements);
}

LIBLSL_C_API int32_t lsl_push_chunk_stn(lsl_outlet out, const int16_t *data, uint32_t data_elements, const double *timestamps) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data, timestamps, data_elements);
}

LIBLSL_C_API int32_t lsl_push_chunk_ctn(lsl_outlet out, const char *data, uint32_t data_elements, const double *timestamps) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data, timestamps, data_elements);
}

LIBLSL_C_API int32_t lsl_push_chunk_ftnp(lsl_outlet out, const float *data, uint32_t data_elements, const double *timestamps, int32_t pushthrough) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data, timestamps, data_elements, pushthrough);
}

LIBLSL_C_API int32_t lsl_push_chunk_dtnp(lsl_outlet out, const double *data, uint32_t data_elements, const double *timestamps, int32_t pushthrough) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data, timestamps, data_elements, pushthrough);
}

LIBLSL_C_API int32_t lsl_push_chunk_ltnp(lsl_outlet out, const long *data, uint32_t data_elements, const double *timestamps, int32_t pushthrough) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data, timestamps, data_elements, pushthrough);
}

LIBLSL_C_API int32_t lsl_push_chunk_itnp(lsl_outlet out, const int32_t *data, uint32_t data_elements, const double *timestamps, int32_t pushthrough) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data, timestamps, data_elements, pushthrough);
}

LIBLSL_C_API int32_t lsl_push_chunk_stnp(lsl_outlet out, const int16_t *data, uint32_t data_elements, const double *timestamps, int32_t pushthrough) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data, timestamps, data_elements, pushthrough);
}

LIBLSL_C_API int32_t lsl_push_chunk_ctnp(lsl_outlet out, const char *data, uint32_t data_elements, const double *timestamps, int32_t pushthrough) {
	return ((stream_outlet_impl*)out)->push_chunk_multiplexed_noexcept(data, timestamps, data_elements, pushthrough);
}

LIBLSL_C_API int32_t lsl_push_chunk_str(lsl_outlet out, const char **data, uint32_t data_elements) {
	return lsl_push_chunk_strtp(out, data, data_elements, 0.0, true);
}

LIBLSL_C_API int32_t lsl_push_chunk_strt(lsl_outlet out, const char **data, uint32_t data_elements, double timestamp) {
	return lsl_push_chunk_strtp(out, data, data_elements, timestamp, true);
}

LIBLSL_C_API int32_t lsl_push_chunk_strtp(lsl_outlet out, const char **data, uint32_t data_elements, double timestamp, int32_t pushthrough) {
	try {
		std::vector<std::string> tmp;
		for (uint32_t k=0;k<data_elements;k++)
			tmp.push_back(std::string(data[k]));
		if (data_elements)
			((stream_outlet_impl*)out)->push_chunk_multiplexed(&tmp[0], tmp.size(), timestamp, pushthrough);
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

LIBLSL_C_API int32_t lsl_push_chunk_strtn(lsl_outlet out, const char **data, uint32_t data_elements, const double *timestamps) {
	return lsl_push_chunk_strtnp(out, data, data_elements, timestamps, true);
}

LIBLSL_C_API int32_t lsl_push_chunk_strtnp(lsl_outlet out, const char **data, uint32_t data_elements, const double *timestamps, int32_t pushthrough) {
	try {
		if (data_elements) {
			std::vector<std::string> tmp;
			for (uint32_t k=0;k<data_elements;k++)
				tmp.push_back(std::string(data[k]));
			((stream_outlet_impl*)out)->push_chunk_multiplexed(&tmp[0], timestamps, tmp.size(), pushthrough);
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

LIBLSL_C_API int32_t lsl_push_chunk_buf(lsl_outlet out, const char **data, const uint32_t *lengths, uint32_t data_elements) {
	return lsl_push_chunk_buftp(out, data, lengths, data_elements, 0.0, true);
}

LIBLSL_C_API int32_t lsl_push_chunk_buft(lsl_outlet out, const char **data, const uint32_t *lengths, uint32_t data_elements, double timestamp) {
	return lsl_push_chunk_buftp(out, data, lengths, data_elements, timestamp, true);
}

LIBLSL_C_API int32_t lsl_push_chunk_buftp(lsl_outlet out, const char **data, const uint32_t *lengths, uint32_t data_elements, double timestamp, int32_t pushthrough) {
	try {
		std::vector<std::string> tmp;
		for (uint32_t k=0;k<data_elements;k++)
			tmp.push_back(std::string(data[k],lengths[k]));
		if (data_elements)
			((stream_outlet_impl*)out)->push_chunk_multiplexed(&tmp[0], tmp.size(), timestamp, pushthrough);
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

LIBLSL_C_API int32_t lsl_push_chunk_buftn(lsl_outlet out, const char **data, const uint32_t *lengths, uint32_t data_elements, const double *timestamps) {
	return lsl_push_chunk_buftnp(out, data, lengths, data_elements, timestamps, true);
}

LIBLSL_C_API int32_t lsl_push_chunk_buftnp(lsl_outlet out, const char **data, const uint32_t *lengths, uint32_t data_elements, const double *timestamps, int32_t pushthrough) {
	try {
		if (data_elements) {
			std::vector<std::string> tmp;
			for (uint32_t k=0;k<data_elements;k++)
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

LIBLSL_C_API int32_t lsl_have_consumers(lsl_outlet out) { 
	try {
		return ((stream_outlet_impl*)out)->have_consumers(); 
	} 
	catch(std::exception &e) {
		std::cerr << "Unexpected error in have_consumers: " << e.what() << std::endl;
		return 1;
	}
}

LIBLSL_C_API int32_t lsl_wait_for_consumers(lsl_outlet out, double timeout) {
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

}
