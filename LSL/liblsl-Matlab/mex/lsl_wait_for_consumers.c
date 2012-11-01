#include "lsl_common.h"

/* function Success = lsl_wait_for_consumers(LibHandle,Outlet,Timeout) */

void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray*prhs[] ) 
{
    /* handle of the desired field */
    mxArray *field;
    /* temp pointer */
    uintptr_t *pTmp;
    /* function handle */
    lsl_wait_for_consumers_t func;
    /* input/output variables */
    uintptr_t out;
    double timeout;
    int result;
    
    if (nrhs != 2)
        mexErrMsgTxt("2 input argument(s) required."); 
    if (nlhs != 0)
        mexErrMsgTxt("0 output argument(s) required."); 
    
    /* get function handle */
    field = mxGetField(prhs[0],0,"lsl_wait_for_consumers");
    if (!field)
        mexErrMsgTxt("The field does not seem to exist.");
    pTmp = (uintptr_t*)mxGetData(field);
    if (!pTmp)
        mexErrMsgTxt("The field seems to be empty.");
    func = (lsl_wait_for_consumers_t*)*pTmp;
    
    /* get additional inputs */
    out = *(uintptr_t*)mxGetData(prhs[1]);
    if (mxGetClassID(prhs[2]) != mxDOUBLE_CLASS)
        mexErrMsgTxt("The timeout argument must be passed as a double.");
    timeout = *(double*)mxGetData(prhs[2]);
    
    /* invoke */
    result = func(out,timeout);
    plhs[0] = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,mxREAL); *(double*)mxGetData(plhs[0]) = (double)result;
}
