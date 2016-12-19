#include "stdafx.h"
#include "d3ddll.hpp"
#include <cmath>
#include <d3d.h>

void CC ConvertColourBank(s32 unknown)
{
    // TODO
   // __debugbreak();
}

int CC DrawLine(int a1, int a2, int a3, int a4, int a5)
{
    __debugbreak();
    return std::abs(a4 - a2);
}

void CC SetShadeTableA(int a1, int a2, int a3, int a4, int a5)
{
    __debugbreak();
}

int CC MakeScreenTable(int* result, int a2, unsigned int a3, int a4)
{
    // TODO
    //__debugbreak();
    return 0;
}

int CC gbh_AddLight(int a1)
{
    __debugbreak();
    return 0;
}

char CC gbh_AssignPalette(int a1, int a2)
{
    __debugbreak();
    return 0;
}

void CC gbh_BeginLevel()
{
    __debugbreak();
}

int gbh_BeginScene()
{
    __debugbreak();
    return 0;
}

int CC gbh_BlitBuffer(int a1, int a2, int a3, int a4, int a5, int a6)
{
    __debugbreak();
    return 0;
}

char CC gbh_BlitImage(int a1, int a2, int a3, int a4, int a5, int a6, int a7)
{
    return 0;
}

void CC gbh_CloseDLL()
{
//    __debugbreak();
}

void CC gbh_CloseScreen(SVideo* pVideo)
{
    __debugbreak();
}

unsigned int CC gbh_Convert16BitGraphic(int a1, unsigned int a2, WORD *a3, signed int a4)
{
    __debugbreak();
    return 0;
}

unsigned int CC gbh_ConvertColour(unsigned __int8 a1, unsigned __int8 a2, unsigned __int8 a3)
{
    __debugbreak();
    return 0;
}

int CC gbh_DrawFlatRect(int a1, int a2)
{
    __debugbreak();
    return 0;
}

void CC gbh_DrawQuad(int a1, int a2, int a3, int a4)
{
    __debugbreak();
}

void CC gbh_DrawQuadClipped(int a1, int a2, int a3, int a4, int a5)
{
    __debugbreak();
}

// Same as gbh_DrawTile
s32 CC gbh_DrawTilePart(int a1, int a2, int a3, int a4)
{
    __debugbreak();
    return 0;
}

void CC gbh_DrawTriangle(int a1, int a2, int a3, int a4)
{
    __debugbreak();
}

void CC gbh_EndLevel()
{
    __debugbreak();
}

__int64 CC gbh_EndScene()
{
    __debugbreak();
    return 0;
}

int CC gbh_FreeImageTable()
{
    __debugbreak();
    return 0;
}

void CC gbh_FreePalette(int a1)
{
//    __debugbreak();
}

void CC gbh_FreeTexture(STexture* pTexture)
{
    // TODO: Other stuff required
    free(pTexture);
}

u32 gTriangleCount = 0; // Some sort of counter


u32* CC gbh_GetGlobals()
{
    return &gTriangleCount;
}

int CC gbh_GetUsedCache(int a1)
{
    __debugbreak();
    return 0;
}


signed int Init_E02340()
{
    // TODO
    return 1;
}

s32 CC gbh_Init(int a1)
{
    int result = Init_E02340();
    if (!result)
    {
        gbh_SetColourDepth();
        result = 0;
    }
    return result;
}

static SVideo* gpVideoDriver_E13DC8 = nullptr;
static SFunctions gVideoDriverFuncs;

static int CC gbh_SetMode_E04D80(SVideo* pVideoDriver, HWND hwnd, int modeId)
{
    pVideoDriver->field_34_active_device_id = 0;

    int result = gVideoDriverFuncs.pVid_SetMode(pVideoDriver, hwnd, modeId);
    if (!result)
    {
        result = Init_E02340();
    }
    return result;
}


u32 CC gbh_InitDLL(SVideo* pVideoDriver)
{
    gpVideoDriver_E13DC8 = pVideoDriver;

    PopulateSFunctions(pVideoDriver->field_7C_self_dll_handle, gVideoDriverFuncs);

    pVideoDriver->field_84_from_initDLL->pVid_CloseScreen = gbh_CloseScreen;
    pVideoDriver->field_84_from_initDLL->pVid_GetSurface = gVideoDriverFuncs.pVid_GetSurface;
    pVideoDriver->field_84_from_initDLL->pVid_FreeSurface = gVideoDriverFuncs.pVid_FreeSurface;
    pVideoDriver->field_84_from_initDLL->pVid_SetMode = gbh_SetMode_E04D80;

    return 1;
}

signed int CC gbh_InitImageTable(int a1)
{
    __debugbreak();
    return 0;
}

signed int CC gbh_LoadImage(int a1)
{
    __debugbreak();
    return 0;
}

int CC gbh_LockTexture(int a1)
{
    __debugbreak();
    return 0;
}

void CC gbh_Plot(int a1, int a2, int a3, int a4)
{
    __debugbreak();
}

int CC gbh_PrintBitmap(int a1, int a2)
{
    __debugbreak();
    return 0;
}

unsigned int CC gbh_RegisterPalette(int paltId, DWORD* pData)
{
    // TODO
    //__debugbreak();
    return 1;
}

static u32 gTextureId = 0;

STexture* CC gbh_RegisterTexture(__int16 width, __int16 height, void* pData, int a4, char a5)
{
    STexture* pTexture = reinterpret_cast<STexture*>(malloc(sizeof(STexture)));
    if (pTexture)
    {
        memset(pTexture, 0, sizeof(STexture));
        pTexture->field_0 = gTextureId++;
        //pTexture->field_4 = palt[a4];
        pTexture->field_E_width = width;
        pTexture->field_10_height = height;
        //pTexture->field_12 = pSomething[a4];
        //if (a5 && isAtiRagePro)
        {
            //pTexture->field_13_flags = 0x80;
        }
        pTexture->field_14_data = pData;
        //pTexture->field_18_pPlat = palt[a4];
    }
    return pTexture;
}

void CC gbh_ResetLights()
{
    __debugbreak();
}

void CC gbh_SetAmbient(float a1)
{
    __debugbreak();
}

int CC gbh_SetCamera(int a1, int a2, int a3, int a4)
{
    __debugbreak();
    return 0;
}

int CC gbh_SetColourDepth()
{
    // TODO
    //__debugbreak();
    return 1;
}

s32 CC gbh_SetWindow(int a1, int a2, int a3, int a4)
{
    //__debugbreak();
    return 0;
}

int CC gbh_UnlockTexture(int a1)
{
    __debugbreak();
    return 0;
}
