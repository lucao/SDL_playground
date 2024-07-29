#include <SDL.h>
#include <SDL_image.h>

#include <CustomSDLObject.hpp>
#include <list>
#include <memory>
#include <stdexcept>
#include <string>
CustomSDLRect::CustomSDLRect() {}
CustomSDLRect::CustomSDLRect(SDL_Rect rect) : SDL_Rect(rect) {}
CustomSDLRect::~CustomSDLRect() {}
void CustomSDLRect::setX(int x) { this->x = x; }
void CustomSDLRect::setY(int y) { this->y = y; }
void CustomSDLRect::setW(int w) { this->w = w; }
void CustomSDLRect::setH(int h) { this->h = h; }
void CustomSDLRect::setPoint(SDL_Point point) {
  this->x = point.x;
  this->y = point.y;
}
void CustomSDLRect::setRect(SDL_Rect rect) {
  this->x = rect.x;
  this->y = rect.y;
  this->w = rect.w;
  this->h = rect.h;
}

SDL_Point CustomSDLRect::getPoint() { return {this->x, this->y}; }
SDL_Rect CustomSDLRect::getInsideMiddleRect(uint8_t reductionProportion) {
  int w_reducted = this->w / reductionProportion;
  int h_reducted = this->h / reductionProportion;
  return {(this->x + (this->w / 2) - (w_reducted / 2)),
          (this->y + (this->h / 2) - (h_reducted / 2)), w_reducted, h_reducted};
}

SDL_Point CustomSDLRect::getCenter() {
  return {this->x + (this->w / 2), this->y + (this->h / 2)};
}
bool CustomSDLRect::xPointIsInBounds(int x) {
  return (x < this->x + this->w && x > this->x);
}
bool CustomSDLRect::yPointIsInBounds(int y) {
  return (y < this->y + this->h && y > this->y);
}
int CustomSDLRect::xGetNearestBoundary(int x) {
  SDL_Point centerPoint = this->getCenter();
  if (x < centerPoint.x) {
    return this->x;
  } else if (x > centerPoint.x) {
    return this->x + this->w;
  } else {
    return this->x;
  }
}
int CustomSDLRect::yGetNearestBoundary(int y) {
  SDL_Point centerPoint = this->getCenter();
  if (y < centerPoint.y) {
    return this->y;
  } else if (y > centerPoint.y) {
    return this->y + this->h;
  } else {
    return this->y;
  }
}
std::vector<SDL_Point> CustomSDLRect::getVertices() {
  return std::vector<SDL_Point>({
      SDL_Point({this->x, this->y}),
      SDL_Point({this->x + this->w, this->y}),
      SDL_Point({this->x, this->y + this->h}),
      SDL_Point({this->x + this->w, this->y + this->h}),
  });
}

CustomSDLMaterialObject::CustomSDLMaterialObject(SDL_Texture *texture,
                                                 CustomSDLRect srcRect,
                                                 CustomSDLRect destination)
    : GlobalPositionalSDLObject(destination) {
  this->texture = texture;
  this->srcRect = srcRect;
}

CustomSDLMaterialObject::CustomSDLMaterialObject(CustomSDLRect srcRect,
                                                 CustomSDLRect destination)
    : GlobalPositionalSDLObject(destination) {
  this->srcRect = srcRect;
}

CustomSDLMaterialObject::~CustomSDLMaterialObject() {
  SDL_DestroyTexture(this->texture);
}
CustomSDLRect CustomSDLMaterialObject::getSrcRect() { return this->srcRect; }
void CustomSDLMaterialObject::setTexture(SDL_Texture *texture) {
  this->texture = texture;
}
SDL_Texture *CustomSDLMaterialObject::getTexture() { return this->texture; }
void GlobalPositionalSDLObject::setDestination(SDL_Rect destination) {
  this->destination = CustomSDLRect(destination);
}
void GlobalPositionalSDLObject::setDestination(SDL_Point destination) {
  this->destination.x = destination.x;
  this->destination.y = destination.y;
}
CustomSDLRect GlobalPositionalSDLObject::getDestination() {
  return this->destination;
}

GlobalPositionalSDLObject::GlobalPositionalSDLObject() {
  this->destination = CustomSDLRect({});
}
GlobalPositionalSDLObject::GlobalPositionalSDLObject(SDL_Rect destination) {
  this->destination = CustomSDLRect(destination);
}
GlobalPositionalSDLObject::~GlobalPositionalSDLObject() {}
BackgroundSDLTexture::BackgroundSDLTexture(SDL_Texture *texture) {
  // SDL_CreateRGBSurface(0, 640, 480, 32, 120, 120, 120, -1);
  this->texture = texture;
  this->srcRect = new CustomSDLRect(SDL_Rect());
}
BackgroundSDLTexture::~BackgroundSDLTexture() {
  delete this->srcRect;
  SDL_DestroyTexture(this->texture);
}
SDL_Texture *BackgroundSDLTexture::getTexture() { return this->texture; }

CustomAnimatedSDLMaterialObject::CustomAnimatedSDLMaterialObject(
    SDL_Texture *texture, CustomSDLRect srcRect, CustomSDLRect destination,
    int typesOfAnimation, int stepsOfAnimation)
    : CustomSDLMaterialObject(texture, srcRect, destination) {
  // TODO steps of animation
}

CustomAnimatedSDLMaterialObject::CustomAnimatedSDLMaterialObject(
    CustomSDLRect srcRect, CustomSDLRect destination, int typesOfAnimation,
    int stepsOfAnimation)
    : CustomSDLMaterialObject(srcRect, destination) {
  // TODO steps of animation
}

CustomAnimatedSDLMaterialObject::~CustomAnimatedSDLMaterialObject() {}
