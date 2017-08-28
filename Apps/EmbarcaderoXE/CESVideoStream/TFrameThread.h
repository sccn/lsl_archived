/**
 * Frame Thread. While frames are in the queue, it pops and processes them.
 *
 */

//---------------------------------------------------------------------------

#ifndef TFrameThreadH
#define TFrameThreadH
#include "useallegro4.h"
#include <queue.h>
#include "UMainCapture.h"




class TFrameThread : public TThread
{
__published:	// IDE-managed Components
private:	// User declarations
	TMainCaptureForm *MainCaptureForm;
	queue<BITMAP*>* bmpQueue;
	BITMAP * bmpCanvas;
	HANDLE hMutex;

protected:  // User declarations
	void __fastcall Execute();

public:		// User declarations
	 TFrameThread(TMainCaptureForm *MainCaptureForm,
		queue<BITMAP*>& bmpQueue,
		HANDLE hMutex,
		bool suspended);
		__fastcall virtual ~TFrameThread();
		bool getTerminated();


	};


//---------------------------------------------------------------------------
#endif
