#include "mex.h"

/* library handling */
#ifdef _WIN32
    #include <windows.h>
    #define LOAD_LIBRARY(lpath) LoadLibrary(lpath)
    #define LOAD_FUNCTION(lhandle,fname) GetProcAddress(lhandle,fname)
    #define FREE_LIBRARY(lhandle) FreeLibrary(lhandle)
    #define ERROR_STRING() itoa(GetLastError(),malloc(32),10)
#else
    #include <dlfcn.h>
    #define LOAD_LIBRARY(lpath) dlopen(lpath,RTLD_NOW | RTLD_LOCAL)
    #define LOAD_FUNCTION(lhandle,fname) dlsym(lhandle,fname)
    #define FREE_LIBRARY(lhandle) dlclose(lhandle)
    #define ERROR_STRING() dlerror()
#endif

/* 32/64 bit handling */
#ifdef __LP64__
    #define uintptr_t uint64_T
    #define PTR_CLASS mxUINT64_CLASS
#else
    #ifdef _WIN64
        #define uintptr_t uint64_T
        #define PTR_CLASS mxUINT64_CLASS
    #else
        #define uintptr_t uint32_T
        #define PTR_CLASS mxUINT32_CLASS
    #endif
#endif
        
#ifdef _WIN32
    #define LINKAGE __cdecl
#else
    #define LINKAGE
#endif
    
/* some type declarations */
typedef void* streaminfo;
typedef void* outlet;
typedef void* inlet;
typedef void* continuous_resolver;
typedef void* xml_ptr;
        
typedef enum {
	cft_float32 = 1,
	cft_double64 = 2,
	cft_string = 3,
	cft_int32 = 4,
	cft_int16 = 5,
	cft_int8 = 6,
    cft_int64 = 7,
	cft_undefined = 0
} channel_format_t;

/**
* Possible error codes.
*/
typedef enum {
	lsl_no_error = 0,			/* No error occurred */
	lsl_timeout_error = -1,		/* The operation has timed out. */
	lsl_lost_error = -2,		/* The stream has been lost. */
	lsl_argument_error = -3,	/* An argument was incorrectly specified (e.g., wrong format or wrong length). */
	lsl_internal_error = -4		/* Some other internal error has happened. */
} lsl_error_code_t;

/* function handle types for free functions*/
typedef int (LINKAGE *lsl_protocol_version_t)(void);
typedef int (LINKAGE *lsl_library_version_t)(void);
typedef double (LINKAGE *lsl_local_clock_t)(void);
typedef int (LINKAGE *lsl_resolve_all_t)(streaminfo *buffer, unsigned buffer_elements, double timeout);
typedef int (LINKAGE *lsl_resolve_byprop_t)(streaminfo *buffer, unsigned buffer_elements, char *prop, char *value, int minimum, double timeout);
typedef int (LINKAGE *lsl_resolve_bypred_t)(streaminfo *buffer, unsigned buffer_elements, char *pred, int minimum, double timeout);

/* streaminfo object */
typedef streaminfo (LINKAGE *lsl_create_streaminfo_t)(char *name, char *type, int channel_count, double nominal_srate, channel_format_t channel_format, char *source_id);
typedef void (LINKAGE *lsl_destroy_streaminfo_t)(streaminfo info);
typedef char *(LINKAGE *lsl_get_name_t)(streaminfo info);
typedef char *(LINKAGE *lsl_get_type_t)(streaminfo info);
typedef int (LINKAGE *lsl_get_channel_count_t)(streaminfo info);
typedef double (LINKAGE *lsl_get_nominal_srate_t)(streaminfo info);
typedef channel_format_t (LINKAGE *lsl_get_channel_format_t)(streaminfo info);
typedef char *(LINKAGE *lsl_get_source_id_t)(streaminfo info);
typedef int (LINKAGE *lsl_get_version_t)(streaminfo info);
typedef double (LINKAGE *lsl_get_created_at_t)(streaminfo info);
typedef char *(LINKAGE *lsl_get_uid_t)(streaminfo info);
typedef char *(LINKAGE *lsl_get_session_id_t)(streaminfo info);
typedef char *(LINKAGE *lsl_get_hostname_t)(streaminfo info);
typedef xml_ptr (LINKAGE *lsl_get_desc_t)(streaminfo info);
typedef char *(LINKAGE *lsl_get_xml_t)(streaminfo info);

