#include "stdafx.h"
#include "d3ddll.hpp"
#include <cmath>
#include <d3d.h>
#include <vector>
#include <iostream>
#include <set>
#include <algorithm>
#include "detours.h"

#define BYTEn(x, n)   (*((BYTE*)&(x)+n))
#define BYTE1(x)   BYTEn(x,  1)

#pragma comment(lib, "dxguid.lib")

static bool gProxyOnly = false;      // Pass through all functions to real DLL
static bool gDetours = false;       // Used in combination with gProxyOnly=true to hook some internal functions to test them in isolation
static bool gRealPtrs = true;

// Other
static S3DFunctions gFuncs;
static std::set<STexture*> gTextures;



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
    struct STextureFormat* field_20;
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
static DWORD g0x30Size_dword_E43F10[12] = {};
static DWORD gBatchFlushes_dword_2B93EA8 = 0;
static DWORD mNumTextureSwaps_2B93EA4 = 0;
static DWORD gNumPolysDrawn_dword_E43EA0 = 0;
static DWORD gActiveTextureId_dword_2B63DF4 = 0;
static double qword_2B60848 = 0;
static DWORD frame_number_2B93E4C = 0;

struct SGlobals
{
    DWORD mNumPolysDrawn;
    DWORD mNumTextureSwaps;
    DWORD mNumBatchFlushes;
    DWORD mSceneTime_2B93EAC;

    // Both statically set during init, cache sizes ?
    DWORD dword_2B93EB0[12];
    DWORD dword_2B93EE0[12];
    DWORD g0x30Size_dword_E43F10[12]; // Cache hit counters
};
static SGlobals gGlobals;


static DWORD bPointFilteringOn_E48604 = 0;
static DWORD renderStateCache_E43E24 = 0;

static SVideo* gpVideoDriver_E13DC8 = nullptr;
static struct SD3dStruct* gD3dPtr_dword_21C85E0;
static SVideoFunctions gVideoDriverFuncs;


static float gWindow_left_dword_E43E08;
static float gWindow_right_dword_E43E0C;
static float gWindow_top_dword_E43E10;
static float gWindow_bottom_dword_E43E14;
static DWORD gWindow_d5_dword_E13DC4;

static float k1_2B638A0;
static float gSceneTime_2B93EAC; // SGlobals

static DWORD gGpuSpecificHack_dword_2B63884 = 0;
static DWORD gbIsAtiRagePro_dword_E13888 = 0;


// TODO: Might contain hard coded table data for cache sizes?
static WORD word_2B607F8[12]; // 0x2B607F8 - 0x2B60810, 12 WORDS apart
static WORD word_2B60810[12];

static DWORD dword_2B93EB0[12]; // 0x2B93EB0-0x2B93EE0, 12 DWORDs apart
static DWORD dword_2B93EE0[12];

static int gScreenTableSize_dword_E13DCC = 0;
static int gScreenTable_dword_E43F40[1700];

static DWORD dword_2B93E88 = 0;
static DWORD dword_2B93E28 = 0;

static DWORD numLights_2B93E38 = 0;
static float gfAmbient_E10838 = 1.0f;

struct SImageTableEntry
{
    IDirectDrawSurface7* field_0_ddsurface;
    DWORD field_4;
    DWORD field_8;
    IDirectDrawSurface7* field_C_pSurface;
};
static SImageTableEntry*  gpImageTable_dword_E13894 = nullptr;
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

// Texture cache related
struct SCache* cache_12_array_dword_E13D80[12] = {};
struct SCache* gPtr_12_array_dword_E13D20[12] = {};




struct SCache
{
    BYTE field_0;
    BYTE field_1_flags;
    BYTE field_2;
    BYTE field_3;
    WORD field_4;
    WORD field_6_cache_idx;
    DWORD field_8_used_Frame_num;
    DWORD field_C;
    DWORD field_10;
    DWORD field_14;
    struct STexture* field_18_pSTexture;
    struct SCache* field_1C_pNext;
    struct SCache* field_20_pCache;
    struct SHardwareTexture* field_24_texture_id;
    DWORD field_28;
};
// TODO: Static size check of SCache

STexture *__stdcall TextureCache_E01EC0(STexture *pTexture)
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
                cache_12_array_dword_E13D80[pCache->field_6_cache_idx] = pCache->field_20_pCache;
            }

            // Set the first item to the root of the cache list ?
            pCache->field_1C_pNext = gPtr_12_array_dword_E13D20[pCache->field_6_cache_idx];

            gPtr_12_array_dword_E13D20[pCache->field_6_cache_idx]->field_20_pCache = pCache;
            gPtr_12_array_dword_E13D20[pCache->field_6_cache_idx] = pCache;
            pCache->field_20_pCache = 0;
        }
    }
    return pTexture;
}

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
    dword_2B93E88 = elementSize;
    dword_2B93E28 = elementSize >> 1;
    return result;
}

