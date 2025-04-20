#include <iostream>
#include <Psapi.h>
#include <Windows.h>
#include <MinHook.h>
#include "../header/offsets.h"

extern uintptr_t baseAddress;
extern CPlayerInformation* player;
extern CCharacter* character;
extern CWorldTime* worldTime;
extern bool statHackEnabled;
extern bool infiniteAmmoEnabled;
extern int timeChangerEnabled;

// ANSI codes
// Standard colors
const std::string RESET = "\033[0m";
const std::string BLACK = "\033[30m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN = "\033[36m";
const std::string WHITE = "\033[37m";

// Bright colors
const std::string BRIGHT_BLACK = "\033[90m";
const std::string BRIGHT_RED = "\033[91m";
const std::string BRIGHT_GREEN = "\033[92m";
const std::string BRIGHT_YELLOW = "\033[93m";
const std::string BRIGHT_BLUE = "\033[94m";
const std::string BRIGHT_MAGENTA = "\033[95m";
const std::string BRIGHT_CYAN = "\033[96m";
const std::string BRIGHT_WHITE = "\033[97m";

// Backgrounds (standard)
const std::string BG_BLACK = "\033[40m";
const std::string BG_RED = "\033[41m";
const std::string BG_GREEN = "\033[42m";
const std::string BG_YELLOW = "\033[43m";
const std::string BG_BLUE = "\033[44m";
const std::string BG_MAGENTA = "\033[45m";
const std::string BG_CYAN = "\033[46m";
const std::string BG_WHITE = "\033[47m";

// Backgrounds (bright)
const std::string BG_BRIGHT_BLACK = "\033[100m";
const std::string BG_BRIGHT_RED = "\033[101m";
const std::string BG_BRIGHT_GREEN = "\033[102m";
const std::string BG_BRIGHT_YELLOW = "\033[103m";
const std::string BG_BRIGHT_BLUE = "\033[104m";
const std::string BG_BRIGHT_MAGENTA = "\033[105m";
const std::string BG_BRIGHT_CYAN = "\033[106m";
const std::string BG_BRIGHT_WHITE = "\033[107m";

// Text effects
const std::string BOLD = "\033[1m";
const std::string UNDERLINE = "\033[4m";
const std::string BLINK = "\033[5m";
const std::string INVERT = "\033[7m";
const std::string HIDDEN = "\033[8m";

// 256-color mode (text)
const std::string ORANGE_256 = "\033[38;5;208m";
const std::string PINK_256 = "\033[38;5;200m";
const std::string TEAL_256 = "\033[38;5;37m";
const std::string GREY_256 = "\033[38;5;244m";
const std::string PURPLE_256 = "\033[38;5;129m";
const std::string NEON_GREEN_256 = "\033[38;5;118m";
const std::string SKY_BLUE_256 = "\033[38;5;75m";

// RGB True Color (foreground)
const std::string RGB_PURPLE = "\033[38;2;204;153;255m";
const std::string RGB_PINK = "\033[38;2;255;105;180m";
const std::string RGB_ORANGE = "\033[38;2;255;165;0m";
const std::string RGB_SKY = "\033[38;2;135;206;250m";
const std::string RGB_AQUA = "\033[38;2;0;255;255m";
const std::string RGB_LIME = "\033[38;2;50;205;50m";
const std::string RGB_GOLD = "\033[38;2;255;215;0m";
const std::string RGB_CORAL = "\033[38;2;255;127;80m";
const std::string RGB_INDIGO = "\033[38;2;75;0;130m";
const std::string RGB_MINT = "\033[38;2;152;255;152m";

void MsgBoxAddy(uintptr_t addy)
{
	char szAddressOnly[32];
	sprintf_s(szAddressOnly, "%016llX", addy);

	const size_t len = strlen(szAddressOnly) + 1;
	HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
	if (hMem)
	{
		memcpy(GlobalLock(hMem), szAddressOnly, len);
		GlobalUnlock(hMem);
		OpenClipboard(0);
		EmptyClipboard();
		SetClipboardData(CF_TEXT, hMem);
		CloseClipboard();
	}

	char szFullMsg[64];
	sprintf_s(szFullMsg, "Address copied: %s", szAddressOnly);
	MessageBoxA(NULL, szFullMsg, "Copied!", MB_OK);
}

MODULEINFO GetModuleInfo(const char* szModule)
{
	MODULEINFO modInfo = { 0 };
	HMODULE hModule = GetModuleHandle(szModule);
	if (hModule == 0)
		return modInfo;
	GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(MODULEINFO));
	return modInfo;
}

void WriteToMemory(uintptr_t addressToWrite, char* valueToWrite, int byteNum)
{
	unsigned long oldProtection;
	VirtualProtect((LPVOID)addressToWrite, byteNum, PAGE_EXECUTE_READWRITE, &oldProtection);
	memcpy((LPVOID)addressToWrite, valueToWrite, byteNum);
	unsigned long temp;
	VirtualProtect((LPVOID)addressToWrite, byteNum, oldProtection, &temp);
}

uintptr_t FindPattern(const char* module, const char* pattern, const char* mask)
{
	MODULEINFO moduleInfo = GetModuleInfo(module);
	uintptr_t baseAddress = (uintptr_t)moduleInfo.lpBaseOfDll;
	uintptr_t moduleSize = (uintptr_t)moduleInfo.SizeOfImage;
	uintptr_t patternLength = (uintptr_t)strlen(mask);
	for (uintptr_t i = 0; i < moduleSize - patternLength; i++)
	{
		bool found = true;
		for (uintptr_t j = 0; j < patternLength; j++)
		{
			found &= mask[j] == '?' || pattern[j] == *(char*)(baseAddress + i + j);
		}
		if (found)
		{
			return baseAddress + i;
		}
	}
	return NULL;
}

