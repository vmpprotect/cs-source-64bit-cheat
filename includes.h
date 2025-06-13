#include <imgui/imgui.h>
#include <imgui/imgui_impl_win32.h>
#include <imgui/imgui_impl_dx9.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <d3d9.h>
#include <dwmapi.h>
#include <iostream>
#include <string>
#include <algorithm>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd , UINT msg , WPARAM wParam , LPARAM lParam );

HWND game_handle = nullptr;
HWND overlay_handle = nullptr;
LPDIRECT3D9 pD3D = nullptr;
LPDIRECT3DDEVICE9 d3d_device = nullptr;
HANDLE process_handle = nullptr;
DWORD prcoess_id = 0;
uintptr_t client_base = 0;
uintptr_t engine_base = 0;

#include <core/helpers/helpers.h>
#include <core/source/sdk/offsets.h>
#include <core/source/sdk/types/matrixs.h> // idk what to call this
#include <core/source/sdk/types/vector.h>

#include <core/source/sdk/player.h>