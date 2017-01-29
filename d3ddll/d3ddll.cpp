#include "stdafx.h"
#include "d3ddll.hpp"
#include <cmath>
#include <d3d.h>
#include <vector>
#include <iostream>
#include <set>
#include <algorithm>
#include "detours.h"
#include <assert.h>
#include "../DmaVideo/logger.hpp"

#define BYTEn(x, n)   (*((BYTE*)&(x)+n))
#define BYTE1(x)   BYTEn(x,  1)

#pragma comment(lib, "dxguid.lib")

static bool gProxyOnly = false;      // Pass through all functions to real DLL
static bool gDetours = false;       // Used in combination with gProxyOnly=true to hook some internal functions to test them in isolation
static bool gRealPtrs = false;

// Other
static S3DFunctions gFuncs;

struct S3DDevice
{
    DWORD field_0_id;
    char* field_4_device_name;
    char* field_8_device_description;
    struct STextureFormat* field_C_first_texture_format;
    struct STextureFormat* field_10_pFirst_TextureFormat;
    struct SHardwareTexture* field_14_phead;
    struct SHardwareTexture* field_18_p0x60_IDirect3dTexture2_list;
    DWORD field_1C_num_texture_enums;
    DWORD field_20_flags;
    D3DDEVICEDESC field_24_deviceDesc;
    DWORD field_120_context;
    D3DDEVICEDESC field_124;
    GUID field_220_device_guid;
    struct S3DDevice* field_230_next_ptr;
    DWORD field_234_backing_buffer;
    DWORD field_238;
};

static_assert(sizeof(D3DDEVICEDESC) == 0xfc, "Wrong size D3DDEVICEDESC");
static_assert(sizeof(S3DDevice) == 0x23C, "Wrong size S3DDevice");

struct SD3dStruct
{
    SVideo* field_0_pVideoDriver;
    S3DDevice* field_4_pnext_device;
    S3DDevice* field_8_pfirst_device;
    DWORD field_C_device_id_gen;
    DWORD field_10_num_enums;
    S3DDevice* field_14_active_device;
    DWORD field_18_current_id;
    struct STextureFormat* field_1c_texture_format;
    struct STextureFormat* field_20_texture_format;
    IDirect3D3* field_24_pID3d;
    IDirect3DDevice3* field_28_ID3D_Device;
    IDirect3DViewport3* field_2C_IViewPort;
    D3DVIEWPORT2 field_30_D3DVIEWPORT2;
    DWORD field_5C_pitchQ;
    IDirectDrawSurface4* field_60_IUnknown;
};
static_assert(sizeof(SD3dStruct) == 0x64, "Wrong size SD3dStruct");

// From dll
static bool gbSurfaceIsFreed_E43E18 = false;

static struct SHardwareTexture* hack2 = nullptr;
static struct SHardwareTexture** gActiveTextureId_dword_2B63DF4 = &hack2;
static double qword_2B60848 = 0;
static DWORD frame_number_2B93E4C = 0;

// Texture cache related
static WORD texture_sizes_word_107E0[12] = { 8, 16, 32, 64, 128, 256, 1032, 1040, 1056, 1088, 1152, 1280 };

struct SGlobals
{
    DWORD mNumPolysDrawn; // gNumPolysDrawn_dword_E43EA0
    DWORD mNumTextureSwaps; // gGlobals.mNumTextureSwaps
    DWORD mNumBatchFlushes;
    DWORD mSceneTime_2B93EAC; // gSceneTime_2B93EAC


    DWORD gCacheSizes_dword_43EB0[12];
    DWORD gCacheSizes_word_10810[12];

    // TODO: This is displayed 1 column before where expected, some other 12 DWORD array
    // must be read for that one ??
    DWORD gCacheHitRates_dword_E43F10[12]; // Cache hit counters


    // TODO: Not really part of this
    DWORD gCacheUnknown_107F8[12] = { 10, 62, 62, 88, 1, 0, 110, 126, 126, 40, 10, 0 };
    SCache* cache_12_array_dword_E13D80[12] = {};

};
static SGlobals gGlobals = {};


static DWORD bPointFilteringOn_E48604 = 0;

DWORD hack3 = 0;
static DWORD* renderStateCache_E43E24 = &hack3;

static SVideo* gpVideoDriver_E13DC8 = nullptr;

SD3dStruct* hack = nullptr;
static struct SD3dStruct** gD3dPtr_dword_21C85E0 = &hack;
static SVideoFunctions gVideoDriverFuncs;


static float gWindow_left_dword_E43E08;
static float gWindow_right_dword_E43E0C;
static float gWindow_top_dword_E43E10;
static float gWindow_bottom_dword_E43E14;
static DWORD gWindow_d5_dword_E13DC4;

static float k1_2B638A0;

static DWORD gGpuSpecificHack_dword_2B63884 = 0;
static DWORD gbIsAtiRagePro_dword_E13888 = 0;




static int gScreenTableSize_dword_E13DCC = 0;
static int gScreenTable_dword_E43F40[1700];
float gfAmbient_E10838 = 1.0f;

struct SImageTableEntry
{
    BOOL bLoaded;
    DWORD field_4_w;
    DWORD field_8_h;
    IDirectDrawSurface4* field_C_pSurface;
};
static_assert(sizeof(SImageTableEntry) == 0x10, "Wrong size SImageTableEntry");

static SImageTableEntry* gpImageTable_dword_E13894 = nullptr;
static DWORD gpImageTableCount_dword_E13898 = 0;

static u32 gTextureId_dword_E13D54 = 0;

struct SPalData
{
    DWORD* mPOriginalData;
    WORD* mPData;
    DWORD mbLoaded;
};
static_assert(sizeof(SPalData) == 0xC, "Wrong size SPalData");

static SPalData pals_2B63E00[16384];


struct SCache
{
    BYTE field_0;
    BYTE field_1_flags;
    BYTE field_2;
    BYTE field_3;
    WORD field_4;
    WORD field_6_cache_idx;
    DWORD field_8_used_Frame_num;
    float field_C;
    DWORD field_10;
    DWORD field_14;
    struct STexture* field_18_pSTexture;
    struct SCache* field_1C_pNext;
    struct SCache* field_20_pCache;
    struct SHardwareTexture* field_24_texture_id;
    DWORD field_28;
};
static_assert(sizeof(SCache) == 0x2C, "Wrong size SCache");

STexture* __stdcall TextureCache_E01EC0(STexture* pTexture)
{
    SCache* pCache = pTexture->field_1C_ptr;
    if (pCache)
    {
        pCache->field_1_flags |= 0x80u;
        
        // Remove the active texture
        pCache->field_18_pSTexture = nullptr;

        // Set the active frame to the current frame
        pCache->field_8_used_Frame_num = frame_number_2B93E4C - 1;

        // Remove the active cache
        pTexture->field_1C_ptr = 0;

        auto p20Cache = pCache->field_20_pCache;
        if (p20Cache)
        {
            // Set next free item to the no longer in use cache ?
            p20Cache->field_1C_pNext = pCache->field_1C_pNext;
            
            // If there is a next item point to root?
            auto pNext = pCache->field_1C_pNext;
            if (pNext)
            {
                pNext->field_20_pCache = pCache->field_20_pCache;
            }
            else
            {
                // Otherwise this item is the root
                gGlobals.cache_12_array_dword_E13D80[pCache->field_6_cache_idx] = pCache->field_20_pCache;
            }

            // Set the first item to the root of the cache list ?
            pCache->field_1C_pNext = gGlobals.cache_12_array_dword_E13D80[pCache->field_6_cache_idx];

            gGlobals.cache_12_array_dword_E13D80[pCache->field_6_cache_idx]->field_20_pCache = pCache;
            gGlobals.cache_12_array_dword_E13D80[pCache->field_6_cache_idx] = pCache;
            pCache->field_20_pCache = 0;
        }
    }
    return pTexture;
}


struct SLightInternal
{
    DWORD field_0_flags;
    float field_4_brightness;
    float field_8_radius;
    float field_C_radius_squared;
    float field_10_radius_normalized; // A byte of SLight field_10

                                      // x from SLight?
    float field_14_x;

    // y from SLight?
    float field_18_y; // Light verts takes pointer to here

                      // z from SLight?
    float field_1C_z;

    // SLight field_10 bytes
    float field_20_r;
    float field_24_g;
    float field_28_b;
};
static_assert(sizeof(SLightInternal) == 0x2c, "Wrong size SLightInternal");

static SLightInternal lights_2B959E0[256] = {};


void CC ConvertColourBank(s32 unknown)
{
    // Empty/NOP in real game
}

const char *__stdcall D3dErr2String_E01000(int err)
{
    switch (err)
    {
    case D3D_OK: return "D3D_OK";
    case D3DERR_BADMAJORVERSION: return "D3DERR_BADMAJORVERSION";
    case D3DERR_ZBUFF_NEEDS_VIDEOMEMORY: return "D3DERR_ZBUFF_NEEDS_VIDEOMEMORY (new for DirectX 5)";
    case D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY: return "D3DERR_ZBUFF_NEEDS_SYSTEMMEMORY (new for DirectX 5)";
    case D3DERR_VIEWPORTHASNODEVICE: return "D3DERR_VIEWPORTHASNODEVICE (new for DirectX 5)";
    case D3DERR_VIEWPORTDATANOTSET: return "D3DERR_VIEWPORTDATANOTSET (new for DirectX 5)";
    case D3DERR_TEXTURE_UNLOCK_FAILED: return "D3DERR_TEXTURE_UNLOCK_FAILED";
    case D3DERR_TEXTURE_SWAP_FAILED: return "D3DERR_TEXTURE_SWAP_FAILED";
    case D3DERR_TEXTURE_NOT_LOCKED: return "D3DERR_TEXTURE_NOT_LOCKED";
    case D3DERR_TEXTURE_NO_SUPPORT: return "D3DERR_TEXTURE_NO_SUPPORT";
    case D3DERR_TEXTURE_LOCKED: return "D3DERR_TEXTURE_LOCKED";
    case D3DERR_TEXTURE_LOCK_FAILED: return "D3DERR_TEXTURE_LOCK_FAILED";
    case D3DERR_TEXTURE_LOAD_FAILED: return "D3DERR_TEXTURE_LOAD_FAILED";
    case D3DERR_TEXTURE_GETSURF_FAILED: return "D3DERR_TEXTURE_GETSURF_FAILED";
    case D3DERR_TEXTURE_DESTROY_FAILED: return "D3DERR_TEXTURE_DESTROY_FAILED";
    case D3DERR_TEXTURE_CREATE_FAILED: return "D3DERR_TEXTURE_CREATE_FAILED";
    case D3DERR_TEXTURE_BADSIZE: return "D3DERR_TEXTURE_BADSIZE (new for DirectX 5)";
    case D3DERR_SURFACENOTINVIDMEM: return "D3DERR_SURFACENOTINVIDMEM (new for DirectX 5)";
    case D3DERR_SETVIEWPORTDATA_FAILED: return "D3DERR_SETVIEWPORTDATA_FAILED";
    case D3DERR_SCENE_NOT_IN_SCENE: return "D3DERR_SCENE_NOT_IN_SCENE";
    case D3DERR_SCENE_IN_SCENE: return "D3DERR_SCENE_IN_SCENE";
    case D3DERR_SCENE_END_FAILED: return "D3DERR_SCENE_END_FAILED";
    case D3DERR_SCENE_BEGIN_FAILED: return "D3DERR_SCENE_BEGIN_FAILED";
    case D3DERR_NOVIEWPORTS: return "D3DERR_NOVIEWPORTS (new for DirectX 5)";
    case D3DERR_NOTINBEGIN: return "D3DERR_NOTINBEGIN (new for DirectX 5)";
    case D3DERR_NOCURRENTVIEWPORT: return "D3DERR_NOCURRENTVIEWPORT (new for DirectX 5)";
    case D3DERR_MATRIX_SETDATA_FAILED: return "D3DERR_MATRIX_SETDATA_FAILED";
    case D3DERR_MATRIX_GETDATA_FAILED: return "D3DERR_MATRIX_GETDATA_FAILED";
    case D3DERR_MATRIX_DESTROY_FAILED: return "D3DERR_MATRIX_DESTROY_FAILED";
    case D3DERR_MATRIX_CREATE_FAILED: return "D3DERR_MATRIX_CREATE_FAILED";
    case D3DERR_MATERIAL_SETDATA_FAILED: return "D3DERR_MATERIAL_SETDATA_FAILED";
    case D3DERR_MATERIAL_GETDATA_FAILED: return "D3DERR_MATERIAL_GETDATA_FAILED";
    case D3DERR_MATERIAL_DESTROY_FAILED: return "D3DERR_MATERIAL_DESTROY_FAILED";
    case D3DERR_MATERIAL_CREATE_FAILED: return "D3DERR_MATERIAL_CREATE_FAILED";
    case D3DERR_LIGHTNOTINTHISVIEWPORT: return "D3DERR_LIGHTNOTINTHISVIEWPORT (new for DirectX 5)";
    case D3DERR_LIGHTHASVIEWPORT: return "D3DERR_LIGHTHASVIEWPORT (new for DirectX 5)";
    case D3DERR_LIGHT_SET_FAILED: return "D3DERR_LIGHT_SET_FAILED";
    case D3DERR_INVALIDVERTEXTYPE: return "D3DERR_INVALIDVERTEXTYPE (new for DirectX 5)";
    case D3DERR_INVALIDRAMPTEXTURE: return "D3DERR_INVALIDRAMPTEXTURE (new for DirectX 5)";
    case D3DERR_INVALIDPRIMITIVETYPE: return "D3DERR_INVALIDPRIMITIVETYPE (new for DirectX 5)";
    case D3DERR_INVALIDPALETTE: return "D3DERR_INVALIDPALETTE(new for DirectX 5)";
    case D3DERR_INVALIDCURRENTVIEWPORT: return "D3DERR_INVALIDCURRENTVIEWPORT (new for DirectX 5)";
    case D3DERR_INVALID_DEVICE: return "D3DERR_INVALID_DEVICE (new for DirectX 5)";
    case D3DERR_INBEGIN: return "D3DERR_INBEGIN (new for DirectX 5)";
    case D3DERR_INITFAILED: return "D3DERR_INITFAILED (new for DirectX 5)";
    case D3DERR_EXECUTE_UNLOCK_FAILED: return "D3DERR_EXECUTE_UNLOCK_FAILED";
    case D3DERR_EXECUTE_NOT_LOCKED: return "D3DERR_EXECUTE_NOT_LOCKED";
    case D3DERR_EXECUTE_LOCKED: return "D3DERR_EXECUTE_LOCKED";
    case D3DERR_EXECUTE_LOCK_FAILED: return "D3DERR_EXECUTE_LOCK_FAILED";
    case D3DERR_EXECUTE_CLIPPED_FAILED: return "D3DERR_EXECUTE_CLIPPED_FAILED";
    case D3DERR_DEVICEAGGREGATED: return "D3DERR_DEVICEAGGREGATED (new for DirectX 5)";
    case D3DERR_BADMINORVERSION: return "D3DERR_BADMINORVERSION";
    default: return "Unrecognized error value.";
    }
}

int CC DrawLine(int a1, int a2, int a3, int a4, int a5)
{
    return std::abs(a4 - a2);
}

void CC SetShadeTableA(int a1, int a2, int a3, int a4, int a5)
{
    __debugbreak();
}

