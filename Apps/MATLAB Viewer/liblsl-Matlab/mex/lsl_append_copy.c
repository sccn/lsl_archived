#include "lsl_common.h"

/* function [DescPtr] = lsl_append_copy(LibHandle,DescPtr,Other) */

void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray*prhs[] ) 
{
    /* handle of the desired field */
    mxArray *field;
    /* temp pointer */
    uintptr_t *pTmp;
    /* function handle */
    lsl_append_copy_t func;
    /* input/output variables */
    xml_ptr in, other;
    xml_ptr out;
    
    if (nrhs != 3)
        mexErrMsgTxt("3 input argument(s) required."); 
    if (nlhs != 1)
        mexErrMsgTxt("1 output argument(s) required."); 
    
    /* get function handle */
    field = mxGetField(prhs[0],0,"lsl_append_copy");
    if (!field)
        mexErrMsgTxt("The field does not seem to exist.");
    pTmp = (uintptr_t*)mxGetData(field);
    if (!pTmp)
        mexErrMsgTxt("The field seems to be empty.");
    func = (lsl_append_copy_t*)*pTmp;
    
    /* get additional inputs */
    in = (xml_ptr)*(uintptr_t*)mxGetData(prhs[1]);
    other = (xml_ptr)*(uintptr_t*)mxGetData(prhs[2]);
    
    /* invoke & return */
    out = func(in,other);
    plhs[0] = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(plhs[0]) = (uintptr_t)out;
}
