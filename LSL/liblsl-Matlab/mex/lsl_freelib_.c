#include "lsl_common.h"
        
/*
 * lsl_freelib_(LibHandle);
 * Internal function to unload the lab streaming layer library.
 *
 * Note: as a user you don't have to call this function directly -- lsl_loadlib() ensures that
 * it gets called automatically when the last reference to the library handle is deleted.s
 * 
 * In:
 *   LibHandle   : handle to the library, as returned by lsl_loadlib_().
 *   
 */

void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray*prhs[] ) 
{
    mxArray *field;
    uintptr_t *pTmp;  /* temp pointer */
    void *hlib;       /* handle to the library */
    if (nrhs != 1)
        mexErrMsgTxt("One input argument required."); 
    if (mxGetNumberOfElements(prhs[0]) != 1)
        mexErrMsgTxt("The input must be a scalar.");
    
    /* get the library handle */
    field = mxGetField(prhs[0],0,"hlib");
    if (!field)
        mexErrMsgTxt("Field does not seem to exist...");
    pTmp = (uintptr_t*)mxGetData(field);
    if (!pTmp)
        mexErrMsgTxt("Entry does not seem to exist...");
    hlib = (void*)*pTmp;
    /* unload the library */
    FREE_LIBRARY(hlib);
}