int* CC MakeScreenTable(int value, int elementSize, unsigned int size)
{
    // TODO
    if (gProxyOnly)
    {
        return gFuncs.pMakeScreenTable(value, elementSize, size);
    }

    int* result = 0;
    gScreenTableSize_dword_E13DCC = size;
    if (size)
    {
        result = gScreenTable_dword_E43F40;
        do
        {
            *result = value;
            value += elementSize;
            ++result;
            --size;
        } while (size);
    }
    return result;
}


char CC gbh_AssignPalette(STexture* pTexture, int palId)
{
    if (gProxyOnly)
    {
        return gFuncs.pgbh_AssignPalette(pTexture, palId);
    }
    
    bool needUnlock = false;
    if (!(pTexture->field_13_flags & 1))
    {
        gbh_LockTexture(pTexture);
        needUnlock = true;
    }

    pTexture->field_18_pPaltData = pals_2B63E00[palId].mPData;
    const auto result = pals_2B63E00[palId].mbLoaded;
    pTexture->field_12_bPalIsValid = result;

    if (needUnlock)
    {
        gbh_UnlockTexture(pTexture);
    }
    return result;
}

void CC gbh_BeginLevel()
{
    __debugbreak();
}

static BOOL __stdcall DeviceBeginScene_E01BF0(SD3dStruct* pCtx)
{
    return pCtx->field_28_ID3D_Device->BeginScene() != 0;
}

int gbh_BeginScene()
{
    if (gProxyOnly)
    {
        return gFuncs.pgbh_BeginScene();
    }

    if (gpVideoDriver_E13DC8->field_4_flags & 1)
    {
        gbSurfaceIsFreed_E43E18 = true;
        (*gpVideoDriver_E13DC8->field_84_from_initDLL->pVid_FreeSurface)(gpVideoDriver_E13DC8);
    }
    else
    {
        gbSurfaceIsFreed_E43E18 = false;
    }
    memset(gGlobals.gCacheHitRates_dword_E43F10, 0, sizeof(gGlobals.gCacheHitRates_dword_E43F10));
    gGlobals.mNumBatchFlushes = 0;
    gGlobals.mNumTextureSwaps = 0;
    gGlobals.mNumPolysDrawn = 0;
    (*gActiveTextureId_dword_2B63DF4) = (SHardwareTexture*)-1;
    qword_2B60848 = 0i64;
    ++frame_number_2B93E4C;
    return DeviceBeginScene_E01BF0((*gD3dPtr_dword_21C85E0));
}

int CC gbh_BlitBuffer(int a1, int a2, int a3, int a4, int a5, int a6)
{
    __debugbreak();
    return 0;
}

char CC gbh_BlitImage(int imageIndex, int srcLeft, int srcTop, int srcRight, int srcBottom, int dstX, int dstY)
{
    if (gProxyOnly)
    {
         return gFuncs.pgbh_BlitImage(imageIndex, srcLeft, srcTop, srcRight, srcBottom, dstX, dstY);
    }

    int result = 0;
    if (imageIndex > gpImageTableCount_dword_E13898)
    {
        result = -1;
    }

    if (!gpImageTable_dword_E13894[imageIndex].bLoaded)
    {
        return -1;
    }

    if (srcLeft < 0
        || srcTop < 0
        || srcRight < 0
        || srcBottom < 0
        || srcLeft >gpImageTable_dword_E13894[imageIndex].field_4_w
        || srcTop >gpImageTable_dword_E13894[imageIndex].field_8_h
        || srcRight >gpImageTable_dword_E13894[imageIndex].field_4_w
        || srcBottom >gpImageTable_dword_E13894[imageIndex].field_8_h)
    {
        return -2;
    }

    if (dstX < 0 || dstY < 0
        || (dstX - srcLeft + srcRight) > gpVideoDriver_E13DC8->field_48_rect_right
        || (dstY - srcTop + srcBottom) > gpVideoDriver_E13DC8->field_4C_rect_bottom)
    {
        return -3;
    }

    if (gpImageTable_dword_E13894[imageIndex].field_C_pSurface->IsLost())
    {
        gpImageTable_dword_E13894[imageIndex].field_C_pSurface->Release();
        gpImageTable_dword_E13894[imageIndex].field_C_pSurface = nullptr;
        return  -10;
    }

    RECT srcRect = {};
    srcRect.left = srcLeft;
    srcRect.top = srcTop;
    srcRect.right = srcRight;
    srcRect.bottom = srcBottom;

    RECT dstRect = {};
    dstRect.left = dstX;
    dstRect.top = dstY;
    dstRect.right = dstX - srcLeft + srcRight;
    dstRect.bottom = dstY - srcTop + srcBottom;

    if (!gpVideoDriver_E13DC8->field_138_Surface)
    {
        return -4;
    }

    if (FAILED(gpVideoDriver_E13DC8->field_138_Surface->Blt(
        &dstRect,
        (LPDIRECTDRAWSURFACE4)gpImageTable_dword_E13894[imageIndex].field_C_pSurface,
        &srcRect,
        DDBLT_WAIT,
        0)))
    {
        return -4;
    }

    return 0;
}

static int __stdcall FreeD3dDThings_E016E0(SD3dStruct* pD3d);

static void CleanUpD3d()
{
    for (int idx = 0; idx < 12; idx++)
    {
        auto pCache = gGlobals.cache_12_array_dword_E13D80[idx];
        if (pCache)
        {
            SCache* pCurrentCache = nullptr;
            do
            {
                pCurrentCache = pCache->field_1C_pNext;
                if (pCache->field_18_pSTexture)
                {
                    TextureCache_E01EC0(pCache->field_18_pSTexture);
                }

                auto nextPtr = pCache->field_1C_pNext;
                if (nextPtr)
                {
                    nextPtr->field_20_pCache = 0;
                    gGlobals.cache_12_array_dword_E13D80[idx] = pCache->field_1C_pNext;
                }
                free(pCache);
                pCache = pCurrentCache;
            } while (pCurrentCache);
        }
        gGlobals.cache_12_array_dword_E13D80[idx] = 0;
    }

    if (*gD3dPtr_dword_21C85E0)
    {
        FreeD3dDThings_E016E0(*gD3dPtr_dword_21C85E0);
    }
    (*gD3dPtr_dword_21C85E0) = 0;
}

static decltype(&Vid_CloseScreen) pOldCloseScreen = nullptr;
static decltype(&Vid_SetMode) pOldSetMode = nullptr;

void CC gbh_CloseDLL()
{
    TRACE_ENTRYEXIT;


    if (gProxyOnly)
    {
        return gFuncs.pgbh_CloseDLL();
    }

    auto pVideoDriver = (*gD3dPtr_dword_21C85E0)->field_0_pVideoDriver;

    CleanUpD3d();

    pOldCloseScreen(pVideoDriver);

    *pVideoDriver->field_84_from_initDLL->pVid_CloseScreen = pOldCloseScreen;
    //*pVideoDriver->field_84_from_initDLL->pVid_GetSurface = gVideoDriverFuncs.pVid_GetSurface;
    //*pVideoDriver->field_84_from_initDLL->pVid_FreeSurface = gVideoDriverFuncs.pVid_FreeSurface;
    *pVideoDriver->field_84_from_initDLL->pVid_SetMode = pOldSetMode;

    /*
    free(gPtr_dword_E13864); // TODO: Lighting table
    gPtr_dword_E13864 = 0;
    free(gPtr_dword_E43E20); // TODO: Allocated but never used
    gPtr_dword_E43E20 = 0;*/

}

void CC gbh_CloseScreen(SVideo* pVideo)
{
    TRACE_ENTRYEXIT;

    CleanUpD3d();
    pOldCloseScreen(pVideo);
    //(*pVideo->field_84_from_initDLL->pVid_CloseScreen)(pVideo);
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

#undef max
#undef min

static bool Clipped(Vert* pVerts, int count)
{
    float maxX = pVerts[0].x;
    for (auto i = 1; i < count; i++)
    {
        maxX = std::max(maxX, pVerts[i].x);
    }

    float minX = pVerts[0].x;
    for (auto i = 1; i < count; i++)
    {
        minX = std::min(minX, pVerts[i].x);
    }

    float maxY = pVerts[0].y;
    for (auto i = 1; i < count; i++)
    {
        maxY = std::max(maxY, pVerts[i].y);
    }

    float minY = pVerts[0].y;
    for (auto i = 1; i < count; i++)
    {
        minY = std::min(minY, pVerts[i].y);
    }

    return (maxX < gWindow_left_dword_E43E08
        && minX >= gWindow_right_dword_E43E0C
        && minY < gWindow_bottom_dword_E43E14
        && maxY >= gWindow_top_dword_E43E10);
}

static void  __stdcall SetRenderStates_E02960(int states);
decltype(&SetRenderStates_E02960) pSetRenderStates_E02960 = 0x0;

static void  __stdcall SetRenderStates_E02960(int states)
{
    if (states & 0x380)
    {
        if (states & 0x200) 
        {
            auto result = (*renderStateCache_E43E24);
            if ((*renderStateCache_E43E24) == 1)
            {
                result = 0;
                (*renderStateCache_E43E24) = 0;
            }

            if (!result)
            {
                (*renderStateCache_E43E24) = 2;
                (*gD3dPtr_dword_21C85E0)->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1); // 27, 1
                (*gD3dPtr_dword_21C85E0)->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE); // 19, 1
                (*gD3dPtr_dword_21C85E0)->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE); // 20, 1
            }
        }
        else if (states & 0x180)
        {
            auto result = (*renderStateCache_E43E24);
            if ((*renderStateCache_E43E24) == 2)
            {
                result = 0;
                (*renderStateCache_E43E24) = 0;
            }
            if (!result)
            {
                (*renderStateCache_E43E24) = 1;

                (*gD3dPtr_dword_21C85E0)->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA); // 21, 4
                (*gD3dPtr_dword_21C85E0)->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1); // 27, 1
                (*gD3dPtr_dword_21C85E0)->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA); // 19, 5
                (*gD3dPtr_dword_21C85E0)->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA); // 20, 6
            }
        }
    }
    else
    {
        if ((*renderStateCache_E43E24))
        {
            (*gD3dPtr_dword_21C85E0)->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0); // 27, 0
            (*renderStateCache_E43E24) = 0;
        }
    }
}

static STexture* pLast = nullptr;

#define HIBYTE(x)   (*((BYTE*)&(x)+1))

static signed int __stdcall D3dTextureUnknown_2B561D0(SHardwareTexture* pHardwareTexture, BYTE* pixelData, WORD* pPalData, int textureW, int textureH, int palSize, int renderFlags, char textureFlags);

static unsigned __int16 __stdcall CacheFlushBatchRelated_2B52810(STexture *pTexture, int renderFlags);
decltype(&CacheFlushBatchRelated_2B52810) pCacheFlushBatchRelated_2B52810 = 0x0;

static unsigned __int16 __stdcall CacheFlushBatchRelated_2B52810(STexture *pTexture, int renderFlags)
{
    int biggestSide = pTexture->field_E_width;
    if (pTexture->field_10_height >biggestSide)
    {
        biggestSide = pTexture->field_10_height;
    }

    auto flagsCopy = renderFlags;
    if (renderFlags & 0x380)
    {
        HIBYTE(biggestSide) |= 4u;
        flagsCopy = renderFlags | 0x80;
    }


    auto cache_index = 0;
    while (biggestSide > texture_sizes_word_107E0[cache_index])
    {
        if (++cache_index >= 12)
        {
            break;
        }
    }


    auto pCache = gGlobals.cache_12_array_dword_E13D80[cache_index];
    if (pCache->field_8_used_Frame_num == frame_number_2B93E4C)
    {
        ++gGlobals.mNumBatchFlushes;
        (*gD3dPtr_dword_21C85E0)->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_FLUSHBATCH, 1);
        ++frame_number_2B93E4C;
    }

    auto pCachedTexture = pCache->field_18_pSTexture;
    if (pCachedTexture)
    {
        pCachedTexture->field_1C_ptr = 0;
    }

    pTexture->field_1C_ptr = pCache;

    pCache->field_0 &= 0x7FFFu;
    pCache->field_18_pSTexture = pTexture;
    auto pPal = pTexture->field_18_pPaltData;
    
    // Pointer to texture pixel data TODO Why are field_C and D used? Some header data ?
    auto v8 = pTexture->field_C + (pTexture->field_D << 8) + pTexture->field_14_original_pixel_data_ptr;

    pCache->field_10 = 0.00390625 / (double)pTexture->field_12_bPalIsValid;
    pCache->field_14 = 0.00390625 / (double)pTexture->field_12_bPalIsValid * 255.0;
    
    auto textureFlags = pTexture->field_13_flags;

    D3dTextureUnknown_2B561D0(
        pCache->field_24_texture_id,
        v8,
        flagsCopy & 0x380 ? (pPal + 256) : pPal,
        pTexture->field_E_width,
        pTexture->field_10_height,
        256,
        flagsCopy,
        textureFlags);

    auto result = pCache->field_6_cache_idx;

    ++gGlobals.gCacheHitRates_dword_E43F10[result];

   return 0;

}

static DWORD numLights_2B93E38 = 0; // 43E38

int __stdcall LightVerts_new(int vertCount, Vert* pVerts, int alwaysZero, unsigned __int8 colourRelated)
{
    for (int vertIdx = 0; vertIdx < vertCount; vertIdx++)
    {
        float light_r = 0.0f;
        float light_g = 0.0f;
        float light_b = 0.0f;
        for (int j = 0; j < numLights_2B93E38; j++)
        {
            if ((lights_2B959E0[j].field_0_flags & 0x30000) == 0x10000) // Light type ?
            {
                // Check if vertex point is within light radius

                // TODO: This has caused me no end of trouble, it appears no matter if quad or tri the 5th index
                // is used to check the position, this means we always have:
                // vert0
                // vert1
                // vert2
                // vert3 - optional, if tri then nothing
                // ?? 0-4 for lighting, perhaps actually normals ?

                const float dx = pVerts[vertIdx + 4].x - lights_2B959E0[j].field_14_x;
                const float dy = pVerts[vertIdx + 4].y - lights_2B959E0[j].field_18_y;
                const float dz = pVerts[vertIdx + 4].z - lights_2B959E0[j].field_1C_z;


                const float distanceSquared = (dx * dx) + (dy * dy) + (dz * dz);

                if (distanceSquared <= lights_2B959E0[j].field_C_radius_squared)
                {
                    const float distance = sqrt(distanceSquared);
                    const float normalizedDistance = (lights_2B959E0[j].field_8_radius - distance) * lights_2B959E0[j].field_10_radius_normalized;
                    if (normalizedDistance > 0.0f)
                    {
                        auto lightWithBrightness = normalizedDistance * lights_2B959E0[j].field_4_brightness;
                        light_r = light_r + lights_2B959E0[j].field_20_r * lightWithBrightness;
                        light_g = light_g + lights_2B959E0[j].field_24_g * lightWithBrightness;
                        light_b = light_b + lights_2B959E0[j].field_28_b * lightWithBrightness;
                    }
                }
            }
        }

        const float colourConverted = colourRelated * 0.0039215689f;

        const auto diffB2 = (double)(((unsigned int)pVerts[vertIdx].diff >> 16) & 0xFF);
        auto b1 = colourConverted * diffB2 * light_r + gfAmbient_E10838;
        if (b1 > 255.0f)
        {
            b1 = 255.0f;
        }

        const auto diffB1 = (double)((unsigned __int16)pVerts[vertIdx].diff >> 8);
        auto b2 = colourConverted * diffB1 * light_g + gfAmbient_E10838;
        if (b2 > 255.0f)
        {
            b2 = 255.0f;
        }

        auto diffB0 = (double)(unsigned __int8)pVerts[vertIdx].diff;
        auto b3 = colourConverted * diffB0 * light_b + gfAmbient_E10838;
        if (b3 > 255.0f)
        {
            b3 = 255.0f;
        }

        pVerts[vertIdx].diff = (signed int)b3 | pVerts[vertIdx].diff & 0xFF000000 | (((signed int)b2 | ((signed int)b1 << 8)) << 8);
    }

    return 0;
}

