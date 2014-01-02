#include "lsl_common.h"

/* function [StreaminfoArray] = lsl_resolve_bypred(LibHandle,Predicate,Minimum,Timeout) */

void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray*prhs[] ) 
{
    /* handle of the desired field */
    mxArray *field;
    /* temp pointer */
    uintptr_t *pTmp;
    /* mex inputs */
    char pred[32768];
    int pred_len;
    int minimum;
    double timeout;
    /* function handle */
    lsl_resolve_bypred_t func;
    /* input/output variables */
    streaminfo buffer[2048];
    int result,k;    
    
    if (nrhs != 4)
        mexErrMsgTxt("4 input argument(s) required."); 
    if (nlhs != 1)
        mexErrMsgTxt("1 output argument(s) required.");
    
    /* get function handle */
    field = mxGetField(prhs[0],0,"lsl_resolve_bypred");
    if (!field)
        mexErrMsgTxt("The field does not seem to exist.");
    pTmp = (uintptr_t*)mxGetData(field);
    if (!pTmp)
        mexErrMsgTxt("The field seems to be empty.");
    func = (lsl_resolve_bypred_t*)*pTmp;
    
    /* get additional inputs */
    pred_len = mxGetNumberOfElements(prhs[1]);
    if (pred_len+1 > sizeof(pred)/sizeof(pred[0]))
        mexErrMsgTxt("The given property name is too long.");    
    mxGetNChars_700(prhs[1], pred, pred_len+1);
    if (mxGetClassID(prhs[2]) != mxDOUBLE_CLASS)
        mexErrMsgTxt("The minimum argument must be passed as a double.");
    minimum = (int)(*(double*)mxGetData(prhs[2]));
    if (mxGetClassID(prhs[3]) != mxDOUBLE_CLASS)
        mexErrMsgTxt("The timeout argument must be passed as a double.");
    timeout = *(double*)mxGetData(prhs[3]);
    
    /* invoke & return */
    result = func(buffer,sizeof(buffer)/sizeof(buffer[0]),pred,minimum,timeout);
    plhs[0] = mxCreateNumericMatrix(1,result,PTR_CLASS,mxREAL);
    for (k=0;k<result;k++)
        *((uintptr_t*)mxGetData(plhs[0])+k) = (uintptr_t)buffer[k];
}
