#pragma once

#include "imgui.h"
#include <windows.h>

struct S3DFunctions;

IMGUI_API void ImGui_Impl_GTA2_Init(void* hwnd, void* device, S3DFunctions& api);
IMGUI_API void ImGui_Impl_GTA2_Shutdown();
IMGUI_API void ImGui_Imp_GTA2_NewFrame();

IMGUI_API LRESULT ImGui_Imp_GTA2_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