int CC gbh_AddLight(int a1)
{
    if (gProxyOnly)
    {
        return gFuncs.pgbh_AddLight(a1);
    }
    return 0;
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
        //pVid_FreeSurface(gpVideoDriver_E13DC8);
    }
    else
    {
        gbSurfaceIsFreed_E43E18 = false;
    }
    memset(g0x30Size_dword_E43F10, 0, sizeof(g0x30Size_dword_E43F10));
    gBatchFlushes_dword_2B93EA8 = 0;
    mNumTextureSwaps_2B93EA4 = 0;
    gNumPolysDrawn_dword_E43EA0 = 0;
    gActiveTextureId_dword_2B63DF4 = -1;
    qword_2B60848 = 0i64;
    ++frame_number_2B93E4C;
    return DeviceBeginScene_E01BF0(gD3dPtr_dword_21C85E0);
}

int CC gbh_BlitBuffer(int a1, int a2, int a3, int a4, int a5, int a6)
{
    __debugbreak();
    return 0;
}

char CC gbh_BlitImage(int a1, int a2, int a3, int a4, int a5, int a6, int a7)
{
    //__debugbreak();
    if (gProxyOnly)
    {
        return gFuncs.pgbh_BlitImage(a1, a2, a3, a4, a5, a6, a7);
    }
    return 0;
}

void CC gbh_CloseDLL()
{
//    __debugbreak();
}

void CC gbh_CloseScreen(SVideo* pVideo)
{
//    __debugbreak();
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

static bool NotClipped(Vert* pVerts, int count)
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

    return (maxX <= gWindow_right_dword_E43E0C
        && minX >= gWindow_left_dword_E43E08
        && maxY <= gWindow_bottom_dword_E43E14
        && minY >= gWindow_top_dword_E43E10);
}


static void SetRenderStates_E02960(int states)
{
    if (states & 0x380)
    {
        if (states & 0x80) // BYTE1(states) & 2
        {
            auto result = renderStateCache_E43E24;
            if (renderStateCache_E43E24 == 1)
            {
                result = 0;
                renderStateCache_E43E24 = 0;
            }

            if (!result)
            {
                renderStateCache_E43E24 = 2;
                gD3dPtr_dword_21C85E0->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1); // 27, 1
                gD3dPtr_dword_21C85E0->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE); // 19, 1
                gD3dPtr_dword_21C85E0->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE); // 20, 1
            }
        }
        else if (states & 0x180)
        {
            auto result = renderStateCache_E43E24;
            if (renderStateCache_E43E24 == 2)
            {
                result = 0;
                renderStateCache_E43E24 = 0;
            }
            if (!result)
            {
                renderStateCache_E43E24 = 1;

                gD3dPtr_dword_21C85E0->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA); // 21, 4
                gD3dPtr_dword_21C85E0->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1); // 27, 1
                gD3dPtr_dword_21C85E0->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA); // 19, 5
                gD3dPtr_dword_21C85E0->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA); // 20, 6
            }
        }
    }
    else
    {
        if (renderStateCache_E43E24)
        {
            gD3dPtr_dword_21C85E0->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0); // 27, 0
            renderStateCache_E43E24 = 0;
        }
    }
}

static STexture* pLast = nullptr;

#define HIBYTE(x)   (*((BYTE*)&(x)+1))

// TODO: Where is this populated?
static WORD word_2B607E0[12] = {};

// TODO
unsigned __int16 __stdcall CacheFlushBatchRelated_2B52810(STexture *pTexture, int renderFlags)
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
    while (biggestSide > word_2B607E0[cache_index])
    {
        if (++cache_index >= 12)
        {
            //cache_index = pTexture;
            break;
        }
    }

    auto pCache = gPtr_12_array_dword_E13D20[cache_index];
    if (pCache->field_8_used_Frame_num == frame_number_2B93E4C)
    {
        ++gBatchFlushes_dword_2B93EA8;
        gD3dPtr_dword_21C85E0->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_FLUSHBATCH, 1);
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
    //pPal = pTexture->field_18_pPalt;
    //v8 = pTexture->field_C + (pTexture->field_D << 8) + pTexture->field_14_data;
    pCache->field_10 = 0.00390625 / (double)pTexture->field_12_bPalIsValid;
    pCache->field_14 = 0.00390625 / (double)pTexture->field_12_bPalIsValid * 255.0;
    auto textureFlags = pTexture->field_13_flags;

    /*
    TODO
    D3dTextureUnknown_2B561D0(
        flagsCopy,
        (int)pCache->field_24_pInternalTexture,
        v8,
        flagsCopy & 0x380 ? (pPal + 512) : pPal,
        pTexture->field_E_width,
        pTexture->field_10_height,
        256,
        flagsCopy,
        textureFlags);
        */

    auto result = pCache->field_6_cache_idx;

    ++g0x30Size_dword_E43F10[result];

    return 0;
}


