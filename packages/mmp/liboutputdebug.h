#pragma once

#include <string>

namespace liboutputdebug
{
	using std::wstring;

	void print(wstring fmt, ...);
	void println(wstring fmt, ...);
}