#include <SDL.h>
#include <SDL_image.h>

#include <CustomSDLObject.hpp>
#include <memory>
#include <string>

CustomSDLRect::CustomSDLRect(SDL_Rect *rect) { this->rect = rect; }
CustomSDLRect::CustomSDLRect(SDL_Rect *rect, SDL_Texture *texture,
                             Uint32 *format, int *access) {
  this->rect = rect;
  this->assingTexture(texture, format, access);
}
CustomSDLRect::~CustomSDLRect() { delete this->rect; }
std::unique_ptr<SDL_Point> CustomSDLRect::getCenter() {
  return std::unique_ptr<SDL_Point>(
      new SDL_Point({this->rect->x + (this->rect->w / 2),
                     this->rect->y + (this->rect->h / 2)}));
}
SDL_Rect *CustomSDLRect::getRect() { return this->rect; }
std::unique_ptr<SDL_Rect> CustomSDLRect::getInsideMiddleRect(
    uint8_t reductionProportion) {
  int w_reducted = this->rect->w / reductionProportion;
  int h_reducted = this->rect->h / reductionProportion;
  return std::unique_ptr<SDL_Rect>(
      new SDL_Rect({(this->rect->x + (this->rect->w / 2) - (w_reducted / 2)),
                    (this->rect->y + (this->rect->h / 2) - (h_reducted / 2)),
                    w_reducted, h_reducted}));
}
void CustomSDLRect::assingTexture(SDL_Texture *texture, Uint32 *format,
                                  int *access) {
  SDL_QueryTexture(texture, format, access, &this->rect->w, &this->rect->h);
}

CustomSDLObject::CustomSDLObject(SDL_Point *position) {
  this->position = position;
}
CustomSDLObject::~CustomSDLObject() {
  if (this->position) {
    delete this->position;
  }
}
SDL_Point *CustomSDLObject::getPosition() { return this->position; }

CustomSDLMaterialObject::CustomSDLMaterialObject(SDL_Texture *texture,
                                                 CustomSDLRect *srcRect,
                                                 CustomSDLRect *destination) {
  this->srcRect = srcRect;
  this->destination = destination;
  this->texture = texture;
  this->srcRect->assingTexture(this->texture, NULL, NULL);
}
std::unique_ptr<SDL_Rect> CustomSDLRect::getRectWith0Axis() {
  return std::unique_ptr<SDL_Rect>(
      new SDL_Rect({0, 0, this->rect->w, this->rect->h}));
}
CustomSDLMaterialObject::~CustomSDLMaterialObject() {
  delete this->destination;
  delete this->srcRect;
  SDL_DestroyTexture(this->texture);
}
void CustomSDLMaterialObject::setTexture(SDL_Texture *texture) {
  this->texture = texture;
}
SDL_Texture *CustomSDLMaterialObject::getTexture() { return this->texture; }
void CustomSDLMaterialObject::setDestination(SDL_Rect *destination) {
  delete this->destination;
  this->destination = new CustomSDLRect(destination);
}
CustomSDLRect *CustomSDLMaterialObject::getDestination() {
  return this->destination;
}
void CustomSDLMaterialObject::setSrcRect(SDL_Rect *srcRect) {
  delete this->srcRect;
  this->srcRect = new CustomSDLRect(srcRect);
}
CustomSDLRect *CustomSDLMaterialObject::getSrcRect() { return this->srcRect; }

#include <stdlib.h>

BackgroundSDLObject::BackgroundSDLObject(SDL_Renderer *renderer,
                                         CustomSDLRect *destination) {
  SDL_Surface *surface =
      SDL_CreateRGBSurface(0, 640, 480, 32, 120, 120, 120, 0);
  this->texture = SDL_CreateTextureFromSurface(renderer, surface);
  this->srcRect = new CustomSDLRect(new SDL_Rect());
  this->destination = destination;
  this->srcRect->assingTexture(this->texture, NULL, NULL);

  SDL_FreeSurface(surface);
}
BackgroundSDLObject::~BackgroundSDLObject() {
  delete this->srcRect;
  delete this->destination;
  SDL_DestroyTexture(this->texture);
}
CustomSDLRect *BackgroundSDLObject::getSrcRect() { return this->srcRect; }
CustomSDLRect *BackgroundSDLObject::getDestination() {
  return this->destination;
}
SDL_Texture *BackgroundSDLObject::getTexture() { return this->texture; }