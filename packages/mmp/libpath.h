#pragma once

#include <string>

namespace libpath
{
	using std::wstring;

	wstring getThisDllPath();
	wstring getAppPath();
	wstring getAppDataPath();
	wstring getLocalAppDataPath();
	wstring getSystemPath();
	wstring getTempPath();

	wstring combine(wstring path1, wstring path2);
	wstring combine(wstring path);

	wstring changeExtension(wstring path, wstring extension);
	wstring getPathRoot(wstring path);
	wstring getFullPath(wstring path);
	wstring getDirectoryName(wstring);
	wstring getFileName(wstring path);
	wstring getFileNameFromUrl(const wstring& path);
	wstring getFileNameWithoutExtension(wstring path);
	wstring getExtension(wstring path);
}