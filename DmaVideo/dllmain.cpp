#include "stdafx.h"
#include <stdio.h>
#include <objbase.h>

static bool gConsoleDone = false;

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        if (!gConsoleDone)
        {
            gConsoleDone = true;
            AllocConsole();
            freopen("CONOUT$", "w", stdout);
            SetConsoleTitleA("GTA2 debug console");
            SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_RED);
        }
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

