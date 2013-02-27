#include "lsl_common.h"

/* function [DescPtr] = lsl_prepend_child(LibHandle,DescPtr,Name) */

void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray*prhs[] ) 
{
    /* handle of the desired field */
    mxArray *field;
    /* temp pointer */
    uintptr_t *pTmp;
    /* function handle */
    lsl_prepend_child_t func;
    /* input/output variables */
    char str[65536];
    int str_len;
    xml_ptr in;
    xml_ptr out;
    
    if (nrhs != 3)
        mexErrMsgTxt("3 input argument(s) required."); 
    if (nlhs != 1)
        mexErrMsgTxt("1 output argument(s) required."); 
    
    /* get function handle */
    field = mxGetField(prhs[0],0,"lsl_prepend_child");
    if (!field)
        mexErrMsgTxt("The field does not seem to exist.");
    pTmp = (uintptr_t*)mxGetData(field);
    if (!pTmp)
        mexErrMsgTxt("The field seems to be empty.");
    func = (lsl_prepend_child_t*)*pTmp;
    
    /* get additional inputs */
    in = (xml_ptr)*(uintptr_t*)mxGetData(prhs[1]);
    str_len = mxGetNumberOfElements(prhs[2]);
    if (str_len+1 > sizeof(str)/sizeof(str[0]))
        mexErrMsgTxt("The given string is too long.");
    mxGetNChars_700(prhs[2], str, str_len+1);
    
    /* invoke & return */
    out = func(in,str);
    plhs[0] = mxCreateNumericMatrix(1,1,PTR_CLASS,mxREAL); *(uintptr_t*)mxGetData(plhs[0]) = (uintptr_t)out;
}
