#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_thread.h>
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
#include <filesystem>

#define DEBUG

#ifdef RUNNING_ON_WINDOWS
#include <windows.h>
#ifdef DEBUG

#define ImTextureID ImU64
#include <imgui.h>
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <tchar.h>

#ifdef _DEBUG
#define DX12_ENABLE_DEBUG_LAYER
#endif

#ifdef DX12_ENABLE_DEBUG_LAYER
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif
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

  long getLastFrameTick() { return this->tick; }
  std::deque<Uint32> getFpsSamples() { return this->samples; }
};

class EventControl {
 private:
  std::queue<CustomEvent*> eventsQueue;
  std::vector<EventListener*> eventListeners;

 public:
  virtual ~EventControl() {}
  void addEventListener(EventListener* listener) {
    eventListeners.push_back(listener);
  }
  virtual void addEvent(CustomEvent* event) { eventsQueue.push(event); }
  void processEvents() {
    while (!eventsQueue.empty()) {
      CustomEvent* event = this->eventsQueue.front();
      for (EventListener* listener : eventListeners) {
        if (!listener->handleEvent(event)) {
          // ERROR
        }
      }
      this->eventsQueue.pop();
      delete event;
    }
  }
};

#ifdef RUNNING_ON_WINDOWS
#ifdef DEBUG
struct FrameContext
{
    ID3D12CommandAllocator* CommandAllocator;
    UINT64                  FenceValue;
};
ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
// Data
static int const                    NUM_FRAMES_IN_FLIGHT = 3;
static FrameContext                 g_frameContext[NUM_FRAMES_IN_FLIGHT] = {};
static UINT                         g_frameIndex = 0;

static int const                    NUM_BACK_BUFFERS = 3;
static ID3D12Device*                g_pd3dDevice = nullptr;
static ID3D12DescriptorHeap*        g_pd3dRtvDescHeap = nullptr;
static ID3D12DescriptorHeap*        g_pd3dSrvDescHeap = nullptr;
static ID3D12CommandQueue*          g_pd3dCommandQueue = nullptr;
static ID3D12GraphicsCommandList*   g_pd3dCommandList = nullptr;
static ID3D12Fence*                 g_fence = nullptr;
static HANDLE                       g_fenceEvent = nullptr;
static UINT64                       g_fenceLastSignaledValue = 0;
static IDXGISwapChain3*             g_pSwapChain = nullptr;
static HANDLE                       g_hSwapChainWaitableObject = nullptr;
static ID3D12Resource*              g_mainRenderTargetResource[NUM_BACK_BUFFERS] = {};
static D3D12_CPU_DESCRIPTOR_HANDLE  g_mainRenderTargetDescriptor[NUM_BACK_BUFFERS] = {};

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
void WaitForLastSubmittedFrame();
FrameContext* WaitForNextFrameResources();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void fpsControlDebugWindow(FPSControl* fpsControl);
void cameraDebugWindow(Camera* camera,
                       CustomSDLMaterialObject* followedObject);
#endif
#endif

int main(int, char**) {
  printf("Inicializando...");
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    SDL_Log("error initializing SDL: %s\n", SDL_GetError());
  }

  std::shared_ptr<Screen> screen(new Screen(640, 480));

  // TODO start screen
  CustomPlayer* player =
      new CustomPlayer(new CustomSDLRect(new SDL_Rect({0, 0, 300, 300})),
                       new CustomSDLRect(new SDL_Rect({0, 0, 50, 50})), 10, 7);

  std::shared_ptr<Camera> camera(
      new Camera(screen->getWindow(), player, new int(1000)));

  namespace fs = std::filesystem;
  const auto p = fs::current_path();
  player->setTexture(IMG_LoadTexture(
      camera->getRenderer(),
      p.string().append("media/img/Naruto.jpg").c_str()));

  Stage* stage = new Stage(
      new CustomSDLRect(new SDL_Rect({-40000, -40000, 80000, 80000})));

  int close = 0;
#ifdef RUNNING_ON_WINDOWS
#ifdef DEBUG
  // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Dear ImGui DirectX12 Example", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX12_Init(g_pd3dDevice, NUM_FRAMES_IN_FLIGHT,
        DXGI_FORMAT_R8G8B8A8_UNORM, g_pd3dSrvDescHeap,
        g_pd3dSrvDescHeap->GetCPUDescriptorHandleForHeapStart(),
        g_pd3dSrvDescHeap->GetGPUDescriptorHandleForHeapStart());
