#include <SDL.h>
#include <SDL_image.h>
#include <SDL_thread.h>
#include <stdio.h>

#include <CustomGameCharacters.hpp>
#include <CustomScreen.hpp>
#include <Stage.hpp>
#include <World.hpp>
#include <algorithm>
#include <deque>
#include <memory>
#include <platform.hpp>
#include <string>
#include <tuple>
#include <vector>

#ifdef RUNNING_ON_WINDOWS
#include <windows.h>
#ifdef DEBUG
#include <d3d11.h>
#include <imgui.h>
#include <imgui_impl_dx11.h>
#include <imgui_impl_win32.h>
#include <tchar.h>
#endif
#endif

int custom_main(int, char**);
class FPSControl {
 private:
  Uint32 tick;
  Uint32 lastSecondTreshold;
  Uint32 lastSecondTick;
  Uint32 frameCounter;
  Uint32 fpsLimit;
  Uint32 defaultDelay;
  std::deque<Uint32> samples;

 public:
  static const Uint32 secondinMS = 1000;
  FPSControl(Uint32 fpsLimit) {
    this->lastSecondTreshold = SDL_GetTicks();
    this->tick = SDL_GetTicks();
    this->lastSecondTick = SDL_GetTicks();
    this->fpsLimit = fpsLimit;
    this->defaultDelay = secondinMS / fpsLimit;
    this->samples = std::deque<Uint32>(20, -1);
  }

  Uint32 getLastSecondTreshold() { return this->lastSecondTreshold; }

  int getNecessarydelay() {
    Uint32 timePassed = SDL_GetTicks() - this->tick;
    this->tick = SDL_GetTicks();

    Uint32 delay;
    if (timePassed > this->defaultDelay) {
      delay = 0;
    } else if (timePassed <= this->defaultDelay) {
      delay = this->defaultDelay;
    }

    frameCounter++;
    if (frameCounter >= 60 ||
        (SDL_GetTicks() - this->lastSecondTick) >= secondinMS) {
      this->lastSecondTreshold = SDL_GetTicks() - this->lastSecondTick;
      this->lastSecondTick = SDL_GetTicks();
      this->samples.pop_back();
      this->samples.push_front(frameCounter);
      frameCounter = 0;
    }

    return delay;
  }
  std::deque<Uint32> getFpsSamples() { return this->samples; }
};

#ifdef RUNNING_ON_WINDOWS
#ifdef DEBUG
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();

void fpsControlDebugWindow(FPSControl* fpsControl);

void cameraDebugWindow(Camera* camera, CustomSDLMaterialObject* followedObject);
void backgroundRenderDebugWindow(
    Camera* camera, std::vector<std::shared_ptr<Region>> regionsToRender);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  return custom_main(0, 0);
}
#else

int main(int, char**) { return custom_main(0, 0); }
#endif

int custom_main(int, char**) {
  printf("Inicializando...");
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    SDL_Log("error initializing SDL: %s\n", SDL_GetError());
  }

  std::shared_ptr<Screen> screen(new Screen(640, 480));

  // TODO start screen
  CustomPlayer* player =
      new CustomPlayer(new CustomSDLRect(new SDL_Rect({0, 0, 300, 300})),
                       new CustomSDLRect(new SDL_Rect({0, 0, 50, 50})), 10, 10);

  std::shared_ptr<Camera> camera(
      new Camera(screen->getWindow(), player, new int(100)));

  player->setTexture(IMG_LoadTexture(
      camera->getRenderer(),
      "C:/Users/lucas/git/C/game/media/img/eu-foto-crianca.JPG"));

  std::shared_ptr<Region> region(new DynamicRegion(
      {}, new CustomSDLRect(new SDL_Rect({-1000, -1000, 2000, 2000})),
      camera->getRenderer()));
  region->addObjectToRegion(player);

  std::unordered_set<std::shared_ptr<Region>> regionsOnStage;
  regionsOnStage.insert(region);
  Stage* stage = new Stage(
      regionsOnStage,
      new CustomSDLRect(new SDL_Rect({-40000, -40000, 80000, 80000})));

  int close = 0;
