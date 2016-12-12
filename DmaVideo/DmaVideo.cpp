#include "stdafx.h"
#include "DmaVideo.hpp"

SVideo* CC Vid_Init_SYS(s32 param1, u16 param2_flags)
{
    return nullptr;
}

s32 CC Vid_CheckMode(SVideo* pVideoDriver, s32 width, s32 height, s32 rgbBitCount)
{
    return 0;
}

SDevice* CC Vid_FindDevice(SVideo* pVideoDriver, s32 deviceId)
{
    return nullptr;
}

SDisplayMode* CC Vid_FindMode(SVideo* pVideoDriver, s32 modeId)
{
    return 0;
}

s32 CC Vid_FindFirstMode(SVideo* pVideoDriver, s32 rgbBitCountFilter)
{
    return 0;
}

s32 CC Vid_FindNextMode(SVideo* pVideoDriver)
{
    return 0;
}

void CC Vid_CloseScreen(SVideo* pVideo)
{

}

s32 CC Vid_SetDevice(SVideo* pVideoDriver, s32 deviceId)
{
    return 0;
}

s32 CC Vid_SetMode(SVideo* pVideoDriver, HWND hWnd, s32 modeId)
{
    return 0;
}

void CC Vid_GrabSurface(SVideo* pVideoDriver)
{

}

void CC Vid_ReleaseSurface(SVideo* pVideoDriver)
{

}

void CC Vid_FlipBuffers(SVideo* pVideo)
{

}

void CC Vid_ShutDown_SYS(SVideo* pVideoDriver)
{

}

s32 CC Vid_EnableWrites(SVideo* pVideoDriver)
{
    return 0;
}

s32 CC Vid_DisableWrites(SVideo* pVideoDriver)
{
    return 0;
}

s32 CC Vid_GetSurface(SVideo* pVideoDriver)
{
    return 0;
}

s32 CC Vid_FreeSurface(SVideo* pVideoDriver)
{
    return 0;
}

s32 CC Vid_ClearScreen(SVideo* pVideoDriver, u8 aR, u8 aG, u8 aB, s32 aLeft, s32 aTop, s32 aRight, s32 aBottom)
{
    return 0;
}

s32 CC Vid_SetGamma(SVideo* pVideoDriver, f32 a2, f32 a3, f32 a4)
{
    return 0;
}

s32 CC Vid_WindowProc(SVideo* pVideoDriver, HWND hwnd, DWORD uMsg, WPARAM wParam, LPARAM lParam)
{
    return 0;
}

s32 CC Vid_InitDLL(HINSTANCE hInstance, s32 a2)
{
    return 0;
}

SVidVersion* CC Vid_GetVersion()
{
    return nullptr;
}