#endif
#endif
printf("Game Loop...");
  EventControl* eventControl = new EventControl();
  eventControl->addEventListener(player);
  FPSControl* fpsControl = new FPSControl(60);
  while (!close) {
    // Events
    SDL_Event event;
    SDL_PumpEvents();
    while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_FIRSTEVENT,
                          SDL_DISPLAYEVENT) &&
           !close) {
      switch (event.type) {
        case SDL_QUIT:
          // handling of close button
          close = 1;
          break;
        case SDL_MOUSEWHEEL:
          if (event.wheel.y > 0) {
            // Put code for handling "scroll up" here!
          } else if (event.wheel.y < 0) {
            // Put code for handling "scroll down" here!
          }

          if (event.wheel.x > 0) {
            // scroll right
          } else if (event.wheel.x < 0) {
            // scroll left
          }
          break;
      }
    }

    const Uint8* keyboardState = SDL_GetKeyboardState(NULL);

    if (keyboardState[SDL_SCANCODE_W] || keyboardState[SDL_SCANCODE_UP]) {
      eventControl->addEvent(new CustomEvent(Action::PLAYER_MOVE_UP,
                                             fpsControl->getLastFrameTick()));
    } else if (keyboardState[SDL_SCANCODE_S] ||
               keyboardState[SDL_SCANCODE_DOWN]) {
      eventControl->addEvent(new CustomEvent(Action::PLAYER_MOVE_DOWN,
                                             fpsControl->getLastFrameTick()));
    }
    if (keyboardState[SDL_SCANCODE_A] || keyboardState[SDL_SCANCODE_LEFT]) {
      eventControl->addEvent(new CustomEvent(Action::PLAYER_MOVE_LEFT,
                                             fpsControl->getLastFrameTick()));
    } else if (keyboardState[SDL_SCANCODE_D] ||
               keyboardState[SDL_SCANCODE_RIGHT]) {
      eventControl->addEvent(new CustomEvent(Action::PLAYER_MOVE_RIGHT,
                                             fpsControl->getLastFrameTick()));
    }

    eventControl->processEvents();

    // Render
    camera->moveCamera();

    SDL_RenderClear(camera->getRenderer());
    try {
      printf("rendering...");
      SDL_RenderPresent(camera->film(stage));
    } catch (StageOutOfBounds err) {
      // Load new stage
      printf(err.what());
    }

#ifdef RUNNING_ON_WINDOWS
#ifdef DEBUG
    MSG msg;
    while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                close = 1;
        }
        if (close)
            break;

        // Start the Dear ImGui frame
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    fpsControlDebugWindow(fpsControl);
    cameraDebugWindow(camera.get(), player);

   ImGui::Render();

        FrameContext* frameCtx = WaitForNextFrameResources();
        UINT backBufferIdx = g_pSwapChain->GetCurrentBackBufferIndex();
        frameCtx->CommandAllocator->Reset();

        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource   = g_mainRenderTargetResource[backBufferIdx];
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_RENDER_TARGET;
        g_pd3dCommandList->Reset(frameCtx->CommandAllocator, nullptr);
        g_pd3dCommandList->ResourceBarrier(1, &barrier);

        // Render Dear ImGui graphics
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dCommandList->ClearRenderTargetView(g_mainRenderTargetDescriptor[backBufferIdx], clear_color_with_alpha, 0, nullptr);
        g_pd3dCommandList->OMSetRenderTargets(1, &g_mainRenderTargetDescriptor[backBufferIdx], FALSE, nullptr);
        g_pd3dCommandList->SetDescriptorHeaps(1, &g_pd3dSrvDescHeap);
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), g_pd3dCommandList);
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
        g_pd3dCommandList->ResourceBarrier(1, &barrier);
        g_pd3dCommandList->Close();

        g_pd3dCommandQueue->ExecuteCommandLists(1, (ID3D12CommandList* const*)&g_pd3dCommandList);

        g_pSwapChain->Present(1, 0); // Present with vsync
        //g_pSwapChain->Present(0, 0); // Present without vsync

        UINT64 fenceValue = g_fenceLastSignaledValue + 1;
        g_pd3dCommandQueue->Signal(g_fence, fenceValue);
        g_fenceLastSignaledValue = fenceValue;
        frameCtx->FenceValue = fenceValue;
#endif
#endif

    SDL_Delay(fpsControl->getNecessarydelay());
  }
