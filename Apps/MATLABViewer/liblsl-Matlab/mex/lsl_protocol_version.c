#include "lsl_common.h"

/* function [ProtocolVersion] = lsl_protocol_version(LibHandle) */

void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray*prhs[] ) 
{
    /* handle of the desired field */
    mxArray *field;
    /* temp pointer */
    uintptr_t *pTmp;
    /* function handle */
    lsl_protocol_version_t func;
    /* input/output variables */
    int result;
    
    if (nrhs != 1)
        mexErrMsgTxt("1 input argument(s) required."); 
    if (nlhs != 1)
        mexErrMsgTxt("1 output argument(s) required."); 
    
    /* get function handle */
    field = mxGetField(prhs[0],0,"lsl_protocol_version");
    if (!field)
        mexErrMsgTxt("The field does not seem to exist.");
    pTmp = (uintptr_t*)mxGetData(field);
    if (!pTmp)
        mexErrMsgTxt("The field seems to be empty.");
    func = (lsl_protocol_version_t*)*pTmp;
    
    /* invoke & return */
    result = func();
    plhs[0] = mxCreateNumericMatrix(1,1,mxINT32_CLASS,mxREAL); *(int32_T*)mxGetData(plhs[0]) = (int32_T)result; 
}
