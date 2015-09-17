#ifndef ___CEnobioCtrl_HEADER__
#define ___CEnobioCtrl_HEADER__
//#include <vector>
using namespace std;
class Enobio;
#ifdef __cplusplus
extern "C" {
#endif

#define FUNCTION __declspec(dllexport)
	FUNCTION Enobio* __cdecl  CEnobioCreate();
	FUNCTION bool __cdecl     CEnobioInitialize(Enobio* enobioClass,int sampleBlockSize);
	FUNCTION bool __cdecl     CEnobioCaptureStart (Enobio* enobioClass) ;
	FUNCTION bool __cdecl     CEnobioWaitForData(Enobio* enobioClass,int** buf,int sampleBlockSize);
	FUNCTION bool __cdecl     CEnobioCaptureStop (Enobio* enobioClass);
	FUNCTION bool __cdecl     CEnobioClose(Enobio* enobioClass);
	FUNCTION int __cdecl      CEnobioGetLastError(Enobio* enobioClass);
	FUNCTION const char * __cdecl    CEnobioStrError(int NumError,Enobio* enobioClass);
	FUNCTION bool __cdecl  CEnobioIsInitialized(Enobio* enobioClass);
	FUNCTION int* __cdecl CEnobioChannelStatus(Enobio*);
	FUNCTION void __cdecl  CEnobioResetBuffer(Enobio* enobioClass);
#endif ___CEnobioCtrl_HEADER__
#ifdef __cplusplus
}
;
#endif
