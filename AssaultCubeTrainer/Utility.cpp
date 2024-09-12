#include "Utility.h"
#include <TlHelp32.h>

namespace Utility
{
	BOOL IsValidHandle(HANDLE handle)
	{
		if (handle == 0 || handle == INVALID_HANDLE_VALUE)
		{
			return FALSE;
		}
		else
		{
			return TRUE;
		}
	}

	DWORD GetProcessId(const WCHAR* procName)
	{
		DWORD procId{};

		HANDLE hSnapShotHandle
		{
			CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)
		};

		if (!IsValidHandle(hSnapShotHandle))
		{
			return procId;
		}

		PROCESSENTRY32 procEntry{};
		procEntry.dwSize = sizeof(PROCESSENTRY32);

		if (!Process32First(hSnapShotHandle, &procEntry))
		{
			CloseHandle(hSnapShotHandle);
			return procId;
		}

		do
		{
			if (!_wcsicmp(procEntry.szExeFile, procName))
			{
				CloseHandle(hSnapShotHandle);
				return procEntry.th32ProcessID;
			}
		} while (Process32Next(hSnapShotHandle, &procEntry));

		CloseHandle(hSnapShotHandle);
		return procId;
	}

	DWORD GetModuleBaseAddress(const WCHAR* modName, const DWORD procId)
	{
		DWORD moduleBaseAddress{};

		HANDLE hSnapShotHandle
		{
			CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId)
		};

		if (!IsValidHandle(hSnapShotHandle))
		{
			return moduleBaseAddress;
		}

		MODULEENTRY32 modEntry{};
		modEntry.dwSize = sizeof(MODULEENTRY32);

		if (!Module32First(hSnapShotHandle, &modEntry))
		{
			CloseHandle(hSnapShotHandle);
			return moduleBaseAddress;
		}

		do
		{
			if (!_wcsicmp(modEntry.szModule, modName))
			{
				CloseHandle(hSnapShotHandle);
				return reinterpret_cast<DWORD>(modEntry.modBaseAddr);
			}
		} while (Module32Next(hSnapShotHandle, &modEntry));

		CloseHandle(hSnapShotHandle);
		return moduleBaseAddress;
	}
}