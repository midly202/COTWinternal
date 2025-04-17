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

void MsgBoxAddy(uintptr_t addy)
{
	char szBuffer[1024];
	sprintf_s(szBuffer, "Address: %016llX", addy);
	MessageBoxA(NULL, szBuffer, "Address", MB_OK);
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

void showMenu(bool statHack, bool infiniteAmmo)
{
	std::cout << "[1] Stat Hack [" << (statHack ? "ON" : "OFF") << "]\n";
	std::cout << "[2] Infinite Ammo [" << (infiniteAmmo ? "ON" : "OFF") << "]\n";
}

void WaitForKeyRelease(int vkKey)
{
	while (GetAsyncKeyState(vkKey)) Sleep(10);
}

bool CanUninject(bool thread1Running, bool thread2Running)
{
	if (thread1Running || thread2Running)
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
	showMenu(statHackEnabled, infiniteAmmoEnabled);
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

		Sleep(5);

		if (GetAsyncKeyState(VK_NUMPAD1) & 1)
		{
			statHackEnabled = false;
			WaitForKeyRelease(VK_NUMPAD1);
			system("cls");
			showMenu(statHackEnabled, infiniteAmmoEnabled);
			break;
		}
	}
}