#include <SDL.h>
#include <SDL_image.h>

#include <CustomSDLObject.hpp>
#include <string>

#include "../CustomPosition.hpp"

CustomSDLObject::CustomSDLObject(CustomPosition *position) {
  this->position = position;
}
CustomPosition *CustomSDLObject::getPosition() { return this->position; }

CustomSDLMaterialObject::CustomSDLMaterialObject(SDL_Surface *surface,
                                                 SDL_Rect destination) {
  this->destination = destination;
  this->surface = surface;
}
void CustomSDLMaterialObject::setSurface(std::string imgPath) {
  this->surface = IMG_Load(imgPath.c_str());
}
SDL_Surface *CustomSDLMaterialObject::getSurface() { return this->surface; }
void CustomSDLMaterialObject::setDestination(SDL_Rect destination) {
  this->destination = destination;
}
SDL_Rect CustomSDLMaterialObject::getDestination() { return this->destination; }
void CustomSDLMaterialObject::setlastDestination(SDL_Rect lastDestination) {
  this->lastDestination = lastDestination;
}
SDL_Rect CustomSDLMaterialObject::getlastDestination() {
  return this->lastDestination;
}
SDL_Texture *CustomSDLMaterialObject::getTexture(SDL_Renderer *renderer) {
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, this->surface);
  SDL_FreeSurface(this->surface);
  return texture;
}
#include <stdlib.h>

BackgroundSDLObject::BackgroundSDLObject() {
  this->surface = SDL_CreateRGBSurface(0, 640, 480, 32, (rand() % 255),
                                       (rand() % 255), (rand() % 255), 1);
}
BackgroundSDLObject::BackgroundSDLObject(SDL_Surface *surface) {
  this->surface = surface;
}
void BackgroundSDLObject::setSurface(std::string imgPath) {
  this->surface = IMG_Load(imgPath.c_str());
}
SDL_Surface *BackgroundSDLObject::getSurface() { return this->surface; }

SDL_Texture *BackgroundSDLObject::getTexture(SDL_Renderer *renderer) {
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, this->surface);
  SDL_FreeSurface(this->surface);
  return texture;
}