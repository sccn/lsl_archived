#ifndef GeneralUtilH
#define GeneralUtilH


#define VERSION 1001010
#define CONSOLE 0
#include <ios>
#include <vector>
#include "stdio.h"


UnicodeString getVersion() {
	char version[16];
	int major = VERSION/1000000;
	int minor = (VERSION/1000) % 1000;
	int revision = VERSION % 1000;
	sprintf(version,"%d.%d.%d",major,minor,revision);
	return UnicodeString(version);

}

void OpenConsole() {
	 // create the console
	if(AllocConsole()) {
		freopen("CONOUT$", "w", stdout);
		SetConsoleTitle("Debug Console");
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
	}
	 ios::sync_with_stdio();
}

void CloseConsole() {
	FreeConsole();
	fclose(stdout);
}

//std::vector<UnicodeString> strs = splitString(pC->Name, L'_');
std::vector<UnicodeString> splitString(UnicodeString string, wchar_t splitCharacter) {
	int startPoint = 1;
	std::vector<UnicodeString> subStrings;
	for(int i =1; i<=string.Length(); i++) {
	if(string[i] == splitCharacter) {
			subStrings.push_back(string.SubString(startPoint, i-startPoint));
			startPoint = i+1;
		}
	}
	if(startPoint<=string.Length()) subStrings.push_back(string.SubString(startPoint, string.Length()));
	return subStrings;
}

char * generateGUID() {
	TGUID guid;
	CreateGUID(guid);
	char * charGuid = ((AnsiString) Sysutils::GUIDToString(guid)).c_str();

	return charGuid;
}

#endif
