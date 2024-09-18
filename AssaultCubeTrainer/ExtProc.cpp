#include "ExtProc.h"
#include <Windows.h>
#include <TlHelp32.h>

namespace Utils
{
	bool IsValidHandle(HANDLE handle)
	{
		return (handle != nullptr && handle != INVALID_HANDLE_VALUE);
	}

	std::wstring StringToWideString(const std::string& string)
	{
		return std::wstring{ string.begin(), string.end() };
	}

	std::string WideStringToString(const std::wstring& wstring)
	{
		return std::string{ wstring.begin(), wstring.end() };
	}


}

namespace Memory
{
	DWORD ExtProc::GetProcId(const std::string& procName)
	{
		DWORD procId{};

		HANDLE hSnapShotHandle
		{
			CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)
		};

		if (!Utils::IsValidHandle(hSnapShotHandle))
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
			if (!_wcsicmp(procEntry.szExeFile, Utils::StringToWideString(procName).c_str()))
			{
				CloseHandle(hSnapShotHandle);
				return procEntry.th32ProcessID;
			}
		} while (Process32Next(hSnapShotHandle, &procEntry));

		CloseHandle(hSnapShotHandle);
		return procId;
	}
	uintptr_t ExtProc::GetModuleBaseAddress(const std::string& moduleName) const
	{
		DWORD moduleBaseAddress{};

		HANDLE hSnapShotHandle
		{
			CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, _procId)
		};

		if (!Utils::IsValidHandle(hSnapShotHandle))
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
			if (!_wcsicmp(modEntry.szModule, Utils::StringToWideString(moduleName).c_str()))
			{
				CloseHandle(hSnapShotHandle);
				return reinterpret_cast<DWORD>(modEntry.modBaseAddr);
			}
		} while (Module32Next(hSnapShotHandle, &modEntry));

		CloseHandle(hSnapShotHandle);
		return moduleBaseAddress;
	}

	ExtProc::ExtProc(const std::string& procName)
		:_procName { procName }
	{
		_procId = GetProcId(_procName);
		_procHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _procId);
	}

	ExtProc::~ExtProc()
	{
		if (Utils::IsValidHandle(_procHandle))
		{
			CloseHandle(_procHandle);
		};
	}
	
}