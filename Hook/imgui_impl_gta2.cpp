#include "stdafx.h"
#include "imgui_impl_gta2.h"
#include "d3ddll.hpp"

void Convert(const ImDrawVert* im, Vert* v)
{
    v->x = im->pos.x;
    v->y = im->pos.y;
    v->u = im->uv.x;
    v->v = im->uv.y;
    v->z = 14;
    v->diff = 0xa0000000;
}

S3DFunctions* gApi = nullptr;
void ImGui_Impl_GTA2_Render(ImDrawData* draw_data)
{
    draw_data->DeIndexAllBuffers();

    for (int n = 0; n < draw_data->CmdListsCount; n++)
    {
        const ImDrawList* cmd_list = draw_data->CmdLists[n];
        const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;
       // const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
        {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];

            for (unsigned int i = 0; i < pcmd->ElemCount / 3; i++)
            {
                Verts v = {};
                for (int q = 0; q < 3; q++)
                {
                    Convert(vtx_buffer, &v.mVerts[q]);
                    vtx_buffer++;
                }

                gApi->pgbh_DrawTriangle(0, nullptr, &v , 255);
            }

            //idx_buffer += pcmd->ElemCount;

            vtx_buffer += pcmd->ElemCount;
        }
    }

}

IMGUI_API void ImGui_Impl_GTA2_Init(void* hwnd, void* device, S3DFunctions& api)
{
    gApi = &api;

    ImGuiIO& io = ImGui::GetIO();

    unsigned char* pixels;
    int width, height;
    io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

    io.Fonts->TexID = (void *)(intptr_t)pixels;
}

IMGUI_API void ImGui_Impl_GTA2_Shutdown()
{

}

IMGUI_API void ImGui_Imp_GTA2_NewFrame()
{
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2((float)640, (float)480);
    io.RenderDrawListsFn = ImGui_Impl_GTA2_Render;
    
    ImGui::NewFrame();
}

IMGUI_API LRESULT ImGui_Imp_GTA2_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return 0;
}
