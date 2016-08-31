/**
  Application for Video display and saving. Can track human gaze position in scene camera,
when calibrated using Eye Calibrator. Designed to be used with a head mounted eye tracker.

Written by Matthew Grivich, Swartz Center for Computational Neuroscience, UCSD
*/

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop
#include "windows.h"
#include "stdio.h"
#include "general_util.h"
//---------------------------------------------------------------------------
USEFORM("CaptureWorkerForm.cpp", CaptureWorkerForm);
USEFORM("UMainCapture.cpp", MainCaptureForm);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

	if(CONSOLE) OpenConsole();
	try
	{
		Application->Initialize();
		Application->CreateForm(__classid(TMainCaptureForm), &MainCaptureForm);
		Application->CreateForm(__classid(TCaptureWorkerForm), &CaptureWorkerForm);
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
