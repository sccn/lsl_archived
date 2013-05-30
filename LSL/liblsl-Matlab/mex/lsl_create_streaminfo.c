#include "lsl_common.h"

/* function [Streaminfo] = lsl_create_streaminfo(LibHandle,Name,Type,ChannelCount,SamplingRate,ChannelFormat,SourceID) */

void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray*prhs[] ) 
{
    /* handle of the desired field */
    mxArray *field;
    /* temp pointer */
    uintptr_t *pTmp;
    /* mex inputs */
    char name[2048], type[2048], source_id[16384];
    int name_len, type_len, source_id_len;
    int channel_count;
    double nominal_srate;
    int channel_format;
    /* function handle */
    lsl_create_streaminfo_t func;
    /* input/output variables */    
    streaminfo result;
    if (nrhs != 7)
        mexErrMsgTxt("7 input argument(s) required."); 
    if (nlhs != 1)
        mexErrMsgTxt("1 output argument(s) required.");
    
    /* get function handle */
    field = mxGetField(prhs[0],0,"lsl_create_streaminfo");
    if (!field)
        mexErrMsgTxt("The field does not seem to exist.");
    pTmp = (uintptr_t*)mxGetData(field);
    if (!pTmp)
        mexErrMsgTxt("The field seems to be empty.");
    func = (lsl_create_streaminfo_t*)*pTmp;
    
    /* get additional inputs */
    name_len = mxGetNumberOfElements(prhs[1]);
    if (name_len+1 > sizeof(name)/sizeof(name[0]))
        mexErrMsgTxt("The given stream name is too long.");    
    mxGetNChars_700(prhs[1], name, name_len+1);
    
    type_len = mxGetNumberOfElements(prhs[2]);
    if (type_len+1 > sizeof(type)/sizeof(type[0]))
        mexErrMsgTxt("The given stream type is too long.");    
    mxGetNChars_700(prhs[2], type, type_len+1);

    if (mxGetClassID(prhs[3]) != mxDOUBLE_CLASS)
        mexErrMsgTxt("The channel count must be passed as a double.");
    channel_count = (int)(*(double*)mxGetData(prhs[3]));
    
    if (mxGetClassID(prhs[4]) != mxDOUBLE_CLASS)
        mexErrMsgTxt("The sampling rate must be passed as a double.");
    nominal_srate = (*(double*)mxGetData(prhs[4]));
    
    if (mxGetClassID(prhs[5]) != mxDOUBLE_CLASS)
        mexErrMsgTxt("The channel format must be passed as a double.");
    channel_format = (*(double*)mxGetData(prhs[5]));
    if (!(channel_format>=1 && channel_format <=6))
        mexErrMsgTxt("The given channel format is unsupported.");
            
    source_id_len = mxGetNumberOfElements(prhs[6]);
    if (source_id_len+1 > sizeof(source_id)/sizeof(source_id[0]))
        mexErrMsgTxt("The given stream source id is too long.");    
    mxGetNChars_700(prhs[6], source_id, source_id_len+1);
    
    /* invoke & return */
    result = func(name,type,channel_count,nominal_srate,channel_format,source_id);
    plhs[0] = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *((uintptr_t*)mxGetData(plhs[0])) = (uintptr_t)result;
}
