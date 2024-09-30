#include "pch.h"
#include "Thirdparty/xorstr.h"

#include "CustomSDK/InterfaceManager/CInterface.h"
#include "CustomSDK/HookManager/HookManager.h"
#include "CustomSDK/Memory/CMemory.h"


/*

TO-DO:
- Add KeyValue Class
- Create own material
- color chams
- screenshot / stream proof chams
- EnginePrediction stuff
- Backtrack stuff :(

*/

// Define CreateThread function prototype
using customCreateThread = HANDLE(NTAPI*)(
    LPSECURITY_ATTRIBUTES   lpThreadAttributes,
    SIZE_T                  dwStackSize,
    LPTHREAD_START_ROUTINE  lpStartAddress,
    __drv_aliasesMem LPVOID lpParameter,
    DWORD                   dwCreationFlags,
    LPDWORD                 lpThreadId
    );

DWORD WINAPI mainRoutine(HMODULE hModule)
{
    AllocConsole();
    FILE* f;
    freopen_s(&f, xorstr("CONOUT$"), xorstr("w"), stdout);

    g_Interface->Initialize();
    HookManager::Initialize();

    std::cout << xorstr(" ////////////////////////////////////////////////////////////////////////// ") << std::endl;
    std::cout << xorstr("  GGGGG  EEEEE   OOOOO  PPPPPP   OOOOO  L      Y   Y  M   M  EEEEE  RRRRR  ") << std::endl;
    std::cout << xorstr(" G       E      O     O P     P O     O L       Y Y   MM MM  E      R    R ") << std::endl;
    std::cout << xorstr(" G  GGG  EEEE   O     O PPPPPP  O     O L        Y    M M M  EEEE   RRRRR  ") << std::endl;
    std::cout << xorstr(" G    G  E      O     O P       O     O L        Y    M   M  E      R  R   ") << std::endl;
    std::cout << xorstr("  GGGGG  EEEEE   OOOOO  P        OOOOO  LLLLL    Y    M   M  EEEEE  R   R  ") << std::endl;
    std::cout << xorstr(" ////////////////////////////////////////////////////////////////////////// ") << std::endl;
    std::cout << xorstr("[Geopolymer] Garry's Mod Internal Cheat x86_64 started.") << std::endl;

    while (!(GetAsyncKeyState(VK_NUMPAD2) & 1))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    HookManager::Destroy();

    if (f) { fclose(f); }
    FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);

    return NULL;
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        // Resolve CreateThread address by hash
        PDWORD functionAddress = g_Memory->GetFunctionAddressByHash(xorstr("kernel32"), 0x00544e304);

        // Point CreateThread function pointer to the CreateThread virtual address resolved by its hash
        auto CreateCustomThread = reinterpret_cast<customCreateThread>(functionAddress);

        const auto hdl = CreateCustomThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(mainRoutine), hModule, 0, nullptr);
        if (hdl)
            CloseHandle(hdl);
    }
    break;
    }
    return TRUE;
}