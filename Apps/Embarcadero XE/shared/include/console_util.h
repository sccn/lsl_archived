#define CONSOLE 0

#include <ios>

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