void PlaceJMP(BYTE* address, uintptr_t jumpTo, uintptr_t length)
{
	DWORD dwOldProtect, dwBkup;
	uintptr_t dwRelAddr;
	VirtualProtect(address, length, PAGE_EXECUTE_READWRITE, &dwOldProtect);
	dwRelAddr = (uintptr_t)(jumpTo - (uintptr_t)address) - 5;
	*address = 0xE9;
	*((uintptr_t*)(address + 0x1)) = dwRelAddr;

	for (uintptr_t x = 0x5; x < length; x++)
	{
		*(address + x) = 0x90;
	}

	VirtualProtect(address, length, dwOldProtect, &dwBkup);
}

void showMenu(bool statHack, bool infiniteAmmo, int timeChanger)
{

	std::cout << RGB_PURPLE + BOLD << R"(
+-----------------------------------------+
|     /\___/\                             |
|    ( o   o )   Injected!                |
|    (  =^=  )                            |
|    (        )                           |
|    (         )   Let's hunt.            |
|    (          )))))))))))))))           |
+-----------------------------------------+
)" << RESET << "\n";

	std::cout << BLINK + "[0] Uninject" + RESET << "\n\n";
	std::cout << "[1] Stat Hack ["
		<< (statHack ? GREEN + "ON" + RESET : RED + "OFF" + RESET)
		<< "]\n";

	std::cout << "[2] Infinite Ammo ["
		<< (infiniteAmmo ? GREEN + "ON" + RESET : RED + "OFF" + RESET)
		<< "]\n";

	std::cout << "[3] Time Changer [";

	switch (timeChanger)
	{
	case 0: std::cout << RED + std::string("Normal") + RESET; break;
	case 1: std::cout << RGB_ORANGE + std::string("Frozen") + RESET; break;
	case 2: std::cout << GREEN + std::string("Fast Forward") + RESET; break;
	default: std::cout << WHITE + std::string("Unknown") + RESET; break;
	}

	std::cout << "]\n";
}

void WaitForKeyRelease(int vkKey)
{
	while (GetAsyncKeyState(vkKey)) Sleep(10);
}

bool CanUninject(bool thread1Running, bool thread2Running, bool thread3Running, bool thread4Running)
{
	if (thread1Running || thread2Running || thread3Running || thread4Running)
		return false; // Returns false if either thread is still running
	else 
		return true; // Returns true if both threads have exited
}

void EjectHook()
{
	MH_DisableHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}

void ToggleStatHack()
{
	statHackEnabled = !statHackEnabled;
	WaitForKeyRelease(VK_NUMPAD1);
	system("cls");
	showMenu(statHackEnabled, infiniteAmmoEnabled, timeChangerEnabled);
}

void MaintainStatHack()
{
	while (statHackEnabled && !GetAsyncKeyState(VK_NUMPAD0))
	{
		uintptr_t playerBase = *reinterpret_cast<uintptr_t*>(baseAddress + offset::playerBase);
		if (!playerBase) return;

		playerBase = *reinterpret_cast<uintptr_t*>(playerBase + 0x198);
		if (!playerBase) return;

		playerBase = *reinterpret_cast<uintptr_t*>(playerBase + 0x48);
		if (!playerBase) return;

		CPlayerInformation* player = reinterpret_cast<CPlayerInformation*>(playerBase);
		player->money = 999999999;
		player->level = 60;
		player->xp = 999999999;
		player->archeryScore = 999999999;
		player->shotgunScore = 999999999;
		player->rifleScore = 999999999;
		player->handgunScore = 999999999;
		player->perkpoints = 999999999;
		player->skillpoints = 999999999;

		Sleep(10);

		if (GetAsyncKeyState(VK_NUMPAD1) & 0x8000)
		{
			statHackEnabled = false;
			WaitForKeyRelease(VK_NUMPAD1);
			system("cls");
			showMenu(statHackEnabled, infiniteAmmoEnabled, timeChangerEnabled);
			break;
		}
	}
}

void MaintainTimeChanger()
{
	while (!GetAsyncKeyState(VK_NUMPAD0))
	{
		uintptr_t worldTimeBase = *reinterpret_cast<uintptr_t*>(baseAddress + offset::worldTimeBase);
		if (!worldTimeBase) return;

		CWorldTime* worldTime = reinterpret_cast<CWorldTime*>(worldTimeBase);

		if (timeChangerEnabled == 0 && worldTime->timeMultiplier != 1)
		{
			worldTime->timeMultiplier = 1;
		}
		else if (timeChangerEnabled == 1 && worldTime->timeMultiplier != 0)
		{
			worldTime->timeMultiplier = 0;
		}
		else if (timeChangerEnabled == 2 && worldTime->timeMultiplier != 1000)
		{
			worldTime->timeMultiplier = 1000;
		}

		Sleep(10);

		if (GetAsyncKeyState(VK_NUMPAD3) & 0x8000)
		{
			timeChangerEnabled++;
			if (timeChangerEnabled > 2)
				timeChangerEnabled = 0;
			WaitForKeyRelease(VK_NUMPAD3);
			system("cls");
			showMenu(statHackEnabled, infiniteAmmoEnabled, timeChangerEnabled);
		}
	}
}