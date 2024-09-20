#include <iostream>
#include "Offsets.h"
#include "ExtProc.h"
#include "MemUtils.h"
#include "Aimbot.h"


int main()
{
    ExtProc assaultCube{"ac_client.exe"};

    DWORD processId{ assaultCube.GetProcessId() }; //assaultCube::GetProcessId("ac_client.exe")
    DWORD moduleBaseAddr{ assaultCube.GetModuleBaseAddress("ac_client.exe")}; //assaultCube::GetModuleBaseAddress("ac_client.exe")
    HANDLE processHandle{ OpenProcess(PROCESS_ALL_ACCESS, FALSE, processId) };

    if (processId == 0)
    {
        std::cout << "(!)  Couldn't resolve a valid processId." << '\n';
        return 1;
    }
    std::cout << "(+)  Resolved a valid processId: " << processId << '\n';

    if (moduleBaseAddr == 0)
    {
        std::cout << "(!)  Couldn't resolve a valid moduleBaseAddress." << '\n';
        return 1;
    }
    std::cout << std::hex << "(+)  Resolved a valid moduleBaseAddress: 0x" << moduleBaseAddr << '\n';

    if (!MemUtils::IsValidHandle(processHandle))
    {
        std::cout << "(!)  Couldn't open a handle to the process." << '\n';
        return 1;
    }
    std::cout << std::hex << "(+)  Opened a valid handle to the process: 0x" << processHandle << '\n';

    while (!(GetAsyncKeyState(VK_NUMLOCK) & 0b1))
    {
        Aimbot::DoAimbot(moduleBaseAddr, processHandle);
    }
    CloseHandle(processHandle);

    return 0;
}