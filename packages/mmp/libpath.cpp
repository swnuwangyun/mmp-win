// libpath.cpp : Defines the exported functions for the DLL application.
//

#include <windows.h>
#include <shlobj.h>
#include "libpath.h"
#include "libtext.h"

using namespace std;

namespace libpath
{

wstring getThisDllPath()
{
	HMODULE hm = NULL;
	wchar_t buffer[512]={0};
	GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, (LPWSTR)&getThisDllPath, &hm);
	GetModuleFileNameW(hm, buffer ,sizeof(buffer));
	wstring path = wstring(buffer);
	path.erase(path.find_last_of(L"\\"));
	return path;
}

wstring getAppPath()
{
	wchar_t buffer[512]={0};
	GetModuleFileNameW(NULL, buffer ,sizeof(buffer));
	wstring path = wstring(buffer);
	path.erase(path.find_last_of(L"\\"));
	return path;
}

wstring getAppDataPath()
{
	wchar_t szPath[MAX_PATH];
	ZeroMemory(szPath, MAX_PATH); 
	SHGetSpecialFolderPathW(NULL, szPath, CSIDL_APPDATA, FALSE);
	return szPath;
}

wstring getLocalAppDataPath()
{
	wchar_t szPath[MAX_PATH];
	ZeroMemory(szPath, MAX_PATH); 
	SHGetSpecialFolderPathW(NULL, szPath, CSIDL_LOCAL_APPDATA, FALSE);
	return szPath;
}

wstring getSystemPath()
{
	wchar_t szPath[MAX_PATH];
	ZeroMemory(szPath, MAX_PATH); 
	SHGetSpecialFolderPathW(NULL, szPath, CSIDL_SYSTEM, FALSE);
	return szPath;
}

wstring getTempPath()
{
	wchar_t szPath[MAX_PATH];
	ZeroMemory(szPath, MAX_PATH); 
	GetTempPathW(MAX_PATH, szPath);
	return szPath;
}

wstring combine(wstring path1, wstring path2)
{
	return libtext::trimRight(path1, L"\\") + L"\\" + libtext::trimLeft(path2, L"\\");
}

wstring combine(wstring path)
{
	return combine(getAppPath(), path);
}

wstring changeExtension(wstring path, wstring extension)
{
	wstring ret = path;
	ret.erase(ret.find_last_of(L".") + 1);
	ret += extension;
	return ret;
}

wstring getPathRoot(wstring path)
{
	if(path == L"")
		return L"";
	wstring ret = path;
	ret = ret.substr(0, ret.find_first_of(L"\\"));
	return ret;
}

wstring getFullPath(wstring path)
{
	if(path == L"")
		return L"";
	wstring ret = path;
	ret = ret.substr(0, ret.find_last_of(L"\\"));
	return ret;
}

wstring getDirectoryName(wstring path)
{
	if(path == L"")
		return L"";
	vector<wstring> items = libtext::split(path, L"\\");
	return items[items.size()-2];
}

wstring getFileName(wstring path)
{
	if(path == L"")
		return L"";
	vector<wstring> items = libtext::split(path, L"\\");
	return items[items.size()-1];
}

wstring getFileNameFromUrl(const wstring& path)
{
	if(path == L"")
		return L"";
	vector<wstring> items = libtext::split(path, L"/");
	return items[items.size()-1];
}

wstring getFileNameWithoutExtension(wstring path)
{
	if(path == L"")
		return L"";
	wstring name = getFileName(path);
	name = name.erase(name.find_last_of(L"."));
	return name;
}

wstring getExtension(wstring path)
{
	if(path == L"")
		return L"";
	wstring name = getFileName(path);
	vector<wstring> items = libtext::split(name, L".");
	return items[items.size()-1];
}

}