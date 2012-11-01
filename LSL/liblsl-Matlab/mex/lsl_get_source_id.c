#include "lsl_common.h"

/* function [SourceID] = lsl_get_source_id(LibHandle,StreamInfo) */

void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray*prhs[] ) 
{
    /* handle of the desired field */
    mxArray *field;
    /* temp pointer */
    uintptr_t *pTmp;
    /* function handle */
    lsl_get_source_id_t func;
    /* input/output variables */
    uintptr_t info;
    char *source_id;
    
    if (nrhs != 2)
        mexErrMsgTxt("2 input argument(s) required."); 
    if (nlhs != 1)
        mexErrMsgTxt("1 output argument(s) required."); 
    
    /* get function handle */
    field = mxGetField(prhs[0],0,"lsl_get_source_id");
    if (!field)
        mexErrMsgTxt("The field does not seem to exist.");
    pTmp = (uintptr_t*)mxGetData(field);
    if (!pTmp)
        mexErrMsgTxt("The field seems to be empty.");
    func = (lsl_get_source_id_t*)*pTmp;
    
    /* get additional inputs */
    info = *(uintptr_t*)mxGetData(prhs[1]);
    
    /* invoke & return */
    source_id = func(info);
    plhs[0] = mxCreateString(source_id);
}
