#include "../CustomSDLObject.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../CustomPosition.h"

CustomSDLObject::CustomSDLObject(SDL_Surface *surface, SDL_Rect destination) {
  this->destination = destination;
  this->position = new CustomPosition(destination.x, destination.y);
  this->surface = surface;
}

void CustomSDLObject::setSurface(char *imgPath) {  // TODO improve this method
  this->surface = IMG_Load(imgPath);
}
SDL_Surface *CustomSDLObject::getSurface() { return this->surface; }

void CustomSDLObject::setDestination(SDL_Rect destination) {
  this->destination = destination;
}

SDL_Rect CustomSDLObject::getDestination() {
  return this->destination;
}

;