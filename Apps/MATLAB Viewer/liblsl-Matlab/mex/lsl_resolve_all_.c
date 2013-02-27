#include "lsl_common.h"

/* function [StreaminfoArray] = lsl_resolve_all(LibHandle,Timeout) */

void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray*prhs[] ) 
{
    /* handle of the desired field */
    mxArray *field;
    /* temp pointer */
    uintptr_t *pTmp;
    /* function handle */
    lsl_resolve_all_t func;
    /* input/output variables */
    streaminfo buffer[2048];
    int result,k;
    double timeout;
    
    if (nrhs != 2)
        mexErrMsgTxt("2 input argument(s) required."); 
    if (nlhs != 1)
        mexErrMsgTxt("1 output argument(s) required.");
    
    /* get function handle */
    field = mxGetField(prhs[0],0,"lsl_resolve_all");
    if (!field)
        mexErrMsgTxt("The field does not seem to exist.");
    pTmp = (uintptr_t*)mxGetData(field);
    if (!pTmp)
        mexErrMsgTxt("The field seems to be empty.");
    func = (lsl_resolve_all_t*)*pTmp;
    if (mxGetClassID(prhs[1]) != mxDOUBLE_CLASS)
        mexErrMsgTxt("The timeout argument must be passed as a double.");
    timeout = *(double*)mxGetData(prhs[1]);
        
    /* invoke & return */
    result = func(buffer,sizeof(buffer)/sizeof(buffer[0]),timeout);
    plhs[0] = mxCreateNumericMatrix(1,result,PTR_CLASS,mxREAL);
    for (k=0;k<result;k++)
        *((uintptr_t*)mxGetData(plhs[0])+k) = (uintptr_t)buffer[k];
}