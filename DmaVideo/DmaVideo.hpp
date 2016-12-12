#pragma once

#include <windows.h>

struct SVideo
{

};

struct SDevice
{

};

struct SDisplayMode
{

};

struct SVidVersion
{

};

using u16 = unsigned short int;
using s32 = signed int;
using f32 = float;
using u8 = unsigned char;

#define CC __stdcall

SVideo* CC Vid_Init_SYS(s32 param1, u16 param2_flags);
s32 CC Vid_CheckMode(SVideo* pVideoDriver, s32 width, s32 height, s32 rgbBitCount);
SDevice* CC Vid_FindDevice(SVideo* pVideoDriver, s32 deviceId);
SDisplayMode* CC Vid_FindMode(SVideo* pVideoDriver, s32 modeId);
s32 CC Vid_FindFirstMode(SVideo* pVideoDriver, s32 rgbBitCountFilter);
s32 CC Vid_FindNextMode(SVideo* pVideoDriver);
void CC Vid_CloseScreen(SVideo* pVideo);
s32 CC Vid_SetDevice(SVideo* pVideoDriver, s32 deviceId);
s32 CC Vid_SetMode(SVideo* pVideoDriver, HWND hWnd, s32 modeId);
void CC Vid_GrabSurface(SVideo* pVideoDriver);
void CC Vid_ReleaseSurface(SVideo* pVideoDriver);
void CC Vid_FlipBuffers(SVideo* pVideo);
void CC Vid_ShutDown_SYS(SVideo* pVideoDriver);
s32 CC Vid_EnableWrites(SVideo* pVideoDriver);
s32 CC Vid_DisableWrites(SVideo* pVideoDriver);
s32 CC Vid_GetSurface(SVideo* pVideoDriver);
s32 CC Vid_FreeSurface(SVideo* pVideoDriver);
s32 CC Vid_ClearScreen(SVideo* pVideoDriver, u8 aR, u8 aG, u8 aB, s32 aLeft, s32 aTop, s32 aRight, s32 aBottom);
s32 CC Vid_SetGamma(SVideo* pVideoDriver, f32 a2, f32 a3, f32 a4);
s32 CC Vid_WindowProc(SVideo* pVideoDriver, HWND hwnd, DWORD uMsg, WPARAM wParam, LPARAM lParam);
s32 CC Vid_InitDLL(HINSTANCE hInstance, s32 a2);
SVidVersion* CC Vid_GetVersion();
