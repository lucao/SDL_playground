#ifndef CUSTOM_SDL_OBJECT_H
#define CUSTOM_SDL_OBJECT_H

#include <SDL.h>

#include <CustomPhysics.hpp>
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <CyclicIterator.hpp>
#include "CustomGameUtils.hpp"

struct CustomSDLRect : SDL_Rect {
  CustomSDLRect();
  CustomSDLRect(SDL_Rect rect);
  ~CustomSDLRect();
  SDL_Point getPoint();
  SDL_Point getCenter();
  bool xPointIsInBounds(int x);
  bool yPointIsInBounds(int y);
  int xGetNearestBoundary(int x);
  int yGetNearestBoundary(int y);
  std::vector<SDL_Point> getVertices();
};

class GlobalPositionalSDLObject {
 private:
  CustomSDLRect destination;

 public:
  GlobalPositionalSDLObject();
  GlobalPositionalSDLObject(SDL_Rect destination);
  virtual ~GlobalPositionalSDLObject();
  CustomSDLRect getDestination();
  void setDestination(SDL_Point destination);
};

class CustomSDLMaterialObject : public GlobalPositionalSDLObject {
 protected:
  SDL_Texture *texture;

 private:
  CustomSDLRect srcRect;

 public:
  CustomSDLMaterialObject(SDL_Texture *texture, SDL_Rect srcRect,
                          SDL_Rect destination);
  CustomSDLMaterialObject(SDL_Rect srcRect, SDL_Rect destination);
  virtual ~CustomSDLMaterialObject();
  virtual void render(const SDL_Rect cameraRect, SDL_Renderer *renderer);
};

const enum ANIMATION_TYPE { IDLE = 0, WALKING = 1, RUNNING = 2 };

class CustomAnimatedSDLMaterialObject : public CustomSDLMaterialObject {
 private:
  std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>> animationSprites;

  const Uint64 animationFrameGap = 90;
  Uint64 lastTick;

 protected:
  CyclicIterator<std::vector<SDL_Rect>::iterator> currentFrameIterator;
  ANIMATION_TYPE currentAnimationType;
  Direction currentAnimationDirection;
 public:
  CustomAnimatedSDLMaterialObject(
      SDL_Texture *texture,
      std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>>
          animationSprites,
      CustomSDLRect destination);
  CustomAnimatedSDLMaterialObject(
      std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>>
          animationSprites,
      CustomSDLRect destination);
  virtual ~CustomAnimatedSDLMaterialObject();

  void changeAnimation(ANIMATION_TYPE animationType, Direction direction);

  virtual void render(const SDL_Rect cameraRect,
                      SDL_Renderer *renderer) override;
};

class BackgroundSDLTexture {
 private:
  SDL_Texture *texture;
  CustomSDLRect *srcRect;

 public:
  BackgroundSDLTexture(SDL_Texture *texture);
  virtual ~BackgroundSDLTexture();
  SDL_Texture *getTexture();
};

#endif