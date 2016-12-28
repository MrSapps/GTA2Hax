#include "stdafx.h"
#include <stdlib.h>
#include <windows.h>
#include <Objbase.h>
#include <type_traits>
#include "imgui_impl_gta2.h"
#include "d3ddll.hpp"
#include "detours.h"

#define FUNC_PTR(x, addr) using T##x = decltype(&x); T##x p##x = (T##x)addr;

LRESULT __stdcall WndProc_4D0A00(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
FUNC_PTR(WndProc_4D0A00, 0x4D0A00);

void __stdcall GetGameVersion_4D0920(DWORD* pMajorVersion, DWORD* pMinorVersion);
FUNC_PTR(GetGameVersion_4D0920, 0x4D0920);

void __stdcall GetDDAndOSVersion_4031C0(DWORD* pOSVersion, DWORD* pDDVersion);
FUNC_PTR(GetDDAndOSVersion_4031C0, 0x4031C0);

void ReadDebugSettings_451930();
FUNC_PTR(ReadDebugSettings_451930, 0x451930);

int __fastcall SetDebugBootMapEtc_45E8D0(void* thisPtr, void* hack); // thiscall
FUNC_PTR(SetDebugBootMapEtc_45E8D0, 0x45E8D0);

char ReadGraphicsSettings_4CB3F0();
FUNC_PTR(ReadGraphicsSettings_4CB3F0, 0x4CB3F0);

char* __cdecl jGetCurrentDirectory_4D8447(CHAR *pBuffer, int bufferSize);
FUNC_PTR(jGetCurrentDirectory_4D8447, 0x4D8447);

char __stdcall NetworkInits_4D0ED0(HINSTANCE hInstance);
FUNC_PTR(NetworkInits_4D0ED0, 0x4D0ED0);

int __stdcall DirectInputCreate8_44BA00(HINSTANCE hInstance);
FUNC_PTR(DirectInputCreate8_44BA00, 0x44BA00);

int __cdecl Vid_Init_4CC6D0(int windowWidth);
FUNC_PTR(Vid_Init_4CC6D0, 0x4CC6D0);

DWORD __cdecl TraceLog_450530(DWORD code, const char* fileName, DWORD line);
FUNC_PTR(TraceLog_450530, 0x450530);

int gbh_Init_4CAEA0();
FUNC_PTR(gbh_Init_4CAEA0, 0x4CAEA0);

char KeyBoardPlaySoundInit_461880();
FUNC_PTR(KeyBoardPlaySoundInit_461880, 0x461880);

int SMenu_ctor_457830();
FUNC_PTR(SMenu_ctor_457830, 0x457830);

char __fastcall FrontEndQ_4587B0(void* thisPtr, void* hack, unsigned __int16 a2); // thiscall
FUNC_PTR(FrontEndQ_4587B0, 0x4587B0);

signed int __fastcall sub_45A320(void* thisPtr, void* hack); //thiscall
FUNC_PTR(sub_45A320, 0x45A320);

bool GetCurrentWindowXY_4CC580();
FUNC_PTR(GetCurrentWindowXY_4CC580, 0x4CC580);

int sub_4CC6A0(DWORD, struct SGame*);
FUNC_PTR(sub_4CC6A0, 0x4CC6A0);

char GameOrRenderLoop_462A30();
FUNC_PTR(GameOrRenderLoop_462A30, 0x462A30);

int DeInits_459760();
FUNC_PTR(DeInits_459760, 0x459760);

DWORD sub_461DE0();
FUNC_PTR(sub_461DE0, 0x461DE0);

#define VAR(TypeName, VarName, Addr) TypeName& VarName = *reinterpret_cast<TypeName*>(Addr);
#define PTR(TypeName, VarName, Addr) std::remove_pointer<TypeName>::type* const VarName = reinterpret_cast<TypeName>(Addr);

VAR(DWORD, window_width_673578, 0x673578);
VAR(DWORD, window_height_6732E8, 0x6732E8);
VAR(DWORD, do_corner_window_5EAD68, 0x5EAD68);
VAR(DWORD, gGameMajorVersion_673F54, 0x673F54);
VAR(DWORD, gGameMinorVersion_673F58, 0x673F58);
VAR(DWORD, debug_skip_frontend_5EAD7C, 0x5EAD7C);
VAR(DWORD, gbDontSkipFrontEnd_595018, 0x595018);
VAR(HWND, hWnd, 0x673D18);
PTR(BYTE*, byte_5EC070, 0x5EC070);
VAR(HANDLE, ghMutex, 0x673D1C);
PTR(char*, gGameDirectory_673D28, 0x673D28); // size 256
VAR(HINSTANCE, ghInstance, 0x673E28);
VAR(BYTE*, g_menu_q_dword_5EB160, 0x5EB160);
VAR(BYTE, byte_673F50, 0x673F50);
VAR(BYTE, byte_6735A5, 0x6735A5);
PTR(struct SGame*, sgame_5EB4FC, 0x5EB4FC);
VAR(BYTE, byte_662786, 0x662786);
VAR(DWORD, dword_673E2C, 0x673E2C);

struct SGame1
{
    void* field_0_p0x85C_struct;
    DWORD field_4;
    DWORD field_8;
    DWORD field_C;
    DWORD field_10;
    DWORD field_14;
};
static_assert(sizeof(SGame1) == 0x18, "Wrong size SGame1");


struct SGame
{
    DWORD field_0;
    SGame1 field_4;
    DWORD field_1C;
    BYTE field_20;
    BYTE field_21;
    BYTE field_22;
    BYTE field_23_count;
    BYTE field_24;
    BYTE field_25;
    BYTE field_26;
    BYTE field_27;
    DWORD field_28;
    DWORD field_2C;
    BYTE field_30;
    BYTE field_31;
    BYTE field_32;
    BYTE field_33;
    DWORD field_34;
    DWORD field_38;
    BYTE field_3C_debug_skip_police_5EAD5E;
    BYTE field_3D;
    BYTE field_3E;
    BYTE field_3F;
};
static_assert(sizeof(SGame) == 0x40, "Wrong size SGame");

int __stdcall ErrorMessageBox_4D0900(LPCSTR lpText)
{
    return MessageBoxA(hWnd, lpText, "Error!", MB_OK);
}

int GetGameState_4D09C0(SGame* thisPtr)
{
    return thisPtr->field_2C;
}

// do_mike debug must be on for this to get called
int __fastcall sub_474530(void* thisPtr, void* hack);
decltype(&sub_474530) psub_474530 = (decltype(&sub_474530))0x474530;

S3DFunctions api;
int __fastcall sub_474530(void* thisPtr, void* hack)
{
    ImGui_Imp_GTA2_NewFrame();


    ImGui::Begin("blargh");
    ImGui::Button("Blah");
    ImGui::Text("Hello, world!");
    ImGui::End();

    ImGui::Render();

    /*
    const float kU = 14;

    // Top left
    Verts d = {};
    d.mVerts[0].x = 0;
    d.mVerts[0].y = 0;
    d.mVerts[0].z = kU;
    d.mVerts[0].diff = 0xa0000000;
    d.mVerts[0].u = 0.3f;
    d.mVerts[0].v = 0.3f;

    // Top right
    d.mVerts[1].x = 900;
    d.mVerts[1].y = 0;
    d.mVerts[1].z = kU;
    d.mVerts[1].diff = 0xa0000000;
    d.mVerts[1].u = 17.6999989f;
    d.mVerts[1].v = 0.3f;

    // Bottom right
    d.mVerts[2].x = 900;
    d.mVerts[2].y = 400;
    d.mVerts[2].z = kU;
    d.mVerts[2].diff = 0xa0000000;
    d.mVerts[2].u = 17.6999989f;
    d.mVerts[2].v = 25.0f;

    // Bottom left
    d.mVerts[3].x = 0;
    d.mVerts[3].y = 400;
    d.mVerts[3].z = kU;
    d.mVerts[3].diff = 0xa0000000;
    d.mVerts[3].u = 0.3f;
    d.mVerts[3].v = 25.0f;

    api.pgbh_DrawQuad(0, 0, &d, 255);
    */

    return 0;
}

void AddHooks()
{
    
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach((PVOID*)(&psub_474530), (PVOID)sub_474530);
    DetourTransactionCommit();
    
}

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    ghInstance = hInstance;

    if (FAILED(CoInitialize(0)))
    {
        return 0;
    }

    if (OpenMutexA(MUTEX_ALL_ACCESS, 0, "GBH_COOP_MUTEX"))
    {
        return 0;
    }

    ghMutex = CreateMutexA(0, 0, "GBH_COOP_MUTEX");
    pGetGameVersion_4D0920(&gGameMajorVersion_673F54, &gGameMinorVersion_673F58);


    DWORD osVersion = 0;
    DWORD ddVersion = 0;
    pGetDDAndOSVersion_4031C0(&osVersion, &ddVersion);
    if (osVersion < 0x601)
    {
        ErrorMessageBox_4D0900("ERROR: Incorrect version of DirectX");
        ReleaseMutex(ghMutex);
        CloseHandle(ghMutex);
        return -1;
    }

    pReadDebugSettings_451930();
    pSetDebugBootMapEtc_45E8D0(byte_5EC070, 0); // If skipping front end then reads the map/style to boot into etc
    gbDontSkipFrontEnd_595018 = debug_skip_frontend_5EAD7C == 0;

    WNDCLASSA WndClass = {};
    WndClass.style = CS_VREDRAW | CS_HREDRAW;

    // TODO: Hook to ImGui_Imp_GTA2_WndProcHandler()
    WndClass.lpfnWndProc = pWndProc_4D0A00;

    WndClass.hInstance = hInstance;
    WndClass.hIcon = LoadIconA(hInstance, MAKEINTRESOURCEA(0x65));
    WndClass.hCursor = LoadCursorA(0, MAKEINTRESOURCEA(IDI_APPLICATION));
    WndClass.lpszMenuName = "WinMain";
    WndClass.lpszClassName = "WinMain";
    RegisterClassA(&WndClass);

    pReadGraphicsSettings_4CB3F0();
    pjGetCurrentDirectory_4D8447(gGameDirectory_673D28, 256);

    // TODO
    //ParseCommandLineArgs_461810(thisPtr, lpCommandLine);

    if (!pNetworkInits_4D0ED0(hInstance))
    {
        ReleaseMutex(ghMutex);
        CloseHandle(ghMutex);
        return 0;
    }

    hWnd = CreateWindowExA(0, "WinMain", "GTA2", WS_OVERLAPPEDWINDOW, 0, 0, 100, 100, 0, 0, hInstance, 0);
    if (!hWnd)
    {
        return 0;
    }

    ShowWindow(hWnd, nShowCmd);
    UpdateWindow(hWnd);

    RECT clientRect = {};
    GetClientRect(hWnd, &clientRect);

    RECT windowRect = {};
    GetWindowRect(hWnd, &windowRect);

    const DWORD windowWidth = window_width_673578 + windowRect.right + clientRect.left - clientRect.right - windowRect.left;
    const DWORD windowHeight = window_height_6732E8 + windowRect.bottom + clientRect.top - clientRect.bottom - windowRect.top;

    int windowXPos = 0;
    int windowYPos = 0;
    if (do_corner_window_5EAD68)
    {
        windowXPos = 0;
        windowYPos = 0;
    }
    else
    {
        windowXPos = (GetSystemMetrics(SM_CXSCREEN) - windowWidth) / 2;
        windowYPos = (GetSystemMetrics(SM_CYSCREEN) - windowHeight) / 2;
    }

    if (!SetWindowPos(hWnd, 0, windowXPos, windowYPos, windowWidth, windowHeight, 0x314u))
    {
        pTraceLog_450530(7, "winmain.cpp", 673);
    }

    pDirectInputCreate8_44BA00(ghInstance);
    pVid_Init_4CC6D0(windowWidth);

    ShowWindow(hWnd, nShowCmd);
    UpdateWindow(hWnd);

    pgbh_Init_4CAEA0();

    // EXTRA
    PopulateS3DFunctions(GetModuleHandle(L"D3DDll.dll"), api);

    // END EXTRA

    pKeyBoardPlaySoundInit_461880();

    // EXTRA
    ImGui_Impl_GTA2_Init(hWnd, 0, api);

    // TODO: Other calls, video playing related etc

    MSG msg = {};
    int v12 = 0;
    bool bQuit = false;
restart_loop:
    while (1)
    {
        bQuit = false;
        if (!gbDontSkipFrontEnd_595018)
        {
            break;
        }
        pSMenu_ctor_457830();
        pFrontEndQ_4587B0(g_menu_q_dword_5EB160, 0, v12);
    after_front_end:
        pGetCurrentWindowXY_4CC580();
        psub_4CC6A0(v12, sgame_5EB4FC);
        while (1)
        {
            do
            {
                while (1)
                {
                    do
                    {
                        while (PeekMessageA(&msg, 0, 0, 0, 1u))
                        {
                            if (msg.message == WM_QUIT)
                            {
                                //sub_44BA40();
                                return msg.wParam;
                            }
                            TranslateMessage(&msg);
                            DispatchMessageA(&msg);
                        }
                    } while (bQuit || byte_673F50 == 2 || byte_6735A5);

                    if (!gbDontSkipFrontEnd_595018)
                    {
                        break;
                    }

                    auto frontEndRet =  psub_45A320(g_menu_q_dword_5EB160, 0); // Runs front end logic and renders it?
                 
                
                    if (frontEndRet == 1)
                    {
                        bQuit = 1;
                        pDeInits_459760();
                        DestroyWindow(hWnd);
                    }
                    else
                    {
                        if (frontEndRet == 3)
                        {
                            pDeInits_459760();
                            gbDontSkipFrontEnd_595018 = 0;
                            goto restart_loop;
                        }
                        if (frontEndRet == 4)
                        {
                            pDeInits_459760();
                            gbDontSkipFrontEnd_595018 = 0;
                            byte_662786 = 1;
                            goto restart_loop;
                        }
                    }
                }
                bQuit = pGameOrRenderLoop_462A30();
            } while (!bQuit);
            if (!dword_673E2C)
            {
                //Returns1();
                return 1;
            }

            if (!debug_skip_frontend_5EAD7C)
            {
                break;
            }

            DestroyWindow(hWnd);
        }
        if (dword_673E2C)
        {
            if (GetGameState_4D09C0(sgame_5EB4FC) == 1)
            {
                DestroyWindow(hWnd);
            }
            else
            {
            LABEL_47:
                v12 = 7;
                //sub_462060();
                gbDontSkipFrontEnd_595018 = 1;
            }
        }
        else
        {
            switch (GetGameState_4D09C0(sgame_5EB4FC))
            {
            case 5:
                goto LABEL_47;
            case 1:
                DestroyWindow(hWnd);
                break;
            case 2:
                /*
                sub_45EC20(sgame_5EB4FC->field_38);
                sub_4A8F90(klass_2BC0_66B404, sgame_5EB4FC->field_38);
                sub_4A90A0(klass_2BC0_66B404);
                v17 = -(sub_45E540((int)byte_5EC070) != 0);
                LOBYTE(v17) = v17 & 0xFB;
                v12 = v17 + 11;
                sub_462060();
                gbDontSkipFrontEnd_595018 = 1;
                */
                break;
            case 3:
                /*
                sub_45EC20(sgame_5EB4FC->field_38);
                sub_4A8F90(klass_2BC0_66B404, sgame_5EB4FC->field_38);
                sub_4A90A0(klass_2BC0_66B404);
                v12 = sub_45E540((int)byte_5EC070) != 0 ? 6 : 2;
                sub_462060();
                gbDontSkipFrontEnd_595018 = 1;
                */
                break;
            case 4:
                /*
                sub_45EC20(sgame_5EB4FC->field_38);
                sub_4A8F90(klass_2BC0_66B404, sgame_5EB4FC->field_38);
                sub_4A90A0(klass_2BC0_66B404);
                v12 = sub_45E540((int)byte_5EC070) != 0 ? 6 : 3;
                sub_462060();
                gbDontSkipFrontEnd_595018 = 1;
                */
                break;
            case 6:
                v12 = 0;
                //sub_462060();
                gbDontSkipFrontEnd_595018 = 1;
                break;
            default:
                continue;
            }
        }
    }

    
    psub_461DE0();
    //if (!dword_673E2C || sub_40BFA0(&unk_674268))
    {
        goto after_front_end;
    }
    
    // EXTRA
    ImGui_Impl_GTA2_Shutdown();

    CoUninitialize();


    return 0;
}

int CALLBACK WinMainWrapper(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    // Don't return to "real" WinMain otherwise it will run again
    AddHooks();
    const int ret = WinMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
    exit(ret);
}
