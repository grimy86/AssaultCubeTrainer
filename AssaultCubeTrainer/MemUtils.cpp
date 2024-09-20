#include "MemUtils.h"

namespace MemUtils
{
	bool IsValidHandle(const HANDLE handle)
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
