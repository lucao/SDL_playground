#ifndef CUSTOM_SCREEN_H
#define CUSTOM_SCREEN_H

#include <SDL.h>

#include <CustomSDLObject.hpp>
#include <Stage.hpp>
#include <list>

class Camera {
 private:
  SDL_Rect cameraRect;
  int x;
  int y;

  /**
   * Centro da câmera é um quadrado de lados iguais
   */
  int h;
  /**
   * Camera segue objeto mantendo-o próximo ao centro da câmera
   */
  CustomSDLMaterialObject* followedObject;

  SDL_Surface* surface;
  SDL_Renderer* renderer;

 public:
  Camera(SDL_Window* window);
  void setFollowedObject(CustomSDLMaterialObject* object);
  SDL_Renderer* getRenderer();
  SDL_Rect getCameraRect();
  void filmStage(Stage* stage);
};

class Screen {
 private:
  int SDL_init_status;
  SDL_Window* window;

 public:
  Screen(int resolution_w, int resolution_h);
  SDL_Window* getWindow();
};

#endif