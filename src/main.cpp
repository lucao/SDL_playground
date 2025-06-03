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

#include <CustomPlayerBuilder.hpp>
#include <CustomTexture.hpp>
#include <stdexcept>
#include <unordered_map>

#include "CustomGameUtils.hpp"
#include "SDL_error.h"
#include "SDL_events.h"
#include "SDL_keyboard.h"
#include "SDL_log.h"
#include "SDL_main.h"
#include "SDL_pixels.h"
#include "SDL_render.h"
#include "SDL_scancode.h"
#include "SDL_stdinc.h"
#include "SDL_surface.h"
#include "SDL_timer.h"

class GameControl {
 private:
  bool gameLoopRunning;

  CameraSDL* camera;
  Screen* screen;

  World* world;
  Stage* currentStage;

  std::vector<EventListener*> eventListeners;

  Box2DPhysicsControl* physicsControl;

  CustomPlayer* mainPlayer;
  CustomGroundPlane* ground;

  CustomGroundPlane* createDefaultGround() {
    Uint32 pixel = 0xFF000000;  // Opaque black in ARGB8888
    SDL_Surface* surface = SDL_CreateSurfaceFrom(1, 1, SDL_PIXELFORMAT_ARGB8888,
                                                 &pixel, sizeof(Uint32));
    std::unordered_map<ANIMATION_TYPE, SDL_Texture*> textures;
    textures[ANIMATION_TYPE::NO_ANIMATION] =
        SDL_CreateTextureFromSurface(camera->getRenderer(), surface);

    CustomTextureManager* textureManager = new CustomTextureManager(textures);
    return new CustomGroundPlane(textureManager,
                                 CustomSDLRect({0, 300, 500, 500}),
                                 physicsControl->getWorldId());
  }

 public:
  GameControl() {
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
      SDL_Log("Error initializing SDL: %s\n", SDL_GetError());

      throw std::runtime_error("Error initializing SDL");
    }

    this->screen = new Screen(640, 480);
    this->camera = new CameraSDL(screen->getWindow(), 1000);

    this->gameLoopRunning = true;
    this->physicsControl =
        new Box2DPhysicsControl(60.0f);  // TODO add frameRate to settings

    this->world = new World(this->camera->getRenderer());

    // TODO create mainPlayer
    this->mainPlayer =
        std::unique_ptr<CustomPlayerBuilder>(
            new CustomPlayerBuilder(this->camera->getRenderer(),
                                    PLAYER_CLASS::ROGUE, std::string("test"),
                                    physicsControl->getWorldId()))
            ->getPlayerCharacter();

    this->currentStage = this->world->createBlankStage(camera->getRenderer());

    this->currentStage->placeMaterialObject(*this->mainPlayer,
                                            this->mainPlayer);
    this->currentStage->placePhysicalObject(*this->mainPlayer,
                                            this->mainPlayer);

    this->eventListeners.push_back(this->mainPlayer);

    // verificar se surface está sendo plotada com a cor correta
    // TODO
    Uint32 pixel = 0xFF000000;  // Opaque black in ARGB8888
    SDL_Surface* surface = SDL_CreateSurfaceFrom(1, 1, SDL_PIXELFORMAT_ARGB8888,
                                                 &pixel, sizeof(Uint32));
    SDL_Texture* texture =
        SDL_CreateTextureFromSurface(camera->getRenderer(), surface);

    this->ground = this->createDefaultGround();
    this->currentStage->placeMaterialObject(*this->ground, this->ground);
    this->currentStage->placePhysicalObject(*this->ground, this->ground);

    this->camera->setFollowedObject(this->mainPlayer);
  }

  CustomPlayer* getMainPlayer() { return this->mainPlayer; }
#ifdef DEBUG
  CustomGroundPlane* getGroundForDebug() { return this->ground; }
#endif  // DEBUG
  Stage* getCurrentStage() { return this->currentStage; }

  std::vector<EventListener*> getEventListeners() {
    return this->eventListeners;
  }

  void processLogic(Uint64 startTick, Uint64 endTick) noexcept {
    this->physicsControl->doPhysics(this->currentStage->getPhysicalObjects(),
                                    startTick, endTick);
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
        listener->handleEvent(event);  // TODO handle exceptions
      }
      this->eventsQueue.pop();
      delete event;
    }
  }
};

int main(int, char**) {
  printf("Inicializando...");

  // TODO verificar pq o player não está sendo mostrado... Acho que é calculo
  // errado do posicionamento
  try {
    GameControl* gameControl = new GameControl();

    FPSControl* fpsControl = new FPSControl();

    printf("Start game loop...");
    EventControl* eventControl = new EventControl();

#ifdef DEBUG
    DebugWindows* debug = new DebugWindows(fpsControl);
#endif

    for (EventListener* listener : gameControl->getEventListeners()) {
      eventControl->addEventListener(listener);
    }
#ifdef DEBUG
    debug->trackGround(gameControl->getGroundForDebug());
    debug->trackPlayer(gameControl->getMainPlayer());
    debug->trackStage(gameControl->getCurrentStage());
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
          case SDL_EVENT_QUIT:
            // handling of close button
            gameControl->setGameLoopRunning(false);
            break;
          case SDL_EVENT_MOUSE_WHEEL:
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
      int* keyboardState;
      SDL_GetKeyboardState(keyboardState);
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
        eventControl->addEvent(new CustomEvent(
            PLAYER_ACTION::PLAYER_LEFT_KEY_PRESSED,
            fpsControl->getLastFrameTick(), fpsControl->getFrameTick()));
        idle_input = false;
      }
      if (keyboardState[SDL_SCANCODE_D] || keyboardState[SDL_SCANCODE_RIGHT]) {
        eventControl->addEvent(new CustomEvent(
            PLAYER_ACTION::PLAYER_RIGHT_KEY_PRESSED,
            fpsControl->getLastFrameTick(), fpsControl->getFrameTick()));
        idle_input = false;
      }

      if (idle_input) {
        eventControl->addEvent(new CustomEvent(PLAYER_ACTION::PLAYER_IDLE_INPUT,
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