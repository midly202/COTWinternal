// uint32_t*    =	4 bytes
// uintptr_t*   =	8 bytes
// float*       =   float

// Pointer Cast (wrong)
//uintptr_t* pBaseAddress = (uintptr_t*)GetModuleHandleA("TheHunterCotW_F.exe");

// Value Cast (correct)
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

// Coordinates are stored in a vec3, I think the base of CCharacter is coords - 0x500;

#define WIN_32_LEAN_AND_MEAN
#include <iostream>
#include <Windows.h>
#include <cstdint>
#include <thread>

namespace offset
{
    constexpr uintptr_t level = 0x260;
    constexpr uintptr_t xp = 0x2D0;
    constexpr uintptr_t skillPoints = 0x2D4;
    constexpr uintptr_t perkPoints = 0x2D8;
    constexpr uintptr_t cash = 0x360;
    constexpr uintptr_t rifleScore = 0x378;
    constexpr uintptr_t handgunScore = 0x37C;
    constexpr uintptr_t shotgunScore = 0x380;
    constexpr uintptr_t archeryScore = 0x384;
	constexpr float worldTime = 0xE8; // different base address
}

bool infiniteMoney = false;
bool levelUp = false;

int counterCash = 0;
int counterXp = 0;

void toggleCheats()
{
    if (GetAsyncKeyState(VK_F1) & 1) infiniteMoney = !infiniteMoney;
    if (GetAsyncKeyState(VK_F2) & 1) levelUp = !levelUp;
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
    while (!GetAsyncKeyState(VK_INSERT))
    {
        Sleep(10);
        toggleCheats();

        __try
        {
            if (playerCash && playerXP && playerLevel && offset::rifleScore && offset::handgunScore && offset::shotgunScore && offset::archeryScore)
            {
                if (infiniteMoney)
                {
                    *playerCash = 999999999;

                    if (counterCash < 1)
                    {
                        std::cout << "Cash: " << std::dec << *playerCash << "\n";
                        counterCash++;
                    }
                }
                if (levelUp)
                {
                    *playerXP = 999999999;
                    *rifleScore = 999999999;
					*handgunScore = 999999999;
					*shotgunScore = 999999999;
					*archeryScore = 999999999;
					*playerLevel = 60;
                    *skillPoints = 999999999;
                    *perkPoints = 999999999;

                    if (counterXp < 1)
                    {
                        std::cout << "XP: " << *playerXP << "\n";
                        std::cout << "Level: " << *playerLevel << "\n";
                        std::cout << "Rifle Score: " << *rifleScore << "\n";
                        std::cout << "Handgun Score: " << *handgunScore << "\n";
                        std::cout << "Shotgun Score: " << *shotgunScore << "\n";
                        std::cout << "Archery Score: " << *archeryScore << "\n";
                        counterXp++;
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
