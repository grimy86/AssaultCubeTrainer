#pragma once
#include <Windows.h>
#include <string>

namespace Memory
{
	class ExtProc
	{
	private:
		std::string _procName{};
		DWORD _procId{};
		HANDLE _procHandle{};

	public:
		static DWORD GetProcId(const std::string& procName);
		uintptr_t GetModuleBaseAddress(const std::string& moduleName) const;

		ExtProc(const std::string& procName);
		ExtProc(const ExtProc&) = delete;				//Removes copy constructor
		ExtProc& operator=(const ExtProc&) = delete;	//Removes copy assignment
		~ExtProc();

		template <typename T>
		T ReadProcMem(const uintptr_t readAddress) const
		{
			T returnValue{};

			ReadProcessMemory(
				_ProcHandle,
				reinterpret_cast<LPCVOID>(readAddress),
				reinterpret_cast<LPVOID>(&returnValue),
				sizeof(returnValue),
				nullptr
			));
		}

		template <typename T>
		bool WriteProcMem(const uintptr_t writeAddress, const T& writeValue) const
		{
			return static_cast<bool>(WriteProcessMemory(
				_ProcHandle,
				reinterpret_cast<LPVOID>(writeAddress),
				reinterpret_cast<LPCVOID>(&writeValue),
				sizeof(writeValue),
				nullptr
				));
		}
	};
}


