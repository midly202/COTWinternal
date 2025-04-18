#include <Windows.h>
#include <vector>

namespace offset
{
	constexpr uintptr_t playerBase = 0x0234E8A0;
	constexpr uintptr_t worldTimeBase = 0x024B0BF0;
}

struct Vector2 
{
	float x, y;
};

struct Vector3 
{
	float x, y, z;
};

struct Vector4 
{
	float x, y, z, w;
};


// Created with ReClass.NET 1.2 by KN4CK3R

class CPlayerInformation
{
public:
	char pad_0000[608]; //0x0000
	int32_t level; //0x0260
	char pad_0264[108]; //0x0264
	int32_t xp; //0x02D0
	int32_t skillpoints; //0x02D4
	int32_t perkpoints; //0x02D8
	char pad_02DC[132]; //0x02DC
	int32_t money; //0x0360
	char pad_0364[20]; //0x0364
	int32_t rifleScore; //0x0378
	int32_t handgunScore; //0x037C
	int32_t shotgunScore; //0x0380
	int32_t archeryScore; //0x0384
}; //Size: 0x0388

class CWorldTime
{
public:
	char pad_0000[232]; //0x0000
	float time; //0x00E8
	char pad_00EC[4]; //0x00EC
	float timeMultiplier; //0x00F0
	float size; //0x00F4
	char pad_00F8[12]; //0x00F8
	float timer; //0x0104
}; //Size: 0x0108

class CCharacter
{
public:
	char pad_0000[896]; //0x0000
	Vector3 headPos; //0x0380
	char pad_038C[372]; //0x038C
	Vector3 bodyPos; //0x0500
	char pad_050C[8]; //0x050C
	Vector3 viewAngles; //0x0514
	char pad_0520[32]; //0x0520
	Vector3 velocityDirectionWhenMoving; //0x0540
	char pad_054C[20]; //0x054C
	Vector3 velocityDirectionWhenMoving2; //0x0560
	char pad_056C[4032]; //0x056C
}; //Size: 0x152C