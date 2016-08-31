//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "UMainCapture.h"
#include "general_util.h"
//---------------------------------------------------------------------------

USEFORM("..\GazeStream\CaptureWorkerForm.cpp", CaptureWorkerForm);
USEFORM("..\GazeStream\UMainCapture.cpp", MainCaptureForm);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

	if(CONSOLE) OpenConsole();
	try
	{
		Application->Initialize();

		Application->CreateForm(__classid(TMainCaptureForm), &MainCaptureForm);
		Application->CreateForm(__classid(TCaptureWorkerForm), &CaptureWorkerForm);
		MainCaptureForm->SetToVideoMode();

		Application->Run();



	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	catch (...)
	{
		try
		{
			throw Exception("");
		}
		catch (Exception &exception)
		{
			Application->ShowException(&exception);
		}
	}
	if(CONSOLE) CloseConsole();
	return 0;
}
//---------------------------------------------------------------------------