#ifdef RUNNING_ON_WINDOWS
#ifdef DEBUG
  WaitForLastSubmittedFrame();

    // Cleanup
    ImGui_ImplDX12_Shutdown();
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
    DXGI_SWAP_CHAIN_DESC1 sd;
    {
        ZeroMemory(&sd, sizeof(sd));
        sd.BufferCount = NUM_BACK_BUFFERS;
        sd.Width = 0;
        sd.Height = 0;
        sd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        sd.Scaling = DXGI_SCALING_STRETCH;
        sd.Stereo = FALSE;
    }

    // [DEBUG] Enable debug interface
#ifdef DX12_ENABLE_DEBUG_LAYER
    ID3D12Debug* pdx12Debug = nullptr;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&pdx12Debug))))
        pdx12Debug->EnableDebugLayer();
#endif

    // Create device
    D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
    if (D3D12CreateDevice(nullptr, featureLevel, IID_PPV_ARGS(&g_pd3dDevice)) != S_OK)
        return false;

    // [DEBUG] Setup debug interface to break on any warnings/errors
#ifdef DX12_ENABLE_DEBUG_LAYER
    if (pdx12Debug != nullptr)
    {
        ID3D12InfoQueue* pInfoQueue = nullptr;
        g_pd3dDevice->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
        pInfoQueue->Release();
        pdx12Debug->Release();
    }
#endif

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        desc.NumDescriptors = NUM_BACK_BUFFERS;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        desc.NodeMask = 1;
        if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dRtvDescHeap)) != S_OK)
            return false;

        SIZE_T rtvDescriptorSize = g_pd3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = g_pd3dRtvDescHeap->GetCPUDescriptorHandleForHeapStart();
        for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
        {
            g_mainRenderTargetDescriptor[i] = rtvHandle;
            rtvHandle.ptr += rtvDescriptorSize;
        }
    }

    {
        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        desc.NumDescriptors = 1;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        if (g_pd3dDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&g_pd3dSrvDescHeap)) != S_OK)
            return false;
    }

    {
        D3D12_COMMAND_QUEUE_DESC desc = {};
        desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
        desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        desc.NodeMask = 1;
        if (g_pd3dDevice->CreateCommandQueue(&desc, IID_PPV_ARGS(&g_pd3dCommandQueue)) != S_OK)
            return false;
    }

    for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
        if (g_pd3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&g_frameContext[i].CommandAllocator)) != S_OK)
            return false;

    if (g_pd3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, g_frameContext[0].CommandAllocator, nullptr, IID_PPV_ARGS(&g_pd3dCommandList)) != S_OK ||
        g_pd3dCommandList->Close() != S_OK)
        return false;

    if (g_pd3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&g_fence)) != S_OK)
        return false;

    g_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (g_fenceEvent == nullptr)
        return false;

    {
        IDXGIFactory4* dxgiFactory = nullptr;
        IDXGISwapChain1* swapChain1 = nullptr;
        if (CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory)) != S_OK)
            return false;
        if (dxgiFactory->CreateSwapChainForHwnd(g_pd3dCommandQueue, hWnd, &sd, nullptr, nullptr, &swapChain1) != S_OK)
            return false;
        if (swapChain1->QueryInterface(IID_PPV_ARGS(&g_pSwapChain)) != S_OK)
            return false;
        swapChain1->Release();
        dxgiFactory->Release();
        g_pSwapChain->SetMaximumFrameLatency(NUM_BACK_BUFFERS);
        g_hSwapChainWaitableObject = g_pSwapChain->GetFrameLatencyWaitableObject();
    }

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D() {
   CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->SetFullscreenState(false, nullptr); g_pSwapChain->Release(); g_pSwapChain = nullptr; }
    if (g_hSwapChainWaitableObject != nullptr) { CloseHandle(g_hSwapChainWaitableObject); }
    for (UINT i = 0; i < NUM_FRAMES_IN_FLIGHT; i++)
        if (g_frameContext[i].CommandAllocator) { g_frameContext[i].CommandAllocator->Release(); g_frameContext[i].CommandAllocator = nullptr; }
    if (g_pd3dCommandQueue) { g_pd3dCommandQueue->Release(); g_pd3dCommandQueue = nullptr; }
    if (g_pd3dCommandList) { g_pd3dCommandList->Release(); g_pd3dCommandList = nullptr; }
    if (g_pd3dRtvDescHeap) { g_pd3dRtvDescHeap->Release(); g_pd3dRtvDescHeap = nullptr; }
    if (g_pd3dSrvDescHeap) { g_pd3dSrvDescHeap->Release(); g_pd3dSrvDescHeap = nullptr; }
    if (g_fence) { g_fence->Release(); g_fence = nullptr; }
    if (g_fenceEvent) { CloseHandle(g_fenceEvent); g_fenceEvent = nullptr; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = nullptr; }

