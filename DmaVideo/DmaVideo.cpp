#include "stdafx.h"
#include "DmaVideo.hpp"

#pragma comment(lib, "ddraw.lib")
#pragma comment(lib, "dxguid.lib")

static HINSTANCE gHinstance;
static DWORD dword_100FFF8; // TODO: Reverse me

using TDirectDrawEnumerateEx = decltype(&DirectDrawEnumerateExA);

BOOL WINAPI DirectDrawEnumerateCallBack(
    _In_ GUID FAR *lpGUID,
    _In_ LPSTR    lpDriverDescription,
    _In_ LPSTR    lpDriverName,
    _In_ LPVOID   lpContext
)
{
    SVideo* pVideoDriver = reinterpret_cast<SVideo*>(lpContext);
    pVideoDriver->field_20_num_enums++;

    const s32 driverDescStrLen = strlen(lpDriverDescription);
    const s32 driverNameStrLen = strlen(lpDriverName);

    const u32 requiredSize = driverDescStrLen + 1 + driverNameStrLen + 1 + sizeof(SDevice);
    SDevice* pDevice = reinterpret_cast<SDevice*>(malloc(requiredSize));
    memset(pDevice, 0, requiredSize);

    pDevice->field_8_driver_name = reinterpret_cast<char*>(&pDevice->field_2C_end_buffer);
    pDevice->field_C_driver_desc = reinterpret_cast<char*>(&pDevice->field_2C_end_buffer) + driverNameStrLen + 1;

    memcpy(pDevice->field_8_driver_name, lpDriverName, driverNameStrLen + 1);
    memcpy(pDevice->field_C_driver_desc, lpDriverDescription, driverDescStrLen + 1);

    SDevice* pHead = pVideoDriver->field_30_device_info_last;
    if (pHead)
    {
        pHead->field_10_next_ptr = pDevice;
    }
    else
    {
        pVideoDriver->field_2C_device_info_head_ptr = pDevice;
    }
    pVideoDriver->field_30_device_info_last = pDevice;

    if (lpGUID)
    {
        pDevice->field_14_pDeviceGuid = &pDevice->field_18_guid;
        pDevice->field_18_guid = *lpGUID;
        pDevice->field_0_id = pVideoDriver->field_18_num_guids++;
    }
    else
    {
        pDevice->field_0_id = 1;
    }

    return TRUE;
}

BOOL WINAPI DirectDrawEnumerateExCallBack(
    _In_ GUID FAR *lpGUID,
    _In_ LPSTR    lpDriverDescription,
    _In_ LPSTR    lpDriverName,
    _In_ LPVOID   lpContext,
    _In_ HMONITOR hm
)
{
    // Pass to other call back
    return DirectDrawEnumerateCallBack(lpGUID, lpDriverDescription, lpDriverName, lpContext);
}

SDevice* Init_DisplayMode_1001010(SDisplayMode* pDisplayMode, DDSURFACEDESC2* ddsurface, SVideo* pVideoDriver)
{
    pDisplayMode->field_8_width = ddsurface->dwWidth;
    pDisplayMode->field_C_height = ddsurface->dwHeight;
    pDisplayMode->field_14_rgb_bit_count = ddsurface->ddpfPixelFormat.dwRGBBitCount;
    pDisplayMode->field_10_pitch = ddsurface->lPitch;
    pDisplayMode->field_38_pnext = 0;

    int bitNum = 0;
    bool bIs32 = false;
    auto result = ddsurface->ddpfPixelFormat.dwRBitMask;
    
    // Count number of bits
    if (result & 1)
    {
        bIs32 = bitNum == 32;
    }
    else
    {
        for (;;)
        {
            bIs32 = bitNum == 32;
            if (bitNum >= 32)
            {
                break;
            }

            result = result >> 1;
            ++bitNum;

            if (result & 1)
            {
                bIs32 = bitNum == 32;
            }
        }
    }

    if (bIs32)
    {
        pDisplayMode->field_18 = 0;
        pDisplayMode->field_1C = 0;
        pDisplayMode->field_28 = 0;
        pDisplayMode->field_2C = 0;
        pDisplayMode->field_20 = 0;
        pDisplayMode->field_24 = 0;
    }
    else
    {
        // TODO
        abort();
    }
    return nullptr;
}

