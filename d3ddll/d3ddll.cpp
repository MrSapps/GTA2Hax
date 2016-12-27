#include "stdafx.h"
#include "d3ddll.hpp"
#include <cmath>
#include <d3d.h>
#include <vector>


#pragma comment(lib, "dxguid.lib")

static S3DFunctions gFuncs;

void CC ConvertColourBank(s32 unknown)
{
    // Empty/NOP in real game
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

std::vector<BYTE> hack(4096);

int CC MakeScreenTable(int* result, int a2, unsigned int a3)
{
    // TODO
    //__debugbreak();
    //return (int)hack.data();

    return gFuncs.pMakeScreenTable(result, a2, a3);
}

int CC gbh_AddLight(int a1)
{
    //__debugbreak();
    //return 0;
    return gFuncs.pgbh_AddLight(a1);
}

char CC gbh_AssignPalette(int a1, int a2)
{
    //__debugbreak();
    //return 0;
    return gFuncs.pgbh_AssignPalette(a1, a2);
}

void CC gbh_BeginLevel()
{
    __debugbreak();
}

int gbh_BeginScene()
{
    //__debugbreak();
    //return 0;

    return gFuncs.pgbh_BeginScene();
}

int CC gbh_BlitBuffer(int a1, int a2, int a3, int a4, int a5, int a6)
{
    __debugbreak();
    return 0;
}

char CC gbh_BlitImage(int a1, int a2, int a3, int a4, int a5, int a6, int a7)
{
    //__debugbreak();
    //return 0;
    return gFuncs.pgbh_BlitImage(a1, a2, a3, a4, a5, a6, a7);
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



STexture* pLast = nullptr;

void CC gbh_DrawQuad(int flags, STexture* pTexture, SPrim* pData, int alpha)
{
    // 0x10000 = rotation or scale related? 
    // 0x20000 =
    // 0x300 = semi trans/some other blending mode?
    // 0x8000 lighting? or shadow
    // 0x2000 = semi trans or use alpha?
    if (pTexture)
    {
        pLast = pTexture;
    }

    if (!pTexture)
    {
        pTexture = pLast;
    }

   // flags = 0;


    return gFuncs.pgbh_DrawQuad(flags, pTexture, pData, alpha);
}

void CC gbh_DrawQuadClipped(int a1, int a2, int a3, int a4, int a5)
{
    __debugbreak();
}

// Same as gbh_DrawTile
s32 CC gbh_DrawTilePart(int a1, int a2, int a3, int a4)
{
    //__debugbreak();
    //return 0;
    return gFuncs.pgbh_DrawTilePart(a1, a2, a3, a4);
}

void CC gbh_DrawTriangle(int a1, int a2, int a3, int a4)
{
//    __debugbreak();
}

void CC gbh_EndLevel()
{
    __debugbreak();
}

__int64 CC gbh_EndScene()
{
    //__debugbreak();
    //return 0;
    return gFuncs.pgbh_EndScene();
}

int CC gbh_FreeImageTable()
{
   // __debugbreak();
   // return 0;
    return gFuncs.pgbh_FreeImageTable();
}

void CC gbh_FreePalette(int a1)
{
//    __debugbreak();
}

void CC gbh_FreeTexture(STexture* pTexture)
{
    // TODO: Other stuff required
   // free(pTexture);
    gFuncs.pgbh_FreeTexture(pTexture);
}

u32 gTriangleCount = 0; // Some sort of counter


u32* CC gbh_GetGlobals()
{
    return &gTriangleCount;
}

// Only called with do_mike / profiling debugging opt enabled
int CC gbh_GetUsedCache(int a1)
{
   // __debugbreak();
    return 999;
}

static SVideo* gpVideoDriver_E13DC8 = nullptr;

struct S3DDevice
{
    DWORD field_0_id;
    char* field_4_device_name;
    char* field_8_device_description;
    DWORD field_C;
    DWORD field_10;
    DWORD field_14;
    DWORD field_18_p0x60_byte_struct;
    DWORD field_1C;
    DWORD field_20_flags;
    D3DDEVICEDESC field_24_deviceDesc;
    DWORD field_120_context;
    D3DDEVICEDESC field_124;
    GUID field_220;
    DWORD field_230_next_ptr;
    DWORD field_234_backing_buffer;
    DWORD field_238;
};

static_assert(sizeof(D3DDEVICEDESC) == 0xfc, "Wrong size D3DDEVICEDESC");
static_assert(sizeof(S3DDevice) == 0x23C, "Wrong size S3DDevice");

struct SD3dStruct
{
    SVideo* field_0_pVideoDriver;
    S3DDevice* field_4_pnext_device;
    void* field_8_pfirst_device;
    void* field_C_device_info_ptr;
    void* field_10;
    S3DDevice* field_14_parray;
    DWORD field_18_current_id;
    DWORD field_1C;
    DWORD field_20;
    IDirect3D3* field_24_pID3d;
    IDirect3DDevice3* field_28_ID3D_Device;
    IDirect3DViewport3* field_2C_IViewPort;
    void* field_30_44_bytes;
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
    DWORD field_5C_pitchQ;
    IUnknown* field_60_IUnknown;
};
static_assert(sizeof(SD3dStruct) == 0x64, "Wrong size SD3dStruct");

static HRESULT WINAPI EnumD3DDevicesCallBack_E014A0(GUID FAR* lpGuid, LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC pDeviceDesc1, LPD3DDEVICEDESC pDeviceDesc2, LPVOID pContext)
{
    return 1;
}

#include "detours.h"

signed int __stdcall CreateD3DDevice_E01840(SD3dStruct* pRenderer)
{
    HRESULT hr = pRenderer->field_24_pID3d->CreateDevice(
        pRenderer->field_14_parray->field_220,
        pRenderer->field_0_pVideoDriver->field_138_Surface,               // dd surface ptr
        &pRenderer->field_28_ID3D_Device,
        nullptr);

    if (FAILED(hr))
    {
        return 0;
    }

    hr = pRenderer->field_24_pID3d->CreateViewport(&pRenderer->field_2C_IViewPort, nullptr);
    if (FAILED(hr))
    {
        return 0;
    }

    return 1;
}

decltype(&CreateD3DDevice_E01840) pCreateD3DDevice_E01840 = (decltype(&CreateD3DDevice_E01840))0x01840;

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
        pd3d->field_C_device_info_ptr = (void*)1;

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
            if (pd3d->field_10)
            {
                return pd3d;
            }
        }

        free(pd3d);
        return 0;
    }
    return pd3d;
}

