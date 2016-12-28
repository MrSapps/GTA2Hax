#include "stdafx.h"
#include "d3ddll.hpp"
#include <cmath>
#include <d3d.h>
#include <vector>

#define BYTEn(x, n)   (*((BYTE*)&(x)+n))
#define BYTE1(x)   BYTEn(x,  1)

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



static bool NotClipped(Verts* pVerts)
{
    float maxX = pVerts->mVerts[0].x;
    if (maxX > pVerts->mVerts[1].x)
        maxX = pVerts->mVerts[1].x;
    if (maxX > pVerts->mVerts[2].x)
        maxX = pVerts->mVerts[2].x;
    if (maxX > pVerts->mVerts[3].x)
        maxX = pVerts->mVerts[3].x;

    float minX = pVerts->mVerts[0].x;
    if (minX < pVerts->mVerts[1].x)
        minX = pVerts->mVerts[1].x;
    if (minX < pVerts->mVerts[2].x)
        minX = pVerts->mVerts[2].x;
    if (minX < pVerts->mVerts[3].x)
        minX = pVerts->mVerts[3].x;

    float maxY = pVerts->mVerts[0].y;
    if (maxY > pVerts->mVerts[1].y)
        maxY = pVerts->mVerts[1].y;
    if (maxY > pVerts->mVerts[2].y)
        maxY = pVerts->mVerts[2].y;
    if (maxY > pVerts->mVerts[3].y)
        maxY = pVerts->mVerts[3].y;

    float minY = pVerts->mVerts[0].y;
    if (minY < pVerts->mVerts[1].y)
        minY = pVerts->mVerts[1].y;
    if (minY < pVerts->mVerts[2].y)
        minY = pVerts->mVerts[2].y;
    if (minY < pVerts->mVerts[3].y)
        minY = pVerts->mVerts[3].y;

    return true;
    /*
    return (maxX <= dword_E43E0C
        && minX >= dword_E43E08
        && maxY <= dword_E43E14
        && minY >= dword_E43E10);
        */
}

DWORD bPointFilteringOn_E48604 = 0;
IDirect3DDevice3* d3ddev_dword_E485E0 = 0;
DWORD renderStateCache_E43E24 = 0;
DWORD gNumTrisDrawn_E43EA0 = 0;

void SetRenderStates_E02960(int states)
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
                d3ddev_dword_E485E0->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1); // 27, 1
                d3ddev_dword_E485E0->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE); // 19, 1
                d3ddev_dword_E485E0->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE); // 20, 1
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

                d3ddev_dword_E485E0->SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATEALPHA); // 21, 4
                d3ddev_dword_E485E0->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 1); // 27, 1
                d3ddev_dword_E485E0->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA); // 19, 5
                d3ddev_dword_E485E0->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA); // 20, 6
            }
        }
    }
    else
    {
        if (renderStateCache_E43E24)
        {
            d3ddev_dword_E485E0->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, 0); // 27, 0
            renderStateCache_E43E24 = 0;
        }
    }
}