/* outlet object */
typedef outlet (LINKAGE *lsl_create_outlet_t)(streaminfo info, int chunk_size, int max_buffered);
typedef int (LINKAGE *lsl_destroy_outlet_t)(outlet out);
typedef int (LINKAGE *lsl_push_sample_f_t)(outlet out, float *data);
typedef int (LINKAGE *lsl_push_sample_ft_t)(outlet out, float *data, double timestamp);
typedef int (LINKAGE *lsl_push_sample_ftp_t)(outlet out, float *data, double timestamp, int pushthrough);
typedef int (LINKAGE *lsl_push_sample_d_t)(outlet out, double *data);
typedef int (LINKAGE *lsl_push_sample_dt_t)(outlet out, double *data, double timestamp);
typedef int (LINKAGE *lsl_push_sample_dtp_t)(outlet out, double *data, double timestamp, int pushthrough);
typedef int (LINKAGE *lsl_push_sample_l_t)(outlet out, long *data);
typedef int (LINKAGE *lsl_push_sample_lt_t)(outlet out, long *data, double timestamp);
typedef int (LINKAGE *lsl_push_sample_ltp_t)(outlet out, long *data, double timestamp, int pushthrough);
typedef int (LINKAGE *lsl_push_sample_i_t)(outlet out, int *data);
typedef int (LINKAGE *lsl_push_sample_it_t)(outlet out, int *data, double timestamp);
typedef int (LINKAGE *lsl_push_sample_itp_t)(outlet out, int *data, double timestamp, int pushthrough);
typedef int (LINKAGE *lsl_push_sample_s_t)(outlet out, short *data);
typedef int (LINKAGE *lsl_push_sample_st_t)(outlet out, short *data, double timestamp);
typedef int (LINKAGE *lsl_push_sample_stp_t)(outlet out, short *data, double timestamp, int pushthrough);
typedef int (LINKAGE *lsl_push_sample_c_t)(outlet out, char *data);
typedef int (LINKAGE *lsl_push_sample_ct_t)(outlet out, char *data, double timestamp);
typedef int (LINKAGE *lsl_push_sample_ctp_t)(outlet out, char *data, double timestamp, int pushthrough);
typedef int (LINKAGE *lsl_push_sample_str_t)(outlet out, char **data);
typedef int (LINKAGE *lsl_push_sample_strt_t)(outlet out, char **data, double timestamp);
typedef int (LINKAGE *lsl_push_sample_strtp_t)(outlet out, char **data, double timestamp, int pushthrough);
typedef int (LINKAGE *lsl_push_sample_buf_t)(outlet out, char **data, unsigned *lengths);
typedef int (LINKAGE *lsl_push_sample_buft_t)(outlet out, char **data, unsigned *lengths, double timestamp);
typedef int (LINKAGE *lsl_push_sample_buftp_t)(outlet out, char **data, unsigned *lengths, double timestamp, int pushthrough);
typedef int (LINKAGE *lsl_push_sample_v_t)(outlet out, void *data);
typedef int (LINKAGE *lsl_push_sample_vt_t)(outlet out, void *data, double timestamp);
typedef int (LINKAGE *lsl_push_sample_vtp_t)(outlet out, void *data, double timestamp, int pushthrough);
typedef int (LINKAGE *lsl_have_consumers_t)(outlet out);
typedef int (LINKAGE *lsl_wait_for_consumers_t)(outlet out, double timeout);
typedef streaminfo (LINKAGE *lsl_get_info_t)(outlet out);

