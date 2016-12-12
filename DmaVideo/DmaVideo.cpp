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
    if (!pVideoDriver)
    {
        return 0;
    }

    SDisplayMode* pDisplayMode = pVideoDriver->field_24_head_ptr;
    if (!pDisplayMode)
    {
        pVideoDriver->field_10_found_rgb_bit_count = 0;
        pVideoDriver->field_8_found_width = 0;
        pVideoDriver->field_C_found_height = 0;
        return 0;
    }

    const DWORD deviceId = pVideoDriver->field_34_active_device_id;
    while (pDisplayMode->field_4_deviceId != deviceId && deviceId
        || pDisplayMode->field_8_width != width
        || pDisplayMode->field_C_height != height
        || pDisplayMode->field_14_rgb_bit_count != rgbBitCount)
    {
        pDisplayMode = pDisplayMode->field_38_pnext;
        if (!pDisplayMode)
        {
            pVideoDriver->field_10_found_rgb_bit_count = 0;
            pVideoDriver->field_8_found_width = 0;
            pVideoDriver->field_C_found_height = 0;
            return 0;
        }
    }
    pVideoDriver->field_10_found_rgb_bit_count = pDisplayMode->field_14_rgb_bit_count;
    pVideoDriver->field_8_found_width = pDisplayMode->field_8_width;
    pVideoDriver->field_C_found_height = pDisplayMode->field_C_height;
    return pDisplayMode->field_0_display_mode_idx;
}

SDevice* CC Vid_FindDevice(SVideo* pVideoDriver, s32 deviceId)
{
    SDevice* result = pVideoDriver->field_2C_device_info_head_ptr;
    if (pVideoDriver && result)
    {
        while (result->field_0_id != deviceId)
        {
            result = result->field_10_next_ptr;
            if (!result)
            {
                return nullptr;
            }
        }
        return result;
    }
    return nullptr;
}

SDisplayMode* CC Vid_FindMode(SVideo* pVideoDriver, s32 modeId)
{
    if (!pVideoDriver || modeId == -2 && pVideoDriver->field_34_active_device_id > 1)
    {
        return 0;
    }

    SDisplayMode* result = pVideoDriver->field_24_head_ptr;
    if (!result)
    {
        pVideoDriver->field_10_found_rgb_bit_count = 0;
        pVideoDriver->field_8_found_width = 0;
        pVideoDriver->field_C_found_height = 0;
        return 0;
    }

    const DWORD deviceId = pVideoDriver->field_34_active_device_id;
    while (result->field_4_deviceId != deviceId && deviceId || result->field_0_display_mode_idx != modeId)
    {
        result = result->field_38_pnext;
        if (!result)
        {
            pVideoDriver->field_10_found_rgb_bit_count = 0;
            pVideoDriver->field_8_found_width = 0;
            pVideoDriver->field_C_found_height = 0;
            return 0;
        }
    }
    pVideoDriver->field_10_found_rgb_bit_count = result->field_14_rgb_bit_count;
    pVideoDriver->field_8_found_width = result->field_8_width;
    pVideoDriver->field_C_found_height = result->field_C_height;
    return result;
}

s32 CC Vid_FindFirstMode(SVideo* pVideoDriver, s32 rgbBitCountFilter)
{
    if (!pVideoDriver)
    {
        return 0;
    }

    SDisplayMode* pMode = pVideoDriver->field_24_head_ptr;
    pVideoDriver->field_10_found_rgb_bit_count = rgbBitCountFilter;
    if (!pMode)
    {
        pVideoDriver->field_8_found_width = 0;
        pVideoDriver->field_C_found_height = 0;
        pVideoDriver->field_10_found_rgb_bit_count = 0;
        pVideoDriver->field_3C_current_enum_ptr = 0;
        return 0;
    }
    const DWORD deviceId = pVideoDriver->field_34_active_device_id;

    while (pMode->field_4_deviceId != deviceId && deviceId || pMode->field_14_rgb_bit_count != rgbBitCountFilter)
    {
        pMode = pMode->field_38_pnext;
        if (!pMode)
        {
            pVideoDriver->field_8_found_width = 0;
            pVideoDriver->field_C_found_height = 0;
            pVideoDriver->field_10_found_rgb_bit_count = 0;
            pVideoDriver->field_3C_current_enum_ptr = 0;
            return 0;
        }
    }
    pVideoDriver->field_3C_current_enum_ptr = pMode->field_38_pnext;
    pVideoDriver->field_8_found_width = pMode->field_8_width;
    pVideoDriver->field_C_found_height = pMode->field_C_height;
    return pMode->field_0_display_mode_idx;
}

