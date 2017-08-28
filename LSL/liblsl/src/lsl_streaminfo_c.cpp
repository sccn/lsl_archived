#include "../include/lsl_c.h"
#include "stream_info_impl.h"
#include <string.h>
#include <iostream>


// === Implementation of the streaminfo-related functions of the C API ===

using namespace lsl;

// boilerplate wrapper code
LIBLSL_C_API lsl_streaminfo lsl_create_streaminfo(char *name, char *type, int channel_count, double nominal_srate, lsl_channel_format_t channel_format, char *source_id) { 
	try {
		if (!source_id)
			source_id = (char*)"";
		return (lsl_streaminfo)new stream_info_impl(name,type,channel_count,nominal_srate,(channel_format_t)channel_format,source_id); 
	} catch(std::exception &e) {
		std::cerr << "Unexpected error during streaminfo construction: " << e.what() << std::endl;
		return NULL;
	}
}

LIBLSL_C_API lsl_streaminfo lsl_copy_streaminfo(lsl_streaminfo info) { 
	try {
		return (lsl_streaminfo)new stream_info_impl(*(stream_info_impl*)info); 
	} catch(std::exception &e) {
		std::cerr << "Unexpected error while copying a streaminfo: " << e.what() << std::endl;
		return NULL;
	}
}

LIBLSL_C_API void lsl_destroy_streaminfo(lsl_streaminfo info) {
	try {
		delete (stream_info_impl*)info;
	} catch(std::exception &e) {
		std::cerr << "Unexpected error while destroying a streaminfo: " << e.what() << std::endl;
	}
}

LIBLSL_C_API char *lsl_get_name(lsl_streaminfo info) { return const_cast<char*>(((stream_info_impl*)info)->name().c_str()); }
LIBLSL_C_API char *lsl_get_type(lsl_streaminfo info) { return const_cast<char*>(((stream_info_impl*)info)->type().c_str()); }
LIBLSL_C_API int lsl_get_channel_count(lsl_streaminfo info) { return ((stream_info_impl*)info)->channel_count(); }
LIBLSL_C_API double lsl_get_nominal_srate(lsl_streaminfo info) { return ((stream_info_impl*)info)->nominal_srate(); }
LIBLSL_C_API lsl_channel_format_t lsl_get_channel_format(lsl_streaminfo info) { return (lsl_channel_format_t)((stream_info_impl*)info)->channel_format(); }
LIBLSL_C_API char *lsl_get_source_id(lsl_streaminfo info) { return const_cast<char*>(((stream_info_impl*)info)->source_id().c_str()); }
LIBLSL_C_API int lsl_get_version(lsl_streaminfo info) { return ((stream_info_impl*)info)->version(); }
LIBLSL_C_API double lsl_get_created_at(lsl_streaminfo info) { return ((stream_info_impl*)info)->created_at(); }
LIBLSL_C_API char *lsl_get_uid(lsl_streaminfo info) { return const_cast<char*>(((stream_info_impl*)info)->uid().c_str()); }
LIBLSL_C_API char *lsl_get_session_id(lsl_streaminfo info) { return const_cast<char*>(((stream_info_impl*)info)->session_id().c_str()); }
LIBLSL_C_API char *lsl_get_hostname(lsl_streaminfo info) { return const_cast<char*>(((stream_info_impl*)info)->hostname().c_str()); }
LIBLSL_C_API lsl_xml_ptr lsl_get_desc(lsl_streaminfo info) { return (lsl_xml_ptr)((stream_info_impl*)info)->desc().internal_object(); }

LIBLSL_C_API char *lsl_get_xml(lsl_streaminfo info) {
	try {
		std::string tmp = ((stream_info_impl*)info)->to_fullinfo_message();
		char *result = (char*)malloc(tmp.size()+1);
		strcpy(result,tmp.c_str());
		return result;
	} catch(std::exception &e) {
		std::cerr << "Unexpected error in lsl_get_xml: " << e.what() << std::endl;
		return NULL;
	}
}
LIBLSL_C_API int lsl_get_channel_bytes(lsl_streaminfo info) { return ((stream_info_impl*)info)->channel_bytes(); }
LIBLSL_C_API int lsl_get_sample_bytes(lsl_streaminfo info) { return ((stream_info_impl*)info)->sample_bytes(); }

LIBLSL_C_API lsl_streaminfo lsl_streaminfo_from_xml(char *xml) {
	try {
		stream_info_impl *impl = new stream_info_impl(); 
		impl->from_fullinfo_message(xml);
		return (lsl_streaminfo)impl;
	} catch(std::exception &e) {
		std::cerr << "Unexpected error during streaminfo construction: " << e.what() << std::endl;
		return NULL;
	}
}