void CC gbh_DrawTriangle(int triFlags, STexture* pTexture, Vert* pVerts, int diffuseColour)
{
    if (gProxyOnly)
    {
        return gFuncs.pgbh_DrawTriangle(triFlags, pTexture, pVerts, diffuseColour);
    }

    if (pVerts[0].z <= 0.0f || !NotClipped(pVerts, 3))
    {
       // return;
    }

    SetRenderStates_E02960(triFlags);

    // TODO: All duplicated in quad rendering func
    if (triFlags & 0x20000)
    {
        if (!bPointFilteringOn_E48604)
        {
            bPointFilteringOn_E48604 = 1;
            gD3dPtr_dword_21C85E0->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DTFG_POINT);
            gD3dPtr_dword_21C85E0->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DTFG_POINT);
        }
    }
    else
    {
        if (bPointFilteringOn_E48604)
        {
            bPointFilteringOn_E48604 = 0;
            gD3dPtr_dword_21C85E0->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DTFG_LINEAR);
            gD3dPtr_dword_21C85E0->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DTFG_LINEAR);
        }
    }

    if (false)
    //if (pTexture->field_1C_ptr)
    {
        /*
        textureFlags = pTexture->field_13_flags;
        if (!(textureFlags & 0x80))
        {
            goto LABEL_40;
        }

        bTextureFlag0x40 = textureFlags & 0x40;
        if (bTextureFlag0x40 && triFlags & 0x300)
        {
            TextureCache_E01EC0(pTexture);
            CacheFlushBatchRelated_2B52810(pTexture, triFlags);
            newTextureFlags = pTexture->field_13_flags & 0xBF;
            goto LABEL_39;
        }

        if (bTextureFlag0x40 || triFlags & 0x300)
        {
            goto LABEL_40;
        }

        TextureCache_E01EC0(pTexture);
        CacheFlushBatchRelated_2B52810(pTexture, triFlags);
        textureFlagsCopy = pTexture->field_13_flags;
        */
    }
    else
    {
        CacheFlushBatchRelated_2B52810(pTexture, triFlags);
        //textureFlagsCopy = pTexture->field_13_flags;
        
        //if (triFlags & 0x300)
        {
            //newTextureFlags = textureFlagsCopy & 0xBF;
        LABEL_39:
            //pTexture->field_13_flags = newTextureFlags;
        LABEL_40:
            //pCache = pTexture->field_1C_pCache;
            //pHardwareTexture = pCache->field_24_pInternalTexture;

            /*
            if (gActiveTextureId_dword_2B63DF4 != pHardwareTexture)
            {
                D3dTextureSetCurrent_2B56110(pCache->field_24_pInternalTexture);
                gActiveTextureId_dword_2B63DF4 = pHardwareTexture;
                ++mNumTextureSwaps_2B93EA4;
                v14 = pCache->field_1C_pNext;
                pCache->field_8_used_Frame_num = frame_number_2B93E4C;
                if (v14)
                {
                    v15 = pCache->field_20_pCache;
                    if (v15)
                        v15->field_1C_pNext = v14;
                    else
                        gPtr_12_array_dword_E13D20[pCache->field_6_cache_idx] = v14;
                    pCache->field_1C_pNext->field_20_pCache = pCache->field_20_pCache;
                    cacheIdx = pCache->field_6_cache_idx;
                    pCache->field_1C_pNext = 0;
                    pCache->field_20_pCache = (SCache *)12_array_dword_E13D80[(unsigned __int16)cacheIdx];
                    *(_DWORD *)(12_array_dword_E13D80[(unsigned __int16)cacheIdx] + 28) = pCache;
                    12_array_dword_E13D80[pCache->field_6_cache_idx] = (int)pCache;
                }
            }
            */

            pVerts[0].w = pVerts[0].z;
            pVerts[1].w = pVerts[1].z;
            pVerts[2].w = pVerts[2].z;

           
            //v17 = pTexture->field_1C_pCache;
            //auto uvScale = v17->field_C_sizeQ;
            float uvScale = 1.0f;

            pVerts[0].u *= uvScale;
            pVerts[0].v *= uvScale;
            pVerts[1].u *= uvScale;
            pVerts[1].v *= uvScale;
            pVerts[2].u *= uvScale;
            pVerts[2].v *= uvScale;
            

            //if (!(BYTE1(triFlags) & 0x20))
            {
                const auto finalDiffuseColour = (unsigned __int8)diffuseColour | (((unsigned __int8)diffuseColour | ((diffuseColour | 0xFFFFFF00) << 8)) << 8);
                pVerts[0].diff = finalDiffuseColour;
                pVerts[1].diff = finalDiffuseColour;
                pVerts[2].diff = finalDiffuseColour;
            }

            pVerts[0].spec = 0;
            pVerts[1].spec = 0;
            pVerts[2].spec = 0;

            /*
            if (BYTE1(triFlags) & 0x80 && gfAmbient_E10838 != 255.0)
            {
                LightVerts_2B52A80(3, pVerts, 0, diffuseColour);
            }
            */

            if (SUCCEEDED(gD3dPtr_dword_21C85E0->field_28_ID3D_Device->DrawPrimitive(
                D3DPT_TRIANGLELIST, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1, pVerts, 3, D3DDP_DONOTUPDATEEXTENTS)))
            {
                gNumPolysDrawn_dword_E43EA0++;
            }

            return;
        }
    }

    //newTextureFlags = textureFlagsCopy | 0x40;
    //goto LABEL_39;
}


