#include "lsl_common.h"

/* function [Success] = lsl_set_child_value(LibHandle,DescPtr,Name,Value) */

void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray*prhs[] ) 
{
    /* handle of the desired field */
    mxArray *field;
    /* temp pointer */
    uintptr_t *pTmp;
    /* function handle */
    lsl_set_child_value_t func;
    /* input/output variables */
    char name[65536], value[65536];
    int name_len, value_len;
    xml_ptr in;
    int out;
    
    if (nrhs != 4)
        mexErrMsgTxt("4 input argument(s) required."); 
    if (nlhs != 1)
        mexErrMsgTxt("1 output argument(s) required."); 
    
    /* get function handle */
    field = mxGetField(prhs[0],0,"lsl_set_child_value");
    if (!field)
        mexErrMsgTxt("The field does not seem to exist.");
    pTmp = (uintptr_t*)mxGetData(field);
    if (!pTmp)
        mexErrMsgTxt("The field seems to be set_child_value.");
    func = (lsl_set_child_value_t*)*pTmp;
    
    /* get additional inputs */
    in = (xml_ptr)*(uintptr_t*)mxGetData(prhs[1]);
    name_len = mxGetNumberOfElements(prhs[2]);
    if (name_len+1 > sizeof(name)/sizeof(name[0]))
        mexErrMsgTxt("The given string is too long.");
    mxGetNChars_700(prhs[2], name, name_len+1);
    value_len = mxGetNumberOfElements(prhs[3]);
    if (value_len+1 > sizeof(value)/sizeof(value[0]))
        mexErrMsgTxt("The given string is too long.");
    mxGetNChars_700(prhs[3], value, value_len+1);
    
    /* invoke & return */
    out = func(in,name,value);
    plhs[0] = mxCreateNumericMatrix(1,1,mxDOUBLE_CLASS,mxREAL); *(double*)mxGetData(plhs[0]) = (double)out;
}
