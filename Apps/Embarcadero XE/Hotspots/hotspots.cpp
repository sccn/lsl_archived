//---------------------------------------------------------------------------

#include <vcl.h>
#include <streambuf>
#include <iostream>
#pragma hdrstop
#include "stdio.h"
#include "windows.h"
#include "general_util.h"
//---------------------------------------------------------------------------
USEFORM("UEdit3D.cpp", Form3);
USEFORM("GridDialog.cpp", GridForm);
USEFORM("ScreenDialog.cpp", ScreenForm);
USEFORM("U3dHotspots.cpp", Form11);
//---------------------------------------------------------------------------
AnsiString par;

WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR str, int)
{
	if(CONSOLE) OpenConsole();

	try
	{


		Application->Initialize();
		Application->CreateForm(__classid(TForm11), &Form11);
		Application->CreateForm(__classid(TForm3), &Form3);
		Application->CreateForm(__classid(TGridForm), &GridForm);
		Application->CreateForm(__classid(TScreenForm), &ScreenForm);
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
