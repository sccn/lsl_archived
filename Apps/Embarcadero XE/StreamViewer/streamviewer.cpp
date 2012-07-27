//---------------------------------------------------------------------------

#include "UNewViewer2.h"
#include <vcl.h>
#pragma hdrstop
#include "windows.h"
//---------------------------------------------------------------------------
USEFORM("UNewViewer2.cpp", Form1);
USEFORM("ULeds.cpp", FormLEDs);
//---------------------------------------------------------------------------
AnsiString par;
bool ok=true;
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR str, int)
{

	BOOL WINAPI console = AllocConsole();
	if(!console) return 0;
	freopen("CONOUT$", "w", stdout);
	par = str;
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
	return 0;
}
//---------------------------------------------------------------------------