s32 CC Vid_FindNextMode(SVideo* pVideoDriver)
{
    if (!pVideoDriver)
    {
        return 0;
    }

    SDisplayMode* pMode = pVideoDriver->field_3C_current_enum_ptr;
    if (!pMode)
    {
        pVideoDriver->field_10_found_rgb_bit_count = 0;
        pVideoDriver->field_3C_current_enum_ptr = 0;
        return 0;
    }

    const DWORD deviceId = pVideoDriver->field_34_active_device_id;
    while (pMode->field_4_deviceId != deviceId && deviceId
        || pMode->field_14_rgb_bit_count != pVideoDriver->field_10_found_rgb_bit_count)
    {
        pMode = pMode->field_38_pnext;
        if (!pMode)
        {
            pVideoDriver->field_10_found_rgb_bit_count = 0;
            pVideoDriver->field_3C_current_enum_ptr = 0;
            return 0;
        }
    }
    pVideoDriver->field_3C_current_enum_ptr = pMode->field_38_pnext;
    pVideoDriver->field_8_found_width = pMode->field_8_width;
    pVideoDriver->field_C_found_height = pMode->field_C_height;
    return pMode->field_0_display_mode_idx;
}

void CC Vid_CloseScreen(SVideo* pVideo)
{
    // TODO
}

static DWORD gCoopResult_dword_100FFE4; // TODO: Not required to be global?

s32 CC Vid_SetDevice(SVideo* pVideoDriver, s32 deviceId)
{
    const DWORD currentDeviceId = pVideoDriver->field_34_active_device_id;
    if (currentDeviceId != deviceId)
    {
        if (currentDeviceId)
        {
            if (pVideoDriver)
            {
                if (pVideoDriver->field_40_minus2IfHaveSurface)
                {
                    auto pDDraw = pVideoDriver->field_8C_DirectDraw7;
                    if (pDDraw)
                    {
                        if (pVideoDriver->field_134_SurfacePrimary)
                        {
                            pDDraw->RestoreDisplayMode();
                            gCoopResult_dword_100FFE4 = pVideoDriver->field_8C_DirectDraw7->SetCooperativeLevel(
                                pVideoDriver->field_4C0_hwnd, 8); // TODO: Constant
                            pVideoDriver->field_134_SurfacePrimary->Release();
                            if (pVideoDriver->field_40_minus2IfHaveSurface == -2)
                            {
                                pVideoDriver->field_138_Surface->Release();
                            }
                            pVideoDriver->field_134_SurfacePrimary = 0;
                            pVideoDriver->field_138_Surface = 0;
                            pVideoDriver->field_40_minus2IfHaveSurface = 0;
                        }
                    }
                }
            }
            if (pVideoDriver->field_120_IDDraw4)
            {
                //--gDD4Refs_dword_100FFF4;
                pVideoDriver->field_120_IDDraw4->Release();
                pVideoDriver->field_120_IDDraw4 = 0;
            }
            if (pVideoDriver->field_8C_DirectDraw7)
            {
                //--gDD7Refs_dword_100FFF0;
                pVideoDriver->field_8C_DirectDraw7->Release();
                pVideoDriver->field_8C_DirectDraw7 = 0;
            }
            pVideoDriver->field_34_active_device_id = 0;
        }
        if (deviceId)
        {
            auto pDevice = pVideoDriver->field_2C_device_info_head_ptr;
            if (pVideoDriver && pDevice != 0)
            {
                while (pDevice->field_0_id != deviceId)
                {
                    pDevice = pDevice->field_10_next_ptr;
                    if (!pDevice)
                    {
                        break;
                    }
                }
            }
            else
            {
                pDevice = 0;
            }

            auto pDeviceGuid = pDevice->field_14_pDeviceGuid;
            auto ppDD4 = &pVideoDriver->field_120_IDDraw4;
            if (pVideoDriver->field_120_IDDraw4)
            {
                //--gDD4Refs_dword_100FFF4;
                (*ppDD4)->Release();
                *ppDD4 = 0;
            }
            auto ppDD7 = &pVideoDriver->field_8C_DirectDraw7;
            if (pVideoDriver->field_8C_DirectDraw7)
            {
                //--gDD7Refs_dword_100FFF0;
                (*ppDD7)->Release();
                *ppDD7 = 0;
            }
            pVideoDriver->field_88_last_error = DirectDrawCreate(
                pDeviceGuid,
                (LPDIRECTDRAW *)&pVideoDriver->field_8C_DirectDraw7,
                0);
            //++gDD7Refs_dword_100FFF0;
            if (pVideoDriver->field_88_last_error)
            {
                return 1;
            }

            pVideoDriver->field_88_last_error = (*ppDD7)->QueryInterface(
                IID_IDirectDraw4,
                (LPVOID*)&pVideoDriver->field_120_IDDraw4);
            //++gDD4Refs_dword_100FFF4;
            if (pVideoDriver->field_88_last_error)
            {
                (*ppDD7)->Release();
                *ppDD7 = 0;
                return 1;
            }
            pVideoDriver->field_34_active_device_id = deviceId;
        }
    }
    return 0;
}