signed int __stdcall Set3dDevice_E01B90(SD3dStruct* pContext, int id)
{
    if (pContext->field_18_current_id)
    {
        //FreeD3dDThings_E016E0(pContext);
    }

    auto pDevice = pContext->field_4_pnext_device;
    if (pDevice)
    {
        /*
        while (pDevice->field_0_id != id)
        {
            pDevice = pDevice->field_230_next_ptr;
            if (!pDevice)
            {
                return 1;
            }
        }*/
        pContext->field_14_parray = pDevice;
        pContext->field_18_current_id = id;
        //result = CreateD3DDevice_E01840(pContext);
        //if (!result)
        {
            //result = SetDeviceDefaultRenderStates_E01A90(pContext);
        }
    }
    else
    {
        return 1;
    }
    return 0; // result
}

SD3dStruct** gD3dPtr_dword_21C85E0 = (SD3dStruct**)0x21C85E0;

signed int Init_E02340()
{
    // TODO

    SD3dStruct* pD3d = D3DCreate_E01300(gpVideoDriver_E13DC8);
   // gD3dPtr_dword_E485E0 = pD3d;
   // if (Set3dDevice_E01B90(pD3d, 2) != 1)
    {
      //  __debugbreak();
    }

    return 1;
}

s32 CC gbh_Init(int a1)
{
    /*
    int result = Init_E02340();
    if (!result)
    {
        gbh_SetColourDepth();
        result = 0;
    }
   // return result;
   */
    auto ret = gFuncs.pgbh_Init(a1);
    return ret;
}

static SPtrVideoFunctions gVideoDriverFuncs;

static int CC gbh_SetMode_E04D80(SVideo* pVideoDriver, HWND hwnd, int modeId)
{
    pVideoDriver->field_34_active_device_id = 0;

    int result = (*gVideoDriverFuncs.pVid_SetMode)(pVideoDriver, hwnd, modeId);
    if (!result)
    {
        result = Init_E02340();
    }
    return result;
}


