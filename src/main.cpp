#include <SDL.h>
#include <SDL_image.h>

#include <CustomPhysics.hpp>
#include <CustomGameCharacters.hpp>
#include <CustomScreen.hpp>
#include <Stage.hpp>
#include <World.hpp>
#include <deque>
#include <vector>
#include <queue>

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
  Stage::StageId currentStageId;

  PhysicsControl* physicsControl;

  CustomPlayer* mainPlayer;

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

    this->mainPlayer = new CustomPlayer(
        new CustomSDLRect(new SDL_Rect({0, 0, 300, 300})),
        new CustomSDLRect(new SDL_Rect({0, 0, 50, 50})), 0, 0, 7, 10);
    this->mainPlayer->setTexture(IMG_LoadTextureTyped_RW(
        camera->getRenderer(),
        SDL_RWFromFile(
            "C:\\Users\\lucas\\git\\SDL_playground\\media\\img\\Naruto.jpg",
            "r"),
        1, "jpeg"));

    Stage* stage = this->world->createBlankStage(camera->getRenderer());
    this->currentStageId = stage->getId();
    stage->placeObject(this->mainPlayer, {GAME_ENTITY_TYPE::PLAYABLE_OBJECT,
                                          GAME_ENTITY_TYPE::PHYSICAL_OBJECT,
                                          GAME_ENTITY_TYPE::MATERIAL_OBJECT});

    this->camera->setFollowedObject(this->mainPlayer);
    this->physicsControl = new PhysicsControl();
  }

  CustomPlayer* getMainPlayer() { return this->mainPlayer; }

  void processLogic() noexcept {
    Stage* stage = this->world->getStage(currentStageId);

    std::vector<CustomPhysicalObject*> physicalObjects =
        stage->getObjectsByType<CustomPhysicalObject>(
            GAME_ENTITY_TYPE::PHYSICAL_OBJECT);

    this->physicsControl->doPhysics(physicalObjects);

    std::vector<std::pair<CustomPhysicalObject*, CustomPhysicalObject*>>
        collisions = this->physicsControl->getCollisions(physicalObjects);

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
      SDL_RenderClear(this->camera->getRenderer());
      SDL_RenderPresent(
          this->camera->film(this->world->getStage(currentStageId)));
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

  try {
    GameControl* gameControl = new GameControl();

    FPSControl* fpsControl = new FPSControl();

    printf("Start game loop...");
    EventControl* eventControl = new EventControl();
    eventControl->addEventListener(gameControl->getMainPlayer());

    while (gameControl->isGameLoopRunning()) {
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
        eventControl->addEvent(new CustomEvent(
            PLAYER_ACTION::PLAYER_LEFT_KEY_PRESSED,
            fpsControl->getLastFrameTick(), fpsControl->getFrameTick()));
      }
      if (keyboardState[SDL_SCANCODE_D] || keyboardState[SDL_SCANCODE_RIGHT]) {
        eventControl->addEvent(new CustomEvent(
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
    SDL_Quit();

    return 0;
  } catch (const std::runtime_error& e) {
    SDL_Quit();
    return 1;
  }
}
