#include <SDL.h>
#include <SDL_image.h>

#include <CustomSDLObject.hpp>
#include <memory>
#include <string>

CustomSDLRect::CustomSDLRect(SDL_Rect *rect) : SDL_Rect(*rect) {}
CustomSDLRect::CustomSDLRect(SDL_Rect *rect, SDL_Texture *texture,
                             Uint32 *format, int *access)
    : SDL_Rect(*rect) {
  this->assingTexture(texture, format, access);
}
CustomSDLRect::~CustomSDLRect() {}
std::shared_ptr<SDL_Point> CustomSDLRect::createCenter() {
  return std::shared_ptr<SDL_Point>(
      new SDL_Point({this->x + (this->w / 2), this->y + (this->h / 2)}));
}
std::shared_ptr<CustomSDLRect> CustomSDLRect::createInsideMiddleRect(
    uint8_t reductionProportion) {
  int w_reducted = this->w / reductionProportion;
  int h_reducted = this->h / reductionProportion;
  return std::shared_ptr<CustomSDLRect>(new CustomSDLRect(new SDL_Rect(
      {(this->x + (this->w / 2) - (w_reducted / 2)),
       (this->y + (this->h / 2) - (h_reducted / 2)), w_reducted, h_reducted})));
}
std::shared_ptr<CustomSDLRect> CustomSDLRect::createRectWith0Axis() {
  return std::shared_ptr<CustomSDLRect>(
      new CustomSDLRect(new SDL_Rect({0, 0, this->w, this->h})));
}
bool CustomSDLRect::xPointIsInBounds(int x) {
  return (x < this->x + this->w && x > this->x);
}
bool CustomSDLRect::yPointIsInBounds(int y) {
  return (y < this->y + this->h && y > this->y);
}
int CustomSDLRect::xGetNearestBoundary(int x) {
  std::shared_ptr<SDL_Point> centerPoint = this->createCenter();
  if (x < centerPoint->x) {
    return this->x;
  } else if (x > centerPoint->x) {
    return this->x + this->w;
  } else {
    return this->x;
  }
}
int CustomSDLRect::yGetNearestBoundary(int y) {
  std::shared_ptr<SDL_Point> centerPoint = this->createCenter();
  if (y < centerPoint->y) {
    return this->y;
  } else if (y > centerPoint->y) {
    return this->y + this->h;
  } else {
    return this->y;
  }
}

std::unique_ptr<SDL_Rect> CustomSDLRect::getRelativeDestinationRect(
    CustomSDLRect *destination) {
  int x_relative = destination->x - this->x;
  int y_relative = destination->y - this->y;
  std::unique_ptr<SDL_Rect> relativeRect(
      new SDL_Rect({x_relative, y_relative, destination->w, destination->h}));
  return relativeRect;
}

std::unique_ptr<SDL_Rect> CustomSDLRect::getRelativeSrcRect(
    CustomSDLRect *destination) {
  return destination->getRelativeDestinationRect(this);
}

std::shared_ptr<CustomSDLRect> CustomSDLRect::clipRect(
    CustomSDLRect *referenceRect) {
  std::shared_ptr<CustomSDLRect> clippedRect(new CustomSDLRect(new SDL_Rect()));
  SDL_IntersectRect(referenceRect, this, clippedRect.get());
  return clippedRect;
}
void CustomSDLRect::assingTexture(SDL_Texture *texture, Uint32 *format,
                                  int *access) {
  SDL_QueryTexture(texture, format, access, &this->w, &this->h);
}

CustomSDLMaterialObject::CustomSDLMaterialObject(SDL_Texture *texture,
                                                 CustomSDLRect *srcRect,
                                                 CustomSDLRect *destination) {
  this->srcRect = srcRect;
  this->destination = destination;
  this->texture = texture;
  this->srcRect->assingTexture(this->texture, NULL, NULL);
}

CustomSDLMaterialObject::CustomSDLMaterialObject(CustomSDLRect *srcRect,
                                                 CustomSDLRect *destination) {
  this->srcRect = srcRect;
  this->destination = destination;
}

CustomSDLMaterialObject::~CustomSDLMaterialObject() {
  delete this->destination;
  delete this->srcRect;
  SDL_DestroyTexture(this->texture);
}
void CustomSDLMaterialObject::setTexture(SDL_Texture *texture) {
  this->texture = texture;
  this->srcRect->assingTexture(this->texture, NULL, NULL);
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
      SDL_CreateRGBSurface(0, 640, 480, 32, 120, 120, 120, -1);
  this->texture = SDL_CreateTextureFromSurface(renderer, surface);
  this->srcRect = new CustomSDLRect(new SDL_Rect({0, 0, 640, 480}));
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