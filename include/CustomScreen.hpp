#ifndef CUSTOM_SCREEN_H
#define CUSTOM_SCREEN_H

#include <SDL.h>

#include <CustomSDLObject.hpp>
#include <Stage.hpp>

class CameraSDL {
 private:
  CustomSDLRect* cameraRect;
  int cameraRectResize_w;
  int cameraRectResize_h;
  CustomSDLMaterialObject* followedObject;

  SDL_Renderer* renderer;
  int speed;

 public:
  CameraSDL(SDL_Window* window,
         int speed);
  ~CameraSDL();
  void setFollowedObject(CustomSDLMaterialObject* object);
  SDL_Renderer* getRenderer();
  CustomSDLRect* getCameraRect();
  void resize(int w, int h);
  SDL_Renderer* film(Stage* stage);
  void moveCamera();
};

class Screen {
 private:
  int SDL_init_status;
  SDL_Window* window;

 public:
  Screen(int resolution_w, int resolution_h);
  ~Screen();
  SDL_Window* getWindow();
};

#endif