static s32 SetDisplayModeFromSurface(SVideo* pVideoDriver,  SDisplayMode* pDisplayMode_1, DWORD modeId)
{
    DDSCAPS2 caps = {};
    caps.dwCaps = 4;
    if (pVideoDriver->field_134_SurfacePrimary->GetAttachedSurface(&caps, &pVideoDriver->field_138_Surface))
    {
        return 1;
    }

    DDSURFACEDESC2 ddsurface = {};
    ddsurface.dwSize = sizeof(LPDDSURFACEDESC2);
    pVideoDriver->field_138_Surface->GetSurfaceDesc(&ddsurface);

    SDisplayMode pDisplayMode = {};
    Init_DisplayMode_1001010(&pDisplayMode, &ddsurface, pVideoDriver);

    pVideoDriver->field_38 = pDisplayMode_1->field_14_rgb_bit_count;
    pVideoDriver->field_40_minus2IfHaveSurface = modeId;
    pVideoDriver->field_48_rect_right = pDisplayMode_1->field_8_width;
    pVideoDriver->field_58 = pDisplayMode.field_18;
    pVideoDriver->field_4C_rect_bottom = pDisplayMode_1->field_C_height;
    pVideoDriver->field_5C = pDisplayMode.field_1C;
    pVideoDriver->field_64_r = pDisplayMode.field_24;
    pVideoDriver->field_60_g = pDisplayMode.field_20;
    pVideoDriver->field_68_b = pDisplayMode.field_28;
    pVideoDriver->field_70 = pDisplayMode.field_30;
    pVideoDriver->field_6C = pDisplayMode.field_2C;
    pVideoDriver->field_74 = pDisplayMode.field_34;
    return 0;
}

