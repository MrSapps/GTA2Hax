#include "stdafx.h"
#include "d3ddll.hpp"
#include <cmath>

void CC ConvertColourBank(s32 unknown)
{

}

int CC DrawLine(int a1, int a2, int a3, int a4, int a5)
{
    return std::abs(a4 - a2);
}

void CC SetShadeTableA(int a1, int a2, int a3, int a4, int a5)
{

}

int CC MakeScreenTable(int* result, int a2, unsigned int a3, int a4)
{
    return 0;
}

int CC gbh_AddLight(int a1)
{
    return 0;
}

char CC gbh_AssignPalette(int a1, int a2)
{
    return 0;
}

void CC gbh_BeginLevel()
{

}

int gbh_BeginScene()
{
    return 0;
}

int CC gbh_BlitBuffer(int a1, int a2, int a3, int a4, int a5, int a6)
{
    return 0;
}

char CC gbh_BlitImage(int a1, int a2, int a3, int a4, int a5, int a6, int a7)
{
    return 0;
}

void CC gbh_CloseDLL()
{

}

int CC gbh_CloseScreen(int a1)
{
    return 0;
}

unsigned int CC gbh_Convert16BitGraphic(int a1, unsigned int a2, WORD *a3, signed int a4)
{
    return 0;
}

unsigned int CC gbh_ConvertColour(unsigned __int8 a1, unsigned __int8 a2, unsigned __int8 a3)
{
    return 0;
}

int CC gbh_DrawFlatRect(int a1, int a2)
{
    return 0;
}

void CC gbh_DrawQuad(int a1, int a2, int a3, int a4)
{

}

void CC gbh_DrawQuadClipped(int a1, int a2, int a3, int a4, int a5)
{

}

// Same as gbh_DrawTile
s32 CC gbh_DrawTilePart(int a1, int a2, int a3, int a4)
{
    return 0;
}

void CC gbh_DrawTriangle(int a1, int a2, int a3, int a4)
{

}

void CC gbh_EndLevel()
{

}

__int64 CC gbh_EndScene()
{
    return 0;
}

int CC gbh_FreeImageTable()
{
    return 0;
}

void CC gbh_FreePalette(int a1)
{

}

void CC gbh_FreeTexture(void *Memory)
{

}

int* CC gbh_GetGlobals()
{
    return 0;
}

int CC gbh_GetUsedCache(int a1)
{
    return 0;
}

s32 CC gbh_Init(int a1)
{
    return 0;
}

u32 CC gbh_InitDLL(int a1, int a2, int a3)
{
    return 0;
}

signed int CC gbh_InitImageTable(int a1)
{
    return 0;
}

signed int CC gbh_LoadImage(int a1)
{
    return 0;
}

int CC gbh_LockTexture(int a1)
{
    return 0;
}

void CC gbh_Plot(int a1, int a2, int a3, int a4)
{

}

int CC gbh_PrintBitmap(int a1, int a2)
{
    return 0;
}

unsigned int CC gbh_RegisterPalette(int paltId, DWORD* pData)
{
    return 0;
}

WORD* CC gbh_RegisterTexture(__int16 width, __int16 height, int pData, int a4, char a5)
{
    return 0;
}

void CC gbh_ResetLights()
{

}

void CC gbh_SetAmbient(float a1)
{

}

int CC gbh_SetCamera(int a1, int a2, int a3, int a4)
{
    return 0;
}

int CC gbh_SetColourDepth()
{
    return 0;
}

s32 CC gbh_SetWindow(int a1, int a2, int a3, int a4)
{
    return 0;
}

int CC gbh_UnlockTexture(int a1)
{
    return 0;
}
