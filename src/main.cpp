#include <SDL.h>
#include <SDL_image.h>

#include <CustomGameCharacters.hpp>
#include <CustomPhysics.hpp>
#include <CustomScreen.hpp>
#include <Stage.hpp>
#include <World.hpp>
#include <deque>
#include <queue>
#include <vector>

#define DEBUG

// debug
#ifdef DEBUG
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

class DebugWindows {
 private:
  bool show_demo_window;
  bool show_another_window;

  ImVec4 clear_color;

  HWND hwnd;
  WNDCLASSEXW wc;

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
                      L"ImGui Example",
                      nullptr};
    ::RegisterClassExW(&wc);
    HWND hwnd = ::CreateWindowW(
        wc.lpszClassName, L"Dear ImGui DirectX9 Example", WS_OVERLAPPEDWINDOW,
        100, 100, 1280, 800, nullptr, nullptr, wc.hInstance, nullptr);

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

    ImGui::Begin("Hello, world!");  // Create a window called "Hello, world!"
                                    // and append into it.

    ImGui::Text("This is some useful text.");  // Display some text (you can
                                               // use a format strings too)

    counter++;
    ImGui::SameLine();
    ImGui::Text("counter = %d", counter);

    ImGui::End();

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
#endif

class FPSControl {
 private:
  Uint64 frameTick;
  Uint64 lastFrameTick;

  Uint64 lastSecondTick;

  Uint16 frameCounter;
  std::deque<Uint64> ticks;
  std::deque<Uint16> samples;

 public:
  static const Uint32 secondinMS = 1000;
  FPSControl() {
    this->frameTick = SDL_GetTicks64();
    this->lastFrameTick = SDL_GetTicks64();
    this->lastSecondTick = 0;
    this->ticks = std::deque<Uint64>(20, -1);
    this->samples = std::deque<Uint16>(20, -1);
  }

  void tick() {
    this->lastFrameTick = this->frameTick;
    this->frameTick = SDL_GetTicks64();

    if (this->ticks.size() > 19) this->ticks.pop_back();
    this->ticks.push_front(this->frameTick);

    frameCounter++;
    if (SDL_GetTicks64() - this->lastSecondTick >= secondinMS) {
      this->lastSecondTick = SDL_GetTicks64();

      if (this->samples.size() > 19) this->samples.pop_back();
      this->samples.push_front(frameCounter);

      frameCounter = 0;
    }
  }

  Uint64 getLastFrameTick() { return this->lastFrameTick; }
  Uint64 getFrameTick() { return this->frameTick; }
  std::deque<Uint16> getFpsSamples() { return this->samples; }
};

class GameControl {
 private:
  bool gameLoopRunning;

  CameraSDL* camera;
  Screen* screen;

  World* world;
  Stage* currentStage;

  PhysicsControl* physicsControl;

  CustomPlayer* mainPlayer;

  std::vector<EventListener> eventListeners;

  std::vector<CustomPhysicalObject> physicalObjects;

  CustomPlayer* createLocalPlayer() {
    CustomPlayer* localPlayer = new CustomPlayer(
        new CustomSDLRect(new SDL_Rect({0, 0, 300, 300})),
        new CustomSDLRect(new SDL_Rect({0, 0, 50, 50})), 0, 0, 7, 10);
    localPlayer->setTexture(IMG_LoadTextureTyped_RW(
        camera->getRenderer(),
        SDL_RWFromFile(
            "C:\\Users\\lucas\\git\\SDL_playground\\media\\img\\Naruto.jpg",
            "r"),
        1, "jpeg"));
    return localPlayer;
  }

 public:
  GameControl() {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
      SDL_Log("Error initializing SDL: %s\n", SDL_GetError());

      throw std::runtime_error("Error initializing SDL");
    } else if (!IMG_Init(IMG_INIT_JPG)) {
      SDL_Log("SDL_image could not initialize! SDL_image Error: %s\n",
              IMG_GetError());

      throw std::runtime_error("Error initializing SDL_image");
    }

    this->screen = new Screen(640, 480);
    this->camera = new CameraSDL(screen->getWindow(), 1000);

    this->gameLoopRunning = true;

    this->world = new World();

    this->mainPlayer = this->createLocalPlayer();

    this->physicalObjects.push_back(*this->mainPlayer);

    this->eventListeners.push_back(*this->mainPlayer);

    this->currentStage = this->world->createBlankStage(camera->getRenderer());
    this->currentStage->placeMaterialObject(this->mainPlayer);

