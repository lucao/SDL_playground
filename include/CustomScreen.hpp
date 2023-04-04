#ifndef CUSTOM_SCREEN_H
#define CUSTOM_SCREEN_H

#include <SDL.h>

#include <CustomSDLObject.hpp>
#include <Stage.hpp>
#include <list>

class Camera {
 private:
  CustomSDLRect* cameraRect;
  CustomSDLMaterialObject* followedObject;

  SDL_Renderer* renderer;
  int speed;

  void follow(SDL_Point* point);

 public:
  Camera(SDL_Window* window, int* speed);
  ~Camera();
  void setFollowedObject(CustomSDLMaterialObject* object);
  SDL_Renderer* getRenderer();
  CustomSDLRect* getCameraRect();
  void setCameraRect(SDL_Rect* rect);
  std::unique_ptr<SDL_Rect> getRelativeDestinationRect(
      CustomSDLRect* destination);
  void renderStage(Stage* stage);
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