s32 CC Vid_SetMode(SVideo* pVideoDriver, HWND hWnd, s32 modeId)
{
    if (!pVideoDriver)
    {
        return 1;
    }

    const bool bModeIsNotMinus2 = modeId != -2;
    UpdateWindow(hWnd);
    pVideoDriver->field_4C0_hwnd = hWnd;

    if (pVideoDriver->field_40_minus2IfHaveSurface)
    {
        if (pVideoDriver->field_8C_DirectDraw7)
        {
            if (pVideoDriver->field_134_SurfacePrimary)
            {
                pVideoDriver->field_8C_DirectDraw7->RestoreDisplayMode();
                gCoopResult_dword_100FFE4 = pVideoDriver->field_8C_DirectDraw7->SetCooperativeLevel(pVideoDriver->field_4C0_hwnd, 8);
                pVideoDriver->field_134_SurfacePrimary->Release();
                if (pVideoDriver->field_40_minus2IfHaveSurface == -2)
                {
                    pVideoDriver->field_138_Surface->Release();
                }
                pVideoDriver->field_134_SurfacePrimary = 0;
                pVideoDriver->field_138_Surface = 0;
                pVideoDriver->field_40_minus2IfHaveSurface = 0;
            }
        }
    }

    const DWORD activeDeviceId = pVideoDriver->field_34_active_device_id;
    pVideoDriver->field_80_active_mode_q = bModeIsNotMinus2;
    if (bModeIsNotMinus2 != 1)
    {
        if (!activeDeviceId)
        {
            auto pDevice = pVideoDriver->field_2C_device_info_head_ptr;
            if (pDevice)
            {
                while (pDevice->field_0_id != 1)
                {
                    pDevice = pDevice->field_10_next_ptr;
                    if (!pDevice)
                    {
                        pDevice = 0;
                        break;
                    }
                }
            }

            auto ppDD4 = &pVideoDriver->field_120_IDDraw4;
            if (pVideoDriver->field_120_IDDraw4)
            {
                //--gDD4Refs_dword_100FFF4;
                (*ppDD4)->Release();
                *ppDD4 = 0;
            }

            auto ppDD7 = &pVideoDriver->field_8C_DirectDraw7;
            if (pVideoDriver->field_8C_DirectDraw7)
            {
                //--gDD7Refs_dword_100FFF0;
                (*ppDD7)->Release();
                *ppDD7 = 0;
            }
            pVideoDriver->field_88_last_error = DirectDrawCreate(pDevice->field_14_pDeviceGuid, (LPDIRECTDRAW *)&pVideoDriver->field_8C_DirectDraw7, 0);
            //++gDD7Refs_dword_100FFF0;
            if (pVideoDriver->field_88_last_error)
            {
                return 1;
            }

            pVideoDriver->field_88_last_error = (*ppDD7)->QueryInterface(IID_IDirectDraw4, (LPVOID*)&pVideoDriver->field_120_IDDraw4);
            //++gDD4Refs_dword_100FFF4;
            if (pVideoDriver->field_88_last_error)
            {
                (*ppDD7)->Release();
                *ppDD7 = 0;
                return 1;
            }
            pVideoDriver->field_34_active_device_id = 1;
        }

        if (pVideoDriver->field_8C_DirectDraw7->SetCooperativeLevel(hWnd, 8))
        {
            return 1;
        }


        memset(&pVideoDriver->field_13C_DDSurfaceDesc7, 0, sizeof(pVideoDriver->field_13C_DDSurfaceDesc7));
        //v38 = pVideoDriver->field_120_IDDraw4;
        pVideoDriver->field_13C_DDSurfaceDesc7.dwSize = sizeof(DDSURFACEDESC2);
        pVideoDriver->field_13C_DDSurfaceDesc7.dwFlags = 1;
        pVideoDriver->field_13C_DDSurfaceDesc7.ddsCaps.dwCaps = 512;

        if (pVideoDriver->field_120_IDDraw4->CreateSurface(&pVideoDriver->field_13C_DDSurfaceDesc7, &pVideoDriver->field_134_SurfacePrimary, 0))
        {
            return 1;
        }

        if (pVideoDriver->field_120_IDDraw4->CreateClipper(0, &pVideoDriver->field_1B8_clipper, 0))
        {
            return 1;
        }

        if (pVideoDriver->field_1B8_clipper->SetHWnd(0, hWnd))
        {
            return 1;
        }

        if (pVideoDriver->field_134_SurfacePrimary->SetClipper(pVideoDriver->field_1B8_clipper))
        {
            return 1;
        }

        RECT Rect = {};
        GetClientRect(hWnd, &Rect);
        const auto rectTop = Rect.top;
        memset(&pVideoDriver->field_13C_DDSurfaceDesc7, 0, sizeof(DDSURFACEDESC2));
        const auto rectBottom_1 = Rect.bottom;
        pVideoDriver->field_13C_DDSurfaceDesc7.dwWidth = Rect.right - Rect.left;
        pVideoDriver->field_13C_DDSurfaceDesc7.dwSize = sizeof(DDSURFACEDESC2);
        pVideoDriver->field_13C_DDSurfaceDesc7.dwFlags = 7;
        pVideoDriver->field_13C_DDSurfaceDesc7.dwHeight = rectBottom_1 - rectTop;

        if (pVideoDriver->field_4_flags & 0x80)
        {
            pVideoDriver->field_13C_DDSurfaceDesc7.ddsCaps.dwCaps = 0x4000;
            pVideoDriver->field_4_flags |= 0x20000000;
        }
        else
        {
            pVideoDriver->field_13C_DDSurfaceDesc7.ddsCaps.dwCaps = 2048;
            pVideoDriver->field_4_flags &= 0xDFFFFFFF;
        }

        if (pVideoDriver->field_4_flags & 0x40)
        {
            pVideoDriver->field_4_flags |= 0x20000000;
            pVideoDriver->field_13C_DDSurfaceDesc7.ddsCaps.dwCaps &= (0xF7 | 0x60); // TODO: Only change 1st byte here
        }

        if (!pVideoDriver->field_120_IDDraw4->CreateSurface(&pVideoDriver->field_13C_DDSurfaceDesc7, &pVideoDriver->field_138_Surface, 0))
        {
            // TODO: Refactor with SetDisplayModeFromSurface
            DDSURFACEDESC2 ddsurface = {};
            ddsurface.dwSize = sizeof(DDSURFACEDESC2);
            pVideoDriver->field_138_Surface->GetSurfaceDesc(&ddsurface);

            SDisplayMode displayMode = {};
            displayMode.field_0_display_mode_idx = -2;
            displayMode.field_3C = 1;
            Init_DisplayMode_1001010(&displayMode, &ddsurface, pVideoDriver);

            pVideoDriver->field_40_minus2IfHaveSurface = modeId;
            pVideoDriver->field_48_rect_right = Rect.right - Rect.left;
            pVideoDriver->field_4C_rect_bottom = Rect.bottom - Rect.top;
            pVideoDriver->field_58 = displayMode.field_18;
            pVideoDriver->field_5C = displayMode.field_1C;
            pVideoDriver->field_60_g = displayMode.field_20;
            pVideoDriver->field_64_r = displayMode.field_24;
            pVideoDriver->field_68_b = displayMode.field_28;
            pVideoDriver->field_6C = displayMode.field_2C;
            pVideoDriver->field_70 = displayMode.field_30;
            pVideoDriver->field_74 = displayMode.field_34;
            pVideoDriver->field_38 = displayMode.field_14_rgb_bit_count;
            return 0;
        }
        return 1;
    }

    if (activeDeviceId)
    {
        if (pVideoDriver->field_40_minus2IfHaveSurface)
        {
            if (pVideoDriver->field_8C_DirectDraw7)
            {
                if (pVideoDriver->field_134_SurfacePrimary)
                {
                    pVideoDriver->field_8C_DirectDraw7->RestoreDisplayMode();
                    gCoopResult_dword_100FFE4 = pVideoDriver->field_8C_DirectDraw7->SetCooperativeLevel(pVideoDriver->field_4C0_hwnd, 8);
                    pVideoDriver->field_134_SurfacePrimary->Release();
                    if (pVideoDriver->field_40_minus2IfHaveSurface == -2)
                    {
                        pVideoDriver->field_138_Surface->Release();
                    }
                    pVideoDriver->field_134_SurfacePrimary = 0;
                    pVideoDriver->field_138_Surface = 0;
                    pVideoDriver->field_40_minus2IfHaveSurface = 0;
                }
            }
        }

        if (pVideoDriver->field_120_IDDraw4)
        {
            //--gDD4Refs_dword_100FFF4;
            pVideoDriver->field_120_IDDraw4->Release();
            pVideoDriver->field_120_IDDraw4 = 0;
        }

        if (pVideoDriver->field_8C_DirectDraw7)
        {
            //--gDD7Refs_dword_100FFF0;
            pVideoDriver->field_8C_DirectDraw7->Release();
            pVideoDriver->field_8C_DirectDraw7 = 0;
        }
        pVideoDriver->field_34_active_device_id = 0;
    }
    if (modeId == -2 && pVideoDriver->field_34_active_device_id > 1)
    {
        return 1;
    }

    auto pDisplayMode_1 = pVideoDriver->field_24_head_ptr;
    if (!pDisplayMode_1)
    {
        pVideoDriver->field_10_found_rgb_bit_count = 0;
        pVideoDriver->field_8_found_width = 0;
        pVideoDriver->field_C_found_height = 0;
        return 1;
    }

    while (pDisplayMode_1->field_4_deviceId != pVideoDriver->field_34_active_device_id && pVideoDriver->field_34_active_device_id
        || pDisplayMode_1->field_0_display_mode_idx != modeId)
    {
        pDisplayMode_1 = pDisplayMode_1->field_38_pnext;
        if (!pDisplayMode_1)
        {
            pVideoDriver->field_10_found_rgb_bit_count = 0;
            pVideoDriver->field_8_found_width = 0;
            pVideoDriver->field_C_found_height = 0;
            return 1;
        }
    }

    pVideoDriver->field_10_found_rgb_bit_count = pDisplayMode_1->field_14_rgb_bit_count;
    pVideoDriver->field_8_found_width = pDisplayMode_1->field_8_width;
    pVideoDriver->field_C_found_height = pDisplayMode_1->field_C_height;


    const auto deviceId = pDisplayMode_1->field_4_deviceId;
    if (pVideoDriver->field_34_active_device_id != deviceId)
    {
        if (pVideoDriver->field_34_active_device_id)
        {
            if (pVideoDriver->field_40_minus2IfHaveSurface)
            {
                if (pVideoDriver->field_8C_DirectDraw7)
                {
                    if (pVideoDriver->field_134_SurfacePrimary)
                    {
                        pVideoDriver->field_8C_DirectDraw7->RestoreDisplayMode();
                        gCoopResult_dword_100FFE4 = pVideoDriver->field_8C_DirectDraw7->SetCooperativeLevel(pVideoDriver->field_4C0_hwnd, 8);
                        pVideoDriver->field_134_SurfacePrimary->Release();
                        if (pVideoDriver->field_40_minus2IfHaveSurface == -2)
                        {
                            pVideoDriver->field_138_Surface->Release();
                        }
                        pVideoDriver->field_134_SurfacePrimary = 0;
                        pVideoDriver->field_138_Surface = 0;
                        pVideoDriver->field_40_minus2IfHaveSurface = 0;
                    }
                }
            }
            if (pVideoDriver->field_120_IDDraw4)
            {
                //--gDD4Refs_dword_100FFF4;
                pVideoDriver->field_120_IDDraw4->Release();
                pVideoDriver->field_120_IDDraw4 = 0;
            }
            if (pVideoDriver->field_8C_DirectDraw7)
            {
                //--gDD7Refs_dword_100FFF0;
                pVideoDriver->field_8C_DirectDraw7->Release();
                pVideoDriver->field_8C_DirectDraw7 = 0;
            }
            pVideoDriver->field_34_active_device_id = 0;
        }
        if (deviceId)
        {
            auto pDevice_1 = pVideoDriver->field_2C_device_info_head_ptr;
            if (pDevice_1)
            {
                while (pDevice_1->field_0_id != deviceId)
                {
                    pDevice_1 = pDevice_1->field_10_next_ptr;
                    if (!pDevice_1)
                    {
                        break;
                    }
                }
            }

            auto ppDD4_1 = &pVideoDriver->field_120_IDDraw4;
            if (pVideoDriver->field_120_IDDraw4)
            {
                //--gDD4Refs_dword_100FFF4;
                (*ppDD4_1)->Release();
                *ppDD4_1 = 0;
            }

            auto ppDD7_1 = &pVideoDriver->field_8C_DirectDraw7;
            if (pVideoDriver->field_8C_DirectDraw7)
            {
                //--gDD7Refs_dword_100FFF0;
                (*ppDD7_1)->Release();
                *ppDD7_1 = 0;
            }

            pVideoDriver->field_88_last_error = DirectDrawCreate(pDevice_1->field_14_pDeviceGuid,
                (LPDIRECTDRAW *)&pVideoDriver->field_8C_DirectDraw7, 0);

            //++gDD7Refs_dword_100FFF0;
            if (pVideoDriver->field_88_last_error)
            {
                return 1;
            }

            pVideoDriver->field_88_last_error = (*ppDD7_1)->QueryInterface(IID_IDirectDraw4, (LPVOID *)&pVideoDriver->field_120_IDDraw4);
            //++gDD4Refs_dword_100FFF4;

            if (pVideoDriver->field_88_last_error)
            {
                (*ppDD7_1)->Release();
                *ppDD7_1 = 0;
                return 1;
            }

            pVideoDriver->field_34_active_device_id = deviceId;
        }
    }

    if (pVideoDriver->field_8C_DirectDraw7->SetCooperativeLevel(hWnd, 81))
    {
        return 1;
    }

    
    if (pVideoDriver->field_8C_DirectDraw7->SetDisplayMode(
        pDisplayMode_1->field_8_width, pDisplayMode_1->field_C_height, pDisplayMode_1->field_14_rgb_bit_count, 0, DDSDM_STANDARDVGAMODE)) // TODO: Last 2 args not used by the game!
    {
        return 1;
    }
    

    pVideoDriver->field_4_flags |= 0xA0000000;
    memset(&pVideoDriver->field_13C_DDSurfaceDesc7, 0, sizeof(pVideoDriver->field_13C_DDSurfaceDesc7));
    pVideoDriver->field_13C_DDSurfaceDesc7.dwSize = 124;
    pVideoDriver->field_13C_DDSurfaceDesc7.dwFlags = 33;
    pVideoDriver->field_13C_DDSurfaceDesc7.dwBackBufferCount = 2;
    pVideoDriver->field_13C_DDSurfaceDesc7.ddsCaps.dwCaps = 16920;

    if (pVideoDriver->field_4_flags & 0x40)
    {
        pVideoDriver->field_13C_DDSurfaceDesc7.ddsCaps.dwCaps = 25112;
    }

    if (pVideoDriver->field_4_flags & 0x10)
    {
        if (!pVideoDriver->field_120_IDDraw4->CreateSurface(&pVideoDriver->field_13C_DDSurfaceDesc7, &pVideoDriver->field_134_SurfacePrimary, 0))
        {
            return SetDisplayModeFromSurface(pVideoDriver, pDisplayMode_1, modeId);
        }
    }

    pVideoDriver->field_13C_DDSurfaceDesc7.dwBackBufferCount = 1;

    auto flags = pVideoDriver->field_4_flags & 0x3FFFFFFF;
    flags |= 0x40000000u;
    pVideoDriver->field_4_flags = flags;

    if (pVideoDriver->field_4_flags & 0x30)
    {
        if (!pVideoDriver->field_120_IDDraw4->CreateSurface(&pVideoDriver->field_13C_DDSurfaceDesc7, &pVideoDriver->field_134_SurfacePrimary, 0))
        {
            return SetDisplayModeFromSurface(pVideoDriver, pDisplayMode_1, modeId);
        }
    }

    pVideoDriver->field_4_flags &= 0x9FFFFFFF;
    if (pVideoDriver->field_4_flags & 0x80)
    {
        return 1;
    }

    pVideoDriver->field_13C_DDSurfaceDesc7.ddsCaps.dwCaps &= 0xFFFFBFFF;
    if (pVideoDriver->field_120_IDDraw4->CreateSurface(&pVideoDriver->field_13C_DDSurfaceDesc7, &pVideoDriver->field_134_SurfacePrimary, 0))
    {
        return 1;
    }
    
    return SetDisplayModeFromSurface(pVideoDriver, pDisplayMode_1, modeId);
}

