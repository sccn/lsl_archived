#include "lsl_common.h"

/* function [SampleData,Timestamp] = lsl_pull_sample_buf(LibHandle,Inlet,NumChannels,Timeout) */

void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray*prhs[] ) 
{
    /* handle of the desired field */
    mxArray *field;
    /* temp pointer */
    uintptr_t *pTmp;
    /* function handle */
    lsl_pull_sample_buf_t func;
    
    /* input/output variables */
    uintptr_t in;
    int numchannels,k;
    double timeout;
    int errcode;
    char *buffer[8192];
    unsigned lengths[8192];
    double timestamp;
    mwSize length;
    mxArray *tmpstr;
    
    if (nrhs != 4)
        mexErrMsgTxt("4 input argument(s) required."); 
    if (nlhs < 1)
        mexErrMsgTxt("At least 1 output argument(s) required."); 
    if (nlhs > 2)
        mexErrMsgTxt("At most 2 output argument(s) required."); 
    
    /* get function handle */
    field = mxGetField(prhs[0],0,"lsl_pull_sample_buf");
    if (!field)
        mexErrMsgTxt("The field does not seem to exist.");
    pTmp = (uintptr_t*)mxGetData(field);
    if (!pTmp)
        mexErrMsgTxt("The field seems to be empty.");
    func = (lsl_pull_sample_buf_t*)*pTmp;
    
    /* get additional inputs */
    in = *(uintptr_t*)mxGetData(prhs[1]);
    if (mxGetClassID(prhs[2]) != mxDOUBLE_CLASS)
        mexErrMsgTxt("The numchannels argument must be passed as a double.");
    numchannels = (int)*(double*)mxGetData(prhs[2]);
    if (mxGetClassID(prhs[3]) != mxDOUBLE_CLASS)
        mexErrMsgTxt("The timeout argument must be passed as a double.");
    timeout = *(double*)mxGetData(prhs[3]);
    
    timestamp = func(in,buffer,lengths,numchannels,timeout,&errcode);
    if (errcode) {
        if (errcode == lsl_timeout_error)
            mexErrMsgIdAndTxt("lsl:timeout_error","The operation timed out.");
        if (errcode == lsl_lost_error)
            mexErrMsgIdAndTxt("lsl:lost_error","The stream has been lost.");
        if (errcode == lsl_internal_error)
            mexErrMsgIdAndTxt("lsl:internal_error","An internal error occurred.");
        mexErrMsgIdAndTxt("lsl:unknown_error","An unknown error occurred.");
    }
    if (timestamp) {
        /* got a sample */
        plhs[0] = mxCreateCellMatrix(1,numchannels);
        /* move data over into the results */
        for (k=0;k<numchannels;k++) {
            tmpstr = mxCreateNumericMatrix(1,lengths[k],mxUINT8_CLASS,mxREAL); 
            memcpy(mxGetData(tmpstr),buffer[k],lengths[k]);
            mxSetCell(plhs[0],k,tmpstr);
            free(buffer[k]);
        }
        if (nlhs == 2) {
            /* also assign the time stamp */
            plhs[1] = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,mxREAL);
            *(double*)mxGetData(plhs[1]) = timestamp;
        }
    } else {
        /* got no sample: return empty arrays */
        plhs[0] = mxCreateCellMatrix(0,0);
        if (nlhs == 2)
            plhs[1] = mxCreateNumericMatrix(0,0,mxDOUBLE_CLASS,mxREAL);
    }
}
