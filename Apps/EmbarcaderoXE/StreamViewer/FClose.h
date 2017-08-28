//---------------------------------------------------------------------------

#ifndef FCloseH
#define FCloseH
//---------------------------------------------------------------------------
 //******************
   //Header
   //******************

   #include <windows.h>

   #define TA_FAILED 0
   #define TA_SUCCESS_CLEAN 1
   #define TA_SUCCESS_KILL 2
   #define TA_SUCCESS_16 3

   DWORD WINAPI TerminateApp( DWORD dwPID, DWORD dwTimeout ) ;
   DWORD WINAPI Terminate16App( DWORD dwPID, DWORD dwThread,
                        WORD w16Task, DWORD dwTimeout );

#endif
