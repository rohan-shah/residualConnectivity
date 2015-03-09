#if defined(WIN32) && defined(_MSC_VER)
#include "windowsConsoleOutput.h"
#include <io.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <fcntl.h>
void redirectConsoleOutput()
{
	int result = AttachConsole(ATTACH_PARENT_PROCESS);
	if(result)
	{
		int consoleOutDescriptor = _open_osfhandle((intptr_t)GetStdHandle(STD_OUTPUT_HANDLE), O_TEXT);
		if(consoleOutDescriptor < 0) return;
		FILE* consoleOutPointer = _fdopen(consoleOutDescriptor, "w");
		if(!consoleOutPointer) return;
		fclose(stdout);
		*stdout = *consoleOutPointer;
		setvbuf(stdout, NULL, _IONBF, 0);

		int consoleInDescriptor = _open_osfhandle((intptr_t)GetStdHandle(STD_INPUT_HANDLE), O_TEXT);
		if(consoleInDescriptor < 0) return;
		FILE* consoleInPointer = _fdopen(consoleInDescriptor, "r");
		if(!consoleInPointer) return;
		fclose(stdin);
		*stdin = *consoleInPointer;
		setvbuf(stdin, NULL, _IONBF, 0);

		int consoleErrorDescriptor = _open_osfhandle((intptr_t)GetStdHandle(STD_ERROR_HANDLE), O_TEXT);
		if(consoleErrorDescriptor< 0) return;
		FILE* consoleErrorPointer = _fdopen(consoleErrorDescriptor, "w");
		if(!consoleErrorPointer) return;
		fclose(stderr);
		*stderr = *consoleErrorPointer;
		setvbuf(stderr, NULL, _IONBF, 0);

		std::cout.clear();
		std::cin.clear();
		std::cerr.clear();
		std::ios::sync_with_stdio();
	}
}
#endif