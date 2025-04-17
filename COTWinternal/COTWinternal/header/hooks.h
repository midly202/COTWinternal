#include <iostream>
#include <Psapi.h>
#include <Windows.h>


typedef void(__fastcall* tOriginalFunc)(void* param);
tOriginalFunc oOriginalFunc = nullptr;

void __fastcall CoordHook(void* param) {

	std::cout << "Hook triggered!\n"; // game crashes after this line

    // Call the original function
    oOriginalFunc(param);
}
