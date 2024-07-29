#ifndef CUSTOM_SDL_OBJECT_H
#define CUSTOM_SDL_OBJECT_H

#include <SDL.h>

#include<CustomPhysics.hpp>

#include <list>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

struct CustomSDLRect : SDL_Rect {
  CustomSDLRect();
  CustomSDLRect(SDL_Rect rect);
  ~CustomSDLRect();
  SDL_Point getPoint();
  SDL_Point getCenter();
  SDL_Rect getInsideMiddleRect(
      uint8_t reductionProportion);
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
  void setDestination(SDL_Rect destination);
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

  void setTexture(SDL_Texture *texture);
  SDL_Texture *getTexture();

  virtual CustomSDLRect getSrcRect();
};

struct CustomSDLAnimation {};

class CustomAnimatedSDLMaterialObject : public CustomSDLMaterialObject {
 private:
  int typesOfAnimation;
  int stepsOfAnimation;

  Uint64 animationFrameGap;
  Uint64 lastTick;
  // TODO
 public:
  CustomAnimatedSDLMaterialObject(SDL_Texture *texture, CustomSDLRect srcRect,
                                  CustomSDLRect destination,
                                  int typesOfAnimation, int stepsOfAnimation);
  CustomAnimatedSDLMaterialObject(CustomSDLRect srcRect,
                                  CustomSDLRect destination,
                                  int typesOfAnimation, int stepsOfAnimation);
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