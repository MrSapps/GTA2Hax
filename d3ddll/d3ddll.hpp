#pragma once


#include "../DmaVideo/DmaVideo.hpp"

using u16 = unsigned short int;
using s32 = signed int;
using u32 = unsigned int;
using f32 = float;
using u8 = unsigned char;

#define CC __stdcall

struct STexture
{
    u16 field_0_id;
    u16 field_2;
    u16 field_4_pal_is_trans;
    u16 field_6_pal_size;
    void* field_8_locked_pixels_ptr;
    u8 field_C;
    u8 field_D;
    u16 field_E_width;
    u16 field_10_height;
    u8 field_12_bPalIsValid;
    u8 field_13_flags;
    BYTE* field_14_original_pixel_data_ptr;
    WORD* field_18_pPaltData;
    struct SCache* field_1C_ptr; // +32 == IUnknown, DDSurface ptr ?
};
static_assert(sizeof(STexture) == 0x20, "Wrong sized STexture");

struct Vert
{
    float x, y, z, w;
    DWORD diff;
    DWORD spec;
    float u, v;
};
static_assert(sizeof(Vert) == 0x20, "wrong size");

struct Verts
{
    Vert mVerts[4];
};

void CC ConvertColourBank(s32 unknown);
int CC DrawLine(int a1, int a2, int a3, int a4, int a5);
void CC SetShadeTableA(int a1, int a2, int a3, int a4, int a5);
int* CC MakeScreenTable(int result, int a2, unsigned int a3);
int CC gbh_AddLight(int a1);
char CC gbh_AssignPalette(STexture* pTexture, int palId);
void CC gbh_BeginLevel();
int gbh_BeginScene();
int CC gbh_BlitBuffer(int a1, int a2, int a3, int a4, int a5, int a6);
char CC gbh_BlitImage(int a1, int a2, int a3, int a4, int a5, int a6, int a7);
void CC gbh_CloseDLL();
void CC gbh_CloseScreen(SVideo* pVideo);
unsigned int CC gbh_Convert16BitGraphic(int a1, unsigned int a2, WORD *a3, signed int a4);
unsigned int CC gbh_ConvertColour(unsigned __int8 a1, unsigned __int8 a2, unsigned __int8 a3);
int CC gbh_DrawFlatRect(int a1, int a2);
void CC gbh_DrawQuad(int flags, STexture* pTexture, Vert* pVerts, int baseColour);
void CC gbh_DrawQuadClipped(int a1, int a2, int a3, int a4, int a5);
s32 CC gbh_DrawTilePart(int flags, STexture* pTexture, Vert* pData, int diffuseColour);
void CC gbh_DrawTriangle(int triFlags, STexture* pTexture, Vert* pVerts, int diffuseColour);
void CC gbh_EndLevel();
double CC gbh_EndScene();
int CC gbh_FreeImageTable();
void CC gbh_FreePalette(int a1);
void CC gbh_FreeTexture(STexture* pTexture);
u32* CC gbh_GetGlobals();
int CC gbh_GetUsedCache(int a1);
s32 CC gbh_Init(int a1);
u32 CC gbh_InitDLL(SVideo* pVideoDriver);
signed int CC gbh_InitImageTable(int tableSize);

#pragma pack(push)
#pragma pack(1)
struct SImage
{
    BYTE field_0;
    BYTE field_1;
    BYTE field_2;
    BYTE field_3;
    DWORD field_4;
    DWORD field_8;
    WORD field_C_w;
    BYTE field_E_h;
    BYTE field_F;
    BYTE field_10;
    BYTE field_11;
    BYTE* field_12;
};
#pragma pack(pop)
static_assert(sizeof(SImage) == 0x16, "Wrong sized SImage");

signed int CC gbh_LoadImage(SImage* pImage);
STexture* CC gbh_LockTexture(STexture* pTexture);
void CC gbh_Plot(int a1, int a2, int a3, int a4);
int CC gbh_PrintBitmap(int a1, int a2);
unsigned int CC gbh_RegisterPalette(int a1, DWORD *a2);
STexture* CC gbh_RegisterTexture(__int16 width, __int16 height, BYTE* pData, int a4, char a5);
void CC gbh_ResetLights();
void CC gbh_SetAmbient(float a1);
int CC gbh_SetCamera(float a1, float a2, float a3, float a4);
int CC gbh_SetColourDepth();
float CC gbh_SetWindow(float left, float top, float right, float bottom);
STexture* CC gbh_UnlockTexture(STexture* pTexture);


