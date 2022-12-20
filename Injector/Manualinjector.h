#pragma once
#include <iostream>
#include <string>
#include <Windows.h>
#include <TlHelp32.h>

typedef HMODULE(__stdcall* pLoadLibraryA)(LPCSTR);
typedef FARPROC(__stdcall* pGetProcAddress)(HMODULE, LPCSTR);
typedef INT(__stdcall* dllmain)(HMODULE, DWORD, LPVOID);

struct loaderdata
{
	LPVOID ImageBase;
	PIMAGE_NT_HEADERS NtHeaders;
	PIMAGE_BASE_RELOCATION BaseReloc;
	PIMAGE_IMPORT_DESCRIPTOR ImportDir;
	pLoadLibraryA fnLoadLibraryA;
	pGetProcAddress fnGetProcAddress;
};

class Manualinjector
{
public:
	bool ManualInject(const wchar_t* ProgramTarget, LPCSTR DllPath);
private:
	DWORD GetPid(const wchar_t* ProcName);
};

