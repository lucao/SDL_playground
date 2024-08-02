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
#include <DebugInterface.hpp>
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

  std::vector<EventListener*> eventListeners;

  std::vector<CustomPhysicalObject*> physicalObjects;

  CustomPlayer* createLocalPlayer() {
    std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>> animationSprites;
    animationSprites[ANIMATION_TYPE::IDLE] = std::vector<SDL_Rect>{
        SDL_Rect{27, 8, 40, 50},  SDL_Rect{76, 8, 40, 50},
        SDL_Rect{128, 8, 40, 50}, SDL_Rect{176, 8, 40, 50},
        SDL_Rect{225, 8, 40, 50}, SDL_Rect{272, 8, 40, 50}};

    return new CustomPlayer(
        IMG_LoadTextureTyped_RW(
            camera->getRenderer(),
            SDL_RWFromFile(
                "C:\\Users\\lucas\\git\\SDL_playground\\media\\img\\Naruto.jpg",
                "r"),
            1, "jpeg"),
        animationSprites, CustomSDLRect({0, 0, 40, 50}), 10, 5);
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

    this->physicalObjects.push_back(this->mainPlayer);

    this->eventListeners.push_back(this->mainPlayer);

    this->currentStage = this->world->createBlankStage(camera->getRenderer());
    this->currentStage->placeMaterialObject(this->mainPlayer);

    this->camera->setFollowedObject(this->mainPlayer);
    this->physicsControl = new PhysicsControl();
  }

  CustomPlayer* getMainPlayer() { return this->mainPlayer; }

  std::vector<EventListener*> getEventListeners() {
    return this->eventListeners;
  }

  void processLogic(Uint64 startTick, Uint64 endTick) noexcept {
    this->physicsControl->doPhysics(this->physicalObjects, startTick, endTick);

    std::vector<std::pair<CustomPhysicalObject*, CustomPhysicalObject*>>
        collisions = this->physicsControl->getCollisions(this->physicalObjects);

    for (std::pair<CustomPhysicalObject*, CustomPhysicalObject*> pair :
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
      this->camera->film(this->currentStage);
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

      gameControl->processLogic(fpsControl->getLastFrameTick(),
                                fpsControl->getFrameTick());

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