#include <Windows.h>
#include <vector>

extern uintptr_t coordAddy;

namespace offset
{
	constexpr uintptr_t playerBase = 0x0234E8A0;
	constexpr uintptr_t worldTimeBase = 0x024B0BF0;
	constexpr uintptr_t sysSettingsBase = 0x024E0BF8; // + 0xF08
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
	char pad_00F4[12]; //0x00F4
	float timer; //0x0100
	char pad_0104[40]; //0x0104
}; //Size: 0x012C

class CCharacter
{
public:
	Vector3 coords; //0x0000
	char pad_000C[8]; //0x000C
	Vector3 viewAngles; //0x0014
	char pad_0020[36]; //0x0020
	Vector3 velocityDirectionWhenMoving; //0x0044
	char pad_0050[20]; //0x0050
	Vector3 velocityDirectionWhenMoving2; //0x0064
}; //Size: 0x0070

class SSysSettings
{
public:
	char pad_0000[4]; //0x0000
	bool motionBlur; //0x0004
	char pad_0005[3]; //0x0005
	int8_t anisotropicFiltering; //0x0008
	char pad_0009[3]; //0x0009
	int8_t sceneComplexity; //0x000C
	char pad_000D[7]; //0x000D
	int8_t textureQuality; //0x0014
	char pad_0015[3]; //0x0015
	int8_t waterQuality; //0x0018
	char pad_0019[3]; //0x0019
	int8_t shadowQuality; //0x001C
	char pad_001D[3]; //0x001D
	bool screenSpaceAmbientOcclusion; //0x0020
	char pad_0021[7]; //0x0021
	bool globalIllumination; //0x0028
	char pad_0029[3]; //0x0029
	bool screenSpaceReflections; //0x002C
	char pad_002D[3]; //0x002D
	int8_t terrainDetailTessellation; //0x0030
	char pad_0031[3]; //0x0031
	int8_t volumeFogQuality; //0x0034
	char pad_0035[3]; //0x0035
	bool heatHaze; //0x0038
	char pad_0039[3]; //0x0039
	int8_t furQuality; //0x003C
	char pad_003D[23]; //0x003D
	bool vSync; //0x0054
	char pad_0055[19]; //0x0055
	int8_t controllerVibration; //0x0068
	char pad_0069[7]; //0x0069
	int8_t controllerInvert; //0x0070
	char pad_0071[11]; //0x0071
	int8_t controllerLookSens; //0x007C
	char pad_007D[3]; //0x007D
	int8_t controllerAimSens; //0x0080
	char pad_0081[27]; //0x0081
	bool mouseInvert; //0x009C
	char pad_009D[3]; //0x009D
	int8_t mouseLookSens; //0x00A0
	char pad_00A1[3]; //0x00A1
	int8_t mouseAimSens; //0x00A4
	char pad_00A5[15]; //0x00A5
	int8_t SFX; //0x00B4
	char pad_00B5[3]; //0x00B5
	int8_t music; //0x00B8
	char pad_00B9[3]; //0x00B9
	int8_t dialogue; //0x00BC
	char pad_00BD[3]; //0x00BD
	bool subtitles; //0x00C0
	char pad_00C1[7]; //0x00C1
	int8_t antiAliasing; //0x00C8
	char pad_00C9[7]; //0x00C9
	bool aimToggle; //0x00D0
	char pad_00D1[3]; //0x00D1
	bool manualReload; //0x00D4
	char pad_00D5[3]; //0x00D5
	bool tutsHints; //0x00D8
	char pad_00D9[3]; //0x00D9
	bool missionSystem; //0x00DC
	char pad_00DD[3]; //0x00DD
	int8_t FOV; //0x00E0
	char pad_00E1[3]; //0x00E1
	bool compassFree; //0x00E4
	char pad_00E5[3]; //0x00E5
	bool multiplayerNametags; //0x00E8
	char pad_00E9[3]; //0x00E9
	bool isImperial; //0x00EC
	char pad_00ED[3]; //0x00ED
	bool downedAnimalHighlight; //0x00F0
	char pad_00F1[3]; //0x00F1
	bool spottingOutline; //0x00F4
	char pad_00F5[3]; //0x00F5
	bool audioClueVis; //0x00F8
	char pad_00F9[3]; //0x00F9
	bool spatialTrackingCone; //0x00FC
	char pad_00FD[3]; //0x00FD
	bool POIicons; //0x0100
	char pad_0101[3]; //0x0101
	bool needZoneIcons; //0x0104
	char pad_0105[71]; //0x0105
	int8_t trackEffects; //0x014C
	char pad_014D[75]; //0x014D
	bool manualChambering; //0x0198
	char pad_0199[3]; //0x0199
	bool hideHuntingHud; //0x019C
	char pad_019D[91]; //0x019D
	int8_t inactiveTrackClueColor; //0x01F8
	char pad_01F9[3]; //0x01F9
	int8_t spottingOutlineColor; //0x01FC
	char pad_01FD[3]; //0x01FD
	int8_t activeTrackClueColor; //0x0200
	char pad_0201[3]; //0x0201
	int8_t collectibleOutlineColor; //0x0204
	char pad_0205[3]; //0x0205
	int8_t treestandOutlineColor; //0x0208
	char pad_0209[23]; //0x0209
	bool breathToggle; //0x0220
	char pad_0221[23]; //0x0221
	bool mouseSmoothing; //0x0238
	char pad_0239[3]; //0x0239
	int8_t controllerLeftStickDeadzone; //0x023C
	char pad_023D[3]; //0x023D
	int8_t controllerRightStickDeadzone; //0x0240
	char pad_0241[3]; //0x0241
	bool dogOutline; //0x0244
	char pad_0245[3]; //0x0245
	int8_t dogOutlineColor; //0x0248
	char pad_0249[3]; //0x0249
	bool dogPosIcon; //0x024C
	char pad_024D[23]; //0x024D
	int8_t coopOutlineColor; //0x0264
	char pad_0265[35]; //0x0265
	int8_t subtitleBackgroundOpacity; //0x0288
	char pad_0289[3]; //0x0289
	bool dolbyAtmos; //0x028C
	char pad_028D[3]; //0x028D
	int8_t reticleShape; //0x0290
	char pad_0291[3]; //0x0291
	int8_t reticleSize; //0x0294
	char pad_0295[3]; //0x0295
	int8_t reticleColor; //0x0298
	char pad_0299[3]; //0x0299
	int8_t amdFidelityFX; //0x029C
	char pad_029D[3]; //0x029D
	int8_t subtitleTextSize; //0x02A0
	char pad_02A1[23]; //0x02A1
	bool POIiconsMap; //0x02B8
	char pad_02B9[3]; //0x02B9
	int8_t reflexReticleShape; //0x02BC
	char pad_02BD[3]; //0x02BD
	int8_t reflexReticleSize; //0x02C0
	char pad_02C1[3]; //0x02C1
	int8_t reflexReticleColor; //0x02C4
	char pad_02C5[3]; //0x02C5
	int8_t reflexReticleOpacity; //0x02C8
	char pad_02C9[3]; //0x02C9
	int8_t reflexLensColor; //0x02CC
	char pad_02CD[3]; //0x02CD
	int8_t reflexLensOpacity; //0x02D0
	char pad_02D1[3]; //0x02D1
	int16_t mouseKeyHoldTime; //0x02D4
	char pad_02D6[2]; //0x02D6
	int16_t mouseDoublePressSpeed; //0x02D8
	char pad_02DA[2]; //0x02DA
	int16_t controllerButtonHoldTime; //0x02DC
	char pad_02DE[2]; //0x02DE
	int16_t controllerDoublePressSpeed; //0x02E0
	char pad_02E2[42]; //0x02E2
	bool depthOfField; //0x030C
	char pad_030D[3]; //0x030D
	bool hideAllAnimalClues; //0x0310
	char pad_0311[3]; //0x0311
	int8_t TAAsharpness; //0x0314
	char pad_0315[23]; //0x0315
	bool controlHints; //0x032C
	char pad_032D[3]; //0x032D
	bool screenSpaceContactShadow; //0x0330
	char pad_0331[3]; //0x0331
	int8_t remoteCaller; //0x0334
	char pad_0335[3]; //0x0335
	bool interactionHints; //0x0338
	char pad_0339[3135]; //0x0339
}; //Size: 0x0F78