#pragma once
#include <Windows.h>

namespace Utility
{
	BOOL IsValidHandle(HANDLE handle);
	DWORD GetProcessId(const WCHAR* procName);
	DWORD GetModuleBaseAddress(const WCHAR* modName, const DWORD procId);
}