u32 CC gbh_InitDLL(SVideo* pVideoDriver)
{
    HMODULE hOld = LoadLibrary(L"C:\\Program Files (x86)\\Rockstar Games\\GTA2\\_d3ddll.dll");
    PopulateS3DFunctions(hOld, gFuncs);

    /*
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    pCreateD3DDevice_E01840 = (decltype(&CreateD3DDevice_E01840))((DWORD)hOld + 0x01840);
    DetourAttach((PVOID*)(&pCreateD3DDevice_E01840), (PVOID)CreateD3DDevice_E01840);
    DetourTransactionCommit();
    */

    auto ret = gFuncs.pgbh_InitDLL(pVideoDriver);


    gpVideoDriver_E13DC8 = pVideoDriver;
    PopulateSVideoFunctions(pVideoDriver->field_7C_self_dll_handle, gVideoDriverFuncs);
    
    /*
    *pVideoDriver->field_84_from_initDLL->pVid_CloseScreen = gbh_CloseScreen;
    *pVideoDriver->field_84_from_initDLL->pVid_GetSurface = *gVideoDriverFuncs.pVid_GetSurface;
    *pVideoDriver->field_84_from_initDLL->pVid_FreeSurface = *gVideoDriverFuncs.pVid_FreeSurface;
    *pVideoDriver->field_84_from_initDLL->pVid_SetMode = gbh_SetMode_E04D80;
    */



    //return 1;

    return ret;
}

struct SImageTableEntry
{
    IDirectDrawSurface7* field_0_ddsurface;
    DWORD field_4;
    DWORD field_8;
    IDirectDrawSurface7* field_C_pSurface;
};
static SImageTableEntry*  gpImageTable_dword_E13894 = nullptr;
static DWORD gpImageTableCount_dword_E13898 = 0;

signed int CC gbh_InitImageTable(int tableSize)
{
    /*
    gpImageTable_dword_E13894 = reinterpret_cast<SImageTableEntry*>(malloc(sizeof(SImageTableEntry) * tableSize));
    if (!gpImageTable_dword_E13894)
    {
        return -1;
    }
    memset(gpImageTable_dword_E13894, 0, sizeof(SImageTableEntry) * tableSize);
    gpImageTableCount_dword_E13898 = tableSize;
    */

    return gFuncs.pgbh_InitImageTable(tableSize);

    //return 0;
}

signed int CC gbh_LoadImage(SImage* pToLoad)
{

    return gFuncs.pgbh_LoadImage(pToLoad);
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

int CC gbh_LockTexture(STexture* pTexture)
{
    //__debugbreak();
    //return 0;
    return gFuncs.pgbh_LockTexture(pTexture);
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
    // TODO
    //__debugbreak();
    //return 1;

    return gFuncs.pgbh_RegisterPalette(paltId, pData);
}

static u32 gTextureId = 0;

STexture* CC gbh_RegisterTexture(__int16 width, __int16 height, void* pData, int a4, char a5)
{
    /*
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
    //return pTexture;
    */
    return gFuncs.pgbh_RegisterTexture(width, height, pData, a4, a5);
}

void CC gbh_ResetLights()
{
   // __debugbreak();		a4	0	int

    gFuncs.pgbh_ResetLights();
}

void CC gbh_SetAmbient(float a1)
{
//    __debugbreak();
    gFuncs.pgbh_SetAmbient(a1);
}

int CC gbh_SetCamera(int a1, int a2, int a3, int a4)
{
   // __debugbreak();
   // return 0;
    return gFuncs.pgbh_SetCamera(a1, a2, a3, a4);
}

int CC gbh_SetColourDepth()
{
    // TODO
    //__debugbreak();
   // return 1;
    return gFuncs.pgbh_SetColourDepth();
}

s32 CC gbh_SetWindow(int a1, int a2, int a3, int a4)
{
    //__debugbreak();
    //return a4;
    return gFuncs.pgbh_SetWindow(a1, a2, a3, a4);
}

int CC gbh_UnlockTexture(STexture* pTexture)
{
   // __debugbreak();
    //return 0;
    return gFuncs.pgbh_UnlockTexture(pTexture);
}
