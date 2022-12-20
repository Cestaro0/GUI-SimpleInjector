#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

#pragma once

class injector
{
public:
	bool DoInject(const wchar_t* ProgramTarget, LPCSTR DllPath);
private:
	DWORD GetPid(const wchar_t* ProcName);
};

