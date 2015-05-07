//---------------------------------------------------------------------------


#pragma hdrstop

#include "UParseLine.h"

//---------------------------------------------------------------------------

#pragma package(smart_init)
TStringList * ParseCmdLine(AnsiString par,char delimiter)
{
	char buf[1001];
	static TStringList * pList = new TStringList;
	pList->Clear();
	if (par.Length())
	{
		strcpy(buf,par.c_str());
		char * h = buf;
		char * pb = strchr(buf,delimiter);
		while (true)
		{
			pb = strchr(buf,delimiter);
			if (pb!=0)
			{
				*pb = 0;pb++;
			}
			pList->Add(h);
			if (pb==0)	break;
			h=pb;
		}
	}
	return pList;
}
