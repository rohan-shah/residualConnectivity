#ifndef WINDOWS_CONSOLE_OUTPUT_HEADER_GUARD
#define WINDOWS_CONSOLE_OUTPUT_HEADER_GUARD
#if defined(WIN32) && defined(_MSC_VER)
	void redirectConsoleOutput();
#endif
#endif