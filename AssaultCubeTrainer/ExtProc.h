#pragma once
#include <Windows.h>
#include <string>

	class ExtProc
	{
		//1 process
		//n amount of modules
		//n amount of threads

	private:
		std::string _processName{};
		DWORD _processId{};
		HANDLE _processHandle{};

	public:
#pragma region Properties
		inline std::string GetProcessName() const { return _processName; }
		inline DWORD GetProcessId() const { return _processId; }
		inline HANDLE GetProcessHandle() const { return _processHandle; }
#pragma endregion
#pragma region Constructor
		ExtProc(const std::string& processName);
		ExtProc(const ExtProc&) = delete;				//Removes copy constructor
		ExtProc& operator=(const ExtProc&) = delete;	//Removes copy assignment
		~ExtProc();
#pragma endregion
#pragma region Functions
		static DWORD GetProcessId(const std::string& processName);

		uintptr_t GetModuleBaseAddress(const std::string& moduleName) const;
#pragma endregion
#pragma region Templates
		template <typename T>
		T RPM(const uintptr_t readAddress) const
		{
			T returnValue{};

			ReadProcessMemory(
				_processHandle,
				reinterpret_cast<LPCVOID>(readAddress),
				reinterpret_cast<LPVOID>(&returnValue),
				sizeof(returnValue),
				nullptr
			);

			return returnValue;
		}

		template <typename T>
		bool WPM(const uintptr_t writeAddress, const T& writeValue) const
		{
			return static_cast<bool>(WriteProcessMemory(
				_processHandle,
				reinterpret_cast<LPVOID>(writeAddress),
				reinterpret_cast<LPCVOID>(&writeValue),
				sizeof(writeValue),
				nullptr
			));
		}
#pragma endregion
	};
