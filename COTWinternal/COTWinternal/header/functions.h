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
	std::cout << "[1] Stat Hack [" << (statHack ? "ON" : "OFF") << "]\n";
	std::cout << "[2] Infinite Ammo [" << (infiniteAmmo ? "ON" : "OFF") << "]\n";
	const char* timeMode;
	switch (timeChanger)
	{
	case 0: timeMode = "NORMAL"; break;
	case 1: timeMode = "FROZEN"; break;
	case 2: timeMode = "FAST FORWARD"; break;
	default: timeMode = "unknown"; break;
	}
	std::cout << "[3] Time Progression [" << timeMode << "]\n";
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