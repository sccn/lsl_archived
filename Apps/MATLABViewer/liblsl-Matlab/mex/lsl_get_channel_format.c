#include "lsl_common.h"

/* function [ChannelFormat] = lsl_get_channel_format(LibHandle,StreamInfo) */

void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray*prhs[] ) 
{
    /* handle of the desired field */
    mxArray *field;
    /* temp pointer */
    uintptr_t *pTmp;
    /* function handle */
    lsl_get_channel_format_t func;
    /* input/output variables */
    uintptr_t info;
    int channel_format;
    
    if (nrhs != 2)
        mexErrMsgTxt("2 input argument(s) required."); 
    if (nlhs != 1)
        mexErrMsgTxt("1 output argument(s) required."); 
    
    /* get function handle */
    field = mxGetField(prhs[0],0,"lsl_get_channel_format");
    if (!field)
        mexErrMsgTxt("The field does not seem to exist.");
    pTmp = (uintptr_t*)mxGetData(field);
    if (!pTmp)
        mexErrMsgTxt("The field seems to be empty.");
    func = (lsl_get_channel_format_t*)*pTmp;
    
    /* get additional inputs */
    info = *(uintptr_t*)mxGetData(prhs[1]);
    
    /* invoke & return */
    channel_format = func(info);
    plhs[0] = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,mxREAL); *(double*)mxGetData(plhs[0]) = (double)channel_format;
}
