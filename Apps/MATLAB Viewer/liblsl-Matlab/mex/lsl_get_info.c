#include "lsl_common.h"

/* function [Streaminfo] = lsl_get_info(LibHandle,Outlet) */

void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray*prhs[] ) 
{
    /* handle of the desired field */
    mxArray *field;
    /* temp pointer */
    uintptr_t *pTmp;
    /* function handle */
    lsl_get_info_t func;
    /* input/output variables */
    uintptr_t out;
    streaminfo result;
    
    if (nrhs != 2)
        mexErrMsgTxt("2 input argument(s) required."); 
    if (nlhs != 1)
        mexErrMsgTxt("1 output argument(s) required."); 
    
    /* get function handle */
    field = mxGetField(prhs[0],0,"lsl_get_info");
    if (!field)
        mexErrMsgTxt("The field does not seem to exist.");
    pTmp = (uintptr_t*)mxGetData(field);
    if (!pTmp)
        mexErrMsgTxt("The field seems to be empty.");
    func = (lsl_get_info_t*)*pTmp;
    
    /* get additional inputs */
    out = *(uintptr_t*)mxGetData(prhs[1]);
    
    /* invoke & return */
    result = func(out);
    plhs[0] = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(plhs[0]) = (uintptr_t)result;
}