#ifdef RUNNING_ON_WINDOWS
#ifdef DEBUG
  WNDCLASSEXW wc = {sizeof(wc),
                    CS_CLASSDC,
                    WndProc,
                    0L,
                    0L,
                    GetModuleHandle(NULL),
                    NULL,
                    NULL,
                    NULL,
                    NULL,
                    L"ImGui Example",
                    NULL};
  ::RegisterClassExW(&wc);
  HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX11 Example",
                              WS_EX_LEFT, 100, 100, 1024, 768, NULL, NULL,
                              wc.hInstance, NULL);

  // Initialize Direct3D
  if (!CreateDeviceD3D(hwnd)) {
    CleanupDeviceD3D();
    ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
    return 1;
  }

  // Show the window
  ::ShowWindow(hwnd, SW_SHOWNORMAL);
  ::UpdateWindow(hwnd);

  ImGui::CreateContext();

  // Initialize Platform + Renderer backends (here: using imgui_impl_win32.cpp +
  // imgui_impl_dx11.cpp)
  ImGui_ImplWin32_Init(hwnd);
  ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
#endif
#endif
  FPSControl* fpsControl = new FPSControl(60);
  while (!close) {
    SDL_Event event;

    SDL_PumpEvents();
    while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_FIRSTEVENT,
                          SDL_DISPLAYEVENT)) {
      switch (event.type) {
        case SDL_QUIT:
          // handling of close button
          close = 1;
          break;
      }
    }

    player->handleEvent(event);
    camera->followObject();
    std::vector<std::shared_ptr<Region>> regionsToRender;
    try {
      regionsToRender = camera->getRegionsToFilm(stage);
    } catch (StageOutOfBounds err) {
      // Load new stage
    }

    SDL_RenderClear(camera->getRenderer());
    std::vector<CustomSDLMaterialObject*> drawableObjects;
    for (std::shared_ptr<Region> region : regionsToRender) {
      SDL_RenderCopy(camera->getRenderer(),
                     region->getBackground()->getTexture(),
                     region->getSrcRect(camera->getCameraRect()).get(),
                     region->getDestinationRect(camera->getCameraRect()).get());
      for (CustomSDLMaterialObject* object : region->getObjectsOnRegion()) {
        if (SDL_HasIntersection(object->getDestination(),
                                camera->getCameraRect())) {
          drawableObjects.push_back(object);
        }
      }
    }
    for (CustomSDLMaterialObject* object : drawableObjects) {
      SDL_RenderCopy(camera->getRenderer(), object->getTexture(),
                     object->getSrcRect(),
                     object->getDestination(camera->getCameraRect()));
    }

    SDL_RenderPresent(camera->getRenderer());

#ifdef RUNNING_ON_WINDOWS
#ifdef DEBUG
    MSG msg;
    while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE)) {
      ::TranslateMessage(&msg);
      ::DispatchMessage(&msg);
      if (msg.message == WM_QUIT) close = 1;
    }
    // Beginning of frame: update Renderer + Platform backend, start Dear ImGui
    // frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    fpsControlDebugWindow(fpsControl);
    cameraDebugWindow(camera.get(), player);
    backgroundRenderDebugWindow(camera.get(), regionsToRender);

    ImGui::Render();
    // End of frame: render Dear ImGui
    const float clear_color_with_alpha[4] = {
        clear_color.x * clear_color.w, clear_color.y * clear_color.w,
        clear_color.z * clear_color.w, clear_color.w};
    g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
    g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView,
                                               clear_color_with_alpha);

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // Swap
    g_pSwapChain->Present(1, 0);
#endif
#endif

    SDL_Delay(fpsControl->getNecessarydelay());
  }
