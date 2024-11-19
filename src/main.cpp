#include <SDL.h>
#include <SDL_image.h>

#include <CustomGameCharacters.hpp>
#include <CustomGameComponents.hpp>
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
#include <DebugInterface.hpp>
#endif
#include <rpcndr.h>

#include <CustomTexture.hpp>
#include <cstdio>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>

#include "CustomGameUtils.hpp"
#include "CustomSDLObject.hpp"
#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_keyboard.h"
#include "SDL_log.h"
#include "SDL_main.h"
#include "SDL_pixels.h"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_scancode.h"
#include "SDL_stdinc.h"
#include "SDL_surface.h"
#include "SDL_timer.h"
#include "begin_code.h"

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
  CustomGroundPlane* ground;

  std::vector<EventListener*> eventListeners;

  std::unordered_set<CustomPhysicalObject*,
                     PhysicsControl::CustomPhysicalObjectHash,
                     PhysicsControl::CustomPhysicalObjectEqual>
      physicalObjects;

  CustomPlayer* createLocalPlayer() {
    SDL_Surface* surface_idle = IMG_Load(
        "C:\\Users\\lucas\\git\\SDL_"
        "playground\\media\\img\\Woodcutter\\Woodcutter_idle.png");
    // preto de fundo da imagem
    Uint8 r = 255;
    Uint8 g = 255;
    Uint8 b = 255;
    SDL_SetColorKey(surface_idle, SDL_TRUE,
                    SDL_MapRGB(surface_idle->format, r, g, b));
    std::unordered_map<ANIMATION_TYPE, SDL_Texture*> textures;
    textures[ANIMATION_TYPE::IDLE] =
        SDL_CreateTextureFromSurface(camera->getRenderer(), surface_idle);

    SDL_FreeSurface(surface_idle);

    std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>> animationSprites;
    animationSprites[ANIMATION_TYPE::IDLE] = std::vector<SDL_Rect>{
        SDL_Rect{20, 14, 40, 50}, SDL_Rect{65, 14, 40, 50},
        SDL_Rect{115, 14, 40, 50}, SDL_Rect{162, 14, 40, 50}};

    /*
    animationSprites[ANIMATION_TYPE::WALKING] = std::vector<SDL_Rect>{
        SDL_Rect{16, 80, 30, 50},  SDL_Rect{48, 80, 30, 50},
        SDL_Rect{76, 80, 30, 50},  SDL_Rect{107, 80, 30, 50},
        SDL_Rect{145, 80, 30, 50}, SDL_Rect{178, 80, 30, 50}};
        */
    CustomTextureManager* texturesManager = new CustomTextureManager(textures);
    return new CustomPlayer(texturesManager, animationSprites,
                            CustomSDLRect({0, 0, 40, 50}), 10, 4);
  }

  CustomGroundPlane* createDefaultGround() {
    std::unordered_map<ANIMATION_TYPE, SDL_Texture*> textures;
    textures[ANIMATION_TYPE::NO_ANIMATION] = SDL_CreateTextureFromSurface(
        camera->getRenderer(), SDL_CreateRGBSurface(0, 1, 1, 32, 0, 0, 0, 0));

    CustomTextureManager* textureManager = new CustomTextureManager(textures);
    return new CustomGroundPlane(textureManager, SDL_Rect({20, 200, 200, 200}));
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
    this->ground = this->createDefaultGround();

    this->physicalObjects.insert(this->mainPlayer);
    this->physicalObjects.insert(this->ground);

    this->eventListeners.push_back(this->mainPlayer);

    this->currentStage = this->world->createBlankStage(camera->getRenderer());
    this->currentStage->placeMaterialObject(this->mainPlayer);
    this->currentStage->placeMaterialObject(this->ground);

    this->camera->setFollowedObject(this->mainPlayer);
    this->physicsControl = new PhysicsControl();
  }

  CustomPlayer* getMainPlayer() { return this->mainPlayer; }
  CustomGroundPlane* getGround() { return this->ground; }

  std::vector<EventListener*> getEventListeners() {
    return this->eventListeners;
  }

  void processLogic(Uint64 startTick, Uint64 endTick) noexcept {
    this->physicsControl->doPhysics(this->physicalObjects, startTick, endTick);
  }

  bool isGameLoopRunning() { return this->gameLoopRunning; }

  void setGameLoopRunning(bool gameLoopRunning) {
    this->gameLoopRunning = gameLoopRunning;
  }

  void executeRenderPhase(Uint64 startTick, Uint64 endTick) {
    this->camera->moveCamera(startTick, endTick);

    try {
      this->camera->film(this->currentStage, startTick, endTick);
    } catch (StageOutOfBounds err) {
      // Load new stage
      printf(err.what());
    }
  }
};

