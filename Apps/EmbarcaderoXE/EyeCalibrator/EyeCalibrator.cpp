/**
Application for calibrating head mounted eye tracker.
Written by Matthew Grivich, Swartz Center for Computational Neuroscience, UCSD, 2011

*/


#include <vcl.h>
#pragma hdrstop
#include <tchar.h>
#include "stdio.h"
#include "windows.h"
#include "general_util.h"
//---------------------------------------------------------------------------
USEFORM("EyeCalibratorForm.cpp", Form4);
//---------------------------------------------------------------------------
WINAPI _tWinMain(HINSTANCE, HINSTANCE, LPTSTR, int)
{

	if(CONSOLE) OpenConsole();
	try
	{
		Application->Initialize();
		Application->MainFormOnTaskBar = true;
		Application->CreateForm(__classid(TForm4), &Form4);
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