HRESULT WINAPI EnumDisplayModesCallBack_1001340(
    _In_ LPDDSURFACEDESC2 lpDDSurfaceDesc,
    _In_ LPVOID           lpContext
)
{
    SVideo* pVideoDriver = reinterpret_cast<SVideo*>(lpContext);
    if (pVideoDriver->field_4_flags & 0x40)
    {
        // TODO: Debug/test code skipped
    }

    pVideoDriver->field_1C_num_display_modes++;
    SDisplayMode* pDisplayMode = new SDisplayMode();
    memset(pDisplayMode, 0, sizeof(SDisplayMode));

    if (pVideoDriver->field_28_display_mode_array)
    {
        pVideoDriver->field_28_display_mode_array[14] = pDisplayMode;
    }
    else
    {
        pVideoDriver->field_24_head_ptr = pDisplayMode;
    }

    pVideoDriver->field_28_display_mode_array = (SDisplayMode **)pDisplayMode; // TODO: Probably wrong
    pDisplayMode->field_0_display_mode_idx = pVideoDriver->field_14_display_mode_count_2_q;
    ++pVideoDriver->field_14_display_mode_count_2_q;
    pDisplayMode->field_3C = 1;
    pDisplayMode->field_4_deviceId = pVideoDriver->field_34_active_device_id;
    Init_DisplayMode_1001010(pDisplayMode, lpDDSurfaceDesc, 0);

    return DDENUMRET_OK;
}

