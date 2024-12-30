#ifndef CUSTOM_DEBUG_INTERFACE_H
#define CUSTOM_DEBUG_INTERFACE_H

#include <d3d9.h>
#include <tchar.h>

#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"

// Data
static LPDIRECT3D9 g_pD3D = nullptr;
static LPDIRECT3DDEVICE9 g_pd3dDevice = nullptr;
static bool g_DeviceLost = false;
static UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
static D3DPRESENT_PARAMETERS g_d3dpp = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void ResetDevice();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#include <exception>
#include <unordered_map>

class DebugWindows {
 private:
  bool show_demo_window;
  bool show_another_window;

  ImVec4 clear_color;

  HWND hwnd;
  WNDCLASSEXW wc;

  CustomPlayer* player;
  Stage* stage;
  CameraSDL* camera;

 public:
  DebugWindows() {
    // Create application window
    // ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = {sizeof(wc),
                      CS_CLASSDC,
                      WndProc,
                      0L,
                      0L,
                      GetModuleHandle(nullptr),
                      nullptr,
                      nullptr,
                      nullptr,
                      nullptr,
                      L"Debug Window",
                      nullptr};
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Debug Window",
                                WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800,
                                nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd)) {
      CleanupDeviceD3D();
      ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
      throw std::exception();
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
    io.ConfigFlags |=
        ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

    // Our state
    this->show_demo_window = true;
    this->show_another_window = false;
    this->clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  }

  virtual ~DebugWindows() {
    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
    /*
      CleanupDeviceD3D();
      ::DestroyWindow(hwnd);
      ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
  */
  }

  void trackPlayer(CustomPlayer* trackedPlayer) {
    this->player = trackedPlayer;
  }

  void trackStage(Stage* trackedStage) { this->stage = trackedStage; }

  bool loop() {
    MSG msg;
    while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
      ::TranslateMessage(&msg);
      ::DispatchMessage(&msg);
      if (msg.message == WM_QUIT) return false;
    }

    // Handle lost D3D9 device
    if (g_DeviceLost) {
      HRESULT hr = g_pd3dDevice->TestCooperativeLevel();
      if (hr == D3DERR_DEVICELOST) {
        ::Sleep(10);
        return false;
      }
      if (hr == D3DERR_DEVICENOTRESET) ResetDevice();
      g_DeviceLost = false;
    }

    // Handle window resize (we don't resize directly in the WM_SIZE handler)
    if (g_ResizeWidth != 0 && g_ResizeHeight != 0) {
      g_d3dpp.BackBufferWidth = g_ResizeWidth;
      g_d3dpp.BackBufferHeight = g_ResizeHeight;
      g_ResizeWidth = g_ResizeHeight = 0;
      ResetDevice();
    }

    // Start the Dear ImGui frame
    ImGui_ImplDX9_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    // 2. Show a simple window that we create ourselves. We use a Begin/End pair
    // to create a named window.
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Player 1 Window");

    ImGui::Separator();

    ImGui::Text("destination x %d", this->player->getDestination().x);
    ImGui::Text("destination y %d", this->player->getDestination().y);
    ImGui::Text("destination w %d", this->player->getDestination().w);
    ImGui::Text("destination h %d", this->player->getDestination().h);
    ImGui::Separator();
    ImGui::Text("Physics");
    ImGui::Text("RigidBody x,y %d,%d",
                static_cast<int>(this->player->getRigidBody()
                                     ->getWorldTransform()
                                     .getOrigin()
                                     .getX()),
                static_cast<int>(this->player->getRigidBody()
                                     ->getWorldTransform()
                                     .getOrigin()
                                     .getY()));
    ImGui::Text("RigidBody linearVel x,y %.2f , %.2f",
                static_cast<float>(
                    this->player->getRigidBody()->getLinearVelocity().getX()),
                static_cast<float>(
                    this->player->getRigidBody()->getLinearVelocity().getY()));

    ImGui::End();
    ImGui::Begin("Regions");

    ImGui::Separator();
    ImGui::BeginTable("TableExample", 2);
    ImGui::TableSetupColumn("Column 1");
    ImGui::TableSetupColumn("Column 2");
    ImGui::TableHeadersRow();

    for (auto regionId : stage->getRegionIds()) {
      ImGui::TableNextRow();
      ImGui::TableSetColumnIndex(0);
      ImGui::Text("%d", regionId.id.first);
      ImGui::TableSetColumnIndex(1);
      ImGui::Text("%d", regionId.id.second);
    }

    ImGui::EndTable();
    ImGui::End();

    // Rendering
    ImGui::EndFrame();

    // Rendering
    ImGui::EndFrame();
    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    g_pd3dDevice->SetRenderState(D3DRS_SCISSORTESTENABLE, FALSE);
    D3DCOLOR clear_col_dx =
        D3DCOLOR_RGBA((int)(clear_color.x * clear_color.w * 255.0f),
                      (int)(clear_color.y * clear_color.w * 255.0f),
                      (int)(clear_color.z * clear_color.w * 255.0f),
                      (int)(clear_color.w * 255.0f));
    g_pd3dDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
                        clear_col_dx, 1.0f, 0);
    if (g_pd3dDevice->BeginScene() >= 0) {
      ImGui::Render();
      ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
      g_pd3dDevice->EndScene();
    }
    HRESULT result = g_pd3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
    if (result == D3DERR_DEVICELOST) g_DeviceLost = true;

    return true;
  }
};

// Helper functions

bool CreateDeviceD3D(HWND hWnd) {
  if ((g_pD3D = Direct3DCreate9(D3D_SDK_VERSION)) == nullptr) return false;

  // Create the D3DDevice
  ZeroMemory(&g_d3dpp, sizeof(g_d3dpp));
  g_d3dpp.Windowed = TRUE;
  g_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  g_d3dpp.BackBufferFormat =
      D3DFMT_UNKNOWN;  // Need to use an explicit format with alpha if needing
                       // per-pixel alpha composition.
  g_d3dpp.EnableAutoDepthStencil = TRUE;
  g_d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
  g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;  // Present with vsync
  // g_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;   // Present
  // without vsync, maximum unthrottled framerate
  if (g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                           D3DCREATE_HARDWARE_VERTEXPROCESSING, &g_d3dpp,
                           &g_pd3dDevice) < 0)
    return false;

  return true;
}

void CleanupDeviceD3D() {
  if (g_pd3dDevice) {
    g_pd3dDevice->Release();
    g_pd3dDevice = nullptr;
  }
  if (g_pD3D) {
    g_pD3D->Release();
    g_pD3D = nullptr;
  }
}

void ResetDevice() {
  ImGui_ImplDX9_InvalidateDeviceObjects();
  HRESULT hr = g_pd3dDevice->Reset(&g_d3dpp);
  if (hr == D3DERR_INVALIDCALL) IM_ASSERT(0);
  ImGui_ImplDX9_CreateDeviceObjects();
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if
// dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your
// main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to
// your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from
// your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
  if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;

  switch (msg) {
    case WM_SIZE:
      if (wParam == SIZE_MINIMIZED) return 0;
      g_ResizeWidth = (UINT)LOWORD(lParam);  // Queue resize
      g_ResizeHeight = (UINT)HIWORD(lParam);
      return 0;
    case WM_SYSCOMMAND:
      if ((wParam & 0xfff0) == SC_KEYMENU)  // Disable ALT application menu
        return 0;
      break;
    case WM_DESTROY:
      ::PostQuitMessage(0);
      return 0;
  }
  return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
#endif