void CC gbh_DrawQuad(int flags, STexture* pTexture, Vert* pVerts, int baseColour)
{
    //return;

    if (gProxyOnly)
    {
        return gFuncs.pgbh_DrawQuad(flags, pTexture, pVerts, baseColour);
    }

    // Flags meanings:
    // 0x10000 = fit quad and texture coords to texture size
    // 0x20000 = texture filtering, force enabled by 0x10000
    // 0x300 = alpha blending, 0x80 picks sub blending mode
    // 0x8000 lighting? or shadow
    // 0x2000 = use alpha in diffuse colour
    
    if (pVerts[0].z > 0.0f)
    {
        //if (NotClipped(pVerts, 4))
        {
            if (flags & 0x10000) // whatever this flag is turns point filtering on
            {
                flags |= 0x20000;
            }
            
            SetRenderStates_E02960(flags);
            
            if (flags & 0x20000)
            {
                if (!bPointFilteringOn_E48604)
                {
                    bPointFilteringOn_E48604 = 1;
                    gD3dPtr_dword_21C85E0->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DTFG_POINT);
                    gD3dPtr_dword_21C85E0->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DTFG_POINT);
                }
            }
            else
            {
                if (bPointFilteringOn_E48604)
                {
                    bPointFilteringOn_E48604 = 0;
                    gD3dPtr_dword_21C85E0->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DTFG_LINEAR);
                    gD3dPtr_dword_21C85E0->field_28_ID3D_Device->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DTFG_LINEAR);
                }
            }
            

            if (pTexture->field_1C_ptr)
            {
                
                //if (!(pTexture->field_13_flags & 0x80))
                {
                    //goto LABEL_50; // Already have a field_1C_ptr
                }

                //const bool bHas0x40Flag = pTexture->field_13_flags & 0x40;
                //if (bHas0x40Flag && flags & 0x300)
                {
                    // Reinit field_1C_ptr?

                    //sub_E01EC0(pTexture);
                    //CacheFlushBatchRelated_2B52810(pTexture, flags);
                    //v10 = pTexture->field_13 & 0xBF;
                    //pTexture->field_13_flags = v10;
                    
                    //goto LABEL_49;
                }
                
                //if (bHas0x40Flag || flags & 0x300)
                {
                    // Nothing changed, can use field_1C_ptr?
                    //goto LABEL_50;
                }

                // Reinit field_1C_ptr? But also set 0x40
                //sub_E01EC0(pTexture);
                //CacheFlushBatchRelated_2B52810(pTexture, flags);
                //v9 = pTexture->field_13_flags;
                //v10 = v9 | 0x40;
                //pTexture->field_13_flags = v10;

                //goto LABEL_49;
            }
            else
            {
                CacheFlushBatchRelated_2B52810(pTexture, flags); // set/reinit field_1C_ptr
                //v9 = pTexture->field_13;

                //if (flags & 0x300) // Blending
                { // else goto label_49 / skip flag changes

                    //v10 = v9 & 0xBF;
                LABEL_49:
                    //pTexture->field_13_flags = v10;
                LABEL_50:
                    //v13 = pTexture->field_1C_ptr;
                    //v14 = *(_DWORD *)(v13 + 0x24);

                    
                   // if (dword_E13DF4 != v14)
                    {
                        //sub_E06110(*(_DWORD *)(v13 + 0x24));
                        //dword_E13DF4 = v14;
                        //++dword_E43EA4;
                       // v15 = *(_DWORD *)(v13 + 28);
                        //*(_DWORD *)(v13 + 8) = dword_E43E4C;
                        //if (v15)
                        {
                            //v16 = *(_DWORD *)(v13 + 32);
                            //if (v16)
                            //    *(_DWORD *)(v16 + 28) = v15;
                            //else
                            //    dword_E13D20[*(unsigned __int16 *)(v13 + 6)] = v15;
                            //*(_DWORD *)(*(_DWORD *)(v13 + 28) + 32) = *(_DWORD *)(v13 + 32);
                            //v17 = *(_WORD *)(v13 + 6);
                            //*(_DWORD *)(v13 + 28) = 0;
                           // *(_DWORD *)(v13 + 32) = dword_E13D80[v17];
                           // *(_DWORD *)(dword_E13D80[v17] + 28) = v13;
                           // dword_E13D80[*(unsigned __int16 *)(v13 + 6)] = v13;
                        }
                    }
                    

                    const auto flagsCopy = flags;
                    float uvScale = 1.0f;
                    //pVertsb = *(SVerts **)(pTexture->field_1C + 12);
                    //if (flags & 0x10000)
                    if (false)
                    {
                        const float textureW = pTexture->field_E_width;
                        const float textureH = pTexture->field_10_height;
                        
                        if (gGpuSpecificHack_dword_2B63884)
                        {
                            //v19 = pVerts->mVerts[0].x;
                            //floor(v19);
                            //pVerts->mVerts[0].x = v19;
                            //v20 = pVerts->mVerts[0].y;
                            //floor(v20);
                            //pVerts->mVerts[0].y = v20;
                        }
                        

                        const float flt_E10830 = 0.0f;
                        const auto v21 = pVerts[0].x + textureW;
                        const auto v23 = v21 - flt_E10830;
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

                        //pVerts[1].u = textureW - flt_E1082C;
                        pVerts[1].v = 0;

                        //pVerts[2].u = textureW - flt_E1082C;
                        //pVerts[2].v = textureH - flt_E1082C;

                        pVerts[3].u = 0;
                        //pVerts[3].v = textureH - flt_E1082C;
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

                   // if (!(flagsCopy & 0x2000))
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

                    if (flagsCopy & 0x8000)
                    {
                        //if (flt_E10838 != 255.0f)
                        {
                            //sub_E02A80(4, pVerts, 0, alpha);
                        }
                    }
                    
                    gD3dPtr_dword_21C85E0->field_28_ID3D_Device->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1, pVerts, 4, D3DDP_DONOTUPDATEEXTENTS);
                    gNumPolysDrawn_dword_E43EA0 += 2;

                    return;
                }
            }

            // dead - edited above
            // v10 = v9 | 0x40;
            // goto LABEL_49;
        }
    }
    

    if (pTexture)
    {
        pLast = pTexture;
    }

    if (!pTexture)
    {
        pTexture = pLast;
    }

    // flags = 0;


