#include "stdafx.h"
#include <stdlib.h>
#include <windows.h>
#include <Objbase.h>

#define FUNC_PTR(x, addr) using T##x = decltype(&x); T##x p##x = (T##x)addr;

LRESULT __stdcall WndProc_4D0A00(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
FUNC_PTR(WndProc_4D0A00, 0x4D0A00);

DWORD dword_673578 = 0;
DWORD dword_6732E8 = 0;
DWORD byte_5EAD68 = 0;

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    CoInitialize(0);

    WNDCLASSA WndClass = {};
    WndClass.style = CS_VREDRAW | CS_HREDRAW;
    WndClass.lpfnWndProc = pWndProc_4D0A00;
    WndClass.hInstance = hInstance;
    WndClass.hIcon = LoadIconA(hInstance, MAKEINTRESOURCEA(0x65));
    WndClass.hCursor = LoadCursorA(0, MAKEINTRESOURCEA(IDI_APPLICATION));
    WndClass.lpszMenuName = "WinMain";
    WndClass.lpszClassName = "WinMain";
    RegisterClassA(&WndClass);


    char windowName[255] = {};
    // TODO: Call 0x4D67D5 to get name
    strcpy_s(windowName, "temp");
    const HWND hWnd = CreateWindowExA(0, "WinMain", windowName, 0xCF0000u, 0, 0, 100, 100, 0, 0, hInstance, 0);
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

    const DWORD windowWidth = dword_673578 + windowRect.right + clientRect.left - clientRect.right - windowRect.left;
    const DWORD windowHeight = dword_6732E8 + windowRect.bottom + clientRect.top - clientRect.bottom - windowRect.top;

    int windowXPos = 0;
    int windowYPos = 0;
    if (byte_5EAD68) // do_corner_window
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
        // Log
    }

    // TODO: Other calls

    ShowWindow(hWnd, nShowCmd);
    UpdateWindow(hWnd);

    // TODO: Other calls

    // TODO: Other loops
    MSG Msg = {};
    while (PeekMessageA(&Msg, 0, 0, 0, PM_REMOVE))
    {
        if (Msg.message == WM_QUIT)
        {
            //sub_44BA40();
            return Msg.wParam;
        }
        TranslateMessage(&Msg);
        DispatchMessageA(&Msg);
    }

    CoUninitialize();

    return 0;
}

int CALLBACK WrapWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    // Don't return to "real" WinMain otherwise it will run again
    const int ret = WinMain(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
    exit(ret);
}
