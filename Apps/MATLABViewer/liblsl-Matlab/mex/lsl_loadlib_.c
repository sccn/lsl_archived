#include "lsl_common.h"

/*
 * [LibHandle] = lsl_loadlib_(LibPath);
 * Internal function to load the lab streaming layer API - please use lsl_loadlib.m instead of 
 * this one.
 * 
 * In:
 *   LibPath : full path to the liblsl library to be loaded
 *
 * Out:
 *   LibHandle : handle to the driver library (actually a struct that contains the library handle,
 *               as well as the function addresses for all library functions).
 *
 */

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray*prhs[]) {
    /* path from where we load the library (first arg) */
    char libpath[16384];    
    int libpath_len;
    /* OS handle to the library */
    void *hlib;
    /* library function pointers */
    lsl_protocol_version_t lsl_protocol_version;
    lsl_library_version_t lsl_library_version;
    lsl_local_clock_t lsl_local_clock;
    lsl_resolve_all_t lsl_resolve_all;
    lsl_resolve_byprop_t lsl_resolve_byprop;
    lsl_resolve_bypred_t lsl_resolve_bypred;
	lsl_create_streaminfo_t lsl_create_streaminfo;
	lsl_destroy_streaminfo_t lsl_destroy_streaminfo;
	lsl_get_name_t lsl_get_name;
	lsl_get_type_t lsl_get_type;
	lsl_get_channel_count_t lsl_get_channel_count;
	lsl_get_nominal_srate_t lsl_get_nominal_srate;
	lsl_get_channel_format_t lsl_get_channel_format;
	lsl_get_source_id_t lsl_get_source_id;
	lsl_get_version_t lsl_get_version;
	lsl_get_created_at_t lsl_get_created_at;
	lsl_get_uid_t lsl_get_uid;
	lsl_get_session_id_t lsl_get_session_id;
	lsl_get_hostname_t lsl_get_hostname;
	lsl_get_desc_t lsl_get_desc;
	lsl_get_xml_t lsl_get_xml;
	lsl_create_outlet_t lsl_create_outlet;
	lsl_destroy_outlet_t lsl_destroy_outlet;
	lsl_push_sample_f_t lsl_push_sample_f;
	lsl_push_sample_ft_t lsl_push_sample_ft;
	lsl_push_sample_ftp_t lsl_push_sample_ftp;
	lsl_push_sample_d_t lsl_push_sample_d;
	lsl_push_sample_dt_t lsl_push_sample_dt;
	lsl_push_sample_dtp_t lsl_push_sample_dtp;
	lsl_push_sample_l_t lsl_push_sample_l;
	lsl_push_sample_lt_t lsl_push_sample_lt;
	lsl_push_sample_ltp_t lsl_push_sample_ltp;
	lsl_push_sample_i_t lsl_push_sample_i;
	lsl_push_sample_it_t lsl_push_sample_it;
	lsl_push_sample_itp_t lsl_push_sample_itp;
	lsl_push_sample_s_t lsl_push_sample_s;
	lsl_push_sample_st_t lsl_push_sample_st;
	lsl_push_sample_stp_t lsl_push_sample_stp;
	lsl_push_sample_c_t lsl_push_sample_c;
	lsl_push_sample_ct_t lsl_push_sample_ct;
	lsl_push_sample_ctp_t lsl_push_sample_ctp;
	lsl_push_sample_str_t lsl_push_sample_str;
	lsl_push_sample_strt_t lsl_push_sample_strt;
	lsl_push_sample_strtp_t lsl_push_sample_strtp;
	lsl_push_sample_buf_t lsl_push_sample_buf;
	lsl_push_sample_buft_t lsl_push_sample_buft;
	lsl_push_sample_buftp_t lsl_push_sample_buftp;
	lsl_push_sample_v_t lsl_push_sample_v;
	lsl_push_sample_vt_t lsl_push_sample_vt;
	lsl_push_sample_vtp_t lsl_push_sample_vtp;
	lsl_have_consumers_t lsl_have_consumers;
	lsl_wait_for_consumers_t lsl_wait_for_consumers;
	lsl_get_info_t lsl_get_info;
	lsl_create_inlet_t lsl_create_inlet;
	lsl_destroy_inlet_t lsl_destroy_inlet;
	lsl_get_fullinfo_t lsl_get_fullinfo;
	lsl_open_stream_t lsl_open_stream;
	lsl_close_stream_t lsl_close_stream;
	lsl_time_correction_t lsl_time_correction;
	lsl_pull_sample_f_t lsl_pull_sample_f;
	lsl_pull_sample_d_t lsl_pull_sample_d;
	lsl_pull_sample_l_t lsl_pull_sample_l;
	lsl_pull_sample_i_t lsl_pull_sample_i;
	lsl_pull_sample_s_t lsl_pull_sample_s;
	lsl_pull_sample_c_t lsl_pull_sample_c;
	lsl_pull_sample_str_t lsl_pull_sample_str;
	lsl_pull_sample_buf_t lsl_pull_sample_buf;
	lsl_pull_sample_v_t lsl_pull_sample_v;
    lsl_samples_available_t lsl_samples_available;
	lsl_first_child_t lsl_first_child;
	lsl_last_child_t lsl_last_child;
	lsl_next_sibling_t lsl_next_sibling;
	lsl_previous_sibling_t lsl_previous_sibling;
	lsl_parent_t lsl_parent;
	lsl_child_t lsl_child;
	lsl_next_sibling_n_t lsl_next_sibling_n;
	lsl_previous_sibling_n_t lsl_previous_sibling_n;
	lsl_empty_t lsl_empty;
	lsl_is_text_t lsl_is_text;
	lsl_name_t lsl_name;
	lsl_value_t lsl_value;
	lsl_child_value_t lsl_child_value;
	lsl_child_value_n_t lsl_child_value_n;
	lsl_append_child_value_t lsl_append_child_value;
	lsl_prepend_child_value_t lsl_prepend_child_value;
	lsl_set_child_value_t lsl_set_child_value;
	lsl_set_name_t lsl_set_name;
	lsl_set_value_t lsl_set_value;
	lsl_append_child_t lsl_append_child;
	lsl_prepend_child_t lsl_prepend_child;
	lsl_append_copy_t lsl_append_copy;
	lsl_prepend_copy_t lsl_prepend_copy;
	lsl_remove_child_n_t lsl_remove_child_n;
	lsl_remove_child_t lsl_remove_child;

	/* the same as list of strings... */
	char *fieldnames[] = {"hlib","lsl_protocol_version", "lsl_library_version", "lsl_local_clock", "lsl_resolve_all", "lsl_resolve_byprop", "lsl_resolve_bypred", "lsl_create_streaminfo", "lsl_destroy_streaminfo", 
		"lsl_get_name", "lsl_get_type", "lsl_get_channel_count", "lsl_get_nominal_srate", "lsl_get_channel_format", "lsl_get_source_id", "lsl_get_version", "lsl_get_created_at",
		"lsl_get_uid", "lsl_get_session_id", "lsl_get_hostname", "lsl_get_desc", "lsl_get_xml", "lsl_create_outlet", "lsl_destroy_outlet", "lsl_push_sample_f", "lsl_push_sample_ft", "lsl_push_sample_ftp", 
        "lsl_push_sample_d", "lsl_push_sample_dt" , "lsl_push_sample_dtp", "lsl_push_sample_l", "lsl_push_sample_lt", "lsl_push_sample_ltp",  "lsl_push_sample_i", "lsl_push_sample_it", 
        "lsl_push_sample_itp", "lsl_push_sample_s", "lsl_push_sample_st", "lsl_push_sample_stp", "lsl_push_sample_c", "lsl_push_sample_ct", "lsl_push_sample_ctp", "lsl_push_sample_str", 
		"lsl_push_sample_strt", "lsl_push_sample_strtp", "lsl_push_sample_buf", "lsl_push_sample_buft", "lsl_push_sample_buftp","lsl_push_sample_v", "lsl_push_sample_vt", "lsl_push_sample_vtp", "lsl_have_consumers", "lsl_wait_for_consumers", "lsl_get_info", 
		"lsl_create_inlet", "lsl_destroy_inlet", "lsl_get_fullinfo", "lsl_open_stream", "lsl_close_stream", "lsl_time_correction", "lsl_pull_sample_f", "lsl_pull_sample_d", "lsl_pull_sample_l", 
		"lsl_pull_sample_i", "lsl_pull_sample_s", "lsl_pull_sample_c", "lsl_pull_sample_str", "lsl_pull_sample_buf", "lsl_pull_sample_v", "lsl_samples_available", "lsl_first_child", "lsl_last_child", "lsl_next_sibling", "lsl_previous_sibling", 
		"lsl_parent", "lsl_child", "lsl_next_sibling_n", "lsl_previous_sibling_n", "lsl_empty", "lsl_is_text", "lsl_name", "lsl_value", "lsl_child_value", "lsl_child_value_n", "lsl_append_child_value", 
		"lsl_prepend_child_value", "lsl_set_child_value", "lsl_set_name", "lsl_set_value", "lsl_append_child", "lsl_prepend_child", "lsl_append_copy", "lsl_prepend_copy", "lsl_remove_child_n", 
		"lsl_remove_child"};

	mxArray *tmp;

    /* check input/output arguments */   
    if (nrhs != 1)
        mexErrMsgTxt("One input argument required."); 
    if (nlhs != 1)
        mexErrMsgTxt("One output argument required."); 
    if (!mxIsChar(prhs[0]) || (mxGetM(prhs[0]) != 1) || (mxGetN(prhs[0]) <= 4))
        mexErrMsgTxt("First input argument must be a string."); 
    
    /* extract the library path */
    libpath_len = mxGetNumberOfElements(prhs[0]);
    mxGetNChars_700(prhs[0], libpath, libpath_len+1);
    
    /* load the library */
    hlib = (void*)LOAD_LIBRARY(libpath);
    if (!hlib) {
        mexPrintf("Could not load library '"); mexPrintf(libpath); mexPrintf("'... \n");
        mexPrintf("Error code: %s\n",ERROR_STRING());
        mexErrMsgTxt("Exiting now.\n");
    }
    
    /* obtain function pointers */
    lsl_protocol_version = (lsl_protocol_version_t)LOAD_FUNCTION(hlib,"lsl_protocol_version");
    lsl_library_version = (lsl_library_version_t)LOAD_FUNCTION(hlib,"lsl_library_version");
    lsl_local_clock = (lsl_local_clock_t)LOAD_FUNCTION(hlib,"lsl_local_clock");
    lsl_resolve_all = (lsl_resolve_all_t)LOAD_FUNCTION(hlib,"lsl_resolve_all");
    lsl_resolve_byprop = (lsl_resolve_byprop_t)LOAD_FUNCTION(hlib,"lsl_resolve_byprop");
    lsl_resolve_bypred = (lsl_resolve_bypred_t)LOAD_FUNCTION(hlib,"lsl_resolve_bypred");
	lsl_create_streaminfo = (lsl_create_streaminfo_t)LOAD_FUNCTION(hlib,"lsl_create_streaminfo");
	lsl_destroy_streaminfo = (lsl_destroy_streaminfo_t)LOAD_FUNCTION(hlib,"lsl_destroy_streaminfo");
	lsl_get_name = (lsl_get_name_t)LOAD_FUNCTION(hlib,"lsl_get_name");
	lsl_get_type = (lsl_get_type_t)LOAD_FUNCTION(hlib,"lsl_get_type");
	lsl_get_channel_count = (lsl_get_channel_count_t)LOAD_FUNCTION(hlib,"lsl_get_channel_count");
	lsl_get_nominal_srate = (lsl_get_nominal_srate_t)LOAD_FUNCTION(hlib,"lsl_get_nominal_srate");
	lsl_get_channel_format = (lsl_get_channel_format_t)LOAD_FUNCTION(hlib,"lsl_get_channel_format");
	lsl_get_source_id = (lsl_get_source_id_t)LOAD_FUNCTION(hlib,"lsl_get_source_id");
	lsl_get_version = (lsl_get_version_t)LOAD_FUNCTION(hlib,"lsl_get_version");
	lsl_get_created_at = (lsl_get_created_at_t)LOAD_FUNCTION(hlib,"lsl_get_created_at");
	lsl_get_uid = (lsl_get_uid_t)LOAD_FUNCTION(hlib,"lsl_get_uid");
	lsl_get_session_id = (lsl_get_session_id_t)LOAD_FUNCTION(hlib,"lsl_get_session_id");
	lsl_get_hostname = (lsl_get_hostname_t)LOAD_FUNCTION(hlib,"lsl_get_hostname");
	lsl_get_desc = (lsl_get_desc_t)LOAD_FUNCTION(hlib,"lsl_get_desc");
	lsl_get_xml = (lsl_get_xml_t)LOAD_FUNCTION(hlib,"lsl_get_xml");
	lsl_create_outlet = (lsl_create_outlet_t)LOAD_FUNCTION(hlib,"lsl_create_outlet");
	lsl_destroy_outlet = (lsl_destroy_outlet_t)LOAD_FUNCTION(hlib,"lsl_destroy_outlet");
	lsl_push_sample_f = (lsl_push_sample_f_t)LOAD_FUNCTION(hlib,"lsl_push_sample_f");
	lsl_push_sample_ft = (lsl_push_sample_ft_t)LOAD_FUNCTION(hlib,"lsl_push_sample_ft");
	lsl_push_sample_ftp = (lsl_push_sample_ftp_t)LOAD_FUNCTION(hlib,"lsl_push_sample_ftp");
	lsl_push_sample_d = (lsl_push_sample_d_t)LOAD_FUNCTION(hlib,"lsl_push_sample_d");
	lsl_push_sample_dt = (lsl_push_sample_dt_t)LOAD_FUNCTION(hlib,"lsl_push_sample_dt");
	lsl_push_sample_dtp = (lsl_push_sample_dtp_t)LOAD_FUNCTION(hlib,"lsl_push_sample_dtp");
	lsl_push_sample_l = (lsl_push_sample_l_t)LOAD_FUNCTION(hlib,"lsl_push_sample_l");
	lsl_push_sample_lt = (lsl_push_sample_lt_t)LOAD_FUNCTION(hlib,"lsl_push_sample_lt");
	lsl_push_sample_ltp = (lsl_push_sample_ltp_t)LOAD_FUNCTION(hlib,"lsl_push_sample_ltp");
	lsl_push_sample_i = (lsl_push_sample_i_t)LOAD_FUNCTION(hlib,"lsl_push_sample_i");
	lsl_push_sample_it = (lsl_push_sample_it_t)LOAD_FUNCTION(hlib,"lsl_push_sample_it");
	lsl_push_sample_itp = (lsl_push_sample_itp_t)LOAD_FUNCTION(hlib,"lsl_push_sample_itp");
	lsl_push_sample_s = (lsl_push_sample_s_t)LOAD_FUNCTION(hlib,"lsl_push_sample_s");
	lsl_push_sample_st = (lsl_push_sample_st_t)LOAD_FUNCTION(hlib,"lsl_push_sample_st");
	lsl_push_sample_stp = (lsl_push_sample_stp_t)LOAD_FUNCTION(hlib,"lsl_push_sample_stp");
	lsl_push_sample_c = (lsl_push_sample_c_t)LOAD_FUNCTION(hlib,"lsl_push_sample_c");
	lsl_push_sample_ct = (lsl_push_sample_ct_t)LOAD_FUNCTION(hlib,"lsl_push_sample_ct");
	lsl_push_sample_ctp = (lsl_push_sample_ctp_t)LOAD_FUNCTION(hlib,"lsl_push_sample_ctp");
	lsl_push_sample_str = (lsl_push_sample_str_t)LOAD_FUNCTION(hlib,"lsl_push_sample_str");
	lsl_push_sample_strt = (lsl_push_sample_strt_t)LOAD_FUNCTION(hlib,"lsl_push_sample_strt");
	lsl_push_sample_strtp = (lsl_push_sample_strtp_t)LOAD_FUNCTION(hlib,"lsl_push_sample_strtp");
	lsl_push_sample_buf = (lsl_push_sample_buf_t)LOAD_FUNCTION(hlib,"lsl_push_sample_buf");
	lsl_push_sample_buft = (lsl_push_sample_buft_t)LOAD_FUNCTION(hlib,"lsl_push_sample_buft");
	lsl_push_sample_buftp = (lsl_push_sample_buftp_t)LOAD_FUNCTION(hlib,"lsl_push_sample_buftp");
	lsl_push_sample_v = (lsl_push_sample_v_t)LOAD_FUNCTION(hlib,"lsl_push_sample_v");
	lsl_push_sample_vt = (lsl_push_sample_vt_t)LOAD_FUNCTION(hlib,"lsl_push_sample_vt");
	lsl_push_sample_vtp = (lsl_push_sample_vtp_t)LOAD_FUNCTION(hlib,"lsl_push_sample_vtp");
	lsl_have_consumers = (lsl_have_consumers_t)LOAD_FUNCTION(hlib,"lsl_have_consumers");
	lsl_wait_for_consumers = (lsl_wait_for_consumers_t)LOAD_FUNCTION(hlib,"lsl_wait_for_consumers");
	lsl_get_info = (lsl_get_info_t)LOAD_FUNCTION(hlib,"lsl_get_info");
	lsl_create_inlet = (lsl_create_inlet_t)LOAD_FUNCTION(hlib,"lsl_create_inlet");
	lsl_destroy_inlet = (lsl_destroy_inlet_t)LOAD_FUNCTION(hlib,"lsl_destroy_inlet");
	lsl_get_fullinfo = (lsl_get_fullinfo_t)LOAD_FUNCTION(hlib,"lsl_get_fullinfo");
	lsl_open_stream = (lsl_open_stream_t)LOAD_FUNCTION(hlib,"lsl_open_stream");
	lsl_close_stream = (lsl_close_stream_t)LOAD_FUNCTION(hlib,"lsl_close_stream");
	lsl_time_correction = (lsl_time_correction_t)LOAD_FUNCTION(hlib,"lsl_time_correction");
	lsl_pull_sample_f = (lsl_pull_sample_f_t)LOAD_FUNCTION(hlib,"lsl_pull_sample_f");
	lsl_pull_sample_d = (lsl_pull_sample_d_t)LOAD_FUNCTION(hlib,"lsl_pull_sample_d");
	lsl_pull_sample_l = (lsl_pull_sample_l_t)LOAD_FUNCTION(hlib,"lsl_pull_sample_l");
	lsl_pull_sample_i = (lsl_pull_sample_i_t)LOAD_FUNCTION(hlib,"lsl_pull_sample_i");
	lsl_pull_sample_s = (lsl_pull_sample_s_t)LOAD_FUNCTION(hlib,"lsl_pull_sample_s");
	lsl_pull_sample_c = (lsl_pull_sample_c_t)LOAD_FUNCTION(hlib,"lsl_pull_sample_c");
	lsl_pull_sample_str = (lsl_pull_sample_str_t)LOAD_FUNCTION(hlib,"lsl_pull_sample_str");
	lsl_pull_sample_buf = (lsl_pull_sample_buf_t)LOAD_FUNCTION(hlib,"lsl_pull_sample_buf");
	lsl_pull_sample_v = (lsl_pull_sample_v_t)LOAD_FUNCTION(hlib,"lsl_pull_sample_v");
	lsl_samples_available = (lsl_samples_available_t)LOAD_FUNCTION(hlib,"lsl_samples_available");
	lsl_first_child = (lsl_first_child_t)LOAD_FUNCTION(hlib,"lsl_first_child");
	lsl_last_child = (lsl_last_child_t)LOAD_FUNCTION(hlib,"lsl_last_child");
	lsl_next_sibling = (lsl_next_sibling_t)LOAD_FUNCTION(hlib,"lsl_next_sibling");
	lsl_previous_sibling = (lsl_previous_sibling_t)LOAD_FUNCTION(hlib,"lsl_previous_sibling");
	lsl_parent = (lsl_parent_t)LOAD_FUNCTION(hlib,"lsl_parent");
	lsl_child = (lsl_child_t)LOAD_FUNCTION(hlib,"lsl_child");
	lsl_next_sibling_n = (lsl_next_sibling_n_t)LOAD_FUNCTION(hlib,"lsl_next_sibling_n");
	lsl_previous_sibling_n = (lsl_previous_sibling_n_t)LOAD_FUNCTION(hlib,"lsl_previous_sibling_n");
	lsl_empty = (lsl_empty_t)LOAD_FUNCTION(hlib,"lsl_empty");
	lsl_is_text = (lsl_is_text_t)LOAD_FUNCTION(hlib,"lsl_is_text");
	lsl_name = (lsl_name_t)LOAD_FUNCTION(hlib,"lsl_name");
	lsl_value = (lsl_value_t)LOAD_FUNCTION(hlib,"lsl_value");
	lsl_child_value = (lsl_child_value_t)LOAD_FUNCTION(hlib,"lsl_child_value");
	lsl_child_value_n = (lsl_child_value_n_t)LOAD_FUNCTION(hlib,"lsl_child_value_n");
	lsl_append_child_value = (lsl_append_child_value_t)LOAD_FUNCTION(hlib,"lsl_append_child_value");
	lsl_prepend_child_value = (lsl_prepend_child_value_t)LOAD_FUNCTION(hlib,"lsl_prepend_child_value");
	lsl_set_child_value = (lsl_set_child_value_t)LOAD_FUNCTION(hlib,"lsl_set_child_value");
	lsl_set_name = (lsl_set_name_t)LOAD_FUNCTION(hlib,"lsl_set_name");
	lsl_set_value = (lsl_set_value_t)LOAD_FUNCTION(hlib,"lsl_set_value");
	lsl_append_child = (lsl_append_child_t)LOAD_FUNCTION(hlib,"lsl_append_child");
	lsl_prepend_child = (lsl_prepend_child_t)LOAD_FUNCTION(hlib,"lsl_prepend_child");
	lsl_append_copy = (lsl_append_copy_t)LOAD_FUNCTION(hlib,"lsl_append_copy");
	lsl_prepend_copy = (lsl_prepend_copy_t)LOAD_FUNCTION(hlib,"lsl_prepend_copy");
	lsl_remove_child_n = (lsl_remove_child_n_t)LOAD_FUNCTION(hlib,"lsl_remove_child_n");
	lsl_remove_child = (lsl_remove_child_t)LOAD_FUNCTION(hlib,"lsl_remove_child");
    
    /* make the result struct */
	plhs[0] = mxCreateStructMatrix(1,1,sizeof(fieldnames)/sizeof(fieldnames[0]),fieldnames);
    /* and assign all pointers to it */
    tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)hlib; mxSetField(plhs[0],0,"hlib",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_protocol_version; mxSetField(plhs[0],0,"lsl_protocol_version",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_library_version; mxSetField(plhs[0],0,"lsl_library_version",tmp);
    tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_local_clock; mxSetField(plhs[0],0,"lsl_local_clock",tmp);
    tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_resolve_all; mxSetField(plhs[0],0,"lsl_resolve_all",tmp);
    tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_resolve_byprop; mxSetField(plhs[0],0,"lsl_resolve_byprop",tmp);
    tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_resolve_bypred; mxSetField(plhs[0],0,"lsl_resolve_bypred",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_create_streaminfo; mxSetField(plhs[0],0,"lsl_create_streaminfo",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_destroy_streaminfo; mxSetField(plhs[0],0,"lsl_destroy_streaminfo",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_get_name; mxSetField(plhs[0],0,"lsl_get_name",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_get_type; mxSetField(plhs[0],0,"lsl_get_type",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_get_channel_count; mxSetField(plhs[0],0,"lsl_get_channel_count",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_get_nominal_srate; mxSetField(plhs[0],0,"lsl_get_nominal_srate",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_get_channel_format; mxSetField(plhs[0],0,"lsl_get_channel_format",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_get_source_id; mxSetField(plhs[0],0,"lsl_get_source_id",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_get_version; mxSetField(plhs[0],0,"lsl_get_version",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_get_created_at; mxSetField(plhs[0],0,"lsl_get_created_at",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_get_uid; mxSetField(plhs[0],0,"lsl_get_uid",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_get_session_id; mxSetField(plhs[0],0,"lsl_get_session_id",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_get_hostname; mxSetField(plhs[0],0,"lsl_get_hostname",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_get_desc; mxSetField(plhs[0],0,"lsl_get_desc",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_get_xml; mxSetField(plhs[0],0,"lsl_get_xml",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_create_outlet; mxSetField(plhs[0],0,"lsl_create_outlet",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_destroy_outlet; mxSetField(plhs[0],0,"lsl_destroy_outlet",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_f; mxSetField(plhs[0],0,"lsl_push_sample_f",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_ft; mxSetField(plhs[0],0,"lsl_push_sample_ft",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_ftp; mxSetField(plhs[0],0,"lsl_push_sample_ftp",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_d; mxSetField(plhs[0],0,"lsl_push_sample_d",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_dt; mxSetField(plhs[0],0,"lsl_push_sample_dt",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_dtp; mxSetField(plhs[0],0,"lsl_push_sample_dtp",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_l; mxSetField(plhs[0],0,"lsl_push_sample_l",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_lt; mxSetField(plhs[0],0,"lsl_push_sample_lt",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_ltp; mxSetField(plhs[0],0,"lsl_push_sample_ltp",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_i; mxSetField(plhs[0],0,"lsl_push_sample_i",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_it; mxSetField(plhs[0],0,"lsl_push_sample_it",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_itp; mxSetField(plhs[0],0,"lsl_push_sample_itp",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_s; mxSetField(plhs[0],0,"lsl_push_sample_s",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_st; mxSetField(plhs[0],0,"lsl_push_sample_st",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_stp; mxSetField(plhs[0],0,"lsl_push_sample_stp",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_c; mxSetField(plhs[0],0,"lsl_push_sample_c",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_ct; mxSetField(plhs[0],0,"lsl_push_sample_ct",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_ctp; mxSetField(plhs[0],0,"lsl_push_sample_ctp",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_str; mxSetField(plhs[0],0,"lsl_push_sample_str",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_strt; mxSetField(plhs[0],0,"lsl_push_sample_strt",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_strtp; mxSetField(plhs[0],0,"lsl_push_sample_strtp",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_buf; mxSetField(plhs[0],0,"lsl_push_sample_buf",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_buft; mxSetField(plhs[0],0,"lsl_push_sample_buft",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_buftp; mxSetField(plhs[0],0,"lsl_push_sample_buftp",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_v; mxSetField(plhs[0],0,"lsl_push_sample_v",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_vt; mxSetField(plhs[0],0,"lsl_push_sample_vt",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_push_sample_vtp; mxSetField(plhs[0],0,"lsl_push_sample_vtp",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_have_consumers; mxSetField(plhs[0],0,"lsl_have_consumers",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_wait_for_consumers; mxSetField(plhs[0],0,"lsl_wait_for_consumers",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_get_info; mxSetField(plhs[0],0,"lsl_get_info",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_create_inlet; mxSetField(plhs[0],0,"lsl_create_inlet",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_destroy_inlet; mxSetField(plhs[0],0,"lsl_destroy_inlet",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_get_fullinfo; mxSetField(plhs[0],0,"lsl_get_fullinfo",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_open_stream; mxSetField(plhs[0],0,"lsl_open_stream",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_close_stream; mxSetField(plhs[0],0,"lsl_close_stream",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_time_correction; mxSetField(plhs[0],0,"lsl_time_correction",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_pull_sample_f; mxSetField(plhs[0],0,"lsl_pull_sample_f",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_pull_sample_d; mxSetField(plhs[0],0,"lsl_pull_sample_d",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_pull_sample_i; mxSetField(plhs[0],0,"lsl_pull_sample_i",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_pull_sample_s; mxSetField(plhs[0],0,"lsl_pull_sample_s",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_pull_sample_c; mxSetField(plhs[0],0,"lsl_pull_sample_c",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_pull_sample_str; mxSetField(plhs[0],0,"lsl_pull_sample_str",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_pull_sample_buf; mxSetField(plhs[0],0,"lsl_pull_sample_buf",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_pull_sample_v; mxSetField(plhs[0],0,"lsl_pull_sample_v",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_samples_available; mxSetField(plhs[0],0,"lsl_samples_available",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_first_child; mxSetField(plhs[0],0,"lsl_first_child",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_last_child; mxSetField(plhs[0],0,"lsl_last_child",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_next_sibling; mxSetField(plhs[0],0,"lsl_next_sibling",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_previous_sibling; mxSetField(plhs[0],0,"lsl_previous_sibling",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_parent; mxSetField(plhs[0],0,"lsl_parent",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_child; mxSetField(plhs[0],0,"lsl_child",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_next_sibling_n; mxSetField(plhs[0],0,"lsl_next_sibling_n",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_previous_sibling_n; mxSetField(plhs[0],0,"lsl_previous_sibling_n",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_empty; mxSetField(plhs[0],0,"lsl_empty",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_is_text; mxSetField(plhs[0],0,"lsl_is_text",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_name; mxSetField(plhs[0],0,"lsl_name",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_value; mxSetField(plhs[0],0,"lsl_value",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_child_value; mxSetField(plhs[0],0,"lsl_child_value",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_child_value_n; mxSetField(plhs[0],0,"lsl_child_value_n",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_append_child_value; mxSetField(plhs[0],0,"lsl_append_child_value",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_prepend_child_value; mxSetField(plhs[0],0,"lsl_prepend_child_value",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_set_child_value; mxSetField(plhs[0],0,"lsl_set_child_value",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_set_name; mxSetField(plhs[0],0,"lsl_set_name",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_set_value; mxSetField(plhs[0],0,"lsl_set_value",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_append_child; mxSetField(plhs[0],0,"lsl_append_child",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_prepend_child; mxSetField(plhs[0],0,"lsl_prepend_child",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_append_copy; mxSetField(plhs[0],0,"lsl_append_copy",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_prepend_copy; mxSetField(plhs[0],0,"lsl_prepend_copy",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_remove_child_n; mxSetField(plhs[0],0,"lsl_remove_child_n",tmp);
	tmp = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(tmp) = (uintptr_t)lsl_remove_child; mxSetField(plhs[0],0,"lsl_remove_child",tmp);
}
