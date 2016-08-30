#ifdef BALERT_EXPORTS
#define BALERT_API __declspec(dllexport)
#else
#define BALERT_API __declspec(dllimport)
#endif




BALERT_API int __stdcall BAlertGetDeviceType(); 
	
BALERT_API int __stdcall BAlertStart(bool bUseFilter);

BALERT_API int __stdcall BAlertStop();
	
BALERT_API int __stdcall BAlertWaitForData(float* fBuffer, int nBlockSize);

