#include "stdafx.h"
#include <stdlib.h>
#include <windows.h>
#include <Objbase.h>
#include <type_traits>

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

int sub_457830();
FUNC_PTR(sub_457830, 0x457830);

char __fastcall FrontEndQ_4587B0(void* thisPtr, void* hack, unsigned __int16 a2); // thiscall
FUNC_PTR(FrontEndQ_4587B0, 0x4587B0);

signed int __fastcall sub_45A320(void* thisPtr, void* hack); //thiscall
FUNC_PTR(sub_45A320, 0x45A320);

bool GetCurrentWindowXY_4CC580();
FUNC_PTR(GetCurrentWindowXY_4CC580, 0x4CC580);

int sub_4CC6A0(DWORD, DWORD);
FUNC_PTR(sub_4CC6A0, 0x4CC6A0);

char GameOrRenderLoop_462A30();
FUNC_PTR(GameOrRenderLoop_462A30, 0x462A30);

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

int __stdcall ErrorMessageBox_4D0900(LPCSTR lpText)
{
    return MessageBoxA(hWnd, lpText, "Error!", MB_OK);
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

    hWnd = CreateWindowExA(0, "WinMain", "GTA2", 0xCF0000u, 0, 0, 100, 100, 0, 0, hInstance, 0);
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
    pKeyBoardPlaySoundInit_461880();

    // TODO: Other calls, video playing related etc

    bool bQuit = false;
    while (1)
    {
        bQuit = false;
        if (!gbDontSkipFrontEnd_595018)
        {
            break;
        }

        psub_457830();
        pFrontEndQ_4587B0(g_menu_q_dword_5EB160, 0, 0 /*v12*/);
    //LABEL_26:
        pGetCurrentWindowXY_4CC580();
        psub_4CC6A0(/*v12*/ 0, windowWidth); // Inits more settings
        while (1)
        {
            do
            {
                while (1)
                {
                    do
                    {
                        MSG Msg = {};
                        while (PeekMessageA(&Msg, 0, 0, 0, PM_REMOVE))
                        {
                            if (Msg.message == WM_QUIT)
                            {
                                //sub_44BA40(); // TODO
                                return Msg.wParam;
                            }
                            TranslateMessage(&Msg);
                            DispatchMessageA(&Msg);
                        }
                    } while (bQuit || byte_673F50 == 2 || byte_6735A5);

                    if (!gbDontSkipFrontEnd_595018)
                    {
                        break;
                    }

                    // Seems to run logic loop and rendering of the front end
                    auto frontEndRet = psub_45A320(g_menu_q_dword_5EB160, 0);
                }
                bQuit = pGameOrRenderLoop_462A30();
            } while (!bQuit);
        }
    }

    CoUninitialize();

    return 0;
}

int CALLBACK WinMainWrapper(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    // Don't return to "real" WinMain otherwise it will run again
    const int ret = WinMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
    exit(ret);
}