/* inlet object */
typedef inlet (LINKAGE *lsl_create_inlet_t)(streaminfo info, int max_buflen, int max_chunklen, int recover);
typedef void (LINKAGE *lsl_destroy_inlet_t)(inlet in);
typedef streaminfo (LINKAGE *lsl_get_fullinfo_t)(inlet in, double timeout, int *ec);
typedef void (LINKAGE *lsl_open_stream_t)(inlet in, double timeout, int *ec);
typedef void (LINKAGE *lsl_close_stream_t)(inlet in);
typedef double (LINKAGE *lsl_time_correction_t)(inlet in, double timeout, int *ec);
typedef double (LINKAGE *lsl_pull_sample_f_t)(inlet in, float *buffer, int buffer_elements, double timeout, int *ec);
typedef double (LINKAGE *lsl_pull_sample_d_t)(inlet in, double *buffer, int buffer_elements, double timeout, int *ec);
typedef double (LINKAGE *lsl_pull_sample_l_t)(inlet in, long *buffer, int buffer_elements, double timeout, int *ec);
typedef double (LINKAGE *lsl_pull_sample_i_t)(inlet in, int *buffer, int buffer_elements, double timeout, int *ec);
typedef double (LINKAGE *lsl_pull_sample_s_t)(inlet in, short *buffer, int buffer_elements, double timeout, int *ec);
typedef double (LINKAGE *lsl_pull_sample_c_t)(inlet in, char *buffer, int buffer_elements, double timeout, int *ec);
typedef double (LINKAGE *lsl_pull_sample_str_t)(inlet in, char **buffer, int buffer_elements, double timeout, int *ec);
typedef double (LINKAGE *lsl_pull_sample_buf_t)(inlet in, char **buffer, unsigned *buffer_lengths, int buffer_elements, double timeout, int *ec);
typedef double (LINKAGE *lsl_pull_sample_v_t)(inlet in, void *buffer, int buffer_bytes, double timeout, int *ec);
typedef unsigned (LINKAGE *lsl_samples_available_t)(inlet in);

/* xml ptr */
typedef xml_ptr (LINKAGE *lsl_first_child_t)(xml_ptr e);
typedef xml_ptr (LINKAGE *lsl_last_child_t)(xml_ptr e);
typedef xml_ptr (LINKAGE *lsl_next_sibling_t)(xml_ptr e);
typedef xml_ptr (LINKAGE *lsl_previous_sibling_t)(xml_ptr e);
typedef xml_ptr (LINKAGE *lsl_parent_t)(xml_ptr e);
typedef xml_ptr (LINKAGE *lsl_child_t)(xml_ptr e, char *name);
typedef xml_ptr (LINKAGE *lsl_next_sibling_n_t)(xml_ptr e, char *name);
typedef xml_ptr (LINKAGE *lsl_previous_sibling_n_t)(xml_ptr e, char *name);
typedef int (LINKAGE *lsl_empty_t)(xml_ptr e);
typedef int  (LINKAGE *lsl_is_text_t)(xml_ptr e);
typedef char *(LINKAGE *lsl_name_t)(xml_ptr e);
typedef char *(LINKAGE *lsl_value_t)(xml_ptr e);
typedef char *(LINKAGE *lsl_child_value_t)(xml_ptr e);
typedef char *(LINKAGE *lsl_child_value_n_t)(xml_ptr e, char *name);
typedef xml_ptr (LINKAGE *lsl_append_child_value_t)(xml_ptr e, char *name, char *value);
typedef xml_ptr (LINKAGE *lsl_prepend_child_value_t)(xml_ptr e, char *name, char *value);
typedef int (LINKAGE *lsl_set_child_value_t)(xml_ptr e, char *name, char *value);
typedef int (LINKAGE *lsl_set_name_t)(xml_ptr e, char *rhs);
typedef int (LINKAGE *lsl_set_value_t)(xml_ptr e, char *rhs);
typedef xml_ptr (LINKAGE *lsl_append_child_t)(xml_ptr e, char *name);
typedef xml_ptr (LINKAGE *lsl_prepend_child_t)(xml_ptr e, char *name);
typedef xml_ptr (LINKAGE *lsl_append_copy_t)(xml_ptr e, xml_ptr e2);
typedef xml_ptr (LINKAGE *lsl_prepend_copy_t)(xml_ptr e, xml_ptr e2);
typedef void (LINKAGE *lsl_remove_child_n_t)(xml_ptr e, char *name);
typedef void (LINKAGE *lsl_remove_child_t)(xml_ptr e, xml_ptr e2);

/* continuous_resolver */
typedef continuous_resolver (LINKAGE *lsl_create_continuous_resolver_t)(double forget_after);
typedef continuous_resolver (LINKAGE *lsl_create_continuous_resolver_byprop_t)(char *prop, char *value, double forget_after);
typedef continuous_resolver (LINKAGE *lsl_create_continuous_resolver_bypred_t)(char *pred, double forget_after);
typedef int (LINKAGE *lsl_resolver_results_t)(continuous_resolver res, streaminfo *buffer, unsigned buffer_elements);
typedef void (LINKAGE *lsl_destroy_continuous_resolver_t)(continuous_resolver res);