static signed int __stdcall D3dTextureSetCurrent_2B56110(SHardwareTexture *pHardwareTexture);

void CC gbh_DrawTriangle(int triFlags, STexture* pTexture, Vert* pVerts, int diffuseColour)
{
    if (gProxyOnly)
    {
        return gFuncs.pgbh_DrawTriangle(triFlags, pTexture, pVerts, diffuseColour);
    }

    if (pVerts[0].z <= 0.0f)
    {
        return;
    }

    if (Clipped(pVerts, 3))
    {
        return;
    }

    SetRenderStates_E02960(triFlags);

    // TODO: All duplicated in quad rendering func
    if (triFlags & 0x20000)
    {
        if (!bPointFilteringOn_E48604)
        {
            bPointFilteringOn_E48604 = 1;
            (*gD3dPtr_dword_21C85E0)->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DTFG_POINT);
            (*gD3dPtr_dword_21C85E0)->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DTFG_POINT);
        }
    }
    else
    {
        if (bPointFilteringOn_E48604)
        {
            bPointFilteringOn_E48604 = 0;
            (*gD3dPtr_dword_21C85E0)->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DTFG_LINEAR);
            (*gD3dPtr_dword_21C85E0)->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DTFG_LINEAR);
        }
    }

    // TODO: Refactor with quad drawing

    pTexture->field_13_flags &= 0xBF;
    pTexture->field_13_flags |= 0x40;
    if (pTexture->field_1C_ptr)
    {
        if (pTexture->field_13_flags & 0x80)
        {
            if (pTexture->field_13_flags & 0x40 && triFlags & 0x300)
            {
                TextureCache_E01EC0(pTexture);
                CacheFlushBatchRelated_2B52810(pTexture, triFlags);
                pTexture->field_13_flags &= 0xBF;
            }
            else if (pTexture->field_13_flags & 0x40 || triFlags & 0x300)
            {
                // Skip
            }
            else
            {
                TextureCache_E01EC0(pTexture);
                CacheFlushBatchRelated_2B52810(pTexture, triFlags);

                pTexture->field_13_flags |= 0x40;
            }
        }
    }
    else
    {
        CacheFlushBatchRelated_2B52810(pTexture, triFlags);
        auto v9 = pTexture->field_13_flags;
        if (triFlags & 0x300)
        {
            pTexture->field_13_flags &= 0xBF;

        }
        else
        {
            pTexture->field_13_flags |= 0x40;
        }
    }

    const auto pTextureCache = pTexture->field_1C_ptr;
    const auto pHardwareTexture = pTextureCache->field_24_texture_id;
    if ((*gActiveTextureId_dword_2B63DF4) != pHardwareTexture)
    {
        D3dTextureSetCurrent_2B56110(pTextureCache->field_24_texture_id);
        (*gActiveTextureId_dword_2B63DF4) = pHardwareTexture;
        ++gGlobals.mNumTextureSwaps;
        const auto v15 = pTextureCache->field_1C_pNext;
        pTextureCache->field_8_used_Frame_num = frame_number_2B93E4C;
        if (v15)
        {
            auto pCache = pTextureCache->field_20_pCache;

            if (pCache)
            {
                pCache->field_1C_pNext = v15;
            }
            else
            {
                gGlobals.cache_12_array_dword_E13D80[pTextureCache->field_6_cache_idx] = v15;
            }

            pTextureCache->field_1C_pNext->field_20_pCache = pTextureCache->field_20_pCache;
            const auto cacheIdx = pTextureCache->field_6_cache_idx;
            pTextureCache->field_1C_pNext = 0;
            pTextureCache->field_20_pCache = gGlobals.cache_12_array_dword_E13D80[cacheIdx];
            gGlobals.cache_12_array_dword_E13D80[cacheIdx]->field_1C_pNext = pTextureCache;
            gGlobals.cache_12_array_dword_E13D80[pTextureCache->field_6_cache_idx] = pTextureCache;
        }
    }

    const float uvScale = pTexture->field_1C_ptr->field_C;


    pVerts[0].w = pVerts[0].z;
    pVerts[1].w = pVerts[1].z;
    pVerts[2].w = pVerts[2].z;

    pVerts[0].u *= uvScale;
    pVerts[0].v *= uvScale;
    pVerts[1].u *= uvScale;
    pVerts[1].v *= uvScale;
    pVerts[2].u *= uvScale;
    pVerts[2].v *= uvScale;


    if (!(BYTE1(triFlags) & 0x20))
    {
        const auto finalDiffuseColour = (unsigned __int8)diffuseColour | (((unsigned __int8)diffuseColour | ((diffuseColour | 0xFFFFFF00) << 8)) << 8);
        pVerts[0].diff = finalDiffuseColour;
        pVerts[1].diff = finalDiffuseColour;
        pVerts[2].diff = finalDiffuseColour;
    }

    pVerts[0].spec = 0;
    pVerts[1].spec = 0;
    pVerts[2].spec = 0;


    if (BYTE1(triFlags) & 0x80 && gfAmbient_E10838 != 255.0)
    {
        LightVerts_new(3, pVerts, 0, diffuseColour);
    }

    if (SUCCEEDED((*gD3dPtr_dword_21C85E0)->field_28_ID3D_Device->DrawPrimitive(
        D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1, pVerts, 3, D3DDP_DONOTUPDATEEXTENTS)))
    {
        gGlobals.mNumPolysDrawn++;
    }

    return;
}


void CC gbh_DrawQuad(int quadFlags, STexture* pTexture, Vert* pVerts, int baseColour)
{

    if (gProxyOnly)
    {

         return gFuncs.pgbh_DrawQuad(quadFlags, pTexture, pVerts, baseColour);
    }

    // Flags meanings:
    // 0x10000 = fit quad and texture coords to texture size
    // 0x20000 = texture filtering, force enabled by 0x10000
    // 0x300 = alpha blending, 0x80 picks sub blending mode
    // 0x8000 lighting? or shadow
    // 0x2000 = use alpha in diffuse colour
    if (pVerts[0].z <= 0.0f)
    {
        return;
    }

    
    if (Clipped(pVerts, 4))
    {
        return;
    }

    if (quadFlags & 0x10000) // whatever this flag is turns point filtering on
    {
        quadFlags |= 0x20000;
    }

    SetRenderStates_E02960(quadFlags);

    if (quadFlags & 0x20000)
    {
        if (!bPointFilteringOn_E48604)
        {
            bPointFilteringOn_E48604 = 1;
            (*gD3dPtr_dword_21C85E0)->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DTFG_POINT);
            (*gD3dPtr_dword_21C85E0)->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DTFG_POINT);
        }
    }
    else
    {
        if (bPointFilteringOn_E48604)
        {
            bPointFilteringOn_E48604 = 0;
            (*gD3dPtr_dword_21C85E0)->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DTFG_LINEAR);
            (*gD3dPtr_dword_21C85E0)->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DTFG_LINEAR);
        }
    } 

    pTexture->field_13_flags &= 0xBF;
    pTexture->field_13_flags |= 0x40;

    if (pTexture->field_1C_ptr)
    {
        
        if (pTexture->field_13_flags & 0x80)
        {
            if (pTexture->field_13_flags & 0x40 && quadFlags & 0x300)
            {
                TextureCache_E01EC0(pTexture);
                CacheFlushBatchRelated_2B52810(pTexture, quadFlags);
                pTexture->field_13_flags &= 0xBF;
            }
            else if (pTexture->field_13_flags & 0x40 || quadFlags & 0x300)
            {
                // Skip
            }
            else
            {
                TextureCache_E01EC0(pTexture);
                CacheFlushBatchRelated_2B52810(pTexture, quadFlags);

                pTexture->field_13_flags |= 0x40;
            }
        }
    }
    else
    {
        CacheFlushBatchRelated_2B52810(pTexture, quadFlags);
        auto v9 = pTexture->field_13_flags;
        if (quadFlags & 0x300)
        {
            pTexture->field_13_flags &= 0xBF;

        }
        else
        {
            pTexture->field_13_flags |= 0x40;
        }
    }

    const auto pTextureCache = pTexture->field_1C_ptr;
    const auto pHardwareTexture = pTextureCache->field_24_texture_id;
    if ((*gActiveTextureId_dword_2B63DF4) != pHardwareTexture)
    {
        D3dTextureSetCurrent_2B56110(pTextureCache->field_24_texture_id);
        (*gActiveTextureId_dword_2B63DF4) = pHardwareTexture;
        ++gGlobals.mNumTextureSwaps;
        const auto v15 = pTextureCache->field_1C_pNext;
        pTextureCache->field_8_used_Frame_num = frame_number_2B93E4C;
        if (v15)
        {
            auto pCache = pTextureCache->field_20_pCache;

            if (pCache)
            {
                pCache->field_1C_pNext = v15;
            }
            else
            {
                gGlobals.cache_12_array_dword_E13D80[pTextureCache->field_6_cache_idx] = v15;
            }

            pTextureCache->field_1C_pNext->field_20_pCache = pTextureCache->field_20_pCache;
            const auto cacheIdx = pTextureCache->field_6_cache_idx;
            pTextureCache->field_1C_pNext = 0;
            pTextureCache->field_20_pCache = gGlobals.cache_12_array_dword_E13D80[cacheIdx];
            gGlobals.cache_12_array_dword_E13D80[cacheIdx]->field_1C_pNext = pTextureCache;
            gGlobals.cache_12_array_dword_E13D80[pTextureCache->field_6_cache_idx] = pTextureCache;
        }
    }

    const float uvScale = pTexture->field_1C_ptr->field_C;
    if (quadFlags & 0x10000)
    {
        const float textureW = (float)pTexture->field_E_width;
        const float textureH = (float)pTexture->field_10_height;

        if (gGpuSpecificHack_dword_2B63884)
        {
            //v19 = pVerts->mVerts[0].x;
            //floor(v19);
            //pVerts->mVerts[0].x = v19;
            //v20 = pVerts->mVerts[0].y;
            //floor(v20);
            //pVerts->mVerts[0].y = v20;
        }


        const float flt_E10830 = 0.001f;
        const auto v21 = pVerts[0].x + textureW;
        const auto v23 = pVerts[0].x + textureW - flt_E10830;
        const auto v24 = pVerts[0].y + textureH;


        pVerts[1].z = pVerts[0].z;
        pVerts[2].z = pVerts[0].z;
        pVerts[3].z = pVerts[0].z;

        pVerts[1].x = v23;
        pVerts[1].y = pVerts[0].y;

        pVerts[2].x = v21 - flt_E10830;
        pVerts[2].y = v24 - flt_E10830;

        pVerts[3].x = pVerts[0].x;
        pVerts[3].y = v24 - flt_E10830;

        pVerts[0].u = 0;
        pVerts[0].v = 0;

        float flt_E1082C = 0.0f; // 0.25 or 0, TODO: Set me in init

        pVerts[1].u = textureW - flt_E1082C;
        pVerts[1].v = 0;

        pVerts[2].u = textureW - flt_E1082C;
        pVerts[2].v = textureH - flt_E1082C;

        pVerts[3].u = 0;
        pVerts[3].v = textureH - flt_E1082C;
    }

    pVerts[0].w = pVerts[0].z;
    pVerts[1].w = pVerts[1].z;
    pVerts[2].w = pVerts[2].z;
    pVerts[3].w = pVerts[3].z;

    pVerts[0].u = uvScale * pVerts[0].u;
    pVerts[0].v = uvScale * pVerts[0].v;

    pVerts[1].u = uvScale * pVerts[1].u;
    pVerts[1].v = uvScale * pVerts[1].v;

    pVerts[2].u = uvScale * pVerts[2].u;
    pVerts[2].v = uvScale * pVerts[2].v;

    pVerts[3].u = uvScale * pVerts[3].u;
    pVerts[3].v = uvScale * pVerts[3].v;

    if (!(quadFlags & 0x2000))
    {
        // Force RGBA to be 255, 255, 255, A
        auto finalDiffuse = (unsigned __int8)baseColour | (((unsigned __int8)baseColour | ((baseColour | 0xFFFFFF00) << 8)) << 8);
        int f = finalDiffuse;
        // f++;
         //finalDiffuse = f;

        pVerts[0].diff = finalDiffuse;
        pVerts[1].diff = finalDiffuse;
        pVerts[2].diff = finalDiffuse;
        pVerts[3].diff = finalDiffuse;
    }

    pVerts[0].spec = 255;
    pVerts[1].spec = 255;
    pVerts[2].spec = 255;
    pVerts[3].spec = 255;

    if (quadFlags & 0x8000)
    {
        if (gfAmbient_E10838 != 255.0f)
        {
            LightVerts_new(4, pVerts, 0, baseColour);
        }
    }


    Vert myCopy[4];
    for (int i = 0; i < 4; i++)
    {
        myCopy[i] = pVerts[i];
    }

    (*gD3dPtr_dword_21C85E0)->field_28_ID3D_Device->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1, pVerts, 4, D3DDP_DONOTUPDATEEXTENTS);
    gGlobals.mNumPolysDrawn += 2;
}

void CC gbh_DrawQuadClipped(int a1, int a2, int a3, int a4, int a5)
{
    __debugbreak();
}

