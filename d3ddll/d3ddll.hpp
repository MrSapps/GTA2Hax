#pragma once

using u16 = unsigned short int;
using s32 = signed int;
using u32 = unsigned int;
using f32 = float;
using u8 = unsigned char;

#define CC __stdcall

struct STexture
{
    u16 field_0;
    u16 field_2;
    u16 field_4;
    u16 field_6;
    u32 field_8;
    u8 field_C;
    u8 field_D;
    u16 field_E_width;
    u16 field_10_height;
    u8 field_12;
    u8 field_13_flags;
    void* field_14_data;
    u32 field_18_pPlat;
    u32 field_1C_ptr;
};
static_assert(sizeof(STexture) == 0x20, "Wrong sized STexture");

void CC ConvertColourBank(s32 unknown);
int CC DrawLine(int a1, int a2, int a3, int a4, int a5);
void CC SetShadeTableA(int a1, int a2, int a3, int a4, int a5);
int CC MakeScreenTable(int* result, int a2, unsigned int a3, int a4);
int CC gbh_AddLight(int a1);
char CC gbh_AssignPalette(int a1, int a2);
void CC gbh_BeginLevel();
int gbh_BeginScene();
int CC gbh_BlitBuffer(int a1, int a2, int a3, int a4, int a5, int a6);
char CC gbh_BlitImage(int a1, int a2, int a3, int a4, int a5, int a6, int a7);
void CC gbh_CloseDLL();
int CC gbh_CloseScreen(int a1);
unsigned int CC gbh_Convert16BitGraphic(int a1, unsigned int a2, WORD *a3, signed int a4);
unsigned int CC gbh_ConvertColour(unsigned __int8 a1, unsigned __int8 a2, unsigned __int8 a3);
int CC gbh_DrawFlatRect(int a1, int a2);
void CC gbh_DrawQuad(int a1, int a2, int a3, int a4);
void CC gbh_DrawQuadClipped(int a1, int a2, int a3, int a4, int a5);
s32 CC gbh_DrawTilePart(int a1, int a2, int a3, int a4);
void CC gbh_DrawTriangle(int a1, int a2, int a3, int a4);
void CC gbh_EndLevel();
__int64 CC gbh_EndScene();
int CC gbh_FreeImageTable();
void CC gbh_FreePalette(int a1);
void CC gbh_FreeTexture(STexture* pTexture);
int* CC gbh_GetGlobals();
int CC gbh_GetUsedCache(int a1);
s32 CC gbh_Init(int a1);
u32 CC gbh_InitDLL(int a1, int a2, int a3);
signed int CC gbh_InitImageTable(int a1);
signed int CC gbh_LoadImage(int a1);
int CC gbh_LockTexture(int a1);
void CC gbh_Plot(int a1, int a2, int a3, int a4);
int CC gbh_PrintBitmap(int a1, int a2);
unsigned int CC gbh_RegisterPalette(int a1, DWORD *a2);
STexture* CC gbh_RegisterTexture(__int16 width, __int16 height, void* pData, int a4, char a5);
void CC gbh_ResetLights();
void CC gbh_SetAmbient(float a1);
int CC gbh_SetCamera(int a1, int a2, int a3, int a4);
int CC gbh_SetColourDepth();
s32 CC gbh_SetWindow(int a1, int a2, int a3, int a4);
int CC gbh_UnlockTexture(int a1);