struct S3DFunctions
{
    // TODO: Should probably also be pointers to function pointers
    decltype(&ConvertColourBank) pConvertColourBank;
    decltype(&DrawLine) pDrawLine;
    decltype(&SetShadeTableA) pSetShadeTableA;
    decltype(&MakeScreenTable) pMakeScreenTable;
    decltype(&gbh_AddLight) pgbh_AddLight;
    decltype(&gbh_AssignPalette) pgbh_AssignPalette;
    decltype(&gbh_BeginLevel) pgbh_BeginLevel;
    decltype(&gbh_BeginScene) pgbh_BeginScene;
    decltype(&gbh_BlitBuffer) pgbh_BlitBuffer;
    decltype(&gbh_BlitImage) pgbh_BlitImage;
    decltype(&gbh_CloseDLL) pgbh_CloseDLL;
    decltype(&gbh_CloseScreen) pgbh_CloseScreen;
    decltype(&gbh_Convert16BitGraphic) pgbh_Convert16BitGraphic;
    decltype(&gbh_ConvertColour) pgbh_ConvertColour;
    decltype(&gbh_DrawFlatRect) pgbh_DrawFlatRect;
    decltype(&gbh_DrawQuad) pgbh_DrawQuad;
    decltype(&gbh_DrawQuadClipped) pgbh_DrawQuadClipped;
    decltype(&gbh_DrawTilePart) pgbh_DrawTilePart;
    decltype(&gbh_DrawTriangle) pgbh_DrawTriangle;
    decltype(&gbh_EndLevel) pgbh_EndLevel;
    decltype(&gbh_EndScene) pgbh_EndScene;
    decltype(&gbh_FreeImageTable) pgbh_FreeImageTable;
    decltype(&gbh_FreePalette) pgbh_FreePalette;
    decltype(&gbh_FreeTexture) pgbh_FreeTexture;
    decltype(&gbh_GetGlobals) pgbh_GetGlobals;
    decltype(&gbh_GetUsedCache) pgbh_GetUsedCache;
    decltype(&gbh_Init) pgbh_Init;
    decltype(&gbh_InitDLL) pgbh_InitDLL;
    decltype(&gbh_InitImageTable) pgbh_InitImageTable;
    decltype(&gbh_LoadImage) pgbh_LoadImage;
    decltype(&gbh_LockTexture) pgbh_LockTexture;
    decltype(&gbh_Plot) pgbh_Plot;
    decltype(&gbh_PrintBitmap) pgbh_PrintBitmap;
    decltype(&gbh_RegisterPalette) pgbh_RegisterPalette;
    decltype(&gbh_RegisterTexture) pgbh_RegisterTexture;
    decltype(&gbh_ResetLights) pgbh_ResetLights;
    decltype(&gbh_SetAmbient) pgbh_SetAmbient;
    decltype(&gbh_SetCamera) pgbh_SetCamera;
    decltype(&gbh_SetColourDepth) pgbh_SetColourDepth;
    decltype(&gbh_SetWindow) pgbh_SetWindow;
    decltype(&gbh_UnlockTexture) pgbh_UnlockTexture;

    HINSTANCE hinstance;
};


inline void PopulateS3DFunctions(HINSTANCE h, S3DFunctions& f)
{
    f.hinstance = h;
    GetFunc(h, f.pConvertColourBank, "ConvertColourBank");
    GetFunc(h, f.pDrawLine, "DrawLine");
    GetFunc(h, f.pSetShadeTableA, "SetShadeTableA");
    GetFunc(h, f.pMakeScreenTable, "MakeScreenTable");
    GetFunc(h, f.pgbh_AddLight, "gbh_AddLight");
    GetFunc(h, f.pgbh_AssignPalette, "gbh_AssignPalette");
    GetFunc(h, f.pgbh_BeginLevel, "gbh_BeginLevel");
    GetFunc(h, f.pgbh_BeginScene, "gbh_BeginScene");
    GetFunc(h, f.pgbh_BlitBuffer, "gbh_BlitBuffer");
    GetFunc(h, f.pgbh_BlitImage, "gbh_BlitImage");
    GetFunc(h, f.pgbh_CloseDLL, "gbh_CloseDLL");
    GetFunc(h, f.pgbh_CloseScreen, "gbh_CloseScreen");
    GetFunc(h, f.pgbh_Convert16BitGraphic, "gbh_Convert16BitGraphic");
    GetFunc(h, f.pgbh_ConvertColour, "gbh_ConvertColour");
    GetFunc(h, f.pgbh_DrawFlatRect, "gbh_DrawFlatRect");
    GetFunc(h, f.pgbh_DrawQuad, "gbh_DrawQuad");
    GetFunc(h, f.pgbh_DrawQuadClipped, "gbh_DrawQuadClipped");
    GetFunc(h, f.pgbh_DrawTilePart, "gbh_DrawTilePart");
    GetFunc(h, f.pgbh_DrawTriangle, "gbh_DrawTriangle");
    GetFunc(h, f.pgbh_EndLevel, "gbh_EndLevel");
    GetFunc(h, f.pgbh_EndScene, "gbh_EndScene");
    GetFunc(h, f.pgbh_FreeImageTable, "gbh_FreeImageTable");
    GetFunc(h, f.pgbh_FreePalette, "gbh_FreePalette");
    GetFunc(h, f.pgbh_FreeTexture, "gbh_FreeTexture");
    GetFunc(h, f.pgbh_GetGlobals, "gbh_GetGlobals");
    GetFunc(h, f.pgbh_GetUsedCache, "gbh_GetUsedCache");
    GetFunc(h, f.pgbh_Init, "gbh_Init");
    GetFunc(h, f.pgbh_InitDLL, "gbh_InitDLL");
    GetFunc(h, f.pgbh_InitImageTable, "gbh_InitImageTable");
    GetFunc(h, f.pgbh_LoadImage, "gbh_LoadImage");
    GetFunc(h, f.pgbh_LockTexture, "gbh_LockTexture");
    GetFunc(h, f.pgbh_Plot, "gbh_Plot");
    GetFunc(h, f.pgbh_PrintBitmap, "gbh_PrintBitmap");
    GetFunc(h, f.pgbh_RegisterPalette, "gbh_RegisterPalette");
    GetFunc(h, f.pgbh_RegisterTexture, "gbh_RegisterTexture");
    GetFunc(h, f.pgbh_ResetLights, "gbh_ResetLights");
    GetFunc(h, f.pgbh_SetAmbient, "gbh_SetAmbient");
    GetFunc(h, f.pgbh_SetCamera, "gbh_SetCamera");
    GetFunc(h, f.pgbh_SetColourDepth, "gbh_SetColourDepth");
    GetFunc(h, f.pgbh_SetWindow, "gbh_SetWindow");
    GetFunc(h, f.pgbh_UnlockTexture, "gbh_UnlockTexture");
}