    this->camera->setFollowedObject(this->mainPlayer);
    this->physicsControl = new PhysicsControl();
  }

  std::vector<EventListener> getEventListeners() {
    return this->eventListeners;
  }

  void processLogic() noexcept {
    this->physicsControl->doPhysics(this->physicalObjects);

    std::vector<std::pair<CustomPhysicalObject, CustomPhysicalObject>>
        collisions = this->physicsControl->getCollisions(this->physicalObjects);

    for (std::pair<CustomPhysicalObject, CustomPhysicalObject> pair :
         collisions) {
    }
  }

  bool isGameLoopRunning() { return this->gameLoopRunning; }

  void setGameLoopRunning(bool gameLoopRunning) {
    this->gameLoopRunning = gameLoopRunning;
  }

  void executeRenderPhase() {
    this->camera->moveCamera();

    try {
      SDL_RenderClear(this->camera->getRenderer());
      SDL_RenderPresent(this->camera->film(this->currentStage));
    } catch (StageOutOfBounds err) {
      // Load new stage
      printf(err.what());
    }
  }
};

class EventControl {
 private:
  std::queue<CustomEvent> eventsQueue;
  std::vector<EventListener> eventListeners;

 public:
  virtual ~EventControl() {}
  void addEventListener(EventListener listener) {
    eventListeners.push_back(listener);
  }
  virtual void addEvent(CustomEvent event) { eventsQueue.push(event); }
  void processEvents() {
    while (!eventsQueue.empty()) {
      CustomEvent event = this->eventsQueue.front();
      for (EventListener listener : eventListeners) {
        listener.handleEvent(event);  // TODO handle exceptions
      }
      this->eventsQueue.pop();
    }
  }
};

int main(int, char**) {
  printf("Inicializando...");

#ifdef DEBUG
  DebugWindows* debug = new DebugWindows();
#endif
//TODO verificar pq o player não está sendo mostrado... Acho que é calculo errado do posicionamento
  try {
    GameControl* gameControl = new GameControl();

    FPSControl* fpsControl = new FPSControl();

    printf("Start game loop...");
    EventControl* eventControl = new EventControl();

    for (EventListener listener : gameControl->getEventListeners()) {
      eventControl->addEventListener(listener);
    }

    while (gameControl->isGameLoopRunning()) {
#ifdef DEBUG
      if (!debug->loop()) {
        gameControl->setGameLoopRunning(false);
      }
#endif
      fpsControl->tick();
      // Events
      SDL_Event event;
      /*
      SDL_PumpEvents();
      while (SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_FIRSTEVENT,
                            SDL_DISPLAYEVENT)) {
                              */

      while (SDL_PollEvent(&event) != 0) {
        switch (event.type) {
          case SDL_QUIT:
            // handling of close button
            gameControl->setGameLoopRunning(false);
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
      /*
        if (keyboardState[SDL_SCANCODE_W] || keyboardState[SDL_SCANCODE_UP]) {
          eventControl->addEvent(new
        CustomEvent(Action::PLAYER_UP_KEY_PRESSED)); } else {
          eventControl->addEvent(new
        CustomEvent(Action::PLAYER_UP_KEY_RELEASED));
        }
        if (keyboardState[SDL_SCANCODE_S] ||
                   keyboardState[SDL_SCANCODE_DOWN]) {
          eventControl->addEvent(new
        CustomEvent(Action::PLAYER_DOWN_KEY_PRESSED)); } else {
          eventControl->addEvent(new
        CustomEvent(Action::PLAYER_DOWN_KEY_RELEASED));
        }
    */

      if (keyboardState[SDL_SCANCODE_A] || keyboardState[SDL_SCANCODE_LEFT]) {
        eventControl->addEvent(CustomEvent(
            PLAYER_ACTION::PLAYER_LEFT_KEY_PRESSED,
            fpsControl->getLastFrameTick(), fpsControl->getFrameTick()));
      }
      if (keyboardState[SDL_SCANCODE_D] || keyboardState[SDL_SCANCODE_RIGHT]) {
        eventControl->addEvent(CustomEvent(
            PLAYER_ACTION::PLAYER_RIGHT_KEY_PRESSED,
            fpsControl->getLastFrameTick(), fpsControl->getFrameTick()));
      }

      eventControl->processEvents();

      gameControl->processLogic();

      // Render phase
      gameControl->executeRenderPhase();
    }
    /*
    delete player;
    delete stage;
    delete camera;
    delete screen;
*/
    // close SDL
#ifdef DEBUG
    delete debug;
#endif
    SDL_Quit();

    return 0;
  } catch (const std::runtime_error& e) {
    SDL_Quit();
    return 1;
  }
}

#ifdef DEBUG
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