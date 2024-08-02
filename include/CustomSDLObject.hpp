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

struct CustomSDLRect : SDL_Rect {
  CustomSDLRect();
  CustomSDLRect(SDL_Rect rect);
  ~CustomSDLRect();
  SDL_Point getPoint();
  SDL_Point getCenter();
  SDL_Rect getInsideMiddleRect(uint8_t reductionProportion);
  bool xPointIsInBounds(int x);
  bool yPointIsInBounds(int y);
  int xGetNearestBoundary(int x);
  int yGetNearestBoundary(int y);
  std::vector<SDL_Point> getVertices();

  void setPoint(SDL_Point point);
  void setRect(SDL_Rect rect);
  void setX(int x);
  void setY(int y);
  void setW(int w);
  void setH(int h);
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
 private:
  SDL_Texture *texture;
  CustomSDLRect srcRect;

 public:
  CustomSDLMaterialObject(SDL_Texture *texture, CustomSDLRect srcRect,
                          CustomSDLRect destination);
  CustomSDLMaterialObject(CustomSDLRect srcRect, CustomSDLRect destination);
  virtual ~CustomSDLMaterialObject();
  virtual void render(const SDL_Rect cameraRect, SDL_Renderer * renderer);
};

const enum ANIMATION_TYPE { IDLE = 0, WALKING = 1, RUNNING = 2 };

class CustomAnimatedSDLMaterialObject : public CustomSDLMaterialObject {
 private:
  std::unordered_map<ANIMATION_TYPE, CustomSDLRect[]> animationSprites;

  Uint64 animationFrameGap;
  Uint64 lastTick;
  // TODO
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