void CC Vid_GrabSurface(SVideo* pVideoDriver)
{
    if (pVideoDriver && !(pVideoDriver->field_4_flags & 1))
    {
        if (pVideoDriver->field_134_SurfacePrimary->IsLost() == DDERR_SURFACELOST)
        {
            pVideoDriver->field_134_SurfacePrimary->Restore();
        }
        memset(&pVideoDriver->field_13C_DDSurfaceDesc7, 0, sizeof(pVideoDriver->field_13C_DDSurfaceDesc7));
        pVideoDriver->field_13C_DDSurfaceDesc7.dwSize = sizeof(DDSURFACEDESC2);
        if (!pVideoDriver->field_138_Surface->Lock(0, &pVideoDriver->field_13C_DDSurfaceDesc7, 1, 0))
        {
            pVideoDriver->field_4_flags |= 1;
        }
    }
}

void CC Vid_ReleaseSurface(SVideo* pVideoDriver)
{
    if (pVideoDriver && pVideoDriver->field_4_flags & 1)
    {
        pVideoDriver->field_138_Surface->Unlock(0);
        if (pVideoDriver->field_134_SurfacePrimary->IsLost() == DDERR_SURFACELOST)
        {
            pVideoDriver->field_134_SurfacePrimary->Restore();
        }
        pVideoDriver->field_4_flags &= 0xFE;
    }
}

