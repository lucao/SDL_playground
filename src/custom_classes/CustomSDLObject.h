#ifndef CUSTOM_SDL_OBJECT_H
#define CUSTOM_SDL_OBJECT_H

#include <SDL.h>

#include <set>

#include "CustomPosition.h"

class CustomSDLObject {
 private:
  SDL_Surface *surface;
  CustomPosition *position;
  SDL_Rect destination;

 public:
  CustomSDLObject(SDL_Surface *surface, SDL_Rect destination);

  void setSurface(char *imgPath);
  SDL_Surface *getSurface();

  void setDestination(SDL_Rect destination);

  SDL_Rect getDestination();
};

#endif