SVideo* CC Vid_Init_SYS(s32 param1, u16 param2_flags)
{
    HMODULE hDirectDraw = ::LoadLibraryA("ddraw.dll");
    if (hDirectDraw)
    {
        ::FreeLibrary(hDirectDraw);
        SVideo* pVideoDriver = new SVideo();
        memset(pVideoDriver, 0, sizeof(SVideo));
        pVideoDriver->field_7C = gHinstance;
        pVideoDriver->field_84_from_initDLL = dword_100FFF8;
        pVideoDriver->field_0 = 1;
        pVideoDriver->field_78 = param1;
        pVideoDriver->field_14_display_mode_count_2_q = 1;
        pVideoDriver->field_18_num_guids = 2;
        pVideoDriver->field_4_flags = param2_flags & 0x40 | 0x200;
        
        TDirectDrawEnumerateEx pDirectDrawEnumerateEx = nullptr;
        
        if (param2_flags & 4)
        {
            // Yes - using a freed HMODULE is correct here! Mainly because this dll linked to ddraw so its not actually unloaded
            pDirectDrawEnumerateEx = reinterpret_cast<TDirectDrawEnumerateEx>(::GetProcAddress(hDirectDraw, "DirectDrawEnumerateEx"));
        }
        if (pDirectDrawEnumerateEx)
        {
            pVideoDriver->field_88_last_error = pDirectDrawEnumerateEx(DirectDrawEnumerateExCallBack, pVideoDriver, 7); // TODO: Constant
        }
        else
        {
            pVideoDriver->field_88_last_error = DirectDrawEnumerateA(DirectDrawEnumerateCallBack, pVideoDriver);
        }

        if (pVideoDriver->field_88_last_error)
        {
            delete pVideoDriver;
            pVideoDriver = nullptr;
        }

      
        if (param2_flags & 8 || (pVideoDriver && !pVideoDriver->field_2C_device_info_head_ptr))
        {
            // TODO: Might be wrong conditional
            return pVideoDriver;
        }
        else
        {
            auto pDeviceInfo = pVideoDriver->field_2C_device_info_head_ptr;
            auto pNextDevice = pVideoDriver->field_2C_device_info_head_ptr;

            auto pDD7 = &pVideoDriver->field_8C_DirectDraw7;
            auto ppDD4 = &pVideoDriver->field_120_IDDraw4;
            auto ppDD7Copy = &pVideoDriver->field_8C_DirectDraw7;

            for (;;)
            {
                pVideoDriver->field_34_active_device_id = pDeviceInfo->field_0_id;
                auto lpGUID = pDeviceInfo->field_14_pDeviceGuid;
                if (*ppDD4)
                {
                    //--gDD4Refs_dword_100FFF4;
                    (*ppDD4)->Release();
                    *ppDD4 = 0;
                }

                if (*pDD7)
                {
                    //--gDD7Refs_dword_100FFF0;
                    (*pDD7)->Release();
                    *pDD7 = 0;
                }

                // Create DDraw7 Instance
                pVideoDriver->field_88_last_error = DirectDrawCreate(lpGUID, (LPDIRECTDRAW *)pDD7, 0);
                //++gDD7Refs_dword_100FFF0;
                if (pVideoDriver->field_88_last_error)
                {
                    break;
                }

                // Query DDraw4 instance from 7
                pVideoDriver->field_88_last_error = (*pDD7)->QueryInterface(
                    IID_IDirectDraw4,
                    (LPVOID *)&pVideoDriver->field_120_IDDraw4);
                //++gDD4Refs_dword_100FFF4;

                if (pVideoDriver->field_88_last_error)
                {
                    (*pDD7)->Release();
                    *pDD7 = 0;
                    return 0;
                }

                memset(&pVideoDriver->field_1C8_device_caps, 0, sizeof(DDCAPS));
                memset(&pVideoDriver->field_344_hel_caps, 0, sizeof(DDCAPS));
                auto pDD4Copy = *ppDD4;
                pVideoDriver->field_1C8_device_caps.dwSize = sizeof(DDCAPS);
                pVideoDriver->field_344_hel_caps.dwSize = sizeof(DDCAPS);
                pVideoDriver->field_88_last_error = pDD4Copy->GetCaps(
                    &pVideoDriver->field_1C8_device_caps,
                    &pVideoDriver->field_344_hel_caps);
      
                if (pVideoDriver->field_1C8_device_caps.dwCaps2 & 0x80000)
                {
                    pNextDevice->field_4_flags |= 1;
                }

                pNextDevice->field_28_dwVidMemTotal = pVideoDriver->field_1C8_device_caps.dwVidMemTotal;
                auto pDD4 = *ppDD4;
                if (pVideoDriver->field_88_last_error)
                {
                    if (pDD4)
                    {
                        //--gDD4Refs_dword_100FFF4;
                        (*ppDD4)->Release();
                        *ppDD4 = 0;
                    }
                    if (*ppDD7Copy)
                    {
                        //--gDD7Refs_dword_100FFF0;
                        (*ppDD7Copy)->Release();
                        *ppDD7Copy = 0;
                    }
                    break;
                }

                pVideoDriver->field_88_last_error = pDD4->EnumDisplayModes(
                    0,
                    0,
                    pVideoDriver,
                    EnumDisplayModesCallBack_1001340);

                pVideoDriver->field_34_active_device_id = 0;
                if (ppDD4)
                {
                    //--gDD4Refs_dword_100FFF4;
                    (*ppDD4)->Release();
                    *ppDD4 = 0;
                }

                if (*ppDD7Copy)
                {
                    //--gDD7Refs_dword_100FFF0;
                    (*ppDD7Copy)->Release();
                    *ppDD7Copy = 0;
                }

                pNextDevice = pNextDevice->field_10_next_ptr;
                pDeviceInfo = pNextDevice;
                
                if (!pNextDevice)
                {
                    // TODO: Check correct
                    return pVideoDriver;
                }
                pDD7 = &pVideoDriver->field_8C_DirectDraw7;
            }
        }

    }
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
    gHinstance = hInstance;
    dword_100FFF8 = a2;
    return 0;
}

static SVidVersion gVersionInfo =
{
    0x3FE7AE14,
    "Reimplementation of DMA Video (Direct Draw implementation). Version 1.8.1"
};

SVidVersion* CC Vid_GetVersion()
{
    return &gVersionInfo;
}