#ifdef RUNNING_ON_WINDOWS
#ifdef DEBUG
  ImGui_ImplDX11_Shutdown();
  ImGui_ImplWin32_Shutdown();
  ImGui::DestroyContext();

  CleanupDeviceD3D();
  ::DestroyWindow(hwnd);
  ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
#endif
#endif
  // close SDL
  SDL_Quit();

  return 0;
}
#ifdef RUNNING_ON_WINDOWS
#ifdef DEBUG
bool CreateDeviceD3D(HWND hWnd) {
  // Setup swap chain
  DXGI_SWAP_CHAIN_DESC sd;
  ZeroMemory(&sd, sizeof(sd));
  sd.BufferCount = 2;
  sd.BufferDesc.Width = 0;
  sd.BufferDesc.Height = 0;
  sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  sd.BufferDesc.RefreshRate.Numerator = 60;
  sd.BufferDesc.RefreshRate.Denominator = 1;
  sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
  sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  sd.OutputWindow = hWnd;
  sd.SampleDesc.Count = 1;
  sd.SampleDesc.Quality = 0;
  sd.Windowed = TRUE;
  sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

  UINT createDeviceFlags = 0;
  // createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
  D3D_FEATURE_LEVEL featureLevel;
  const D3D_FEATURE_LEVEL featureLevelArray[2] = {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_0,
  };
  HRESULT res = D3D11CreateDeviceAndSwapChain(
      NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags,
      featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain,
      &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext);
  if (res == DXGI_ERROR_UNSUPPORTED)  // Try high-performance WARP software
                                      // driver if hardware is not available.
    res = D3D11CreateDeviceAndSwapChain(
        NULL, D3D_DRIVER_TYPE_WARP, NULL, createDeviceFlags, featureLevelArray,
        2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel,
        &g_pd3dDeviceContext);
  if (res != S_OK) return false;

  CreateRenderTarget();
  return true;
}

void CleanupDeviceD3D() {
  CleanupRenderTarget();
  if (g_pSwapChain) {
    g_pSwapChain->Release();
    g_pSwapChain = NULL;
  }
  if (g_pd3dDeviceContext) {
    g_pd3dDeviceContext->Release();
    g_pd3dDeviceContext = NULL;
  }
  if (g_pd3dDevice) {
    g_pd3dDevice->Release();
    g_pd3dDevice = NULL;
  }
}

void CreateRenderTarget() {
  ID3D11Texture2D* pBackBuffer;
  g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
  g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL,
                                       &g_mainRenderTargetView);
  pBackBuffer->Release();
}

void CleanupRenderTarget() {
  if (g_mainRenderTargetView) {
    g_mainRenderTargetView->Release();
    g_mainRenderTargetView = NULL;
  }
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
      if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED) {
        CleanupRenderTarget();
        g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam),
                                    (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN,
                                    0);
        CreateRenderTarget();
      }
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

void fpsControlDebugWindow(FPSControl* fpsControl) {
  ImGui::SetNextWindowSize(ImVec2(250, 250));
  ImGui::Begin("FPS Control");

  float samples[fpsControl->getFpsSamples().size()];
  std::string fps[fpsControl->getFpsSamples().size()];
  for (int n = 0; n < fpsControl->getFpsSamples().size(); n++) {
    int sample = fpsControl->getFpsSamples()[n];
    if (sample > -1) {
      samples[n] = sample;
      fps[n] = ("FPS: " + std::to_string(sample)).c_str();
    }
  }
  ImGui::PlotLines("FPS Samples", samples, fpsControl->getFpsSamples().size());

  // Display contents in a scrolling region
  ImGui::TextColored(
      ImVec4(1, 1, 0, 1),
      ("FPS: " + fps[0] + " in " +
       std::to_string(fpsControl->getLastSecondTreshold()) + " ms")
          .c_str());
  ImGui::BeginChild("Scrolling");
  for (std::string s : fps) ImGui::Text(s.c_str());
  ImGui::EndChild();

  ImGui::End();
}

