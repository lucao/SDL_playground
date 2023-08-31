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

int main(int, char**) {
  printf("Inicializando...");
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    SDL_Log("error initializing SDL: %s\n", SDL_GetError());
    return 1;
  } else if (!IMG_Init(IMG_INIT_JPG)) {
    SDL_Log("SDL_image could not initialize! SDL_image Error: %s\n",
            IMG_GetError());
    return 1;
  }

  Screen* screen(new Screen(640, 480));

  // TODO start screen
  CustomPlayer* player =
      new CustomPlayer(new CustomSDLRect(new SDL_Rect({0, 0, 300, 300})),
                       new CustomSDLRect(new SDL_Rect({0, 0, 50, 50})), 10, 7);

  CameraSDL* camera(new CameraSDL(screen->getWindow(), player, new int(1000)));

  player->setTexture(IMG_LoadTextureTyped_RW(
      camera->getRenderer(),
      SDL_RWFromFile(
          "C:\\Users\\lucas\\git\\SDL_playground\\media\\img\\Naruto.jpg", "r"),
      1, "jpeg"));

  Stage* stage =
      new Stage(new CustomSDLRect(new SDL_Rect({-40000, -40000, 80000, 80000})),
                camera->getRenderer());

  int close = 0;

  printf("Start game loop...");
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

    try {
      SDL_RenderClear(camera->getRenderer());
      SDL_RenderPresent(camera->film(stage));
    } catch (StageOutOfBounds err) {
      // Load new stage
      printf(err.what());
    }
  }
  
  delete player;
  delete stage;
  delete camera;
  delete screen;

  // close SDL
  SDL_Quit();

  return 0;
}
