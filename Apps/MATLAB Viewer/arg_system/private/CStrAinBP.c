// CStrAinBP.c
// Cell strings: Find positions of strings of A in B [MEX]
//
// [AI, BI] = CStrAinBP(A, B, CaseSensitive)
// INPUT:
//   A, B: Cell strings of any size.
//   CaseSensitive: If this string starts with 'i', the upper/lower case of the
//         strings is ignored. Optional, default: 'sensitive'.
// OUTPUT:
//   AI:   Indices of common strings in A as [1 x N] vector.
//         Each occurence of repeated strings in A is considered.
//         AI is sorted from low to high indices.
//   BI:   Indices of common strings in B as [1 x N] vector. If B is not unique,
//         the first occurrence is used.
//   such that A{AI} == B{BI}.
//
// NOTES:
// - CHAR-arrays are treated as strings and the shape is ignored in the MEX.
// - If A or B are multi-dimensional cell arrays, AI and BI are the linear
//   indices (see IND2SUB).
// - The M-version is about 10 times faster than FIND(ISMEMBER) depending on
//   the input. The compiled MEX script is about 20 times faster.
// - In opposite to Matlab's INTERSECT, A(AI) is not necessarily unique if
//   A is not either.
// - In the MEX verion, CHAR arrays are treated as string with linear index:
//   ['ac'; 'bd'] is processed as 'abcd'.
//
// EXAMPLES:
//   [AI, BI] = CStrAinBP({'a', 'b', 'q', 'a'}, {'a', 'c', 'd', 'a', 'b'})
//   replies: AI = [1, 2, 4]  and: BI = [1, 5, 1]
//
//   [AI, BI] = CStrAinBP({'a', 'b', 'A'}, {'a', 'c', 'a', 'B', 'b'}, 'i')
//   replies: AI = [1, 2, 3]  and: BI = [1, 4, 1]
//
// Compile with:
//   mex -O CStrAinBP.c
//
// Tested: Matlab 6.5, 7.7, 7.8, BCC5.5, LCC2.4/3.8, Win2k/XP
// Author: Jan Simon, Heidelberg, (C) 2006-2009 J@n-Simon.De

/*
% $JRev: R0s V:024 Sum:AD6A67D1 Date:13-Sep-2009 02:08:29 $
% $File: Tools\Mex\Source\CStrAinBP.c $
% Hostory:
% 024: 12-Sep-2009 14:16, Case-insensitive method included.
*/

#include "mex.h"
#include <string.h>

int my_memicmpW(char *A, char *B, int N);

