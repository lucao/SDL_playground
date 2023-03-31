#ifndef CUSTOM_SDL_OBJECT_H
#define CUSTOM_SDL_OBJECT_H

#include <SDL.h>

#include <CustomPosition.hpp>
#include <set>
#include <string>

class CustomSDLObject {
 private:
  CustomPosition *position;

 public:
  CustomSDLObject(CustomPosition *position);
  CustomPosition *getPosition();
};

class CustomSDLMaterialObject {
 private:
  SDL_Surface *surface;
  SDL_Rect destination;
  SDL_Rect lastDestination;

 public:
  CustomSDLMaterialObject(SDL_Surface *surface, SDL_Rect destination);

  void setSurface(std::string imgPath);
  SDL_Surface *getSurface();

  void setDestination(SDL_Rect destination);
  SDL_Rect getDestination();
  void setlastDestination(SDL_Rect lastDestination);
  SDL_Rect getlastDestination();

  SDL_Texture *getTexture(SDL_Renderer *renderer);
};

class BackgroundSDLObject {
 private:
  SDL_Surface *surface;

 public:
  BackgroundSDLObject();
  BackgroundSDLObject(SDL_Surface *surface);
  void setSurface(std::string imgPath);
  SDL_Surface *getSurface();
  SDL_Texture *getTexture(SDL_Renderer *renderer);
};

#endif