#ifdef DX12_ENABLE_DEBUG_LAYER
    IDXGIDebug1* pDebug = nullptr;
    if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&pDebug))))
    {
        pDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
        pDebug->Release();
    }
#endif
}

void CreateRenderTarget() {
  for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
    {
        ID3D12Resource* pBackBuffer = nullptr;
        g_pSwapChain->GetBuffer(i, IID_PPV_ARGS(&pBackBuffer));
        g_pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, g_mainRenderTargetDescriptor[i]);
        g_mainRenderTargetResource[i] = pBackBuffer;
    }
}

void CleanupRenderTarget() {
  WaitForLastSubmittedFrame();

    for (UINT i = 0; i < NUM_BACK_BUFFERS; i++)
        if (g_mainRenderTargetResource[i]) { g_mainRenderTargetResource[i]->Release(); g_mainRenderTargetResource[i] = nullptr; }
}

void WaitForLastSubmittedFrame()
{
    FrameContext* frameCtx = &g_frameContext[g_frameIndex % NUM_FRAMES_IN_FLIGHT];

    UINT64 fenceValue = frameCtx->FenceValue;
    if (fenceValue == 0)
        return; // No fence was signaled

    frameCtx->FenceValue = 0;
    if (g_fence->GetCompletedValue() >= fenceValue)
        return;

    g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
    WaitForSingleObject(g_fenceEvent, INFINITE);
}

FrameContext* WaitForNextFrameResources()
{
    UINT nextFrameIndex = g_frameIndex + 1;
    g_frameIndex = nextFrameIndex;

    HANDLE waitableObjects[] = { g_hSwapChainWaitableObject, nullptr };
    DWORD numWaitableObjects = 1;

    FrameContext* frameCtx = &g_frameContext[nextFrameIndex % NUM_FRAMES_IN_FLIGHT];
    UINT64 fenceValue = frameCtx->FenceValue;
    if (fenceValue != 0) // means no fence was signaled
    {
        frameCtx->FenceValue = 0;
        g_fence->SetEventOnCompletion(fenceValue, g_fenceEvent);
        waitableObjects[1] = g_fenceEvent;
        numWaitableObjects = 2;
    }

    WaitForMultipleObjects(numWaitableObjects, waitableObjects, TRUE, INFINITE);

    return frameCtx;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd,
                                                             UINT msg,
                                                             WPARAM wParam,
                                                             LPARAM lParam);

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != nullptr && wParam != SIZE_MINIMIZED)
        {
            WaitForLastSubmittedFrame();
            CleanupRenderTarget();
            HRESULT result = g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT);
            assert(SUCCEEDED(result) && "Failed to resize swapchain.");
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
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

  float samples[20];
  std::string fps[20];
  for (int n = 0; n < 20; n++) {
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
      ("X: " + std::to_string(followedObject->getGlobalDestination()->x))
          .c_str());
  ImGui::TableNextColumn();
  ImGui::Text(("X relative: " +
               std::to_string(followedObject->getGlobalDestination()->x))
                  .c_str());
  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::Text(
      ("Y: " + std::to_string(followedObject->getGlobalDestination()->y))
          .c_str());
  ImGui::TableNextColumn();
  ImGui::Text(("Y relative: " +
               std::to_string(followedObject->getGlobalDestination()->y))
                  .c_str());
  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::Text(
      ("W: " + std::to_string(followedObject->getGlobalDestination()->w))
          .c_str());
  ImGui::TableNextColumn();
  ImGui::Text(("src X - W " + std::to_string(followedObject->getSrcRect()->x) +
               " - " + std::to_string(followedObject->getSrcRect()->w))
                  .c_str());
  ImGui::TableNextRow();
  ImGui::TableNextColumn();
  ImGui::Text(
      ("H: " + std::to_string(followedObject->getGlobalDestination()->h))
          .c_str());
  ImGui::TableNextColumn();
  ImGui::Text(("src Y - H " + std::to_string(followedObject->getSrcRect()->y) +
               " - " + std::to_string(followedObject->getSrcRect()->h))
                  .c_str());
  ImGui::EndTable();
  ImGui::Separator();
  ImGui::Text(("Is intersecting: " + std::to_string(SDL_HasIntersection(
                                         followedObject->getGlobalDestination(),
                                         camera->getCameraRect())))
                  .c_str());

  ImGui::End();
}

#endif
#endif