#include <SDL.h>
#include <SDL_image.h>

#include <CustomSDLObjects.hpp>
#include <list>
#include <memory>
#include <stdexcept>
#include <string>

CustomSDLRect::CustomSDLRect(int x, int y, int w, int h)
    : SDL_Rect({x, y, w, h}) {}

CustomSDLRect::CustomSDLRect() {}
CustomSDLRect::CustomSDLRect(SDL_Rect rect) : SDL_Rect(rect) {}
CustomSDLRect::~CustomSDLRect() {}

SDL_Point CustomSDLRect::getPoint() const { return {this->x, this->y}; }

SDL_Point CustomSDLRect::getCenter() const {
  return {this->x + (this->w / 2), this->y + (this->h / 2)};
}

std::vector<SDL_Point> CustomSDLRect::getSDLVertices() const {
  return std::vector<SDL_Point>({
      SDL_Point({this->x, this->y}),
      SDL_Point({this->x + this->w, this->y}),
      SDL_Point({this->x, this->y + this->h}),
      SDL_Point({this->x + this->w, this->y + this->h}),
  });
}

CustomSDLMaterialObject::CustomSDLMaterialObject(
    CustomTextureManager *textureManager, CustomSDLRect srcRect,
    CustomSDLRect destination) {
  this->textureManager = textureManager;
  this->srcRect = srcRect;
}

CustomSDLMaterialObject::~CustomSDLMaterialObject() {}


// TODO calcular screenDestination?
void CustomSDLMaterialObject::render(const SDL_Rect screenDestination,
                                     SDL_Renderer *renderer) {
  SDL_RenderCopy(renderer, this->textureManager->getTexture(), &this->srcRect,
                 &screenDestination);
}
Position::Position(double x, double y) {
  this->x = x;
  this->y = y;
}

PositionObject::PositionObject(double x, double y, double w, double h)
    : Position(x, y) {
  this->w = w;
  this->h = h;
}
PositionObject::~PositionObject() {}
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
    CustomTextureManager *textureManager,
    std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>> animationSprites,
    CustomSDLRect destination)
    : CustomSDLMaterialObject(textureManager,
                              animationSprites.at(ANIMATION_TYPE::IDLE).front(),
                              destination) {
  this->animationSprites = animationSprites;
  this->currentFrameIterator = CyclicIterator<std::vector<SDL_Rect>::iterator>(
      this->animationSprites.at(ANIMATION_TYPE::IDLE).begin(),
      this->animationSprites.at(ANIMATION_TYPE::IDLE).end());
  this->currentAnimationType = ANIMATION_TYPE::IDLE;
  this->currentAnimationDirection = Direction::RIGHT;
  this->lastTick = SDL_GetTicks64();
}

void CustomAnimatedSDLMaterialObject::changeAnimation(
    ANIMATION_TYPE animationType, Direction direction) {
  if (this->currentAnimationDirection != direction ||
      this->currentAnimationType != animationType) {
    this->currentFrameIterator =
        CyclicIterator<std::vector<SDL_Rect>::iterator>(
            this->animationSprites.at(animationType).begin(),
            this->animationSprites.at(animationType).end());
    this->currentAnimationType = animationType;
    this->currentAnimationDirection = direction;
  }
}

void CustomAnimatedSDLMaterialObject::render(const SDL_Rect screenDestination,
                                             SDL_Renderer *renderer) {
  if (SDL_GetTicks64() > (this->lastTick + this->animationFrameGap)) {
    this->lastTick = SDL_GetTicks64();
    ++this->currentFrameIterator;
  }

  SDL_RendererFlip flip = SDL_FLIP_NONE;
  if (this->currentAnimationDirection == Direction::LEFT)
    flip = SDL_FLIP_HORIZONTAL;

  SDL_RenderCopyEx(
      renderer, this->textureManager->getTexture(this->currentAnimationType),
      &*this->currentFrameIterator, &screenDestination, 0, NULL, flip);
}

CustomAnimatedSDLMaterialObject::~CustomAnimatedSDLMaterialObject() {}
