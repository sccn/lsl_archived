#include "lsl_common.h"

/* function lsl_push_sample(LibHandle, Outlet, SampleData [,Timestamp] [,Pushthrough]) */

void mexFunction( int nlhs, mxArray *plhs[], 
		  int nrhs, const mxArray*prhs[] ) 
{
    /* handle of the desired field */
    mxArray *field, *cell;
    /* temp pointer */
    uintptr_t *pTmp;
    /* function handle */
    lsl_push_sample_dtp_t func_num;   /* numeric data */
    lsl_push_sample_buftp_t func_buf; /* string data */
    /* input/output variables */
    double timestamp=0.0;
    int pushthrough=1;
    int numcells,k,len;
    int returncode=0;
    char **buffer;
    unsigned *lengths;
    outlet out;
    
    if (nrhs < 3)
        mexErrMsgTxt("At least 3 input argument(s) required."); 
    if (nrhs > 5)
        mexErrMsgTxt("At most 5 input argument(s) required."); 
    if (nlhs != 0)
        mexErrMsgTxt("0 output argument(s) required.");
        
    /* get outlet ptr */
    out = *(uintptr_t*)mxGetData(prhs[1]);
    
    if (nrhs >= 4) {
        /* read timestamp value */
        if (mxGetClassID(prhs[3]) != mxDOUBLE_CLASS)
            mexErrMsgTxt("The timestamp argument must be passed as a double.");
        timestamp = *(double*)mxGetData(prhs[3]);
    }

    if (nrhs >= 5) {
        /* read pushthrough value */
        if (mxGetClassID(prhs[4]) != mxDOUBLE_CLASS)
            mexErrMsgTxt("The pushthrough argument must be passed as a double (1 or 0).");
        pushthrough = ((int)*(double*)mxGetData(prhs[4])) != 0;
    }
        
    if (mxGetClassID(prhs[2]) == mxDOUBLE_CLASS) {
        /* sending numeric data; get function handle */
        field = mxGetField(prhs[0], 0, "lsl_push_sample_dtp");
        if (!field)
            mexErrMsgTxt("The field does not seem to exist.");
        pTmp = (uintptr_t*)mxGetData(field);
        if (!pTmp)
            mexErrMsgTxt("The field seems to be empty.");
        func_num = (lsl_push_sample_dtp_t*)*pTmp;
        /* invoke */
        returncode = func_num(out,(double*)mxGetData(prhs[2]),timestamp,pushthrough);
    } else  {
        if (mxGetClassID(prhs[2]) == mxCELL_CLASS) {
            /* string data; get function handle */
            field = mxGetField(prhs[0], 0, "lsl_push_sample_buftp");
            if (!field)
                mexErrMsgTxt("The field does not seem to exist.");
            pTmp = (uintptr_t*)mxGetData(field);
            if (!pTmp)
                mexErrMsgTxt("The field seems to be empty.");
            func_buf = (lsl_push_sample_buftp_t*)*pTmp;            
            
            /* allocate temporary buffer space */
            numcells = mxGetNumberOfElements(prhs[2]);
            buffer = malloc(numcells * sizeof(char*));
            lengths = malloc(numcells * sizeof(unsigned));
            
            for (k=0;k<numcells;k++) {
                cell = mxGetCell(prhs[2],k);
                if (cell) {
                    if (mxGetClassID(cell) == mxCHAR_CLASS) {
                        len = mxGetNumberOfElements(cell);
                        buffer[k] = malloc(len+1);
                        lengths[k] = len;
                        mxGetString(cell, buffer[k], len+1);
                    }
                    if (mxGetClassID(cell) == mxINT8_CLASS || mxGetClassID(cell) == mxUINT8_CLASS) {
                        len = mxGetNumberOfElements(cell);
                        buffer[k] = malloc(len);
                        lengths[k] = len;
                        memcpy(buffer[k],mxGetData(cell),len);
                    }
                } else {
                    buffer[k] = malloc(1);
                    buffer[k][0] = 0;
                }
            }
            
            /* invoke */
            returncode = func_buf(out,buffer,lengths,timestamp,pushthrough);
            
            /* free buffer space */
            for (k=0;k<numcells;k++)
                free(buffer[k]);
            free(lengths);
            free(buffer);
        } else {
            mexErrMsgTxt("Please call this function only with double or string data (it will be converted into the appropriate sample format).");
        }
    }
    if (returncode == -1)
        mexErrMsgTxt("Error while performing a push_sample: timeout.");
    if (returncode == -3)
        mexErrMsgTxt("Error while performing a push_sample: invalid argument.");
    if (returncode == -4)
        mexErrMsgTxt("Error while performing a push_sample: internal error.");
    if (returncode < 0)
        mexErrMsgTxt("Error while performing a push_sample: unknown error.");
}
