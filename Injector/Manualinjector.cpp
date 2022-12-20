#include "Manualinjector.h"

DWORD __stdcall LibraryLoader(LPVOID Memory)
{
	loaderdata* LoaderParams = (loaderdata*)Memory;

	PIMAGE_BASE_RELOCATION ImageRelocation = LoaderParams->BaseReloc;

	DWORD delta = (DWORD)((LPBYTE)LoaderParams->ImageBase - LoaderParams->NtHeaders->OptionalHeader.ImageBase);

	while (ImageRelocation->VirtualAddress)
	{
		if (ImageRelocation->SizeOfBlock >= sizeof(PIMAGE_BASE_RELOCATION))
		{
			int count = (ImageRelocation->SizeOfBlock - sizeof(PIMAGE_BASE_RELOCATION) / sizeof(WORD));
			PWORD list = (PWORD)(ImageRelocation + 1);

			for (int i = 0; i < count; i++)
			{
				PWORD ptr = (PWORD)((LPBYTE)LoaderParams->ImageBase + (ImageRelocation->VirtualAddress + (list[i] & 0xFFF)));
				*ptr += delta;
			}
		}
	}

	PIMAGE_IMPORT_DESCRIPTOR ImportDesc = LoaderParams->ImportDir;

	while (ImportDesc->Characteristics)
	{
		PIMAGE_THUNK_DATA OriginalFirstThunk = (PIMAGE_THUNK_DATA)((LPBYTE)LoaderParams->ImageBase + ImportDesc->OriginalFirstThunk);
		PIMAGE_THUNK_DATA FirstThunk = (PIMAGE_THUNK_DATA)((LPBYTE)LoaderParams->ImageBase + ImportDesc->FirstThunk);

		HMODULE hModule = LoaderParams->fnLoadLibraryA((LPCSTR)LoaderParams->ImageBase + ImportDesc->Name);

		if (!hModule)
		{
			return FALSE;
		}

		while (OriginalFirstThunk->u1.AddressOfData)
		{
			if (OriginalFirstThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
			{
				DWORD Function = (DWORD)LoaderParams->fnGetProcAddress(hModule, (LPCSTR)(OriginalFirstThunk->u1.Ordinal & 0xFFFF));

				if (!Function)
				{
					return FALSE;
				}

				FirstThunk->u1.Function = Function;
			}
		}
	}

	if (LoaderParams->NtHeaders->OptionalHeader.AddressOfEntryPoint)
	{
		dllmain EntryPoint = (dllmain)((LPBYTE)LoaderParams->ImageBase + LoaderParams->NtHeaders->OptionalHeader.AddressOfEntryPoint);

		return EntryPoint((HMODULE)LoaderParams->ImageBase, DLL_PROCESS_ATTACH, NULL);
	}

	return true;
}

DWORD __stdcall stub()
{
	return 0;
}

DWORD Manualinjector::GetPid(const wchar_t* ProcName)
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

bool Manualinjector::ManualInject(const wchar_t* ProgramTarget, LPCSTR DllPath)
{
	loaderdata LoaderParams;

	HANDLE hDll = CreateFileA(DllPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);

	DWORD FileSize = GetFileSize(hDll, NULL);

	LPVOID FileBuffer = VirtualAlloc(NULL, FileSize, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	ReadFile(hDll, FileBuffer, FileSize, NULL, NULL);


	PIMAGE_DOS_HEADER DosHeader = (PIMAGE_DOS_HEADER)FileBuffer;
	PIMAGE_NT_HEADERS NtHeaders = (PIMAGE_NT_HEADERS)((LPBYTE)FileBuffer + DosHeader->e_lfanew);

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, GetPid(ProgramTarget));

	LPVOID ExecutableImage = VirtualAllocEx(hProcess, NULL, NtHeaders->OptionalHeader.SizeOfImage, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	WriteProcessMemory(hProcess, ExecutableImage, FileBuffer, NtHeaders->OptionalHeader.SizeOfHeaders, NULL);

	PIMAGE_SECTION_HEADER SecHeader = (PIMAGE_SECTION_HEADER)(NtHeaders + 1);

	for (int i = 0; i < NtHeaders->FileHeader.NumberOfSections; i++)
	{
		WriteProcessMemory(hProcess, (PVOID)((LPBYTE)ExecutableImage + SecHeader[i].VirtualAddress), (PVOID)((LPBYTE)FileBuffer + SecHeader[i].PointerToRawData), SecHeader[i].SizeOfRawData, NULL);
	}

	LPVOID LoaderMemory = VirtualAllocEx(hProcess, NULL, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	LoaderParams.ImageBase = ExecutableImage;
	LoaderParams.NtHeaders = (PIMAGE_NT_HEADERS)((LPBYTE)ExecutableImage + DosHeader->e_lfanew);

	LoaderParams.BaseReloc = (PIMAGE_BASE_RELOCATION)((LPBYTE)ExecutableImage + NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress);
	LoaderParams.ImportDir = (PIMAGE_IMPORT_DESCRIPTOR)((LPBYTE)ExecutableImage + NtHeaders->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

	WriteProcessMemory(hProcess, LoaderMemory, &LoaderParams, sizeof(loaderdata), NULL);
	WriteProcessMemory(hProcess, (PVOID)((loaderdata*)LoaderMemory + 1), LibraryLoader, (DWORD)stub - (DWORD)LibraryLoader, NULL);

	HANDLE hThread = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)((loaderdata*)LoaderMemory + 1), LoaderMemory, NULL, NULL);

	std::cout << "Address of Loader: " << std::hex << LoaderMemory << std::endl;
	std::cout << "Address of Image: " << std::hex << ExecutableImage << std::endl;

	WaitForSingleObject(hThread, INFINITE);

	
	VirtualFreeEx(hProcess, LoaderMemory, NULL, MEM_RELEASE);

	return true;
}
