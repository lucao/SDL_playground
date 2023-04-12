#ifndef CUSTOM_SCREEN_H
#define CUSTOM_SCREEN_H

#include <SDL.h>

#include <CustomSDLObject.hpp>
#include <Stage.hpp>
#include <vector>

class Camera {
 private:
  CustomSDLRect* cameraRect;
  CustomSDLMaterialObject* followedObject;

  SDL_Renderer* renderer;
  int speed;

 public:
  Camera(SDL_Window* window, CustomSDLMaterialObject* followedObject,
         int* speed);
  ~Camera();
  void setFollowedObject(CustomSDLMaterialObject* object);
  SDL_Renderer* getRenderer();
  CustomSDLRect* getCameraRect();
  void setCameraRect(SDL_Rect* rect);
  std::vector<std::shared_ptr<Region>> getRegionsToFilm(Stage* stage);
  void followObject();
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