//    return gFuncs.pgbh_DrawQuad(flags, pTexture, pVerts, baseColour);
}

void CC gbh_DrawQuadClipped(int a1, int a2, int a3, int a4, int a5)
{
    __debugbreak();
}

// Same as gbh_DrawTile
s32 CC gbh_DrawTilePart(int flags, STexture* pTexture, Vert* pData, int diffuseColour)
{
    if (gProxyOnly)
    {
        return gFuncs.pgbh_DrawTilePart(flags, pTexture, pData, diffuseColour);
    }

    if (!(BYTE1(flags) & 0x40))
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
    
    flags &= 0x60u;

    switch (flags)
    {
    case 0x20:
    {
        const auto u0 = pData[0].u;
        const auto v0 = pData[0].v;
        pData[0].u = pData[3].u;
        pData[0].v = pData[3].v;
        pData[3].u = pData[2].u;
        pData[3].v = pData[2].v;
        const auto u1 = pData[1].u;
        pData[1].u = u0;
        pData[2].u = u1;
        pData[2].v = pData[1].v;
        pData[1].v = v0;
        break;
    }
    case 0x40:
        flags ^= 0x18u;
        break;
    case 0x60:
    {
        const auto u0 = pData[0].u;
        const auto v0 = pData[0].v;
        pData[0].u = pData[1].u;
        pData[0].v = pData[1].v;
        pData[1].u = pData[2].u;
        pData[1].v = pData[2].v;
        const auto u3 = pData[3].u;
        pData[3].u = u0;
        pData[2].u = u3;
        pData[2].v = pData[3].v;
        pData[3].v = v0;
        break;
    }
    }

    if (flags & 8)
    {
        const auto u0 = pData[0].u;
        const auto v0 = pData[0].v;
        pData[0].u = pData[1].u;
        const auto v1 = pData[1].v;
        pData[1].u = u0;
        pData[0].v = v1;
        const auto v12 = pData[3].u;
        pData[1].v = v0;
        const auto v3 = pData[3].v;
        const auto u2 = pData[2].u;
        pData[2].u = v12;
        pData[3].u = u2;
        pData[3].v = pData[2].v;
        pData[2].v = v3;
    }

    if (flags & 0x10)
    {
        const auto u0 = pData[0].u;
        const auto v0 = pData[0].v;
        pData[0].u = pData[3].u;
        const auto v3 = pData[3].v;
        pData[3].u = u0;
        pData[0].v = v3;
        const auto u1 = pData[1].u;
        pData[3].v = v0;
        const auto v1 = pData[1].v;
        const auto u2 = pData[2].u;
        pData[2].u = u1;
        pData[1].u = u2;
        pData[1].v = pData[2].v;
        pData[2].v = v1;
    }
    gbh_DrawQuad(flags, pTexture, pData, diffuseColour);
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

    DeviceEndScene_E01C10(gD3dPtr_dword_21C85E0);
    if (gbSurfaceIsFreed_E43E18 == 1)
    {
        (*gpVideoDriver_E13DC8->field_84_from_initDLL->pVid_GetSurface)(gpVideoDriver_E13DC8);
        //pVid_GetSurface(gpVideoDriver_E13DC8);
    }
    double result = qword_2B60848 / k1_2B638A0;
    gSceneTime_2B93EAC = qword_2B60848 / k1_2B638A0;// always 0 / 1 ?
    return result;
}

