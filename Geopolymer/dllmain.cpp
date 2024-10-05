#include "pch.h"
#include "Thirdparty/xorstr.h"
#include "CustomSDK/InterfaceManager/CInterface.h"
#include "CustomSDK/HookManager/HookManager.h"
#include "CustomSDK/Memory/CMemory.h"
#include "globals.h"

/*
TO-DO:
- screenshot / stream proof chams
- EnginePrediction stuff
- Backtrack stuff :(
- visible chams
*/

uintptr_t globals::Sig_KeyValue_LoadFromBuffer = 0;
uintptr_t globals::Sig_KeyValue_Intialize = 0;
uintptr_t globals::Sig_KeyValue_FindKey = 0;

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

    globals::Sig_KeyValue_LoadFromBuffer = g_PatternScan->InPatternScan(xorstr("55 8B EC 83 EC ? 53 8B 5D ? 89 4D ? 85 DB"), xorstr("engine.dll"));
    globals::Sig_KeyValue_Intialize = g_PatternScan->InPatternScan(xorstr("55 8B EC 56 8B F1 6A ? FF 75 ? C7 06 ? ? ? ? C7 46 ? ? ? ? ? C7 46 ? ? ? ? ? C7 46 ? ? ? ? ? C7 46 ? ? ? ? ? C7 46 ? ? ? ? ? C7 46 ? ? ? ? ? C7 46 ? ? ? ? ? FF 15 ? ? ? ? 83 C4 ? 89 06 8B C6"), xorstr("engine.dll"));
    globals::Sig_KeyValue_FindKey = g_PatternScan->InPatternScan(xorstr("55 8B EC 81 EC ? ? ? ? 56 8B 75 ? 57 8B F9 85 F6 0F 84"), xorstr("client.dll"));

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
        PDWORD functionAddress = g_Memory->GetFunctionAddressByHash((char*)"kernel32", 0x00544e304);

        // Point CreateThread function pointer to the CreateThread virtual address resolved by its hash
        customCreateThread CreateCustomThread = (customCreateThread)functionAddress;

        const HANDLE hdl = CreateCustomThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(mainRoutine), hModule, 0, nullptr);
        if (hdl)
            CloseHandle(hdl);
    }
    break;
    }
    return TRUE;
}