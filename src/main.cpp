#include <SDL.h>
#include <SDL_image.h>
#include <windows.h>

#include <CustomGameCharacters.hpp>
#include <CustomScreen.hpp>
#include <Stage.hpp>
#include <World.hpp>
#include <cstdio>
#include <memory>
#include <platform.hpp>
#include <tuple>

int custom_main(int, char**);

#ifdef RUNNING_ON_WINDOWS
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  return custom_main(0, 0);
}
#else

int main(int, char**) { return custom_main(0, 0); }
#endif

int custom_main(int, char**) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printf("error initializing SDL: %s\n", SDL_GetError());
  }
  std::shared_ptr<Screen> screen(new Screen(640, 480));
  std::shared_ptr<Camera> camera(new Camera(screen->getWindow(), new int(100)));
  // TODO start screen

  Region* region =
      new DynamicRegion({}, new CustomSDLRect(new SDL_Rect({0, 0, 500, 500})),
                        camera->getRenderer());
  CustomPlayer* player = new CustomPlayer(
      IMG_LoadTexture(
          camera->getRenderer(),
          "C:/Users/lucas/git/C/game/media/img/eu-foto-crianca.JPG"),
      new CustomSDLRect(new SDL_Rect({0, 0, 50, 50})),
      new CustomSDLRect(new SDL_Rect({0, 0, 50, 50})), 10, 10);
  camera->setFollowedObject(player);
  region->addObjectToRegion(player);

  std::unordered_set<Region*> regionsOnStage;
  regionsOnStage.insert(region);
  Stage* stage =
      new Stage(regionsOnStage,
                new CustomSDLRect(new SDL_Rect({-2000, -2000, 4000, 4000})));

  World* world = new World();
  world->loadStage(stage);

  int close = 0;
  while (!close) {
    SDL_Event event;

    // Events management
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          // handling of close button
          close = 1;
          break;
      }
    }
    player->handleEvent(event);

    camera->renderStage(stage);

    SDL_Delay(1000 / 60);  // controlar melhor o fps
  }

  // destruir tudo

  // close SDL
  SDL_Quit();

  return 0;
}

/*
int main(int, char**) {
  // returns zero on success else non-zero
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    printf("error initializing SDL: %s\n", SDL_GetError());
  }
  SDL_Window* win = SDL_CreateWindow("GAME",  // creates a window
                                     SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED, 640, 480, 0);

  // triggers the program that controls
  // your graphics hardware and sets flags
  Uint32 render_flags = SDL_RENDERER_ACCELERATED;

  // creates a renderer to render our images
  SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);

  // creates a surface to load an image into the main memory
  SDL_Surface* surface;
  // please provide a path for your image
  surface =
IMG_Load("C:/Users/lucas/git/C/game/media/img/eu-foto-crianca.JPG");

  // loads image to our graphics hardware memory.
  SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surface);

  // clears main-memory
  SDL_FreeSurface(surface);

  // let us control our image position
  // so that we can move it with our keyboard.
  SDL_Rect dest;

  // connects our texture with dest to control position
  SDL_QueryTexture(tex, NULL, NULL, &dest.w, &dest.h);

  // adjust height and width of our image box.
  dest.w = 100;
  dest.h = 100;
  // sets initial x-position of object
  dest.x = 0;
  // sets initial y-position of object
  dest.y = 0;

  // controls animation loop
  int close = 0;

  // speed of box
  int speed = 100;

  // animation loop
  while (!close) {
    SDL_Event event;

    // Events management
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          // handling of close button
          close = 1;
          break;

        case SDL_KEYDOWN:
          // keyboard API for key pressed
          switch (event.key.keysym.scancode) {
            case SDL_SCANCODE_W:
            case SDL_SCANCODE_UP:
              dest.y -= speed / 30;
              break;
            case SDL_SCANCODE_A:
            case SDL_SCANCODE_LEFT:
              dest.x -= speed / 30;
              break;
            case SDL_SCANCODE_S:
            case SDL_SCANCODE_DOWN:
              dest.y += speed / 30;
              break;
            case SDL_SCANCODE_D:
            case SDL_SCANCODE_RIGHT:
              dest.x += speed / 30;
              break;
            default:
              break;
          }
      }
    }
    // clears the screen
    SDL_RenderClear(rend);
    SDL_RenderCopy(rend, tex, NULL, &dest);

    // triggers the double buffers
    // for multiple rendering
    SDL_RenderPresent(rend);

    // calculates to 60 fps
    SDL_Delay(1000 / 60);
  }

  // destroy texture
  SDL_DestroyTexture(tex);

  // destroy renderer
  SDL_DestroyRenderer(rend);

  // destroy window
  SDL_DestroyWindow(win);

  // close SDL
  SDL_Quit();

  return 0;
}
*/