#include "lsl_common.h"

/* function lsl_push_chunk(LibHandle, Outlet, ChunkData [,Timestamps] [,Pushthrough]) 
 * - ChunkData must be a MxN double matrix, where each column is a separate sample to push.
 * - Timestamps is a vector of the same length as ChunkData has columns.
 */

void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray*prhs[] ) 
{
    /* handle of the desired field */
    mxArray *field, *cell;
    /* temp pointer */
    uintptr_t *pTmp;
    /* function handle */
    lsl_push_sample_dtp_t func_num;   /* numeric data */
    /* input/output variables */
    double *chunkdata, *timestamps;
    int pushthrough=1;
    int k,numsamples,numchannels;
    outlet out;
    
    if (nrhs < 4)
        mexErrMsgTxt("At least 4 input argument(s) required."); 
    if (nrhs > 5)
        mexErrMsgTxt("At most 5 input argument(s) required."); 
    if (nlhs != 0)
        mexErrMsgTxt("0 output argument(s) required.");

    /* get function handle */
    field = mxGetField(prhs[0], 0, "lsl_push_sample_dtp");
    if (!field)
        mexErrMsgTxt("The field does not seem to exist.");
    pTmp = (uintptr_t*)mxGetData(field);
    if (!pTmp)
        mexErrMsgTxt("The field seems to be empty.");    
    func_num = (lsl_push_sample_dtp_t*)*pTmp;
    
    /* get outlet ptr */
    out = *(uintptr_t*)mxGetData(prhs[1]);

    /* get ChunkData ptr & check format */
    if (mxGetClassID(prhs[2]) != mxDOUBLE_CLASS)
        mexErrMsgTxt("The ChunkData must be numeric (string-formatted samples must currently be submitted sample-by-sample).");
    chunkdata = (double*)mxGetData(prhs[2]);    
    if (mxGetNumberOfElements(prhs[2]) == 0)
        return;
    numchannels = mxGetM(prhs[2]);
    numsamples = mxGetN(prhs[2]);
    
    /* get timestamp pointer */
    if (mxGetClassID(prhs[3]) != mxDOUBLE_CLASS)
        mexErrMsgTxt("The timestamp argument must be passed as a double.");
    if (mxGetNumberOfElements(prhs[3]) != mxGetN(prhs[2]))
        mexErrMsgTxt("The number of timestamps provided must match the number of columns in the chunk data matrix.");
    timestamps = (double*)mxGetData(prhs[3]);
    
    if (nrhs >= 5) {
        /* optionally read pushthrough value */
        if (mxGetClassID(prhs[4]) != mxDOUBLE_CLASS)
            mexErrMsgTxt("The pushthrough argument must be passed as a double (1 or 0).");
        pushthrough = ((int)*(double*)mxGetData(prhs[4])) != 0;
    }

    /* now send the samples (pushthrough is only set for the last sample in the chunk) */
    for (k=0;k<numsamples-1;k++,chunkdata+=numchannels,timestamps++)
        func_num(out, chunkdata, *timestamps, 0);
    func_num(out, chunkdata, *timestamps, pushthrough);
}