void cameraDebugWindow(Camera* camera,
                       CustomSDLMaterialObject* followedObject) {
  ImGui::SetNextWindowSize(ImVec2(320, 300));
  ImGui::Begin("Camera Control");
  ImGui::Text("Camera SDL_Rect");
  ImGui::Separator();
  ImGui::Text(("X: " + std::to_string(camera->getCameraRect()->x)).c_str());
  ImGui::Text(("Y: " + std::to_string(camera->getCameraRect()->y)).c_str());
  ImGui::Text(("W: " + std::to_string(camera->getCameraRect()->w)).c_str());
  ImGui::Text(("H: " + std::to_string(camera->getCameraRect()->h)).c_str());

  ImGui::Separator();
  ImGui::Separator();

  ImGui::Text("Followed Object SDL_Rect");
  ImGui::BeginTable("Followed Object", 2);
  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::Text(
      ("X: " + std::to_string(followedObject->getDestination()->x)).c_str());
  ImGui::TableNextColumn();
  ImGui::Text(
      ("X relative: " + std::to_string(followedObject->getDestination()->x))
          .c_str());
  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::Text(
      ("Y: " + std::to_string(followedObject->getDestination()->y)).c_str());
  ImGui::TableNextColumn();
  ImGui::Text(
      ("Y relative: " + std::to_string(followedObject->getDestination()->y))
          .c_str());
  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::Text(
      ("W: " + std::to_string(followedObject->getDestination()->w)).c_str());
  ImGui::TableNextColumn();
  ImGui::Text(("src X - W " + std::to_string(followedObject->getSrcRect()->x) +
               " - " + std::to_string(followedObject->getSrcRect()->w))
                  .c_str());
  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::Text(
      ("H: " + std::to_string(followedObject->getDestination()->h)).c_str());
  ImGui::TableNextColumn();
  ImGui::Text(("src Y - H " + std::to_string(followedObject->getSrcRect()->y) +
               " - " + std::to_string(followedObject->getSrcRect()->h))
                  .c_str());
  ImGui::EndTable();
  ImGui::Separator();
  ImGui::Text(("Is intersecting: " +
               std::to_string(SDL_HasIntersection(
                   followedObject->getDestination(), camera->getCameraRect())))
                  .c_str());

  ImGui::End();
}

void backgroundRenderDebugWindow(
    Camera* camera, std::vector<std::shared_ptr<Region>> regionsToRender) {
  ImGui::SetNextWindowSize(ImVec2(320, 600));
  ImGui::Begin("Background Region Control");
  ImGui::Text(("Regions: " + std::to_string(regionsToRender.size())).c_str());
  ImGui::Separator();
  ImGui::Columns(regionsToRender.size(), "regionsColumns");
  for (std::shared_ptr<Region> region : regionsToRender) {
    ImGui::Text(("X src: " +
                 std::to_string(region->getSrcRect(camera->getCameraRect())->x))
                    .c_str());
    ImGui::Text(("Y src: " +
                 std::to_string(region->getSrcRect(camera->getCameraRect())->y))
                    .c_str());
    ImGui::Text(("W src: " +
                 std::to_string(region->getSrcRect(camera->getCameraRect())->w))
                    .c_str());
    ImGui::Text(("H src: " +
                 std::to_string(region->getSrcRect(camera->getCameraRect())->h))
                    .c_str());

    ImGui::Text(
        ("X dest: " +
         std::to_string(region->getDestinationRect(camera->getCameraRect())->x))
            .c_str());
    ImGui::Text(
        ("Y dest: " +
         std::to_string(region->getDestinationRect(camera->getCameraRect())->y))
            .c_str());
    ImGui::Text(
        ("W dest: " +
         std::to_string(region->getDestinationRect(camera->getCameraRect())->w))
            .c_str());
    ImGui::Text(
        ("H dest: " +
         std::to_string(region->getDestinationRect(camera->getCameraRect())->h))
            .c_str());

    ImGui::NextColumn();
  }

  ImGui::Separator();

  ImGui::End();
}

#endif
#endif