class EventControl {
 private:
  std::queue<CustomEvent> eventsQueue;
  std::vector<EventListener*> eventListeners;

 public:
  virtual ~EventControl() {}
  void addEventListener(EventListener* listener) {
    eventListeners.push_back(listener);
  }
  virtual void addEvent(CustomEvent event) { eventsQueue.push(event); }
  void processEvents() {
    while (!eventsQueue.empty()) {
      CustomEvent event = this->eventsQueue.front();
      for (EventListener* listener : eventListeners) {
        listener->handleEvent(event);  // TODO handle exceptions
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
  // TODO verificar pq o player não está sendo mostrado... Acho que é calculo
  // errado do posicionamento
  try {
    GameControl* gameControl = new GameControl();

    FPSControl* fpsControl = new FPSControl();

    printf("Start game loop...");
    EventControl* eventControl = new EventControl();

    for (EventListener* listener : gameControl->getEventListeners()) {
      eventControl->addEventListener(listener);
    }
#ifdef DEBUG
    debug->trackPlayer(gameControl->getMainPlayer());
    debug->trackGround(gameControl->getGround());
#endif
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
        CustomEvent(PLAYER_ACTION::PLAYER_UP_KEY_PRESSED)); } else {
          eventControl->addEvent(new
        CustomEvent(PLAYER_ACTION::PLAYER_UP_KEY_RELEASED));
        }
        if (keyboardState[SDL_SCANCODE_S] ||
                   keyboardState[SDL_SCANCODE_DOWN]) {
          eventControl->addEvent(new
        CustomEvent(PLAYER_ACTION::PLAYER_DOWN_KEY_PRESSED)); } else {
          eventControl->addEvent(new
        CustomEvent(PLAYER_ACTION::PLAYER_DOWN_KEY_RELEASED));
        }
    */
      boolean idle_input = true;
      if (keyboardState[SDL_SCANCODE_A] || keyboardState[SDL_SCANCODE_LEFT]) {
        eventControl->addEvent(CustomEvent(
            PLAYER_ACTION::PLAYER_LEFT_KEY_PRESSED,
            fpsControl->getLastFrameTick(), fpsControl->getFrameTick()));
        idle_input = false;
      }
      if (keyboardState[SDL_SCANCODE_D] || keyboardState[SDL_SCANCODE_RIGHT]) {
        eventControl->addEvent(CustomEvent(
            PLAYER_ACTION::PLAYER_RIGHT_KEY_PRESSED,
            fpsControl->getLastFrameTick(), fpsControl->getFrameTick()));
        idle_input = false;
      }

      if (idle_input) {
        eventControl->addEvent(CustomEvent(PLAYER_ACTION::PLAYER_IDLE_INPUT,
                                           fpsControl->getLastFrameTick(),
                                           fpsControl->getFrameTick()));
      }

      eventControl->processEvents();

      gameControl->processLogic(fpsControl->getLastFrameTick(),
                                fpsControl->getFrameTick());

      // Render phase
      gameControl->executeRenderPhase(fpsControl->getLastFrameTick(),
                                      fpsControl->getFrameTick());
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