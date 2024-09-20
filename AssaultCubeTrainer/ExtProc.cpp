#include "ExtProc.h"
#include "MemUtils.h"
#include <Windows.h>
#include <TlHelp32.h>

	DWORD ExtProc::GetProcessId(const std::string& processName)
	{
		DWORD procId{};
		HANDLE hSnapShotHandle{ CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0) }; //Handle to the snapshot of all processes
		if (!MemUtils::IsValidHandle(hSnapShotHandle))
		{
			return procId;
		}

		PROCESSENTRY32 procEntry{}; //buffer for 1 process object
		procEntry.dwSize = sizeof(PROCESSENTRY32); //If you do not initialize dwSize, Process32First fails.
		if (!Process32First(hSnapShotHandle, &procEntry)) //If we didn't fill the buffer
		{
			CloseHandle(hSnapShotHandle);
			return procId; //return "0"
		}

		do //use tail-controlled loop to run once before the condition check
		{
			if (!_wcsicmp(procEntry.szExeFile, MemUtils::StringToWideString(processName).c_str())) //wchar string compare case incensitive
			{
				CloseHandle(hSnapShotHandle);
				return procEntry.th32ProcessID;
			}
		} while (Process32Next(hSnapShotHandle, &procEntry)); //Copy next process into procEntry

		CloseHandle(hSnapShotHandle);
		return procId; //return "0"
	}

	uintptr_t ExtProc::GetModuleBaseAddress(const std::string& moduleName) const
	{
		DWORD moduleBaseAddress{};
		HANDLE hSnapShotHandle{ CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, _processId) }; //Take a snapshot of 32 & 64-bit modules 
		if (!MemUtils::IsValidHandle(hSnapShotHandle))
		{
			return moduleBaseAddress; //return "0"
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
			if (!_wcsicmp(modEntry.szModule, MemUtils::StringToWideString(moduleName).c_str()))
			{
				CloseHandle(hSnapShotHandle);
				return reinterpret_cast<DWORD>(modEntry.modBaseAddr);
			}
		} while (Module32Next(hSnapShotHandle, &modEntry));

		CloseHandle(hSnapShotHandle);
		return moduleBaseAddress;
	}

	ExtProc::ExtProc(const std::string& processName)
		: _processName { processName } //initializer list
	{
		_processId = GetProcessId(processName);
		_processHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, _processId);
	}

	ExtProc::~ExtProc()
	{
		if (MemUtils::IsValidHandle(_processHandle))
		{
			CloseHandle(_processHandle);
		};
	}
