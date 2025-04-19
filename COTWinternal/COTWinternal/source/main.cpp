#define WIN_32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <cstdint>
#include <thread>
#include "../header/functions.h"
#include "../header/hooks.h"
#include "MinHook.h"

#if _WIN64
#pragma comment(lib, "libMinHook.x64.lib")
#else
#pragma comment(lib, "libMinHook.x86.lib")
#endif

// global flag
uintptr_t baseAddress = (uintptr_t)GetModuleHandleA("TheHunterCotW_F.exe");
CPlayerInformation* player = NULL;
CCharacter* character = NULL;
CWorldTime* worldTime = NULL;

uintptr_t clipFunc = baseAddress + 0x76CB01;
char clipOriginalBytes[4];
char clipOpCode[] = { 0x90, 0x90, 0x90, 0x90 };

uintptr_t clipShotgunFunc = baseAddress + 0x76CAF0;
char clipShotgunOriginalBytes[4];
char clipShotgunOpCode[] = { 0x90, 0x90, 0x90, 0x90 };

uintptr_t ammoFunc = baseAddress + 0xA1698D;
char ammoOriginalBytes[7];
char ammoOpCode[] = { 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90 };

uintptr_t coordAddy;

bool statHackEnabled = false;
bool infiniteAmmoEnabled = false;
int timeChangerEnabled = 0;

bool thread1Running = true;
bool thread2Running = true;
bool thread3Running = true;
bool thread4Running = true;

void Initialization(HMODULE instance) noexcept
{
    AllocConsole();
    FILE* f = nullptr;
    freopen_s(&f, "CONOUT$", "w", stdout);
    memcpy(clipOriginalBytes, (void*)clipFunc, 4);
    memcpy(ammoOriginalBytes, (void*)ammoFunc, 7);

    uintptr_t coordAddy = FindPattern("theHunterCotW_F.exe", "\x0F\x28\x45\xD0\x0F\x29\x06", "xxxxxxx");
    coordAddy += 0x7;
    MsgBoxAddy(coordAddy);
    system("cls");
    showMenu(statHackEnabled, infiniteAmmoEnabled, timeChangerEnabled);

    // wait for uninjection process
    while (!GetAsyncKeyState(VK_NUMPAD0))
    {
        Sleep(10);
    }

    // cleanup
    Sleep(100);
    while (!CanUninject(thread1Running, thread2Running, thread3Running, thread4Running))
    {
        std::cout << "Cannot uninject yet! Threads still running.\n";
        Sleep(100);
    }

    if (f) fclose(f);
    FreeConsole();
    EjectHook();
    FreeLibraryAndExitThread(instance, 0);
}

void OverwriteOpcodes(HMODULE instance) noexcept
{
	while (!GetAsyncKeyState(VK_NUMPAD0))
	{
        // infinite ammo
        if (GetAsyncKeyState(VK_NUMPAD2) & 1)
        {
            infiniteAmmoEnabled = !infiniteAmmoEnabled;

            if (infiniteAmmoEnabled)
            {
                WriteToMemory(clipFunc, clipOpCode, 4);
                WriteToMemory(clipShotgunFunc, clipShotgunOpCode, 4);
                WriteToMemory(ammoFunc, ammoOpCode, 7);
                system("cls");
                showMenu(statHackEnabled, infiniteAmmoEnabled, timeChangerEnabled);
            }
            else
            {
                WriteToMemory(clipFunc, clipOriginalBytes, 4);
                WriteToMemory(clipShotgunFunc, clipShotgunOriginalBytes, 4);
                WriteToMemory(ammoFunc, ammoOriginalBytes, 7);
                system("cls");
                showMenu(statHackEnabled, infiniteAmmoEnabled, timeChangerEnabled);
            }
        }

        Sleep(10);
	}

    thread1Running = false;
}

void StatHack(HMODULE instance) noexcept
{
    while (!GetAsyncKeyState(VK_NUMPAD0))
    {
        Sleep(5);
        if (GetAsyncKeyState(VK_NUMPAD1) & 1)
        {
            ToggleStatHack();
            if (statHackEnabled)
                MaintainStatHack();
        }
    }

    thread2Running = false;
}

void TimeChanger(HMODULE instance) noexcept
{
    while (!GetAsyncKeyState(VK_NUMPAD0))
    {
        Sleep(5);
        if (GetAsyncKeyState(VK_NUMPAD3) & 1)
        {
            MaintainTimeChanger();
        }
    }

    thread3Running = false;
}

void testthread(HMODULE instance) noexcept
{
    while (!GetAsyncKeyState(VK_NUMPAD0))
    {
        Sleep(5);
        if (GetAsyncKeyState(VK_NUMPAD4) & 1)
        {
            // code
        }
    }

    thread4Running = false;
}

int __stdcall DllMain(HMODULE instance, std::uintptr_t reason, const void* reserved)
{
	switch (reason)
	{
	case DLL_PROCESS_ATTACH:
		DisableThreadLibraryCalls(instance);
		const auto thread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(Initialization), instance, 0, nullptr);
		if (thread) CloseHandle(thread);
        const auto thread2 = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(OverwriteOpcodes), instance, 0, nullptr);
        if (thread2) CloseHandle(thread2);
        const auto thread3 = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(StatHack), instance, 0, nullptr);
        if (thread3) CloseHandle(thread3);
        const auto thread4 = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(TimeChanger), instance, 0, nullptr);
        if (thread4) CloseHandle(thread4);
        const auto thread5 = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(testthread), instance, 0, nullptr);
        if (thread5) CloseHandle(thread5);
        break;
	}

    return TRUE;
}