void CC Vid_FlipBuffers(SVideo* pVideo)
{
    if (pVideo)
    {
        if (pVideo->field_134_SurfacePrimary)
        {
            if (pVideo->field_138_Surface)
            {
                if (pVideo->field_134_SurfacePrimary->IsLost() == DDERR_SURFACELOST)
                {
                    pVideo->field_4_flags |= 0x10000000u;
                    if (pVideo->field_134_SurfacePrimary->Restore())
                    {
                        return;
                    }
                }
                else
                {
                    pVideo->field_4_flags &= 0xEFFFFFFF;
                }

                if (pVideo->field_138_Surface->IsLost() == DDERR_SURFACELOST)
                {
                    pVideo->field_4_flags |= 0x10000000;
                    if (pVideo->field_138_Surface->Restore())
                        return;
                }
                else
                {
                    pVideo->field_4_flags &= 0xEFFFFFFF;
                }

                if (pVideo->field_80_active_mode_q == 1)
                {
                    if (pVideo->field_4_flags & 2)
                    {
                        pVideo->field_134_SurfacePrimary->Flip(0, 9);
                    }
                    else
                    {
                        pVideo->field_134_SurfacePrimary->Flip(0, 1);
                    }
                }
                else
                {
                    RECT r = {};
                    r.bottom = pVideo->field_4C_rect_bottom;
                    r.top = 0;
                    r.left = 0;
                    r.right = pVideo->field_48_rect_right;

                    RECT clientRect = {};
                    GetClientRect(pVideo->field_4C0_hwnd, &clientRect);
                    /* TODO FIX ME
                    ClientToScreen(pVideo->field_4C0_hwnd, &clientRect.left);
                    clientRect.right += Point.x - clientRect.left;
                    clientRect.bottom += Point.y - clientRect.top;
                    */
                    pVideo->field_134_SurfacePrimary->Blt(
                        &clientRect,
                        pVideo->field_138_Surface,
                        &r,
                        0x1000000,
                        0);
                }
            }
        }
    }
}

void CC Vid_ShutDown_SYS(SVideo* pVideoDriver)
{
    // TODO
}

s32 CC Vid_EnableWrites(SVideo* pVideoDriver)
{
    if (pVideoDriver && (pVideoDriver->field_4_flags & 1) && !(pVideoDriver->field_4_flags & 2))
    {
        pVideoDriver->field_4_flags |= 2;
        if (pVideoDriver->field_4_flags & 1)
        {
            pVideoDriver->field_50_surface_pixels_ptr = pVideoDriver->field_13C_DDSurfaceDesc7.lpSurface;
            pVideoDriver->field_54_surface_pixels_pitch = pVideoDriver->field_13C_DDSurfaceDesc7.lPitch;
        }
        else
        {
            pVideoDriver->field_50_surface_pixels_ptr = 0;
            pVideoDriver->field_54_surface_pixels_pitch = 0;
        }
        return 0;
    }
    return 1;
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