// TODO: Refactor/clean up this func
// Same as gbh_DrawTile
s32 CC gbh_DrawTilePart(unsigned int flags, STexture* pTexture, Vert* pData, int diffuseColour)
{
    if (gProxyOnly)
    {
         return gFuncs.pgbh_DrawTilePart(flags, pTexture, pData, diffuseColour);
    }

    auto oldFlags = flags;

    if (!(flags & 0x4000))
    {
        pData[0].u = 0.5f;
        pData[0].v = 0.5f;
        pData[1].u = 63.499901f;
        pData[1].v = 0.5f;
        pData[2].u = 63.499901f;
        pData[2].v = 63.499901f;
        pData[3].u = 0.5f;
        pData[3].v = 63.499901f;
    }

    struct uv { float u; float v; };
    uv uvs[4];
    for (int i = 0; i < 4; i++)
    {
        uvs[i].u = pData[i].u;
        uvs[i].v = pData[i].v;
    }


    // Rotate around the texture
    bool updated = false;
    switch (flags & 0x60)
    {
    case 0x20:
        pData[0].u = uvs[3].u;
        pData[0].v = uvs[3].v;

        pData[1].u = uvs[0].u;
        pData[1].v = uvs[0].v;

        pData[2].u = uvs[1].u;
        pData[2].v = uvs[1].v;

        pData[3].u = uvs[2].u;
        pData[3].v = uvs[2].v;
        updated = true;
        break;

    case 0x40:
        oldFlags = flags ^ 0x18;
        break;

    case 0x60:
        pData[0].u = uvs[1].u;
        pData[0].v = uvs[1].v;

        pData[1].u = uvs[2].u;
        pData[1].v = uvs[2].v;

        pData[2].u = uvs[3].u;
        pData[2].v = uvs[3].v;

        pData[3].u = uvs[0].u;
        pData[3].v = uvs[0].v;
        updated = true;
        break;
    }

    if (updated)
    {
        for (int i = 0; i < 4; i++)
        {
            uvs[i].u = pData[i].u;
            uvs[i].v = pData[i].v;
        }
    }

    // Flip up/down?
    if (oldFlags & 8)
    {
        pData[0].u = uvs[1].u;
        pData[0].v = uvs[1].v;

        pData[1].u = uvs[0].u;
        pData[1].v = uvs[0].v;

        pData[2].u = uvs[3].u;
        pData[2].v = uvs[3].v;

        pData[3].u = uvs[2].u;
        pData[3].v = uvs[2].v;
        if (oldFlags & 0x10)
        {
            for (int i = 0; i < 4; i++)
            {
                uvs[i].u = pData[i].u;
                uvs[i].v = pData[i].v;
            }
        }
    }

    // Flip left/right?
    if (oldFlags & 0x10)
    {
        pData[0].u = uvs[3].u;
        pData[0].v = uvs[3].v;

        pData[1].u = uvs[2].u;
        pData[1].v = uvs[2].v;

        pData[2].u = uvs[1].u;
        pData[2].v = uvs[1].v;

        pData[3].u = uvs[0].u;
        pData[3].v = uvs[0].v;
    }

    gbh_DrawQuad(oldFlags, pTexture, pData, diffuseColour);
    return 0;
}

void CC gbh_EndLevel()
{
    __debugbreak();
}

static BOOL __stdcall DeviceEndScene_E01C10(SD3dStruct* pD3d)
{
    return pD3d->field_28_ID3D_Device->EndScene() != 0;
}

double CC gbh_EndScene()
{
    if (gProxyOnly)
    {
        return gFuncs.pgbh_EndScene();
    }

    DeviceEndScene_E01C10((*gD3dPtr_dword_21C85E0));
    if (gbSurfaceIsFreed_E43E18 == 1)
    {
        (*gpVideoDriver_E13DC8->field_84_from_initDLL->pVid_GetSurface)(gpVideoDriver_E13DC8);
    }
    double result = qword_2B60848 / k1_2B638A0;
    gGlobals.mSceneTime_2B93EAC = qword_2B60848 / k1_2B638A0;// always 0 / 1 ?
    return result;
}

int CC gbh_FreeImageTable()
{
    if (gProxyOnly)
    {
        return gFuncs.pgbh_FreeImageTable();
    }

    if (gpImageTableCount_dword_E13898 <= 0)
    {
        free(gpImageTable_dword_E13894);
    }
    else
    {
        for (int idx = 0; idx < gpImageTableCount_dword_E13898; idx++)
        {
            if (gpImageTable_dword_E13894[idx].bLoaded)
            {
                gpImageTable_dword_E13894[idx].field_C_pSurface->Release();
                gpImageTable_dword_E13894[idx].bLoaded = FALSE;
            }
        }
    }
    free(gpImageTable_dword_E13894);
    gpImageTable_dword_E13894 = 0;
    return 0;
}

void CC gbh_FreePalette(int palId)
{
    if (gProxyOnly)
    {
        return gFuncs.pgbh_FreePalette(palId);
    }

    free(pals_2B63E00[palId].mPData);
    pals_2B63E00[palId].mPData = 0;
}

void CC gbh_FreeTexture(STexture* pTexture)
{
    if (gProxyOnly)
    {
        return gFuncs.pgbh_FreeTexture(pTexture);
    }

    TextureCache_E01EC0(pTexture);
    free(pTexture);
}

u32* CC gbh_GetGlobals()
{
    if (gProxyOnly)
    {
        return gFuncs.pgbh_GetGlobals();
    }

    return (u32*)&gGlobals;
}

// Only called with do_mike / profiling debugging opt enabled
int CC gbh_GetUsedCache(int cacheIdx)
{
    if (gProxyOnly)
    {
        return gFuncs.pgbh_GetUsedCache(cacheIdx);
    }

    int usedCacheCount = 0;
    SCache* pCache = gGlobals.cache_12_array_dword_E13D80[cacheIdx];
    while (pCache)
    {
        if (pCache->field_8_used_Frame_num == frame_number_2B93E4C)
        {
            ++usedCacheCount;
        }
        pCache = pCache->field_1C_pNext;
    }
    return usedCacheCount;
}

static HRESULT WINAPI EnumD3DDevicesCallBack_E014A0(
    GUID FAR* lpGuid, 
    LPSTR lpDeviceDescription, 
    LPSTR lpDeviceName, 
    LPD3DDEVICEDESC pDeviceDesc1, 
    LPD3DDEVICEDESC pDeviceDesc2, 
    LPVOID pContext)
{
    LPD3DDEVICEDESC pDeviceDesc = pDeviceDesc1;
    auto dcmColorModel = pDeviceDesc->dcmColorModel;
    if (dcmColorModel == 0)
    {
        pDeviceDesc = pDeviceDesc2;
    }

    SD3dStruct* pCtx = (SD3dStruct*)pContext;
    if (!(pCtx->field_5C_pitchQ & pDeviceDesc->dwDeviceRenderBitDepth))
    {
        return 1;
    }

    const auto shadeCaps = pDeviceDesc->dpcTriCaps.dwShadeCaps;
    if (pDeviceDesc->dcmColorModel == 1 || shadeCaps & 8)
    {
        if (!(shadeCaps & 8))
        {
            if (!(shadeCaps & 4))
            {
                return 1;
            }

        }

        int deviceDescrptionIndex = lpDeviceDescription ? strlen(lpDeviceDescription) : 0;
        int deviceNameIndex = lpDeviceName ? strlen(lpDeviceName) : 0;

        S3DDevice* pDevice = (S3DDevice *)malloc(deviceNameIndex + deviceDescrptionIndex + 566);
        if (!pDevice)
        {
            return 0;
        }

        ++pCtx->field_10_num_enums;

        pDevice->field_1C_num_texture_enums = 0;
        pDevice->field_20_flags = 0;
        pDevice->field_230_next_ptr = 0;
        pDevice->field_C_first_texture_format = 0;
        pDevice->field_10_pFirst_TextureFormat = 0;
        pDevice->field_14_phead = 0;
        pDevice->field_18_p0x60_IDirect3dTexture2_list = 0;
        pDevice->field_0_id = pCtx->field_C_device_id_gen++;

        auto pFirstDevice = pCtx->field_8_pfirst_device;
        if (pFirstDevice)
        {
            pFirstDevice->field_230_next_ptr = pDevice;
            pCtx->field_8_pfirst_device = pDevice;
        }
        else
        {
            pCtx->field_8_pfirst_device = pDevice;
            pCtx->field_4_pnext_device = pDevice;
        }

        
        pDevice->field_220_device_guid = *lpGuid;
       
       // pDevice->field_124.wMaxTextureBlendStages = pDeviceDesc->wMaxTextureBlendStages; //  TODO: (char *)v13 + 544; ???

        pDevice->field_4_device_name = (char *)&pDevice->field_234_backing_buffer;

        char* v20 = (char *)&pDevice->field_234_backing_buffer + deviceNameIndex + 1;
        pDevice->field_8_device_description = v20;

        strcpy(v20, lpDeviceDescription);
        strcpy(pDevice->field_4_device_name, lpDeviceName);

        memcpy(&pDevice->field_24_deviceDesc, pDeviceDesc1, sizeof(D3DDEVICEDESC));
        
        // can't see how this would work 
        //memcpy(&pDevice->field_120_context, pContext, 0xFCu); // TODO: Actually copies to offset 0x120 the field before ??
        memcpy(&pDevice->field_124, pDeviceDesc2, sizeof(D3DDEVICEDESC));


        if (dcmColorModel != 0)
        {
            pDevice->field_20_flags |= 1u;
        }

        auto v21 = pDeviceDesc->dwDeviceZBufferBitDepth;

        if (BYTE1(v21) & 4)
        {
            pDevice->field_20_flags |= 2;
        }
        else if (BYTE1(v21) & 2)
        {
            pDevice->field_20_flags |= 4;
        }
        else
        {
            if (!(BYTE1(v21) & 1))
            {
                
            }
            else
            {
                pDevice->field_20_flags |= 8;
            }
        }

        if (pDeviceDesc->dpcTriCaps.dwDestBlendCaps & 2 && pDeviceDesc->dpcTriCaps.dwSrcBlendCaps & 2)
        {
            pDevice->field_20_flags |= 0x40u;
        }

        if (pDeviceDesc->dpcTriCaps.dwAlphaCmpCaps & 0x10)
        {
            pDevice->field_20_flags |= 0x10u;
        }

        if (pDeviceDesc->dpcTriCaps.dwTextureBlendCaps & 4)
        {
            pDevice->field_20_flags |= 0x20u;
        }

        if (BYTE1(pDeviceDesc->dwDevCaps) & 0x10)
        {
            pDevice->field_20_flags |= 0x80;
        }

        if (pDeviceDesc->dcmColorModel & 2)
        {
            pDevice->field_20_flags |= 1;
        }

        if (pDeviceDesc->dpcTriCaps.dwTextureFilterCaps & 0x3C)
        {
            pDevice->field_20_flags |= 2;
        }
        return 1;
    }
    
    return 1;
}

struct STextureFormat
{
    DWORD field_0_enum_index;
    DWORD field_4_dwRGBBitCount;
    DWORD field_8_aBitCount;
    DWORD field_C_aBitIndex;
    DWORD field_10_rBitCount;
    DWORD field_14_rBitIndex;
    DWORD field_18_gBitIndex;
    DWORD field_1C_gBitCount;
    DWORD field_20_bBitCount;
    DWORD field_24_bBitIndex;
    DWORD field_28_flags;
    struct STextureFormat* field_2C_next_texture_format;
    DWORD field_30;
    DWORD field_34;
    DWORD field_38;
    DWORD field_3C;
    DWORD field_40;
    DWORD field_44;
    DWORD field_48;
    DWORD field_4C;
    DWORD field_50;
    DWORD field_54;
    DWORD field_58;
    DWORD field_5C;
    DWORD field_60;
    DWORD field_64;
    DWORD field_68;
    DWORD field_6C;
    DWORD field_70;
    DWORD field_74;
    DWORD field_78;
    DWORD field_7C;
    DWORD field_80;
    DWORD field_84;
    DWORD field_88;
    DWORD field_8C;
    DWORD field_90;
    DWORD field_94;
    DWORD field_98;
    DDPIXELFORMAT field_9C_dd_texture_format;
};
static_assert(sizeof(STextureFormat) == 0xBC, "Wrong size STextureFormat");

struct SHardwareTexture
{
    DWORD field_0_texture_id;
    DWORD field_4_flags;
    DWORD field_8_bitCount;
    DWORD field_C_bBitIndex;
    DWORD field_10_bUnknown;
    DWORD field_14_bBitCount;
    DWORD field_18_gBitIndex;
    DWORD field_1C_gUnknown;
    DWORD field_20_gBitCount;
    DWORD field_24_rBitIndex;
    DWORD field_28_rUnknown;
    DWORD field_2C_rBitCount;
    DWORD field_30_aBitIndex;
    DWORD field_34_aUnknown;
    DWORD field_38_aBitCount;
    DWORD field_3C_locked_pixel_data;
    DWORD field_40_pitch;
    WORD field_44_width;
    WORD field_46_height;
    struct SD3dStruct* field_48_d3d_struct;
    struct SHardwareTexture* field_4C_pNext;
    struct SHardwareTexture* field_50_pther;
    IDirect3DTexture2* field_54_IDirect3dTexture2;
    IDirectDrawSurface4* field_58_other_surface;
    IDirectDrawSurface4* field_5C_psurface_for_texture;
//    DWORD field_60; 
};
static_assert(sizeof(SHardwareTexture) == 0x60, "Wrong size SHardwareTexture");

static void __stdcall ConvertPixelFormat_2B55A10(STextureFormat* pTextureFormat, const DDPIXELFORMAT* pDDFormat);
decltype(&ConvertPixelFormat_2B55A10) pConvertPixelFormat_2B55A10 = (decltype(&ConvertPixelFormat_2B55A10))0x4A10;

static unsigned int countSetBits(unsigned int value)
{
    int i = 0;
    for (i = 0; (value & 1); ++i)
    {
        value >>= 1;
        if (i >= 32)
        {
            return i;
        }
    }
    return i;
}

static unsigned int firstUnSetBitIndex(DWORD& value)
{
    int i = 0;
    for (i = 0; !(value & 1); ++i)
    {
        value >>= 1;
        if (i >= 32)
        {
            return i;
        }
    }
    return i;
}

// TODO: Test VS real
static void __stdcall ConvertPixelFormat_2B55A10(STextureFormat* pTextureFormat, const DDPIXELFORMAT* pDDFormat)
{
    //STextureFormat old = *pTextureFormat;
    //pConvertPixelFormat_2B55A10(&old, pDDFormat);

    pTextureFormat->field_4_dwRGBBitCount = pDDFormat->dwRGBBitCount;

    DWORD r = pDDFormat->dwRBitMask;
    unsigned int rBitIndex = 0;
    if (r & 1)
    {
        rBitIndex = 1;
    }
    else
    {
        rBitIndex = firstUnSetBitIndex(r);
    }

    const bool bHaveNoRedBits = rBitIndex == 32;

    if (bHaveNoRedBits)
    {
        pTextureFormat->field_14_rBitIndex = 0;
        pTextureFormat->field_10_rBitCount = 0;
        pTextureFormat->field_24_bBitIndex = 0;
        pTextureFormat->field_20_bBitCount = 0;
        pTextureFormat->field_1C_gBitCount = 0;
        pTextureFormat->field_18_gBitIndex = 0;
        return;
    }
    pTextureFormat->field_14_rBitIndex = rBitIndex;

    // TODO: This actually counted set bits up to the first non set bit, and the real function counted unset to set bits and returned the resulting value
    pTextureFormat->field_10_rBitCount = countSetBits(r);

    DWORD g = pDDFormat->dwGBitMask;
    pTextureFormat->field_1C_gBitCount = firstUnSetBitIndex(g);
    pTextureFormat->field_18_gBitIndex = countSetBits(g);

    DWORD b = pDDFormat->dwBBitMask;
    pTextureFormat->field_24_bBitIndex = firstUnSetBitIndex(b);
    pTextureFormat->field_20_bBitCount = countSetBits(b);

    pTextureFormat->field_C_aBitIndex = 0;
    pTextureFormat->field_8_aBitCount = 0;

    if (pDDFormat->dwFlags & DDPF_ALPHAPIXELS)
    {
        auto rgbMask = (pDDFormat->dwBBitMask | pDDFormat->dwRBitMask | pDDFormat->dwGBitMask);

        // Get the alpha bits ??
        switch (pDDFormat->dwRGBBitCount)
        {
        case 8:
            rgbMask ^= 0xFFu;
            break;
        case 16:
            rgbMask ^= 0xFFFFu;
            break;
        case 24:
            rgbMask ^= 0xFFFFFFu;
            break;
        case 32:
            rgbMask = ~rgbMask;
            break;
        }

        if (rgbMask)
        {
            auto idx = firstUnSetBitIndex(rgbMask);
            pTextureFormat->field_C_aBitIndex = idx;
            pTextureFormat->field_8_aBitCount = countSetBits(rgbMask);

            if (idx >= pTextureFormat->field_4_dwRGBBitCount)
            {
                pTextureFormat->field_C_aBitIndex = 0;
                pTextureFormat->field_8_aBitCount = 0;
            }
            OutputDebugStringA("ALPHA - ");
        }
    }

    char buffer[120] = {};
    wsprintfA(buffer,
        "AlphaIn = %0x, Size<%d>, Shift<%d>",
        pDDFormat->dwRGBAlphaBitMask,
        pTextureFormat->field_8_aBitCount,
        pTextureFormat->field_C_aBitIndex);
    OutputDebugStringA(buffer);

    /*
    assert(old.field_4_dwRGBBitCount == pTextureFormat->field_4_dwRGBBitCount);
    assert(old.field_8_aBitCount == pTextureFormat->field_8_aBitCount);
    assert(old.field_C_aBitIndex == pTextureFormat->field_C_aBitIndex);
    assert(old.field_10_rBitCount == pTextureFormat->field_10_rBitCount);
    assert(old.field_14_rBitIndex == pTextureFormat->field_14_rBitIndex);
    assert(old.field_18_gBitCount == pTextureFormat->field_18_gBitCount);
    assert(old.field_1C_gBitIndex == pTextureFormat->field_1C_gBitIndex);
    assert(old.field_20_bBitCount == pTextureFormat->field_20_bBitCount);
    assert(old.field_24_bBitIndex == pTextureFormat->field_24_bBitIndex);
    assert(old.field_28_flags == pTextureFormat->field_28_flags);
    */
}

