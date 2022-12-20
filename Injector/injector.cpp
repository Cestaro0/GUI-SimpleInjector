#include "injector.h"
#pragma warning(disable:4996)

DWORD injector::GetPid(const wchar_t* ProcName)
{
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);

	HANDLE hProcSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	do
	{
		if (wcscmp(pe32.szExeFile, ProcName) == 0)
		{
			CloseHandle(hProcSnap);

			return pe32.th32ProcessID;
		}
	} while (Process32Next(hProcSnap, &pe32));

	CloseHandle(hProcSnap);

	return 0;
}

bool injector::DoInject(const wchar_t* ProgramTarget, LPCSTR DllPath)
{
	HANDLE hProcess = OpenProcess(
		PROCESS_ALL_ACCESS,
		FALSE,
		GetPid(ProgramTarget)
	);

	if (hProcess == INVALID_HANDLE_VALUE)
		return false;

	LPVOID pDllPath = VirtualAllocEx(
		hProcess,
		NULL,
		strlen(DllPath) + 1,	//null terminator
		MEM_COMMIT, PAGE_READWRITE
	);

	if (pDllPath == NULL)
		return false;

	WriteProcessMemory(
		hProcess,
		pDllPath,
		(LPVOID)DllPath,
		strlen(DllPath) + 1, //null terminator
		NULL
	);

	HANDLE hLoadThread = CreateRemoteThread(
		hProcess,
		NULL,
		NULL,
		(LPTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandleA("Kernel32.dll"), "LoadLibraryA"),
		pDllPath,
		NULL,
		NULL
	);

	if (hLoadThread == INVALID_HANDLE_VALUE)
		return false;

	WaitForSingleObject(
		hLoadThread,
		INFINITE
	);

	std::cin.get();

	VirtualFreeEx(
		hProcess,
		pDllPath,
		strlen(DllPath) + 1, //null terminator
		MEM_RELEASE
	);

	return true;
}