int CC gbh_FreeImageTable()
{
   // __debugbreak();
   // return 0;
    if (gProxyOnly)
    {
        return gFuncs.pgbh_FreeImageTable();
    }
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

    // TODO: Other stuff required
   // free(pTexture);
    gTextures.erase(pTexture);
    //gFuncs.pgbh_FreeTexture(pTexture);
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
    SCache* pCache = gPtr_12_array_dword_E13D20[cacheIdx];
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
    DWORD field_8_size;
    DWORD field_C_shift;
    DWORD field_10;
    DWORD field_14;
    DWORD field_18_bitcount;
    DWORD field_1C_bitcount;
    DWORD field_20_bitcount0;
    DWORD field_24_bitcount1;
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
    DWORD field_C;
    DWORD field_10;
    DWORD field_14;
    DWORD field_18;
    DWORD field_1C;
    DWORD field_20;
    DWORD field_24;
    DWORD field_28;
    DWORD field_2C;
    DWORD field_30;
    DWORD field_34;
    DWORD field_38_size;
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

static void __stdcall ConvertPixelFormat_2B55A10(STextureFormat* pTextureFormat, DDPIXELFORMAT* pDDFormat);
decltype(&ConvertPixelFormat_2B55A10) pConvertPixelFormat_2B55A10 = (decltype(&ConvertPixelFormat_2B55A10))0x4A10;

static void __stdcall ConvertPixelFormat_2B55A10(STextureFormat* pTextureFormat, DDPIXELFORMAT* pDDFormat)
{
    // TODO: Not impl, call real func
    pConvertPixelFormat_2B55A10(pTextureFormat, pDDFormat);
}

static STextureFormat* FindTextureFormatHelper(SD3dStruct* pD3d, DWORD sizeToFind, DWORD flagsToMatch, bool flagsAndSizeValid)
{
    S3DDevice* device = pD3d->field_14_active_device;
    for (STextureFormat* result = device->field_C_first_texture_format; result; result = result->field_2C_next_texture_format)
    {
        // If flagsAndSizeValid is false then look at this format, otherwise only look at it if the size and flags match the search criteria
        if (!flagsAndSizeValid || (flagsAndSizeValid && (result->field_28_flags & flagsToMatch) && result->field_8_size == sizeToFind))
        {
            if (result->field_4_dwRGBBitCount == 16 || result->field_4_dwRGBBitCount == 15)
            {
                pD3d->field_20 = result;
                return result;
            }
        }
    }
    return nullptr;
}

static STextureFormat *__stdcall FindTextureFormat_2B55C60(SD3dStruct* pD3d, int flags)
{
    STextureFormat* result = nullptr;
    S3DDevice* device = pD3d->field_14_active_device;

    // 0x80000000 = reverse flag, or smallest/largest first ?
    // 0x40000000 = skip 4 sized, only valid when 0x80000000 is enabled

    if (flags & 0x80000000)
    {
        if (!(flags & 0x40000000))
        {
            result = FindTextureFormatHelper(pD3d, 4, 0x8000, true);
            if (result)
            {
                return result;
            }
        }

        result = FindTextureFormatHelper(pD3d, 1, 0x8000, true);
        if (result)
        {
            return result;
        }


        return FindTextureFormatHelper(pD3d, 0, 0, false);
    }
    

    result = FindTextureFormatHelper(pD3d, 0, 0, false);
    if (result)
    {
        return result;
    }

    result = FindTextureFormatHelper(pD3d, 1, 0x8000, true);
    if (result)
    {
        return result;
    }

    return FindTextureFormatHelper(pD3d, 4, 0x8000, true);
}

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
            pTextureFormat->field_28_flags |= 0x80;
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
            pMem->field_48_d3d_struct = pD3d;
            pMem->field_0_texture_id = 0;
            pMem->field_4_flags = 0;
            pMem->field_4C_pNext = 0;
            pMem->field_50_pther = 0;
            pMem->field_3C_locked_pixel_data = 0;
            pMem->field_40_pitch = 0;
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

            pMem->field_58_other_surface = 0;
            pMem->field_5C_psurface_for_texture = 0;

            surfaceDesc.ddsCaps.dwCaps2 = 4;
            surfaceDesc.ddsCaps.dwCaps |= 0x10;

            if (pD3d->field_14_active_device->field_20_flags & 2)
            {
                surfaceDesc.ddsCaps.dwCaps |= 0x401008;
                surfaceDesc.dwMipMapCount = 1;
                surfaceDesc.dwFlags |= 0x20000u;
            }

            if (pD3d->field_0_pVideoDriver->field_120_IDDraw4->CreateSurface(&surfaceDesc, &pMem->field_5C_psurface_for_texture, 0))
            {
                free(pMem);
                return 0;
            }
            else
            {
                pMem->field_5C_psurface_for_texture->QueryInterface(IID_IDirect3DTexture2, (LPVOID*)&pMem->field_54_IDirect3dTexture2);
                if (!(flags & 2))
                {
                    pMem->field_4_flags |= 4;
                }

                pMem->field_44_width = width;
                pMem->field_46_height = height;
                pMem->field_8_bitCount = pTextureFormat->field_4_dwRGBBitCount;
                pMem->field_C = pTextureFormat->field_24_bitcount1;
                pMem->field_10 = 8 - pTextureFormat->field_20_bitcount0;

                /*
                // TODO: v15 == ??
                pMem->field_14 = (unsigned __int8)*(&v15 + pTextureFormat->field_20_bitcount0);
                pMem->field_18 = pTextureFormat->field_1C_bitcount;
                pMem->field_1C = 16 - pTextureFormat->field_18_bitcount;
                pMem->field_20 = (unsigned __int8)*(&v15 + pTextureFormat->field_18_bitcount);
                pMem->field_24 = pTextureFormat->field_14;
                pMem->field_28 = 24 - pTextureFormat->field_10;
                pMem->field_2C = (unsigned __int8)*(&v15 + pTextureFormat->field_10);
                pMem->field_30 = pTextureFormat->field_C_shift;
                pMem->field_34 = 32 - pTextureFormat->field_8_size;
                pMem->field_38_size = (unsigned __int8)*(&v15 + pTextureFormat->field_8_size);
                */

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

signed int __stdcall CreateD3DDevice_E01840(SD3dStruct* pRenderer)
{
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

static SHardwareTexture *__stdcall D3DTextureAllocate_2B560A0(SD3dStruct* pd3d, int width, int height, int flags)
{
    return TextureAlloc_2B55DA0(pd3d, width, height, flags | 2);
}

static SHardwareTexture *__stdcall TextureAllocLocked_2B560C0(SD3dStruct* pD3d, int width, int height, int flags)
{
    return TextureAlloc_2B55DA0(pD3d, width, height, flags | 1);
}



// TODO
static int Init2_2B51F40()
{
    return 1;
}

static signed int Init_E02340()
{
    SD3dStruct* pD3d = D3DCreate_E01300(gpVideoDriver_E13DC8);
    gD3dPtr_dword_21C85E0 = pD3d;
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

        if (cacheSize == 1 && gD3dPtr_dword_21C85E0->field_14_active_device->field_20_flags & 0x80)
        {
            cacheSize = 2;
        }

        D3DDEVICEDESC hardwareCaps = {};
        D3DDEVICEDESC softwareCaps = {};
        hardwareCaps.dwSize = 0xFC;
        softwareCaps.dwSize = 0xFC;
        gD3dPtr_dword_21C85E0->field_28_ID3D_Device->GetCaps(&hardwareCaps, &softwareCaps);

        DWORD idx = 0;
        do
        {
            word_2B60810[idx]  = word_2B607F8[idx] * cacheSize;
            dword_2B93EB0[idx] = word_2B607F8[idx] * cacheSize;
            ++idx;
        } while (idx < 12);
        
        Init2_2B51F40();
  
        idx = 0;
        do
        {
            if (word_2B607F8[idx] && !dword_2B93EE0[idx])
            {
                return 1;
            }
            ++idx;
        } while (idx < 12);
        

        gActiveTextureId_dword_2B63DF4 = -1;

        DDDEVICEIDENTIFIER deviceId = {};
        if (CheckIfSpecialFindGfxEnabled_E02250()
            && !gD3dPtr_dword_21C85E0->field_0_pVideoDriver->field_120_IDDraw4->GetDeviceIdentifier(&deviceId, 0))
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
    if (gProxyOnly)
    {
        return gFuncs.pgbh_Init(a1);
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

decltype(&CreateD3DDevice_E01840) pCreateD3DDevice_E01840 = (decltype(&CreateD3DDevice_E01840))0x01840;

static void InstallHooks()
{
    DetourAttach((PVOID*)(&pCreateD3DDevice_E01840), (PVOID)CreateD3DDevice_E01840);
    DetourAttach((PVOID*)(&pConvertPixelFormat_2B55A10), (PVOID)ConvertPixelFormat_2B55A10);
}

static void RebasePtrs(DWORD baseAddr)
{
    pCreateD3DDevice_E01840 = (decltype(&CreateD3DDevice_E01840))(baseAddr + 0x01840);
    pConvertPixelFormat_2B55A10 = (decltype(&ConvertPixelFormat_2B55A10))(baseAddr + 0x5A10);
}

u32 CC gbh_InitDLL(SVideo* pVideoDriver)
{
    HMODULE hOld = LoadLibrary(L"C:\\Program Files (x86)\\Rockstar Games\\GTA2\\_d3ddll.dll");

    if (gProxyOnly)
    {
        PopulateS3DFunctions(hOld, gFuncs);
    }

    if (gDetours || gRealPtrs)
    {
        RebasePtrs((DWORD)hOld);
    }

    if (gDetours)
    {
        DetourTransactionBegin();
        InstallHooks();
        DetourUpdateThread(GetCurrentThread());
        DetourTransactionCommit();
    }

    if (gProxyOnly)
    {
        return gFuncs.pgbh_InitDLL(pVideoDriver);
    }


    gpVideoDriver_E13DC8 = pVideoDriver;
    PopulateSVideoFunctions(pVideoDriver->field_7C_self_dll_handle, gVideoDriverFuncs);
    
    
    *pVideoDriver->field_84_from_initDLL->pVid_CloseScreen = gbh_CloseScreen;
    *pVideoDriver->field_84_from_initDLL->pVid_GetSurface = gVideoDriverFuncs.pVid_GetSurface;
    *pVideoDriver->field_84_from_initDLL->pVid_FreeSurface = gVideoDriverFuncs.pVid_FreeSurface;
    *pVideoDriver->field_84_from_initDLL->pVid_SetMode = gbh_SetMode_E04D80;
    
    return 1;
}


signed int CC gbh_InitImageTable(int tableSize)
{
    __debugbreak();

    if (gProxyOnly)
    {
        return gFuncs.pgbh_InitImageTable(tableSize);
    }
    return 0;

    /*
    gpImageTable_dword_E13894 = reinterpret_cast<SImageTableEntry*>(malloc(sizeof(SImageTableEntry) * tableSize));
    if (!gpImageTable_dword_E13894)
    {
        return -1;
    }
    memset(gpImageTable_dword_E13894, 0, sizeof(SImageTableEntry) * tableSize);
    gpImageTableCount_dword_E13898 = tableSize;
    return 0;
    */
}

signed int CC gbh_LoadImage(SImage* pToLoad)
{
    if (gProxyOnly)
    {
        return gFuncs.pgbh_LoadImage(pToLoad);
    }
    return 0;

    /*
    DWORD freeImageIndex = 0;
    if (gpImageTableCount_dword_E13898 > 0)
    {
        SImageTableEntry* pFreeImage = gpImageTable_dword_E13894;
        do
        {
            if (!pFreeImage->field_0_ddsurface)
            {
                break;
            }
            ++freeImageIndex;
            ++pFreeImage;
        } while (freeImageIndex < gpImageTableCount_dword_E13898);
    }
    
    if (freeImageIndex < gpImageTableCount_dword_E13898)
    {
        if (!pToLoad || pToLoad->field_1 || pToLoad->field_2 != 2 || pToLoad->field_10 != 16)
        {
            return -2;
        }
        else
        {
            DDSURFACEDESC2 surfaceDesc = {};
            surfaceDesc.dwSize = sizeof(DDSURFACEDESC2);
            surfaceDesc.dwFlags = 7;
            surfaceDesc.ddsCaps.dwCaps = 64;
            if (gpVideoDriver_E13DC8->field_1C8_device_caps.dwCaps & 0x80000000)
            {
                surfaceDesc.ddsCaps.dwCaps = 2112;
            }
            
            DWORD idx = 16 * freeImageIndex;

            // TODO: Partially implemented here
            if (gpVideoDriver_E13DC8->field_120_IDDraw4->CreateSurface(&surfaceDesc,
                (LPDIRECTDRAWSURFACE4 *)gpImageTable_dword_E13894->field_C_pSurface + idx, 0))
            {
               // result = -3;
            }
        }

        return -3;
    }
    else
    {
        return -1;
    }*/
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
    //__debugbreak();
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

    // TODO: Set for each format
    DWORD bMask_2B60828 = 0;
    DWORD bShift_2B93E00 = 0;
    DWORD bShift2_2B985F0 = 0;

    DWORD gMask_2B63DB4 = 0;
    DWORD gShift_2B93E84 = 0;
    DWORD gShift2_2B93E90 = 0;
    
    DWORD rMask_2B63DB8 = 0;
    DWORD rShift_2B93E44 = 0;
    DWORD rShift2_2B63D60 = 0;

    // Set the first pal to be a 16bit converted copy of the original
    pData = pOriginal;
    for (int i = 0; i < 256; i++)
    {
        DWORD r = *pData;
        DWORD g = *pData;
        DWORD b = (*pData) >> bShift2_2B985F0;

        pAllocatedData[i] =  (((unsigned __int16)bMask_2B60828 & (unsigned __int16)b) << bShift_2B93E00) 
            | ((gMask_2B63DB4 & (g >> gShift_2B93E84)) << gShift2_2B93E90) 
            | ((rMask_2B63DB8 & (r >> rShift_2B93E44)) << rShift2_2B63D60);

        // TEMP HACK
        pAllocatedData[i] = *pData;

        pData += 64; // Pal data is stored in columns not rows
    }

    // Set the 2nd pal to be a 16bit texture format converted copy of the original
    WORD* pSecond = pAllocatedData + 256;
    pData = pOriginal;
    for (int i = 0; i < 256; i++)
    {
        DWORD r = *pData;
        DWORD g = *pData;
        DWORD b = (*pData) >> bShift2_2B985F0;

        pSecond[i] = ((bMask_2B60828 & b) << bShift_2B93E00) 
            | ((gMask_2B63DB4 & (g >> gShift_2B93E84)) << gShift2_2B93E90)
            | ((rMask_2B63DB8 & (r >> rShift_2B93E44)) << rShift2_2B63D60);

        // TEMP HACK
        pAllocatedData[i] = *pData;

        pData += 64; // Pal data is stored in columns not rows
    }

    return paltId; // TODO: Func probably doesn't really return anything?
}

STexture* CC gbh_RegisterTexture(__int16 width, __int16 height, void* pData, int pal_idx, char a5)
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

void CC gbh_ResetLights()
{
    if (gProxyOnly)
    {
        gFuncs.pgbh_ResetLights();
    }
    numLights_2B93E38 = 0;
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
    if (gProxyOnly)
    {
        return gFuncs.pgbh_SetWindow(left, top, right, bottom);
    }

    gWindow_left_dword_E43E08 = left;
    gWindow_right_dword_E43E0C = right;
    gWindow_top_dword_E43E10 = top;
    gWindow_bottom_dword_E43E14 = bottom;
    gWindow_d5_dword_E13DC4 = gpVideoDriver_E13DC8->field_4_flags & 1; // TODO: Never used?
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
