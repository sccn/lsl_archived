#include "lsl_common.h"

/* function [Outlet] = lsl_create_outlet(LibHandle,StreamInfo,ChunkSize,MaxBuffered) */

void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray*prhs[] ) 
{
    /* handle of the desired field */
    mxArray *field;
    /* temp pointer */
    uintptr_t *pTmp;
    /* mex inputs */
    streaminfo info;
    int chunk_size;
    int max_buffered;
    /* function handle */
    lsl_create_outlet_t func;
    /* input/output variables */    
    outlet result;
    
    if (nrhs != 4)
        mexErrMsgTxt("4 input argument(s) required."); 
    if (nlhs != 1)
        mexErrMsgTxt("1 output argument(s) required.");
    
    /* get function handle */
    field = mxGetField(prhs[0],0,"lsl_create_outlet");
    if (!field)
        mexErrMsgTxt("The field does not seem to exist.");
    pTmp = (uintptr_t*)mxGetData(field);
    if (!pTmp)
        mexErrMsgTxt("The field seems to be empty.");
    func = (lsl_create_outlet_t*)*pTmp;
    
    /* get additional inputs */
    if (mxGetClassID(prhs[1]) != PTR_CLASS)
        mexErrMsgTxt("The streaminfo must be a pointer.");
    info = (uintptr_t)(*(uintptr_t*)mxGetData(prhs[1]));

    if (mxGetClassID(prhs[2]) != mxDOUBLE_CLASS)
        mexErrMsgTxt("The chunk size must be passed as a double.");
    chunk_size = (int)(*(double*)mxGetData(prhs[2]));
    
    if (mxGetClassID(prhs[3]) != mxDOUBLE_CLASS)
        mexErrMsgTxt("The max buffer size must be passed as a double.");
    max_buffered = (int)(*(double*)mxGetData(prhs[3]));

    /* invoke & return */
    result = func(info,chunk_size,max_buffered);
    plhs[0] = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *((uintptr_t*)mxGetData(plhs[0])) = (uintptr_t)result;
}