static STextureFormat* FindTextureFormatHelper(SD3dStruct* pD3d, DWORD sizeToFind, DWORD flagsToMatch, bool flagsAndSizeValid, bool storeInFirstField)
{
    S3DDevice* device = pD3d->field_14_active_device;
    for (STextureFormat* result = device->field_C_first_texture_format; result; result = result->field_2C_next_texture_format)
    {
        // If flagsAndSizeValid is false then look at this format, otherwise only look at it if the size and flags match the search criteria
        if (!flagsAndSizeValid || (flagsAndSizeValid && (result->field_28_flags & flagsToMatch) && result->field_8_aBitCount == sizeToFind))
        {
            if (result->field_4_dwRGBBitCount == 16 || result->field_4_dwRGBBitCount == 15)
            {
                if (storeInFirstField)
                {
                    pD3d->field_20_texture_format = result;
                }
                else
                {
                    pD3d->field_1c_texture_format = result;
                }
                return result;
            }
        }
    }
    return nullptr;
}

static STextureFormat *__stdcall FindTextureFormat_2B55C60(SD3dStruct* pD3d, unsigned int flags);
decltype(&FindTextureFormat_2B55C60) pFindTextureFormat_2B55C60 = 0x0;


static STextureFormat *__stdcall FindTextureFormat_2B55C60(SD3dStruct* pD3d, unsigned int flags)
{
    STextureFormat* result = nullptr;
    S3DDevice* device = pD3d->field_14_active_device;

    // 0x80000000 = reverse flag, or smallest/largest first ?
    // 0x40000000 = skip 4 sized, only valid when 0x80000000 is enabled

    if (flags & 0x80000000)
    {
        if (!(flags & 0x40000000))
        {
            result = FindTextureFormatHelper(pD3d, 4, 0x8000, true, true);
            if (result)
            {
                return result;
            }
        }

        result = FindTextureFormatHelper(pD3d, 1, 0x8000, true, true);
        if (result)
        {
            return result;
        }


        return FindTextureFormatHelper(pD3d, 0, 0, false, true);
    }
    

    result = FindTextureFormatHelper(pD3d, 0, 0, false, false);
    if (result)
    {
        return result;
    }

    result = FindTextureFormatHelper(pD3d, 1, 0x8000, true, false);
    if (result)
    {
        return result;
    }

    return FindTextureFormatHelper(pD3d, 4, 0x8000, true, false);
}

static HRESULT CALLBACK EnumTextureFormatsCallBack_E05BA0(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext);
decltype(&EnumTextureFormatsCallBack_E05BA0) pEnumTextureFormatsCallBack_E05BA0 = 0x0;

static HRESULT CALLBACK EnumTextureFormatsCallBack_E05BA0(LPDDPIXELFORMAT lpDDPixFmt, LPVOID lpContext)
{
    S3DDevice* pDevice = (S3DDevice*)lpContext;
    const auto flags = lpDDPixFmt->dwFlags;
    if (!(flags & 0xC0000) && !(flags & 0x20002) && !(flags & 0x204) && flags & 0x40)
    {
        auto pTextureFormat = (STextureFormat *)malloc(sizeof(STextureFormat));
        if (!pTextureFormat)
        {
            return 0;
        }

        memset(pTextureFormat, 0, sizeof(STextureFormat));

        ++pDevice->field_1C_num_texture_enums;
        pTextureFormat->field_28_flags = 0;
        pTextureFormat->field_2C_next_texture_format = 0;
        pTextureFormat->field_0_enum_index = pDevice->field_1C_num_texture_enums;
        auto pFirst = pDevice->field_10_pFirst_TextureFormat;
        if (pFirst)
        {
            pFirst->field_2C_next_texture_format = pTextureFormat;
            pDevice->field_10_pFirst_TextureFormat = pTextureFormat;
        }
        else
        {
            pDevice->field_10_pFirst_TextureFormat = pTextureFormat;
            pDevice->field_C_first_texture_format = pTextureFormat;
        }
        
        memcpy(&pTextureFormat->field_9C_dd_texture_format, lpDDPixFmt, sizeof(DDPIXELFORMAT));

        if (lpDDPixFmt->dwFlags & 1)
        {
            pTextureFormat->field_28_flags |= 0x8000;
        }

        OutputDebugStringA("ENUM TEXTURE FORMAT - ");
        ConvertPixelFormat_2B55A10(pTextureFormat, lpDDPixFmt);
    }
    return 1;
}

static int CleanUpHelper(SD3dStruct* pRenderer)
{
    // TODO: Also attempts to free memory that can't be seen being assigned to anywhere

    if (pRenderer->field_60_IUnknown)
    {
        pRenderer->field_0_pVideoDriver->field_138_Surface->DeleteAttachedSurface(0, pRenderer->field_60_IUnknown);
        pRenderer->field_60_IUnknown->Release();
    }

    if (pRenderer->field_2C_IViewPort)
    {
        pRenderer->field_2C_IViewPort->Release();
    }
    return 1;
}

static SHardwareTexture *__stdcall TextureAlloc_2B55DA0(SD3dStruct* pD3d, int width, int height, int flags)
{
    STextureFormat* pTextureFormat = FindTextureFormat_2B55C60(pD3d, flags);
    if (pTextureFormat)
    {
        SHardwareTexture* pMem = (SHardwareTexture *)malloc(sizeof(SHardwareTexture));
        if (pMem)
        {
            memset(pMem, 0, sizeof(SHardwareTexture));
            pMem->field_48_d3d_struct = pD3d;

            D3DDEVICEDESC hardwareCaps = {};
            D3DDEVICEDESC softwareCaps = {};
            hardwareCaps.dwSize = sizeof(D3DDEVICEDESC);
            softwareCaps.dwSize = sizeof(D3DDEVICEDESC);
            pD3d->field_28_ID3D_Device->GetCaps(&hardwareCaps, &softwareCaps);

            DDSURFACEDESC2 surfaceDesc = {};
            memcpy(&surfaceDesc.ddpfPixelFormat, &pTextureFormat->field_9C_dd_texture_format, sizeof(DDPIXELFORMAT));
            surfaceDesc.dwWidth = width;
            surfaceDesc.dwFlags |= 0x1007u;
            surfaceDesc.dwSize = sizeof(DDSURFACEDESC2);// 124;
            surfaceDesc.dwHeight = height;

            // v21 == ?
            if (/*v21 &&*/ flags & 2)
            {
                surfaceDesc.ddsCaps.dwCaps = 0x4005000;
            }
            else
            {
                surfaceDesc.ddsCaps.dwCaps = 6144;
                pMem->field_4_flags |= 1;
            }


            surfaceDesc.ddsCaps.dwCaps2 = 4;
            surfaceDesc.ddsCaps.dwCaps |= 0x1000;

            if (BYTE1(pD3d->field_14_active_device->field_20_flags) & 2)
            {
                surfaceDesc.ddsCaps.dwCaps |= 0x401008;
                surfaceDesc.dwMipMapCount = 1;
                surfaceDesc.dwFlags |= 0x20000u;
            }

            if (FAILED(pD3d->field_0_pVideoDriver->field_120_IDDraw4->CreateSurface(&surfaceDesc, &pMem->field_5C_psurface_for_texture, 0)))
            {
                free(pMem);
                return 0;
            }

            pMem->field_5C_psurface_for_texture->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pMem->field_54_IDirect3dTexture2);
            if (!(flags & 2))
            {
                pMem->field_4_flags |= 4;
            }

            pMem->field_44_width = width;
            pMem->field_46_height = height;
            
            pMem->field_8_bitCount = pTextureFormat->field_4_dwRGBBitCount;

            /*
             nothing before this even though its usually 0x76 ?? Seems to be random value which you'd expect!
            .text:00005DBA mov byte ptr [esp+18h], 1
            .text:00005DBF mov byte ptr [esp+19h], 3
            .text:00005DC4 mov byte ptr [esp+1Ah], 7
            .text:00005DC9 mov byte ptr [esp+1Bh], 15
            .text:00005DCE mov byte ptr [esp+1Ch], 31
            .text:00005DD3 mov byte ptr [esp+1Dh], 63
            .text:00005DD8 mov byte ptr [esp+1Eh], 127
            */

            const BYTE unknown[] = { 0 /*not inited in real func??*/, 1,3,7,15,31,64,127 };

            // Blue
            pMem->field_14_bBitCount = unknown[pTextureFormat->field_20_bBitCount];
            pMem->field_C_bBitIndex = pTextureFormat->field_24_bBitIndex;
            pMem->field_10_bUnknown = 8 - pTextureFormat->field_20_bBitCount;

            // Green
            pMem->field_20_gBitCount = unknown[pTextureFormat->field_1C_gBitCount]; 
            pMem->field_18_gBitIndex = pTextureFormat->field_18_gBitIndex;
            pMem->field_1C_gUnknown = 16 - pTextureFormat->field_1C_gBitCount;

            // Red
            pMem->field_2C_rBitCount = unknown[pTextureFormat->field_10_rBitCount]; 
            pMem->field_24_rBitIndex = pTextureFormat->field_14_rBitIndex;
            pMem->field_28_rUnknown = 24 - pTextureFormat->field_10_rBitCount;

            // Alpha
            pMem->field_38_aBitCount = unknown[pTextureFormat->field_8_aBitCount];
            pMem->field_30_aBitIndex = pTextureFormat->field_C_aBitIndex;
            pMem->field_34_aUnknown = 32 - pTextureFormat->field_8_aBitCount;


            pMem->field_4_flags |= pTextureFormat->field_28_flags & 0x8000;

            auto pDevice = pD3d->field_14_active_device;
            if (pDevice->field_14_phead)
            {
                pDevice->field_18_p0x60_IDirect3dTexture2_list->field_4C_pNext = pMem;
                pD3d->field_14_active_device->field_18_p0x60_IDirect3dTexture2_list = pMem;
            }
            else
            {
                pDevice->field_14_phead = pMem;
                pD3d->field_14_active_device->field_18_p0x60_IDirect3dTexture2_list = pMem;
            }
            return pMem;
        }
    }
    return 0;
}

static signed int __stdcall D3dTextureSetCurrent_2B56110(SHardwareTexture *pHardwareTexture)
{
    SHardwareTexture* pOther = pHardwareTexture->field_50_pther;
    if (pHardwareTexture->field_4_flags & 1
        && pHardwareTexture->field_48_d3d_struct->field_14_active_device->field_20_flags & 1
        && pOther == 0) // TODO: Check last conditional
    {
        return 1;
    }
    else
    {
        pHardwareTexture = pOther;
        if (pHardwareTexture->field_5C_psurface_for_texture->IsLost() == DDERR_SURFACELOST)
        {
            pHardwareTexture->field_5C_psurface_for_texture->Restore();
            pHardwareTexture->field_58_other_surface->PageLock(0);
            const auto ret = pHardwareTexture->field_54_IDirect3dTexture2->Load(pHardwareTexture->field_50_pther->field_54_IDirect3dTexture2) == 1;
            if (ret)
            {
                pHardwareTexture->field_58_other_surface->Unlock(0);
            }
            else
            {
                pHardwareTexture->field_58_other_surface->PageUnlock(0);
            }
        }

        pHardwareTexture->field_48_d3d_struct->field_28_ID3D_Device->SetTexture(0,  pHardwareTexture->field_54_IDirect3dTexture2);
        return 0;
    }
}

static int __stdcall D3dTextureCopy_2B560E0(SHardwareTexture* pFirst, SHardwareTexture* pSecond)
{
    pFirst->field_58_other_surface = pSecond->field_5C_psurface_for_texture;
    pSecond->field_50_pther = pFirst;
    pFirst->field_50_pther = pSecond;
    pSecond->field_4_flags |= 2;
    pFirst->field_4_flags |= 2;
    return 0;
}

static int __stdcall ClearTexture_E061B0(SD3dStruct* pCtx)
{
    pCtx->field_28_ID3D_Device->SetTexture(0, 0);
    return 0;
}

static int __stdcall FreeD3dDThings_E016E0(SD3dStruct* pD3d)
{
    if (pD3d->field_18_current_id)
    {
        // Free texture formats
        STextureFormat* pCurrentTextureFormat = pD3d->field_14_active_device->field_C_first_texture_format;
        if (pCurrentTextureFormat)
        {
            struct STextureFormat* pTextureFormat = nullptr;
            do
            {
                pTextureFormat = pCurrentTextureFormat->field_2C_next_texture_format;
                free(pCurrentTextureFormat);
                pCurrentTextureFormat = pTextureFormat;
            } while (pTextureFormat);
        }

        pD3d->field_14_active_device->field_C_first_texture_format = 0;
        pD3d->field_14_active_device->field_10_pFirst_TextureFormat = 0;

        
        // Free hardware textures
        SHardwareTexture* pCurrentHardwareTexture = pD3d->field_14_active_device->field_14_phead;
        if (pCurrentHardwareTexture)
        {
            SHardwareTexture* pHardwareTexture = nullptr;
            do
            {
                pCurrentHardwareTexture->field_54_IDirect3dTexture2->Release();
                pCurrentHardwareTexture->field_5C_psurface_for_texture->Release();
                pHardwareTexture = pCurrentHardwareTexture->field_4C_pNext;
                free(pCurrentHardwareTexture);
                pCurrentHardwareTexture = pHardwareTexture;
            } while (pHardwareTexture);
        }

        pD3d->field_14_active_device->field_14_phead = 0;
        pD3d->field_14_active_device->field_18_p0x60_IDirect3dTexture2_list = 0;

        if (pD3d->field_2C_IViewPort)
        {
            pD3d->field_28_ID3D_Device->DeleteViewport(pD3d->field_2C_IViewPort);
            pD3d->field_2C_IViewPort->Release();
            pD3d->field_2C_IViewPort = 0;
        }
        if (pD3d->field_28_ID3D_Device)
        {
            pD3d->field_28_ID3D_Device->Release();
            pD3d->field_28_ID3D_Device = 0;
        }

        pD3d->field_18_current_id = 0;
        pD3d->field_14_active_device = 0;
    }
    return 0;
}

