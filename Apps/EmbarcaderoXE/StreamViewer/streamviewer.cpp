//---------------------------------------------------------------------------

#include "UNewViewer2.h"
#include <vcl.h>
#pragma hdrstop
#include "windows.h"
#include "general_util.h"
//---------------------------------------------------------------------------
USEFORM("UNewViewer2.cpp", Form1);
USEFORM("ULeds.cpp", FormLEDs);
//---------------------------------------------------------------------------
bool ok=true;
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR str, int)
{

	if(CONSOLE) OpenConsole();
	try
	{
		Application->Initialize();
		Application->CreateForm(__classid(TForm1), &Form1);
		Application->CreateForm(__classid(TFormLEDs), &FormLEDs);
		if (!ok)
		PostMessage(Form1->Handle,WM_CLOSE,0,0);
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
