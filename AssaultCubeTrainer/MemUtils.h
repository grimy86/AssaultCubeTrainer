#pragma once
#include <Windows.h>
#include <string>

namespace MemUtils
{
	bool IsValidHandle(const HANDLE handle);
	std::wstring StringToWideString(const std::string& string);
	std::string WideStringToString(const std::wstring& wstring);
}