signed int __stdcall CreateD3DDevice_E01840(SD3dStruct* pRenderer);
decltype(&CreateD3DDevice_E01840) pCreateD3DDevice_E01840 = 0x0;

signed int __stdcall CreateD3DDevice_E01840(SD3dStruct* pRenderer)
{
    hack = pRenderer;
  
    //return pCreateD3DDevice_E01840(pRenderer);


    if (FAILED(pRenderer->field_24_pID3d->CreateDevice(
        pRenderer->field_14_active_device->field_220_device_guid,
        pRenderer->field_0_pVideoDriver->field_138_Surface,
        &pRenderer->field_28_ID3D_Device,
        nullptr)))
    {
        return CleanUpHelper(pRenderer);
    }

    if (FAILED(pRenderer->field_24_pID3d->CreateViewport(&pRenderer->field_2C_IViewPort, nullptr)))
    {
        return CleanUpHelper(pRenderer);
    }

    if (FAILED(pRenderer->field_28_ID3D_Device->AddViewport(pRenderer->field_2C_IViewPort)))
    {
        return CleanUpHelper(pRenderer);
    }

    if (FAILED(pRenderer->field_28_ID3D_Device->SetCurrentViewport(pRenderer->field_2C_IViewPort)))
    {
        return CleanUpHelper(pRenderer);
    }

    memset(&pRenderer->field_30_D3DVIEWPORT2, 0, sizeof(pRenderer->field_30_D3DVIEWPORT2));
    pRenderer->field_30_D3DVIEWPORT2.dwSize = sizeof(pRenderer->field_30_D3DVIEWPORT2);
    pRenderer->field_30_D3DVIEWPORT2.dwX = 0;
    pRenderer->field_30_D3DVIEWPORT2.dwY = 0;
    pRenderer->field_30_D3DVIEWPORT2.dwWidth = pRenderer->field_0_pVideoDriver->field_48_rect_right;
    pRenderer->field_30_D3DVIEWPORT2.dwHeight = pRenderer->field_0_pVideoDriver->field_4C_rect_bottom;
    pRenderer->field_30_D3DVIEWPORT2.dvClipWidth = pRenderer->field_0_pVideoDriver->field_48_rect_right;
    pRenderer->field_30_D3DVIEWPORT2.dvClipX = pRenderer->field_0_pVideoDriver->field_48_rect_right;
    pRenderer->field_30_D3DVIEWPORT2.dvClipY = pRenderer->field_0_pVideoDriver->field_48_rect_right;
    pRenderer->field_30_D3DVIEWPORT2.dvMinZ = pRenderer->field_0_pVideoDriver->field_48_rect_right;
    pRenderer->field_30_D3DVIEWPORT2.dvClipHeight = pRenderer->field_0_pVideoDriver->field_4C_rect_bottom;
    pRenderer->field_30_D3DVIEWPORT2.dvMaxZ = 500.0f;

    if (FAILED(pRenderer->field_2C_IViewPort->SetViewport2(&pRenderer->field_30_D3DVIEWPORT2)))
    {
        return CleanUpHelper(pRenderer);
    }

    return pRenderer->field_28_ID3D_Device->EnumTextureFormats(EnumTextureFormatsCallBack_E05BA0, pRenderer->field_14_active_device) != 0;
}

SD3dStruct* D3DCreate_E01300(SVideo* pVideoDriver)
{
    SD3dStruct* pd3d = reinterpret_cast<SD3dStruct*>(malloc(sizeof(SD3dStruct)));
    if (pd3d)
    {
        memset(pd3d, 0, sizeof(SD3dStruct));

        if (pVideoDriver->field_8C_DirectDraw7->QueryInterface(IID_IDirect3D3,
            (LPVOID*)&pd3d->field_24_pID3d))
        {
            free(pd3d);
            return 0;
        }

        pd3d->field_0_pVideoDriver = pVideoDriver;
        pd3d->field_C_device_id_gen = 1;

        DDSURFACEDESC2 surfaceDesc = {};
        surfaceDesc.dwSize = sizeof(DDSURFACEDESC2);
        
        DWORD pitch = 0;
        if (pVideoDriver->field_138_Surface->GetSurfaceDesc(&surfaceDesc) >= 0)
        {
            switch (surfaceDesc.ddpfPixelFormat.dwRGBBitCount)
            {
            case 1:     pitch = 16384;  break;
            case 2:     pitch = 8192;   break;
            case 4:     pitch = 4096;   break;
            case 8:     pitch = 2048;   break;
            case 16:    pitch = 1024;   break;
            case 24:    pitch = 512;    break;
            case 32:    pitch = 256;    break;
            default:    pitch = 0;      break;
            }
        }
        pd3d->field_5C_pitchQ = pitch;

        if (pd3d->field_24_pID3d->EnumDevices(EnumD3DDevicesCallBack_E014A0, pd3d) >= 0)
        {
            if (pd3d->field_10_num_enums)
            {
                return pd3d;
            }
        }

        free(pd3d);
        return 0;
    }
    return pd3d;
}

static void SetDeviceDefaultRenderStates_E01A90(SD3dStruct* pCtx)
{
    pCtx->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
    pCtx->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
    pCtx->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
    pCtx->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
    pCtx->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
    pCtx->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
    pCtx->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE);
    pCtx->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_ALPHAREF, 0x8000);
    pCtx->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_ALPHAFUNC, D3DCMP_GREATER);
    pCtx->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE, FALSE);
    pCtx->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_SUBPIXEL, TRUE);
    pCtx->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_TEXTUREPERSPECTIVE, TRUE);
    pCtx->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_DITHERENABLE, TRUE);
    pCtx->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DFILTER_LINEAR);
    pCtx->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DFILTER_LINEAR);
    pCtx->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
    pCtx->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_MIRROR);
    pCtx->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_TEXTUREADDRESS, D3DTADDRESS_CLAMP);
}

static signed int __stdcall Set3dDevice_E01B90(SD3dStruct* pContext, int id)
{
    if (pContext->field_18_current_id)
    {
        FreeD3dDThings_E016E0(pContext);
    }

    auto pDevice = pContext->field_4_pnext_device;
    if (!pDevice)
    {
        return 1;
    }

    while (pDevice->field_0_id != id)
    {
        pDevice = pDevice->field_230_next_ptr;
        if (!pDevice)
        {
            return 1;
        }
    }
    pContext->field_14_active_device = pDevice;
    pContext->field_18_current_id = id;

    if (CreateD3DDevice_E01840(pContext))
    {
        return 1;
    }
    
    SetDeviceDefaultRenderStates_E01A90(pContext);
    return 0;
}

