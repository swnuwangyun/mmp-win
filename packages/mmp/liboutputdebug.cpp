#include "liboutputdebug.h"
#include <stdio.h>
#include <Windows.h>

namespace liboutputdebug
{

	void print(wstring fmt, ...)
	{
		wchar_t buffer[4096] = {0};

		va_list valist;
		va_start(valist, fmt);
		_vsnwprintf_s(buffer, sizeof(buffer)/sizeof(wchar_t), fmt.c_str(), valist);
		va_end(valist);

		OutputDebugString(buffer);
	}

	void println(wstring fmt, ...)
	{
		wchar_t buffer[4096] = {0};

		va_list valist;
		va_start(valist, fmt);
		_vsnwprintf_s(buffer, sizeof(buffer)/sizeof(wchar_t), fmt.c_str(), valist);
		va_end(valist);

		OutputDebugString(buffer);
		OutputDebugString(L"\r\n");
	}

}