void CC gbh_DrawQuad(int flags, STexture* pTexture, Verts* pVerts, int baseColour)
{
    // Flags meanings:
    // 0x10000 = fit quad and texture coords to texture size
    // 0x20000 = texture filtering, force enabled by 0x10000
    // 0x300 = alpha blending, 0x80 picks sub blending mode
    // 0x8000 lighting? or shadow
    // 0x2000 = use alpha in diffuse colour
    /*
    if (pVerts->mVerts[0].z > 0.0f)
    {
        if (NotClipped(pVerts))
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
                    d3ddev_dword_E485E0->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DTFG_POINT);
                    d3ddev_dword_E485E0->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DTFG_POINT);
                }
            }
            else
            {
                if (bPointFilteringOn_E48604)
                {
                    bPointFilteringOn_E48604 = 0;
                    d3ddev_dword_E485E0->SetRenderState(D3DRENDERSTATE_TEXTUREMAG, D3DTFG_LINEAR);
                    d3ddev_dword_E485E0->SetRenderState(D3DRENDERSTATE_TEXTUREMIN, D3DTFG_LINEAR);
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
                    //sub_E02810(pTexture, flags);
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
                //sub_E02810(pTexture, flags);
                //v9 = pTexture->field_13_flags;
                //v10 = v9 | 0x40;
                //pTexture->field_13_flags = v10;

                //goto LABEL_49;
            }
            else
            {
                //sub_E02810(pTexture, flags); // set/reinit field_1C_ptr
                //v9 = pTexture->field_13;

                if (flags & 0x300) // Blending
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
                    if (flags & 0x10000)
                    {
                        const float textureW = pTexture->field_E_width;
                        const float textureH = pTexture->field_10_height;
                        
                        // GPU specific hack
                        //if (dword_E13884)
                        {
                            //v19 = pVerts->mVerts[0].x;
                            //floor(v19);
                            //pVerts->mVerts[0].x = v19;
                            //v20 = pVerts->mVerts[0].y;
                            //floor(v20);
                            //pVerts->mVerts[0].y = v20;
                        }
                        

                        
                        //v21 = pVerts->mVerts[0].x + textureW;
                        //v23 = v21 - flt_E10830;
                        //v24 = pVerts->mVerts[0].y + textureH;
                        

                        pVerts->mVerts[1].z = pVerts->mVerts[0].z;
                        pVerts->mVerts[2].z = pVerts->mVerts[0].z;
                        pVerts->mVerts[3].z = pVerts->mVerts[0].z;

                        //pVerts->mVerts[1].x = v23;
                        pVerts->mVerts[1].y = pVerts->mVerts[0].y;

                        //pVerts->mVerts[2].x = v21 - flt_E10830;
                        //pVerts->mVerts[2].y = v24 - flt_E10830;

                        pVerts->mVerts[3].x = pVerts->mVerts[0].x;
                        //pVerts->mVerts[3].y = v24 - flt_E10830;

                        pVerts->mVerts[0].u = 0;
                        pVerts->mVerts[0].v = 0;

                        //pVerts->mVerts[1].u = textureW - flt_E1082C;
                        pVerts->mVerts[1].v = 0;

                        //pVerts->mVerts[2].u = textureW - flt_E1082C;
                        //pVerts->mVerts[2].v = textureH - flt_E1082C;

                        pVerts->mVerts[3].u = 0;
                        //pVerts->mVerts[3].v = textureH - flt_E1082C;
                    }
                    pVerts->mVerts[0].w = pVerts->mVerts[0].z;
                    pVerts->mVerts[1].w = pVerts->mVerts[1].z;
                    pVerts->mVerts[2].w = pVerts->mVerts[2].z;
                    pVerts->mVerts[3].w = pVerts->mVerts[3].z;

                    pVerts->mVerts[0].u = uvScale * pVerts->mVerts[0].u;
                    pVerts->mVerts[0].v = uvScale * pVerts->mVerts[0].v;

                    pVerts->mVerts[1].u = uvScale * pVerts->mVerts[1].u;
                    pVerts->mVerts[1].v = uvScale * pVerts->mVerts[1].v;

                    pVerts->mVerts[2].u = uvScale * pVerts->mVerts[2].u;
                    pVerts->mVerts[2].v = uvScale * pVerts->mVerts[2].v;

                    pVerts->mVerts[3].u = uvScale * pVerts->mVerts[3].u;
                    pVerts->mVerts[3].v = uvScale * pVerts->mVerts[3].v;

                    if (!(flagsCopy & 0x2000))
                    {
                        // Force RGBA to be 255, 255, 255, A
                        const auto finalDiffuse = (unsigned __int8)alpha | (((unsigned __int8)alpha | ((alpha | 0xFFFFFF00) << 8)) << 8);
                        pVerts->mVerts[0].diff = finalDiffuse;
                        pVerts->mVerts[1].diff = finalDiffuse;
                        pVerts->mVerts[2].diff = finalDiffuse;
                        pVerts->mVerts[3].diff = finalDiffuse;
                    }

                    pVerts->mVerts[0].spec = 0;
                    pVerts->mVerts[1].spec = 0;
                    pVerts->mVerts[2].spec = 0;
                    pVerts->mVerts[3].spec = 0;

                    if (flagsCopy & 0x8000)
                    {
                        //if (flt_E10838 != 255.0f)
                        {
                            //sub_E02A80(4, pVerts, 0, alpha);
                        }
                    }
                    
                    d3ddev_dword_E485E0->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1, pVerts, 4, D3DDP_DONOTUPDATEEXTENTS);
                    gNumTrisDrawn_E43EA0 += 2;

                    return;
                }
            }

            // dead - edited above
            // v10 = v9 | 0x40;
            // goto LABEL_49;
        }
    }
    */

    if (pTexture)
    {
        pLast = pTexture;
    }

    if (!pTexture)
    {
        pTexture = pLast;
    }

    // flags = 0;


    return gFuncs.pgbh_DrawQuad(flags, pTexture, pVerts, baseColour);
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

static u32 gTextureId_dword_E13D54 = 0;


struct SPal
{
    void* field_0_pOriginalData;
    void* field_4_pNewData;
    DWORD field_8;
};
static_assert(sizeof(SPal) == 0xC, "Wrong size SPal");

SPal stru_E13E00[128]; // TODO: Size is probably huge

STexture* CC gbh_RegisterTexture(__int16 width, __int16 height, void* pData, int pal_idx, char a5)
{
    STexture* result = reinterpret_cast<STexture*>(malloc(sizeof(STexture)));
    if (!result)
    {
        return 0;
    }

    result->field_0_id = gTextureId_dword_E13D54++;
    result->field_2 = 0;
    result->field_4 = 0; // Yes, wtf ? LOBYTE(stru_E13E00[pal_idx].field_4_pNewData);
    result->field_E_width = width;
    result->field_6 = 0;
    result->field_8 = 0;
    result->field_C = 0;
    result->field_D = 0;
    result->field_10_height = height;
    result->field_12 = stru_E13E00[pal_idx].field_8;
    if (a5 /*&& isAtiRagePro*/)
    {
        result->field_13_flags_from_SPal_field8 = 0x80u;
    }
    else
    {
        result->field_13_flags_from_SPal_field8 = 0;
    }
    result->field_14_data = pData;
    result->field_18_pPaltData = stru_E13E00[pal_idx].field_4_pNewData;
    result->field_1C_ptr = 0;

//    return result;

    return gFuncs.pgbh_RegisterTexture(width, height, pData, pal_idx, a5);
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