static int CheckIfSpecialFindGfxEnabled_E02250()
{
    HKEY hKey = 0;
    DWORD Data = 1;
    DWORD cbData = sizeof(DWORD);
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\DMA Design Ltd\\GTA2\\Screen", 0, KEY_READ, &hKey) == ERROR_SUCCESS)
    {
        DWORD Type = 0;
        if (RegQueryValueExA(hKey, "special_recognition", 0, &Type, reinterpret_cast<BYTE*>(&Data), &cbData) != ERROR_SUCCESS)
        {
            Data = 1;
            RegSetValueExA(hKey, "special_recognition", 0, REG_DWORD, reinterpret_cast<BYTE*>(&Data), cbData);
        }
    }
    else
    {
        DWORD dwDisposition = 0;
        if (RegCreateKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\DMA Design Ltd\\GTA2\\Screen", 0, "", 0, KEY_ALL_ACCESS, 0, &hKey, &dwDisposition) == ERROR_SUCCESS)
        {
            RegSetValueExA(hKey, "special_recognition", 0, REG_DWORD, reinterpret_cast<BYTE*>(&Data), cbData);
        }
    }

    if (hKey)
    {
        RegCloseKey(hKey);
    }

    return Data;
}

static SHardwareTexture *__stdcall D3DTextureAllocate_2B560A0(SD3dStruct* pd3d, int width, int height, int flags);

decltype(&D3DTextureAllocate_2B560A0) pD3DTextureAllocate_2B560A0 = 0x0;

struct WidthFlags
{
    int width;
    int flags;
};
std::vector<WidthFlags> debugData;

static SHardwareTexture *__stdcall D3DTextureAllocate_2B560A0(SD3dStruct* pd3d, int width, int height, int flags)
{
    debugData.emplace_back(WidthFlags{ width, flags });

   // STextureFormat* pTextureFormat = FindTextureFormat_2B55C60(pd3d, flags);
   // pTextureFormat->field_20_bBitCount = 0;

//    auto real = pD3DTextureAllocate_2B560A0(pd3d, width, height, flags);
    auto re = TextureAlloc_2B55DA0(pd3d, width, height, flags  | 2);
    /*
    assert(real->field_0_texture_id == re->field_0_texture_id);
    assert(real->field_4_flags == re->field_4_flags);
    assert(real->field_8_bitCount == re->field_8_bitCount);
    
    assert(real->field_C_bBitIndex == re->field_C_bBitIndex);
    assert(real->field_10_bUnknown == re->field_10_bUnknown);
    assert(real->field_14_bBitCount == re->field_14_bBitCount);

    assert(real->field_18_gBitIndex == re->field_18_gBitIndex);
    assert(real->field_1C_gUnknown == re->field_1C_gUnknown);
    assert(real->field_20_gBitCount == re->field_20_gBitCount);

    assert(real->field_24_rBitIndex == re->field_24_rBitIndex);
    assert(real->field_28_rUnknown == re->field_28_rUnknown);
    assert(real->field_2C_rBitCount == re->field_2C_rBitCount);

    
    assert(real->field_30_aBitIndex == re->field_30_aBitIndex);
    assert(real->field_34_aUnknown == re->field_34_aUnknown);
    if (real->field_30_aBitIndex)
    {
        assert(real->field_38_aBitCount == re->field_38_aBitCount); // Possibly not inited sometimes? 0 seems to be random mem
    }

    assert(real->field_3C_locked_pixel_data == re->field_3C_locked_pixel_data);

    assert(real->field_40_pitch == re->field_40_pitch);

    assert(real->field_44_width == re->field_44_width);
    assert(real->field_46_height == re->field_46_height);
    */

    return re;
}

static SHardwareTexture *__stdcall TextureAllocLocked_2B560C0(SD3dStruct* pD3d, int width, int height, int flags)
{
    return TextureAlloc_2B55DA0(pD3d, width, height, flags | 1);
}


static int Init2_2B51F40();
decltype(&Init2_2B51F40) pInit2_2B51F40 = 0x0;

static DWORD bShift2_2B63D6C = 0;
static DWORD bMask_2B93E30 = 0;
static DWORD bShift_2B63D58 = 0;
static DWORD gShift_2B93E34 = 0;
static DWORD gMask_2B985FC = 0;
static DWORD gShift2_2B985EC = 0;
static DWORD rShift_2B63DB0 = 0;
static DWORD rMask_2B63DF0 = 0;
static DWORD rShift2_2B93E8C = 0;

static DWORD bShift2_2B63DEC = 0;
static DWORD gShift_2B63DC0 = 0;
static DWORD bMask_2B959D0 = 0;
static DWORD bShift_2B93E1C = 0;
static DWORD rShift_2B63DD4 = 0;
static DWORD gMask_2B93E94 = 0;
static DWORD gShift2_2B93E2C = 0;
static DWORD rMask_2B959D4 = 0;
static DWORD rShift2_2B63DD0 = 0;


// TODO
static int Init2_2B51F40()
{
    int flags = 0;
    for (int i=12; --i >= 0;  )
    {
        WORD width = texture_sizes_word_107E0[i];
        if (width & 0x400)
        {
            width &= 0xFBFF;
            flags = 0x80000000;
        }
        else
        {
            flags = 0;
        }
        gGlobals.cache_12_array_dword_E13D80[i] = 0;

        for (int j = 0; j < gGlobals.gCacheSizes_word_10810[i]; j++)
        {
            SCache* pCache = (SCache *)malloc(sizeof(SCache));
            memset(pCache, 0, sizeof(SCache));

            // Correct num calls + args
            SHardwareTexture* pTexture = D3DTextureAllocate_2B560A0((*gD3dPtr_dword_21C85E0), width, width, flags);
            if (!pTexture)
            {
                break;
            }

            // probably correct, TODO CHECK ME
            SHardwareTexture* pTexture2 = TextureAllocLocked_2B560C0((*gD3dPtr_dword_21C85E0), width, width, flags);
            if (!pTexture2)
            {
                break;
            }

            D3dTextureCopy_2B560E0(pTexture, pTexture2);

            pCache->field_0 = 0x8000u;
            pCache->field_2 = width;  // TODO: Check me
            pCache->field_4 = width; // TODO: Check me
            pCache->field_C = 1.0f / static_cast<float>(width);
            pCache->field_18_pSTexture = 0;
            pCache->field_8_used_Frame_num = frame_number_2B93E4C;
            pCache->field_6_cache_idx = i;
            pCache->field_24_texture_id = pTexture2;
            pCache->field_20_pCache = 0;
            pCache->field_1C_pNext = 0;

            if (gGlobals.cache_12_array_dword_E13D80[i])
            {
                pCache->field_20_pCache = gGlobals.cache_12_array_dword_E13D80[i];
                gGlobals.cache_12_array_dword_E13D80[i]->field_1C_pNext = pCache;
                gGlobals.cache_12_array_dword_E13D80[i] = pCache;
            }
            else
            {
                gGlobals.cache_12_array_dword_E13D80[i] = pCache;
            }
        }
    }

    ClearTexture_E061B0(*gD3dPtr_dword_21C85E0);

    (*renderStateCache_E43E24) = 0;
    bPointFilteringOn_E48604 = 0;

    // Same code as texture alloc func.. TODO: Merge into a helper
    const BYTE unknown[] = { 0 /*not inited in real func??*/, 1,3,7,15,31,64,127 };

    {
        STextureFormat format = {};
        ConvertPixelFormat_2B55A10(&format, &(*gD3dPtr_dword_21C85E0)->field_1c_texture_format->field_9C_dd_texture_format);



        rShift2_2B93E8C = format.field_24_bBitIndex;            // 0        // 43E8C
        rMask_2B63DF0 = unknown[format.field_20_bBitCount];     // 1f       // 13DF0
        rShift_2B63DB0 = 8 - format.field_20_bBitCount;         // 5        // 13DB0

        gShift2_2B985EC = format.field_1C_gBitCount;            // 5        //
        gMask_2B985FC = unknown[format.field_18_gBitIndex];     // 1f       //
        gShift_2B93E34 = 16 - format.field_18_gBitIndex;        // b        //

        bShift_2B63D58 = format.field_14_rBitIndex;             // a        //
        bMask_2B93E30 = unknown[format.field_10_rBitCount];     // 1f       //
        bShift2_2B63D6C = 24 - format.field_10_rBitCount;       // 13       //
    }


    {
        STextureFormat format = {};
        ConvertPixelFormat_2B55A10(&format, &(*gD3dPtr_dword_21C85E0)->field_20_texture_format->field_9C_dd_texture_format);

        rShift2_2B63DD0 = format.field_24_bBitIndex;            // 0 | 0
        rMask_2B959D4 = unknown[format.field_20_bBitCount];     // f | 5
        rShift_2B63DD4 = 8 - format.field_20_bBitCount;         // 4 | 5

        gShift2_2B93E2C = format.field_1C_gBitCount;            // 4 | 5
        gMask_2B93E94 = unknown[format.field_18_gBitIndex];     // f | 5
        gShift_2B63DC0 = 16 - format.field_18_gBitIndex;        // c | 5

        bShift_2B93E1C = format.field_14_rBitIndex;             // 8 | a
        bMask_2B959D0 = unknown[format.field_10_rBitCount];     // f | 5
        bShift2_2B63DEC = 24 - format.field_10_rBitCount;       // 14 | 5

        /*
        rShift2_2B63DD0 = 0x0;
        rMask_2B959D4 = 0xf;
        rShift_2B63DD4 = 0x4;

        gShift2_2B93E2C = 0x4;
        gMask_2B93E94 = 0xf;
        gShift_2B63DC0 = 0xc;

        bShift_2B93E1C = 0x8;
        bMask_2B959D0 = 0xf;
        bShift2_2B63DEC = 0x14;
        */
    }

    return 1;
}

static signed int Init_E02340()
{
    SD3dStruct* pD3d = D3DCreate_E01300(gpVideoDriver_E13DC8);
    (*gD3dPtr_dword_21C85E0) = pD3d;
    if (Set3dDevice_E01B90(pD3d, 2) != 1) // If it worked
    {
        memset(&gpVideoDriver_E13DC8->field_1C8_device_caps, 0, sizeof(DDCAPS));
        memset(&gpVideoDriver_E13DC8->field_344_hel_caps, 0, sizeof(DDCAPS));
        gpVideoDriver_E13DC8->field_1C8_device_caps.dwSize = 0x17C;
        gpVideoDriver_E13DC8->field_344_hel_caps.dwSize = 0x17C;
        auto err = gpVideoDriver_E13DC8->field_120_IDDraw4->GetCaps(&gpVideoDriver_E13DC8->field_1C8_device_caps, &gpVideoDriver_E13DC8->field_344_hel_caps);
        if (err)
        {
            D3dErr2String_E01000(err);
        }

        DDSCAPS2 ddcaps2 = {};
        ddcaps2.dwCaps = DDSCAPS_TEXTURE;

        DWORD totalMem = 0;
        DWORD freeMem = 0;
        gpVideoDriver_E13DC8->field_120_IDDraw4->GetAvailableVidMem(&ddcaps2, &totalMem, &freeMem);
        auto cacheSize = totalMem >> 21;

        char buffer[120] = {};
        wsprintfA(buffer, "CacheMul = %d/VideMul %d", totalMem >> 21, totalMem);
        OutputDebugStringA(buffer);

        if (cacheSize <= 0)
        {
            cacheSize = 1;
        }

        if (cacheSize > 2)
        {
            cacheSize = 2;
        }

        if (cacheSize == 1 && (*gD3dPtr_dword_21C85E0)->field_14_active_device->field_20_flags & 0x80)
        {
            cacheSize = 2;
        }

        D3DDEVICEDESC hardwareCaps = {};
        D3DDEVICEDESC softwareCaps = {};
        hardwareCaps.dwSize = 0xFC;
        softwareCaps.dwSize = 0xFC;
        (*gD3dPtr_dword_21C85E0)->field_28_ID3D_Device->GetCaps(&hardwareCaps, &softwareCaps);

        for (int i = 0; i < 12; i++)
        {
            gGlobals.gCacheSizes_word_10810[i] = gGlobals.gCacheUnknown_107F8[i] * cacheSize;
            gGlobals.gCacheSizes_dword_43EB0[i] = gGlobals.gCacheUnknown_107F8[i] * cacheSize;
        }
        
        Init2_2B51F40();
  
        for (int i = 0; i < 12; i++)
        {
            if (gGlobals.gCacheUnknown_107F8[i] && !gGlobals.gCacheSizes_dword_43EB0[i])
            {
                return 1;
            }
        }

        

        (*gActiveTextureId_dword_2B63DF4) = (SHardwareTexture*)-1;

        DDDEVICEIDENTIFIER deviceId = {};
        if (CheckIfSpecialFindGfxEnabled_E02250()
            && !(*gD3dPtr_dword_21C85E0)->field_0_pVideoDriver->field_120_IDDraw4->GetDeviceIdentifier(&deviceId, 0))
        {
            if (deviceId.dwVendorId == 4818)
            {
                if (deviceId.dwDeviceId == 24 || deviceId.dwDeviceId == 25)
                {
                    // TODO
                    //flt_2B60830 = 0;
                    //flt_2B6082C = 0x3E800000;

                    gGpuSpecificHack_dword_2B63884 = 1;
                    OutputDebugStringA("THIS IS A RIVA");
                    return 0;
                }
            }
            else if (deviceId.dwVendorId == 4098)
            {
                if (deviceId.dwDeviceId == 18242)
                {
                    gbIsAtiRagePro_dword_E13888 = 1;
                    OutputDebugStringA("THIS IS AN ATI_RAGEPRO");
                    return 0;
                }
            }
            else if (deviceId.dwVendorId == 4172 && deviceId.dwDeviceId == 15623)
            {
                gGpuSpecificHack_dword_2B63884 = 1;
                OutputDebugStringA("THIS IS AN PERMEDIA2 (with delta)");
            }
        }
        return 0;
    }

    return 1;
}

s32 CC gbh_Init(int a1)
{
    TRACE_ENTRYEXIT;

    if (gProxyOnly)
    {
        auto r = gFuncs.pgbh_Init(a1);
        return r;
    }

    
    int result = Init_E02340();
    if (!result)
    {
        gbh_SetColourDepth();
        result = 0;
    }
    return result;
}

static int CC gbh_SetMode_E04D80(SVideo* pVideoDriver, HWND hwnd, int modeId)
{
    pVideoDriver->field_34_active_device_id = 0;

    int result = (gVideoDriverFuncs.pVid_SetMode)(pVideoDriver, hwnd, modeId);
    if (!result)
    {
        result = Init_E02340();
    }
    return result;
}



decltype(&D3dTextureUnknown_2B561D0) pD3dTextureUnknown_2B561D0 = 0;


static signed int __stdcall D3dTextureUnknown_2B561D0(SHardwareTexture* pHardwareTexture, BYTE* pixelData, WORD* pPalData, int textureW, int textureH, int palSize, int renderFlags, char textureFlags)
{
    SVideo* pVideoDriver = pHardwareTexture->field_48_d3d_struct->field_0_pVideoDriver;
    memset(&pVideoDriver->field_13C_DDSurfaceDesc7, 0, sizeof(DDSURFACEDESC2));
    pVideoDriver->field_13C_DDSurfaceDesc7.dwSize = sizeof(DDSURFACEDESC2);

    if (pHardwareTexture->field_5C_psurface_for_texture->Lock(0, &pVideoDriver->field_13C_DDSurfaceDesc7, 1, 0))
    {
        return 1;
    }

    DWORD local_dword_2B63CF0 = 0;
    if (renderFlags & 0x80)
    {
        // Transparent
        local_dword_2B63CF0 = 0xFFFFFFFF >> pHardwareTexture->field_34_aUnknown;
        local_dword_2B63CF0 &= pHardwareTexture->field_38_aBitCount;
        local_dword_2B63CF0 <<= pHardwareTexture->field_30_aBitIndex;
    }
    else if (renderFlags & 0x380)
    {
        // Semi transparent
        local_dword_2B63CF0 = 0x88FFFFFF >> pHardwareTexture->field_34_aUnknown;
        local_dword_2B63CF0 &= pHardwareTexture->field_38_aBitCount;
        local_dword_2B63CF0 <<= pHardwareTexture->field_30_aBitIndex;
    }

    BYTE* pPixels = (BYTE*)pVideoDriver->field_13C_DDSurfaceDesc7.lpSurface;
    DWORD pitch = pVideoDriver->field_13C_DDSurfaceDesc7.lPitch;


    DWORD sourcePixelIndex = 0;
    for (int y = 0; y < textureH; y++)
    {
        for (int x = 0; x < textureW; x++)
        {
            const DWORD surfaceIndex = (x * 2 + (y*(pitch)));
            const BYTE palIndex = pixelData[sourcePixelIndex++];
            WORD palValue = pPalData[palIndex];
            
            if (palIndex != 0)
            {
                palValue |= local_dword_2B63CF0;
            }

            WORD* p = (WORD*)(&pPixels[surfaceIndex]);
            *p = palValue;
        }
        
        const DWORD val =  palSize - textureW;
        sourcePixelIndex += val;
    }
  

    if (textureW < pHardwareTexture->field_44_width)
    {
        // TODO: Clear remaining? Hardware texture is pre-cleared so probably don't need to?
    }
  
    if (textureH < pHardwareTexture->field_46_height)
    {
        // TODO: Clear remaining? Hardware texture is pre-cleared so probably don't need to?
    }

    if (!pHardwareTexture->field_5C_psurface_for_texture->Unlock(0))
    {
        pHardwareTexture->field_3C_locked_pixel_data = 0;
        pHardwareTexture->field_40_pitch = 0;
    }
    
    auto field_50_pther = pHardwareTexture->field_50_pther;
    if (field_50_pther)
    {
        field_50_pther->field_5C_psurface_for_texture->Restore();
        field_50_pther->field_58_other_surface->PageLock(0);
        auto hr = field_50_pther->field_54_IDirect3dTexture2->Load(field_50_pther->field_50_pther->field_54_IDirect3dTexture2);
        auto pSurface = field_50_pther->field_58_other_surface;
        if (hr == 1)
        {
            pSurface->Unlock(0);
            return 0;
        }
        pSurface->PageUnlock(0);
    }

    return 0;
}


decltype(&gbh_DrawQuad) pgbh_DrawQuad = 0x0;
decltype(&TextureAlloc_2B55DA0) pTextureAlloc_2B55DA0 = 0x0;
decltype(&TextureCache_E01EC0) pTextureCache_E01EC0 = 0x0;
decltype(&D3dTextureSetCurrent_2B56110) pD3dTextureSetCurrent_2B56110 = 0x0;


static void InstallHooks()
{
   // DetourAttach((PVOID*)(&pConvertPixelFormat_2B55A10), (PVOID)ConvertPixelFormat_2B55A10);
    //DetourAttach((PVOID*)(&pD3DTextureAllocate_2B560A0), (PVOID)D3DTextureAllocate_2B560A0);
   // DetourAttach((PVOID*)(&pFindTextureFormat_2B55C60), (PVOID)FindTextureFormat_2B55C60);

    
   
    //DetourAttach((PVOID*)(&pCreateD3DDevice_E01840), (PVOID)CreateD3DDevice_E01840);
    /*
    DetourAttach((PVOID*)(&pCacheFlushBatchRelated_2B52810), (PVOID)CacheFlushBatchRelated_2B52810);
    DetourAttach((PVOID*)(&pD3dTextureUnknown_2B561D0), (PVOID)D3dTextureUnknown_2B561D0);
   // DetourAttach((PVOID*)(&pgbh_DrawQuad), (PVOID)gbh_DrawQuad);
   
    DetourAttach((PVOID*)(&pTextureCache_E01EC0), (PVOID)TextureCache_E01EC0);
    DetourAttach((PVOID*)(&pD3dTextureSetCurrent_2B56110), (PVOID)D3dTextureSetCurrent_2B56110);
    DetourAttach((PVOID*)(&pSetRenderStates_E02960), (PVOID)SetRenderStates_E02960);
    */
   //DetourAttach((PVOID*)(&pInit2_2B51F40), (PVOID)Init2_2B51F40);

   //DetourAttach((PVOID*)(&pEnumTextureFormatsCallBack_E05BA0), (PVOID)EnumTextureFormatsCallBack_E05BA0);

}


//SLightInternal lights_2B959E0[256]; // 459E0

void CC gbh_ResetLights()
{
    if (gProxyOnly)
    {
        gFuncs.pgbh_ResetLights();
    }
    numLights_2B93E38 = 0;
}

int CC gbh_AddLight(SLight* pLight)
{
    if (gProxyOnly)
    {
        auto ret = gFuncs.pgbh_AddLight(pLight);
        return ret;
    }

    DWORD idx = numLights_2B93E38;

    lights_2B959E0[idx].field_0_flags = pLight->field_0;
    lights_2B959E0[idx].field_4_brightness = (float)((pLight->field_0 & 0xFF)) * 0.0039215689;
    lights_2B959E0[idx].field_14_x = pLight->field_4_x;
    lights_2B959E0[idx].field_18_y = pLight->field_8_y;
    lights_2B959E0[idx].field_1C_z = pLight->field_C_z;

    lights_2B959E0[idx].field_20_r = (float)(((unsigned int)pLight->field_10_colour >> 16) & 0xFF) * 0.0039215689;
    lights_2B959E0[idx].field_24_g = (float)(((unsigned int)pLight->field_10_colour >> 8) & 0xFF) * 0.0039215689;
    lights_2B959E0[idx].field_28_b = (float)(pLight->field_10_colour & 0xFF) * 0.0039215689;

    lights_2B959E0[idx].field_8_radius =  ((pLight->field_0 >> 8) & 0xFF) * 0.0039215689 * 8.0;

    lights_2B959E0[idx].field_C_radius_squared = lights_2B959E0[idx].field_8_radius * lights_2B959E0[idx].field_8_radius;

    lights_2B959E0[idx].field_10_radius_normalized = 1.0 / lights_2B959E0[idx].field_8_radius;

    numLights_2B93E38++;

    return idx * sizeof(SLightInternal);
}

static void RebasePtrs(DWORD baseAddr)
{
    // Funcs
    pConvertPixelFormat_2B55A10 = (decltype(&ConvertPixelFormat_2B55A10))(baseAddr + 0x5A10);
    pD3DTextureAllocate_2B560A0 = (decltype(&D3DTextureAllocate_2B560A0))(baseAddr + 0x60A0);
    pFindTextureFormat_2B55C60 = (decltype(&FindTextureFormat_2B55C60))(baseAddr + 0x5C60);
    pInit2_2B51F40 = (decltype(&Init2_2B51F40))(baseAddr + 0x1F40);
    pCreateD3DDevice_E01840 = (decltype(&CreateD3DDevice_E01840))(baseAddr + 0x01840);
    pEnumTextureFormatsCallBack_E05BA0 = (decltype(&EnumTextureFormatsCallBack_E05BA0))(baseAddr + 0x5BA0);

    /*
  
    pD3dTextureUnknown_2B561D0 = (decltype(&D3dTextureUnknown_2B561D0))(baseAddr + 0x61D0);
    pCacheFlushBatchRelated_2B52810 = (decltype(&CacheFlushBatchRelated_2B52810))(baseAddr + 0x2810);

    pTextureCache_E01EC0 = (decltype(&TextureCache_E01EC0))(baseAddr + 0x01EC0);
    pD3dTextureSetCurrent_2B56110 = (decltype(&D3dTextureSetCurrent_2B56110))(baseAddr + 0x6110);
    pSetRenderStates_E02960 = (decltype(&SetRenderStates_E02960))(baseAddr + 0x2960);
    */

    // Vars
    //gGlobals.cache_12_array_dword_E13D80 = (decltype(gGlobals.cache_12_array_dword_E13D80))(baseAddr + 0x13D4C);
    gActiveTextureId_dword_2B63DF4 = (decltype(gActiveTextureId_dword_2B63DF4))(baseAddr + 0x13DF4);


    //real_texture_sizes_word_107E0 = (WORD*)(baseAddr + 0x107E0);
    //real_CacheSizes_word_10810 = (WORD*)(baseAddr + 0x10810);
    //real_CacheSizes_dword_43EB0 = (DWORD*)(baseAddr + 0x43EB0);


    //renderStateCache_E43E24 = (decltype(renderStateCache_E43E24))(baseAddr + 0x43E24);

    //DWORD off = baseAddr + 0x485E0;
    //hack = (SD3dStruct*)(off);
    //gD3dPtr_dword_21C85E0 = &hack;

}


u32 CC gbh_InitDLL(SVideo* pVideoDriver)
{
    TRACE_ENTRYEXIT;

    
    HMODULE hOld = LoadLibrary(L"C:\\Program Files (x86)\\Rockstar Games\\GTA2\\_d3ddll.dll");

   // if (gProxyOnly)
    {
        PopulateS3DFunctions(hOld, gFuncs);
    }

    if (gDetours || gRealPtrs)
    {
        pgbh_DrawQuad = (decltype(pgbh_DrawQuad))GetProcAddress(hOld, "gbh_DrawQuad");
        RebasePtrs((DWORD)hOld);
    }

    if (gDetours)
    {
        DetourTransactionBegin();
        InstallHooks();
        DetourUpdateThread(GetCurrentThread());
        DetourTransactionCommit();
    }

    gpVideoDriver_E13DC8 = pVideoDriver;

    if (gProxyOnly)
    {
        auto r = gFuncs.pgbh_InitDLL(pVideoDriver);
        return r;
    }
    


   
    PopulateSVideoFunctions(pVideoDriver->field_7C_self_dll_handle, gVideoDriverFuncs);
    
    pOldCloseScreen = (*pVideoDriver->field_84_from_initDLL->pVid_CloseScreen);
    pOldSetMode = (*pVideoDriver->field_84_from_initDLL->pVid_SetMode);

    *pVideoDriver->field_84_from_initDLL->pVid_CloseScreen = gbh_CloseScreen;
    //*pVideoDriver->field_84_from_initDLL->pVid_GetSurface = gVideoDriverFuncs.pVid_GetSurface;
    //*pVideoDriver->field_84_from_initDLL->pVid_FreeSurface = gVideoDriverFuncs.pVid_FreeSurface;
    *pVideoDriver->field_84_from_initDLL->pVid_SetMode = gbh_SetMode_E04D80;
    
    return 1;
}


signed int CC gbh_InitImageTable(int tableSize)
{
    if (gProxyOnly)
    {
        auto ret = gFuncs.pgbh_InitImageTable(tableSize);
        return ret;
    }
   
    gpImageTable_dword_E13894 = reinterpret_cast<SImageTableEntry*>(malloc(sizeof(SImageTableEntry) * tableSize));
    if (!gpImageTable_dword_E13894)
    {
        return -1;
    }
    memset(gpImageTable_dword_E13894, 0, sizeof(SImageTableEntry) * tableSize);
    gpImageTableCount_dword_E13898 = tableSize;
    return 0;
}

static WORD ConvertPixel(
    DWORD pixel,
    DWORD local_bMask_2B60828, DWORD local_bShift_2B93E00, DWORD local_Shift2_2B985F0,
    DWORD local_gMask_2B63DB4, DWORD local_gShift_2B93E84, DWORD local_gShift2_2B93E90,
    DWORD local_rMask_2B63DB8, DWORD local_rShift2_2B63D60, DWORD local_rShift_2B93E44
)
{
    DWORD r = pixel;
    DWORD g = pixel;
    DWORD b = pixel >> local_Shift2_2B985F0;

    return
        (((unsigned __int16)local_bMask_2B60828 & (unsigned __int16)b)
            << local_bShift_2B93E00) |
            ((local_gMask_2B63DB4 & (g >> local_gShift_2B93E84)) << local_gShift2_2B93E90)
        | ((local_rMask_2B63DB8 & (r >> local_rShift_2B93E44)) << local_rShift2_2B63D60);
}

signed int CC gbh_LoadImage(SImage* pToLoad)
{
    if (gProxyOnly)
    {
        auto ret = gFuncs.pgbh_LoadImage(pToLoad);
        return ret;
    }

    DWORD freeImageIndex = 0;
    if (gpImageTableCount_dword_E13898 > 0)
    {
        SImageTableEntry* pFreeImage = gpImageTable_dword_E13894;
        do
        {
            if (!pFreeImage->bLoaded)
            {
                break;
            }
            ++freeImageIndex;
            ++pFreeImage;
        } while (freeImageIndex < gpImageTableCount_dword_E13898);
    }

    if (freeImageIndex >= gpImageTableCount_dword_E13898)
    {
        return -1;
    }

    if (!pToLoad || pToLoad->field_1 || pToLoad->field_2 != 2 || pToLoad->field_10 != 16)
    {
        return -2;
    }

    DDSURFACEDESC2 surfaceDesc = {};
    surfaceDesc.dwSize = sizeof(DDSURFACEDESC2);
    surfaceDesc.dwFlags = DDSD_HEIGHT | DDSD_WIDTH | DDSD_CAPS;
    surfaceDesc.ddsCaps.dwCaps = DDCAPS_BLT;
    if (gpVideoDriver_E13DC8->field_1C8_device_caps.dwCaps & DDCAPS_CANBLTSYSMEM)
    {
        surfaceDesc.ddsCaps.dwCaps = DDCAPS_BLT | DDCAPS_OVERLAY;
    }

    surfaceDesc.dwWidth = pToLoad->field_C_width;
    surfaceDesc.dwHeight = pToLoad->field_E_height;

    
    if (FAILED(gpVideoDriver_E13DC8->field_120_IDDraw4->CreateSurface(&surfaceDesc,
        &gpImageTable_dword_E13894[freeImageIndex].field_C_pSurface, 0)))
    {
        return -3;
    }
    

    if (FAILED(gpImageTable_dword_E13894[freeImageIndex].field_C_pSurface->Lock(0,
        &surfaceDesc,
        DDLOCK_NOSYSLOCK | DDLOCK_WAIT,
        0)))
    {
        return -3;
    }

    STextureFormat format = {};
    ConvertPixelFormat_2B55A10(&format, &surfaceDesc.ddpfPixelFormat);

    const DWORD shiftR = format.field_14_rBitIndex + format.field_10_rBitCount - 5;
    const DWORD shiftG = format.field_1C_gBitCount + format.field_18_gBitIndex - 5;
    const DWORD shiftB = format.field_24_bBitIndex + format.field_20_bBitCount - 5;


    BYTE* pPixels = (BYTE*)surfaceDesc.lpSurface;

    DWORD sourcePixelIndex = 0;
    for (int y = surfaceDesc.dwHeight-1; y >=0 ; y--)
    {
        for (int x = 0; x < surfaceDesc.dwWidth; x++)
        {
            const DWORD surfaceIndex = (x * 2 + (y*(surfaceDesc.lPitch)));
     
            BYTE* p = (BYTE*)(&pPixels[surfaceIndex]);
           
            BYTE* pSrc = (BYTE*)&pToLoad->field_12;
            pSrc += pToLoad->field_0;

            WORD pixelValue = ((WORD*)pSrc)[sourcePixelIndex];
            WORD r = (pixelValue & 0x7C00) >> 10;
            WORD g = (pixelValue & 0x03E0) >> 5;
            WORD b = pixelValue & 0x1F;
            WORD rgb = (r << shiftR) | (g << shiftG) | (b << shiftB);

            *(WORD*)p = rgb;
            
            sourcePixelIndex++;
        }
    }

    if (FAILED(gpImageTable_dword_E13894[freeImageIndex].field_C_pSurface->Unlock(NULL)))
    {
        // TODO: Real game bug, should be free'ing surface here?
        return -3;
    }

    gpImageTable_dword_E13894[freeImageIndex].bLoaded = TRUE;
    gpImageTable_dword_E13894[freeImageIndex].field_4_w = surfaceDesc.dwWidth;
    gpImageTable_dword_E13894[freeImageIndex].field_8_h = surfaceDesc.dwHeight;

    return freeImageIndex;
}

STexture* CC gbh_LockTexture(STexture* pTexture)
{
    if (gProxyOnly)
    {
        return gFuncs.pgbh_LockTexture(pTexture);
    }

    pTexture->field_8_locked_pixels_ptr = pTexture->field_14_original_pixel_data_ptr;
    pTexture->field_13_flags |= 1;
    pTexture->field_6_pal_size = 256;
    return TextureCache_E01EC0(pTexture);
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
    if (gProxyOnly)
    {
        auto ret = gFuncs.pgbh_RegisterPalette(paltId, pData);
        return ret;
    }

    DWORD* pOriginal = pData;

    // A pass to fix up the source data
    for (int i = 0; i < 256; i++)
    {
        if (i == 0)
        {
            *pData = 0; // colour 0 of each palette is always transparent
        }
        else
        {
            if (*pData == 0)
            {
                *pData = 0x10000; // Flag to mark other would be transparent stuff as not transparent?
            }
        }
        pData += 64; // Pal data is stored in columns not rows
    }

    
    WORD* pAllocatedData = (WORD *)malloc(1024u); // Space for 2 16bit pals

    pals_2B63E00[paltId].mPOriginalData = pOriginal;
    pals_2B63E00[paltId].mbLoaded = 1;
    pals_2B63E00[paltId].mPData = pAllocatedData;



    DWORD local_Shift2_2B985F0 = bShift2_2B63D6C;
    DWORD local_bMask_2B60828 = bMask_2B93E30;
    DWORD local_bShift_2B93E00 = bShift_2B63D58;
    DWORD local_gShift_2B93E84 = gShift_2B93E34;
    DWORD local_gMask_2B63DB4 = gMask_2B985FC;
    DWORD local_gShift2_2B93E90 = gShift2_2B985EC;
    DWORD local_rShift_2B93E44 = rShift_2B63DB0;
    DWORD local_rMask_2B63DB8 = rMask_2B63DF0;
    DWORD local_rShift2_2B63D60 = rShift2_2B93E8C;

    // Set the first pal to be a 16bit converted copy of the original
    pData = pOriginal;
    for (int i = 0; i < 256; i++)
    {
        pAllocatedData[i] = ConvertPixel(*pData,
            bMask_2B93E30, bShift_2B63D58, bShift2_2B63D6C,
            gMask_2B985FC, gShift_2B93E34, gShift2_2B985EC,
            rMask_2B63DF0, rShift2_2B93E8C, rShift_2B63DB0);

        pData += 64; // Pal data is stored in columns not rows
    }

    local_Shift2_2B985F0 = bShift2_2B63DEC;
    local_gShift_2B93E84 = gShift_2B63DC0;
    local_bMask_2B60828 = bMask_2B959D0;
    local_bShift_2B93E00 = bShift_2B93E1C;
    local_rShift_2B93E44 = rShift_2B63DD4;
    local_gMask_2B63DB4 = gMask_2B93E94;
    local_gShift2_2B93E90 = gShift2_2B93E2C;
    local_rMask_2B63DB8 = rMask_2B959D4;
    local_rShift2_2B63D60 = rShift2_2B63DD0;

    // Set the 2nd pal to be a 16bit texture format converted copy of the original
    WORD* pSecond = pAllocatedData + 256;
    pData = pOriginal;
    for (int i = 0; i < 256; i++)
    {
        pSecond[i] = ConvertPixel(*pData,
            bMask_2B959D0, bShift_2B93E1C, bShift2_2B63DEC,
            gMask_2B93E94, gShift_2B63DC0, gShift2_2B93E2C,
            rMask_2B959D4, rShift2_2B63DD0, rShift_2B63DD4);

        pData += 64; // Pal data is stored in columns not rows
    }

    return paltId; // TODO: Func probably doesn't really return anything?
}

STexture* CC gbh_RegisterTexture(__int16 width, __int16 height, BYTE* pData, int pal_idx, char a5)
{
    if (gProxyOnly)
    {
        auto r = gFuncs.pgbh_RegisterTexture(width, height, pData, pal_idx, a5);
        return r;
    }

    
    STexture* result = reinterpret_cast<STexture*>(malloc(sizeof(STexture)));
    if (!result)
    {
        return 0;
    }

    memset(result, 0, sizeof(STexture));

    result->field_0_id = gTextureId_dword_E13D54++;
    result->field_2 = 0;

    // TODO: What does this mean, should it have a de-ref to check if contains transparent colour ??
    result->field_4_pal_is_trans = LOBYTE(pals_2B63E00[pal_idx].mPData);

    result->field_E_width = width;
    result->field_6_pal_size = 0;
    result->field_8_locked_pixels_ptr = 0;
    result->field_C = 0;
    result->field_D = 0;
    result->field_10_height = height;
    result->field_12_bPalIsValid =  pals_2B63E00[pal_idx].mbLoaded;

    if (a5 && gbIsAtiRagePro_dword_E13888)
    {
        result->field_13_flags = 0x80u;
    }
    else
    {
        result->field_13_flags = 0;
    }
    result->field_14_original_pixel_data_ptr = pData;
    result->field_18_pPaltData = pals_2B63E00[pal_idx].mPData;
    result->field_1C_ptr = 0;

    return result;
}

void CC gbh_SetAmbient(float ambient)
{
    if (gProxyOnly)
    {
        gFuncs.pgbh_SetAmbient(ambient);
    }
    gfAmbient_E10838 = ambient * 255.0f;
}


int CC gbh_SetCamera(float a1, float a2, float a3, float a4)
{
    // This function seems to do nothing
    return 0;
}

int CC gbh_SetColourDepth()
{
    // TODO
    //__debugbreak();
   // return 1;
    if (gProxyOnly)
    {
        return gFuncs.pgbh_SetColourDepth();
    }
    return 1;
}

float CC gbh_SetWindow(float left, float top, float right, float bottom)
{
    float ret = 0.0f;
    if (gProxyOnly)
    {
        ret = gFuncs.pgbh_SetWindow(left, top, right, bottom);
    }

    gWindow_left_dword_E43E08 = left;
    gWindow_right_dword_E43E0C = right;
    gWindow_top_dword_E43E10 = top;
    gWindow_bottom_dword_E43E14 = bottom;
   // gWindow_d5_dword_E13DC4 = gpVideoDriver_E13DC8->field_4_flags & 1; // TODO: Never used?

    if (gProxyOnly)
    {
        return ret;
    }

    return bottom;
}

STexture* CC gbh_UnlockTexture(STexture* pTexture)
{
    if (gProxyOnly)
    {
        return gFuncs.pgbh_UnlockTexture(pTexture);
    }

    pTexture->field_6_pal_size = 0;
    pTexture->field_8_locked_pixels_ptr = 0;
    pTexture->field_13_flags &= 0xFEu;
    return pTexture;
}
