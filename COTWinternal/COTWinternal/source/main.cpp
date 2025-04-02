// DWORD*       =	4 byte
// uint32_t*    =	4 bytes
// uintptr_t*   =	8 bytes
// float*       =   float

// Pointer Cast (wrong)
//uintptr_t* pBaseAddress = (uintptr_t*)GetModuleHandleA("TheHunterCotW_F.exe");

// Value Cast (correct)
//uintptr_t baseAddress = (uintptr_t)GetModuleHandleA("TheHunterCotW_F.exe");

// if ejecting the thread from within the same thread doesn't work do it in a seperate thread.
/*
void __stdcall EjectThread(LPVOID lpParameter)
{
    Sleep(100);
	FreeLibraryAndExitThread(static_cast<HMODULE>(lpParameter), 0);
}
*/

#define WIN_32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <cstdint>
#include <thread>
#include "../header/functions.h"
#include "MinHook.h"

#if _WIN64
#pragma comment(lib, "libMinHook.x64.lib")
#else
#pragma comment(lib, "libMinHook.x86.lib")
#endif

namespace offset
{
    constexpr uintptr_t level = 0x260; // wrong 
    constexpr uintptr_t xp = 0x2D0; // correct but verify if xp is even relevant
    constexpr uintptr_t skillPoints = 0x2D4;
    constexpr uintptr_t perkPoints = 0x2D8;
    constexpr uintptr_t cash = 0x360;
    constexpr uintptr_t rifleScore = 0x378;
    constexpr uintptr_t handgunScore = 0x37C;
    constexpr uintptr_t shotgunScore = 0x380;
    constexpr uintptr_t archeryScore = 0x384;
	constexpr float worldTime = 0xE8; // different base 
}

// global flags
bool hooks = false;
bool statHack = false;
bool cheat2 = false;
bool thread1Running = true;
bool thread2Running = true;
bool shouldExit = false;

void InitiateHooks(HMODULE instance) noexcept
{
    // Allocates console for debugging
    AllocConsole();
    FILE* f = nullptr;
    freopen_s(&f, "CONOUT$", "w", stdout);
    std::cout << "Injected! Debugging active...\n";

    // hook

    while (!GetAsyncKeyState(VK_NUMPAD0))
    {
		Sleep(10);
		std::cout << "Hook thread print...\n";
    }

    // cleanup
    thread1Running = false; // Mark this thread as finished

    // Only free the DLL if both threads are done
    while (GetAsyncKeyState(VK_NUMPAD0))
    {
        Sleep(10);
        if (CanUninject(thread1Running, thread2Running))
        {
            fclose(f);
            FreeConsole();
            FreeLibraryAndExitThread(instance, 0);
        }
        else
            std::cout << "Cannot uninject yet! Threads still running.\n";
    }
}

void StatHack(HMODULE instance) noexcept
{
    // module base
    uintptr_t baseAddress = (uintptr_t)GetModuleHandleA("TheHunterCotW_F.exe");

    //player base
    uintptr_t* playerBasePtr = reinterpret_cast<uintptr_t*>(baseAddress + 0x0234E8A0);
    if (!playerBasePtr || !*playerBasePtr) return;

    uintptr_t playerBase = *playerBasePtr;
    if (!playerBase) return;

    playerBase = *(uintptr_t*)(playerBase + 0x198);
    if (!playerBase) return;

    playerBase = *(uintptr_t*)(playerBase + 0x48);
    if (!playerBase) return;

    std::cout << "Module Base Found at: 0x" << std::hex << baseAddress << "\n";
    std::cout << "Player Base Found at: 0x" << std::hex << playerBase << "\n";

    // Pointers to values
    uint32_t* playerCash = reinterpret_cast<uint32_t*>(playerBase + offset::cash);
    uint32_t* playerXP = reinterpret_cast<uint32_t*>(playerBase + offset::xp);
    uint32_t* playerLevel = reinterpret_cast<uint32_t*>(playerBase + offset::level);
    uint32_t* rifleScore = reinterpret_cast<uint32_t*>(playerBase + offset::rifleScore);
    uint32_t* handgunScore = reinterpret_cast<uint32_t*>(playerBase + offset::handgunScore);
    uint32_t* shotgunScore = reinterpret_cast<uint32_t*>(playerBase + offset::shotgunScore);
    uint32_t* archeryScore = reinterpret_cast<uint32_t*>(playerBase + offset::archeryScore);
    uint32_t* skillPoints = reinterpret_cast<uint32_t*>(playerBase + offset::skillPoints);
    uint32_t* perkPoints = reinterpret_cast<uint32_t*>(playerBase + offset::perkPoints);

    int count = 1;
    while (!GetAsyncKeyState(VK_NUMPAD0))
    {
        Sleep(10);

        __try
        {
            if (playerCash && playerXP && rifleScore && handgunScore && shotgunScore && archeryScore && playerLevel && skillPoints && perkPoints && offset::cash && offset::xp && offset::rifleScore && offset::handgunScore && offset::shotgunScore && offset::archeryScore && offset::level && offset::skillPoints && offset::perkPoints)
            {
                if (statHack)
                {
                    *playerCash = 999999999;
                    *playerXP = 999999999;
                    *rifleScore = 999999999;
                    *handgunScore = 999999999;
                    *shotgunScore = 999999999;
                    *archeryScore = 999999999;
                    *playerLevel = 60;
                    *skillPoints = 999999999;
                    *perkPoints = 999999999;

					std::cout << "Maxed out stats and money!\n";
                    statHack = false;
				}
                else
                {
					while (GetAsyncKeyState(VK_NUMPAD1))
					{
                        statHack = true;
					}
                }
            }
            else
            {
                std::cout << "Invalid pointer detected!\n";
            }
        }
        __except (EXCEPTION_EXECUTE_HANDLER)
        {
            std::cout << "Dereference failed " << count << "\n";
            count++;
        }
    }

	// cleanup
    thread2Running = false; // Mark this thread as finished
}

int __stdcall DllMain(HMODULE instance, std::uintptr_t reason, const void* reserved)
{
    if (reason == 1) // DLL_PROCESS_ATTACH
    {
        DisableThreadLibraryCalls(instance);
        const auto thread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(InitiateHooks), instance, 0, nullptr);
        if (thread) CloseHandle(thread);

        const auto thread2 = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(StatHack), instance, 0, nullptr);
        if (thread2) CloseHandle(thread2);
    }
    
    return TRUE;
}
