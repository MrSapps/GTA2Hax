#pragma once

#include <windows.h>
#include <ddraw.h>

struct SVideo
{
    DWORD field_0;
    DWORD field_4_flags;
    DWORD field_8_found_width;
    DWORD field_C_found_height;
    DWORD field_10_found_rgb_bit_count;
    DWORD field_14_display_mode_count_2_q;
    DWORD field_18_num_guids;
    DWORD field_1C_num_display_modes;
    DWORD field_20_num_enums;
    struct SDisplayMode* field_24_head_ptr;
    struct SDisplayMode** field_28_display_mode_array;
    struct SDevice* field_2C_device_info_head_ptr;
    struct SDevice* field_30_device_info_last;
    DWORD field_34_active_device_id;
    DWORD field_38;
    struct SDisplayMode* field_3C_current_enum_ptr;
    DWORD field_40_full_screen;
    DWORD field_44;
    DWORD field_48_rect_right;
    DWORD field_4C_rect_bottom;
    LPVOID field_50_surface_pixels_ptr;
    DWORD field_54_surface_pixels_pitch;
    DWORD field_58;
    DWORD field_5C;
    DWORD field_60_g;
    DWORD field_64_r;
    DWORD field_68_b;
    DWORD field_6C;
    DWORD field_70;
    DWORD field_74;
    DWORD field_78;
    HINSTANCE field_7C;
    DWORD field_80_active_mode_q;
    DWORD field_84_from_initDLL;
    DWORD field_88_last_error;
    IDirectDraw7* field_8C_DirectDraw7;
    DWORD field_90;
    DWORD field_94;
    DWORD field_98;
    DWORD field_9C;
    DWORD field_A0;
    DWORD field_A4;
    DWORD field_A8;
    DWORD field_AC;
    DWORD field_B0;
    DWORD field_B4;
    DWORD field_B8;
    DWORD field_BC;
    DWORD field_C0;
    DWORD field_C4;
    DWORD field_C8;
    DWORD field_CC;
    DWORD field_D0;
    DWORD field_D4;
    DWORD field_D8;
    DWORD field_DC;
    DWORD field_E0;
    DWORD field_E4;
    DWORD field_E8;
    DWORD field_EC;
    DWORD field_F0;
    DWORD field_F4;
    DWORD field_F8;
    DWORD field_FC;
    DWORD field_100;
    DWORD field_104;
    DWORD field_108;
    DWORD field_10C;
    DWORD field_110;
    DWORD field_114;
    DWORD field_118;
    DWORD field_11C;
    IDirectDraw4* field_120_IDDraw4;
    DWORD field_124;
    DWORD field_128;
    DWORD field_12C;
    DWORD field_130;
    IDirectDrawSurface4* field_134_SurfacePrimary;
    IDirectDrawSurface4* field_138_Surface;
    DDSURFACEDESC2 field_13C_DDSurfaceDesc7;
    LPDIRECTDRAWCLIPPER field_1B8_clipper;
    DWORD field_1BC;
    DWORD field_1C0;
    DWORD field_1C4;
    DDCAPS field_1C8_device_caps;
    DDCAPS field_344_hel_caps;
    HWND field_4C0_hwnd;
};

static_assert(sizeof(DDCAPS) == 0x17C, "Wrong sized DDCAPS");
static_assert(sizeof(DDSURFACEDESC2) == 0x7C, "Wrong sized DDSURFACEDESC2");
static_assert(sizeof(SVideo) == 0x4C4, "Wrong sized SVideo");

struct SDevice
{
    DWORD field_0_id;
    DWORD field_4_flags;
    char* field_8_driver_name;
    char* field_C_driver_desc;
    SDevice* field_10_next_ptr;
    GUID* field_14_pDeviceGuid;
    GUID field_18_guid;
    DWORD field_28_dwVidMemTotal;
    DWORD field_2C_end_buffer; // TODO: Actually a word and SDevice is 0x2E ?
};
static_assert(sizeof(SDevice) == 0x30, "Wrong sized SDevice");

struct SDisplayMode
{
    DWORD field_0_display_mode_idx;
    DWORD field_4_deviceId;
    DWORD field_8_width;
    DWORD field_C_height;
    DWORD field_10_pitch;
    DWORD field_14_rgb_bit_count;
    DWORD field_18;
    DWORD field_1C;
    DWORD field_20;
    DWORD field_24;
    DWORD field_28;
    DWORD field_2C;
    DWORD field_30;
    DWORD field_34;
    struct SDisplayMode* field_38_pnext;
    DWORD field_3C;
};
static_assert(sizeof(SDisplayMode) == 0x40, "Wrong sized SDisplayMode");

struct SVidVersion
{
    DWORD mVersion;
    char mVersionString[255];
};

using u16 = unsigned short int;
using s32 = signed int;
using u32 = unsigned int;
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