// Main function ===============================================================
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
  const mxArray *A, *AC, *B, *BC;
  int           nA, nB, iA, iB, ASlen, *BSlenp, *BSlen, Rlen,
                CaseSensitive = 1;
  double        *RA, *RAp, *RB, *RBp;
  unsigned char **BS, **BSp, **BSEnd, *AS, *CaseArg;
  
  // Check for proper number of arguments:
  if (nrhs < 2 || nrhs > 3) {
    mexErrMsgTxt("*** CStrAinBP[mex]: 2 or 3 inputs required.");
  }
  if (nlhs > 2) {
    mexErrMsgTxt("*** CStrAinBP[mex]: 1 or 2 outputs allowed.");
  }
  
  // Set flag according to optional 3rd input:
  if (nrhs == 3) {
    // Compare first character of 3rd argument:
    if (mxGetNumberOfElements(prhs[2]) > 0) {
      CaseArg = (unsigned char *)mxGetData(prhs[2]);
      if (*CaseArg == 'i' || *CaseArg == 'I') {
        CaseSensitive = 0;
      }
    }
  }
  
  // Create a pointer to the input arrays:
  A = prhs[0];
  B = prhs[1];
  
  // Check that inputs are cells:
  if (!mxIsCell(A) || !mxIsCell(B)) {
    mexErrMsgTxt("*** CStrAinBP[mex]: Inputs must be cells.");
  }
  
  // Get number of dimensions of the input string and cell:
  nA = mxGetNumberOfElements(A);
  nB = mxGetNumberOfElements(B);
  
  // Create container for output arguments:
  RA = (double *)mxMalloc(nA * sizeof(double));
  RB = (double *)mxMalloc(nA * sizeof(double));
    
  // Make list of pointers to elements of B and their length:
  BSlen = (int *) mxMalloc(nB * sizeof(int));
  BS    = (unsigned char **)mxMalloc(nB * sizeof(unsigned char *));
  BSEnd = BS + nB;
  for (iB = 0, BSp = BS, BSlenp = BSlen; iB < nB; iB++) {
    if ((BC = mxGetCell(B, iB)) == NULL) {  // Uninitialized cell element:
      mexErrMsgTxt("*** CStrAinBP[mex]: Element of B is NULL.");
    }
    
    if (!mxIsChar(BC)) {
      mexErrMsgTxt("*** CStrAinBP[mex]: Element of B is no string.");
    }
    
    *BSlenp++ = mxGetNumberOfElements(BC);
    *BSp++    = (unsigned char *)mxGetData(BC);
  }  // end for iB
  
  // Pointers to list of strings:
  RAp = RA;
  RBp = RB;
  
  if (CaseSensitive) {  // -----------------------------------------------------
    for (iA = 0; iA < nA; iA++) {  // Loop over all strings of A:
      if ((AC = mxGetCell(A, iA)) == NULL) {  // Uninitialized cell element:
        mexErrMsgTxt("*** CStrAinBP[mex]: Element of A is NULL.");
      }
      
      if (!mxIsChar(AC)) {         // Check if cell element is a string:
        mexErrMsgTxt("*** CStrAinBP[mex]: Element of A is no string.");
      }
      
      AS     = (unsigned char *)mxGetData(AC);
      ASlen  = mxGetNumberOfElements(AC);
      BSlenp = BSlen;  // Pointer to list of lengths of B's strings
      
      // Loop over pointers to strings of B:
      for (BSp = BS; BSp < BSEnd; BSp++) {
        if (ASlen == *BSlenp++) {     // Cheap comparison of length
          if (memcmp(AS, *BSp, ASlen * sizeof(mxChar)) == 0) {
            *RAp++ = (double) (iA + 1);
            *RBp++ = (double) (BSp - BS + 1);
            break;  // Just take first occurrence in B
          }
        }
      }   // end for BSp
     }  // end for iA
     
  } else {  // Not case-sensitive: ---------------------------------------------
    // Same method except for memcmp -> my_memicmpW:
    for (iA = 0; iA < nA; iA++) {  // Loop over all strings of A:
      if ((AC = mxGetCell(A, iA)) == NULL) {  // Uninitialized cell element:
        mexErrMsgTxt("*** CStrAinBP[mex]: Element of A is NULL.");
      }
      
      if (!mxIsChar(AC)) {         // Check if cell element is a string:
        mexErrMsgTxt("*** CStrAinBP[mex]: Element of A is no string.");
      }
      
      AS     = (unsigned char *)mxGetData(AC);
      ASlen  = mxGetNumberOfElements(AC);
      BSlenp = BSlen;  // Pointer to list of lengths of B's strings
      
      // Loop over pointers to strings of B:
      for (BSp = BS; BSp < BSEnd; BSp++) {
        if (ASlen == *BSlenp++) {     // Cheap comparison of length
          if (my_memicmpW(AS, *BSp, ASlen) == 0) {
            *RAp++ = (double) (iA + 1);
            *RBp++ = (double) (BSp - BS + 1);
            break;  // Just take first occurrence in B
          }
        }
      }   // end for BSp
     }  // end for iA
     
  }
  
  // Create output arguments, [0 x 0] for empty arrays:
  Rlen    = (int) (RAp - RA);  // Number of found occurrences
  plhs[0] = mxCreateDoubleMatrix(Rlen ? 1 : 0, Rlen, mxREAL);
  memcpy(mxGetPr(plhs[0]), RA, Rlen * sizeof(double));
  
  if (nlhs == 2) {
    plhs[1] = mxCreateDoubleMatrix(Rlen ? 1 : 0, Rlen, mxREAL);
    memcpy(mxGetPr(plhs[1]), RB, Rlen * sizeof(double));
  }
  
  // Free memory for indices, strings and strings lengths:
  mxFree(RA);
  mxFree(RB);
  mxFree(BS);
  mxFree(BSlen);
  
  return;
}

// =============================================================================
int my_memicmpW(char *A, char *B, int N)
{
  // MEMCMP, but accept letters with different case.
  // Just the ASCII part of the mxChar is considered: only every 2nd byte is
  // compared!
  // Upper and lower case letters differ in bit 5 only. So if both characters
  // differ from eachother, but differ in bit 5 only and one is a letter, they
  // are i-equal and 0 is replied. Otherwise 1 is replied.
  
  // INTERNAL:
  // MEMICMP slower then my_memicmpW: my_memicmpW assumes, that every 2nd
  // byte equals 0, what is the case for ASCII characters!
  // MEMICMP is available in BCC v5.5 and LCC v3, but not in LCC v2 shipped
  // with Matlab. Nevertheless, MEMICMP has no unicode support also. So what
  // actually is needed is STRCMPI for Unicode and with a defined string length.

  int  i;
  char U;
  
  for (i = 0; i < N; i++) {
    if (*A != *B) {
      if ((*A ^ *B) != 32) {  // Differ in more than in bit 5
        return 1;
      } else {                // Is upper(*A) inside ['A':'Z']
        U = (*A | 32);
        if ((U < 97) || (U > 122)) {
          return 1;
        }
      }
    }
    A += 2;
    B += 2;
  }
  return 0;
}
