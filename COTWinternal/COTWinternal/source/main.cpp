// uint32_t*    =	4 bytes
// uintptr_t*   =	8 bytes
// float*       =   float

// Pointer Cast (wrong, according to chatgpt)
//uintptr_t* pBaseAddress = (uintptr_t*)GetModuleHandleA("TheHunterCotW_F.exe");

// Value Cast (correct, according to chatgpt)
//uintptr_t baseAddress = (uintptr_t)GetModuleHandleA("TheHunterCotW_F.exe");

/* do NOT use this, IsBadReadPtr is unreliable and deprecated
    __try
    {
        if (IsBadReadPtr(xCoord, sizeof(float)) ||
            IsBadReadPtr(yCoord, sizeof(float)) ||
            IsBadReadPtr(zCoord, sizeof(float)))
        {
            std::cout << "Memory read failed (Invalid Pointer)\n";
        }
        else
        {
            std::cout << "X: " << *xCoord << " | Y: " << *yCoord << " | Z: " << *zCoord << "\n";
        }
*/

#define WIN_32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <cstdint>
#include <thread>

namespace offset
{
    constexpr uintptr_t cash = 0x360;
    constexpr uintptr_t xp = 0x2D0;
    constexpr uintptr_t level = 0x260;
}

bool infiniteMoney = false;
bool instantLevel60 = false;

void toggleCheats()
{
    if (GetAsyncKeyState(VK_F1) & 1) infiniteMoney = !infiniteMoney;
    if (GetAsyncKeyState(VK_F2) & 1) instantLevel60 = !instantLevel60;
}

void injected_thread(HMODULE instance) noexcept
{
    // Allocates console for debugging
    AllocConsole();
    FILE* f = nullptr;
    freopen_s(&f, "CONOUT$", "w", stdout);

    std::cout << "Injected! Debugging active...\n";

    // Get base address
    uintptr_t baseAddress = (uintptr_t)GetModuleHandleA("TheHunterCotW_F.exe");

    // Resolve player base dynamically
    uintptr_t* playerBasePtr = reinterpret_cast<uintptr_t*>(baseAddress + 0x024B2F20);
    if (!playerBasePtr || !*playerBasePtr) return;

    uintptr_t playerBase = *playerBasePtr;
    if (!playerBase) return;

    playerBase = *(uintptr_t*)(playerBase + 0x18);
    if (!playerBase) return;

    playerBase = *(uintptr_t*)(playerBase + 0x38);
    if (!playerBase) return;


    std::cout << "Player Base Found at: 0x" << std::dec << playerBase << "\n";

    // Pointers to values
    uint32_t* playerCash = reinterpret_cast<uint32_t*>(playerBase + offset::cash);
    uint32_t* playerXP = reinterpret_cast<uint32_t*>(playerBase + offset::xp);
    uint32_t* playerLevel = reinterpret_cast<uint32_t*>(playerBase + offset::level);

    int count = 1;
    while (!GetAsyncKeyState(VK_INSERT))
    {
        Sleep(10);
        toggleCheats();

        __try
        {
            if (playerCash && playerXP && playerLevel)
            {
                if (infiniteMoney) *playerCash = 999999999;
                if (instantLevel60) *playerXP = 999999999;

                std::cout << "Cash: " << std::dec << *playerCash << " | XP: " << *playerXP << " | Level: " << *playerLevel << "\n";
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

    // Cleanup
    fclose(f);
    FreeConsole();
    FreeLibraryAndExitThread(instance, 0);
}

int __stdcall DllMain(HMODULE instance, std::uintptr_t reason, const void* reserved)
{
    if (reason == 1) // DLL_PROCESS_ATTACH
    {
        DisableThreadLibraryCalls(instance);
        const auto thread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(injected_thread), instance, 0, nullptr);
        if (thread) CloseHandle(thread);
    }
    return TRUE;
}
