//---------------------------------------------------------------------------


#pragma hdrstop

#include "TFrameThread.h"
#include "float.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)


/*__fastcall*/ TFrameThread::TFrameThread(TMainCaptureForm *MainCaptureForm,
	queue<BITMAP*>& bmpQueue, HANDLE hMutex,
	bool suspended) : TThread(suspended)
{
	this->MainCaptureForm = MainCaptureForm;
	this->bmpQueue = &bmpQueue;
	this->hMutex = hMutex;
}


void __fastcall TFrameThread::Execute()
{

	while(!Terminated) {
		_clearfp();
//	printf("          %d\n", bmpQueue->size());
		if(!bmpQueue->empty()) {
			BITMAP * aBmp = NULL;
			unsigned int res = WaitForSingleObject(hMutex,INFINITE);
			if (res!=WAIT_OBJECT_0) {ReleaseMutex(hMutex);continue;}
			__try
			{
				aBmp = bmpQueue->front();
				bmpQueue->pop();
			}
			__finally
			{
				ReleaseMutex(hMutex);

			}

  			MainCaptureForm->DoFrame(aBmp);

			destroy_bitmap(aBmp);


		} else {
			Sleep(1);
		}
	}
}

bool TFrameThread::getTerminated() {
	return Terminated;
}

__fastcall TFrameThread::~TFrameThread()
{
	while(!bmpQueue->empty()) {
		BITMAP * aBmp = bmpQueue->front();
		bmpQueue->pop();
		destroy_bitmap(aBmp);
	}

}