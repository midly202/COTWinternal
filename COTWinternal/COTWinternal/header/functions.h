#include <iostream>
#include <Windows.h>
#include <Psapi.h>

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

uintptr_t FindPattern(char* module, char* pattern, char* mask)
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

// Function to check if it's safe to uninject
bool CanUninject(bool thread1Running, bool thread2Running)
{
	if (thread1Running || thread2Running)
		return false; // Returns false if either thread is still running
	else 
		return true; // Returns true if both threads have exited
}