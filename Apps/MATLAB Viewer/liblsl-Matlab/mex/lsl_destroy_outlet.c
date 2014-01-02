#include "lsl_common.h"

/* function lsl_destroy_outlet(LibHandle,Outlet) */

void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray*prhs[] ) 
{
    /* handle of the desired field */
    mxArray *field;
    /* temp pointer */
    uintptr_t *pTmp;
    /* function handle */
    lsl_destroy_outlet_t func;
    /* input/output variables */
    uintptr_t out;
    
    if (nrhs != 2)
        mexErrMsgTxt("2 input argument(s) required."); 
    if (nlhs != 0)
        mexErrMsgTxt("0 output argument(s) required."); 
    
    /* get function handle */
    field = mxGetField(prhs[0],0,"lsl_destroy_outlet");
    if (!field)
        mexErrMsgTxt("The field does not seem to exist.");
    pTmp = (uintptr_t*)mxGetData(field);
    if (!pTmp)
        mexErrMsgTxt("The field seems to be empty.");
    func = (lsl_destroy_outlet_t*)*pTmp;
    
    /* get additional inputs */
    out = *(uintptr_t*)mxGetData(prhs[1]);
    
    /* invoke & return */
    func(out);
}
