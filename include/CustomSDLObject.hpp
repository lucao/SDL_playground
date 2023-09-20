#ifndef CUSTOM_SDL_OBJECT_H
#define CUSTOM_SDL_OBJECT_H

#include <SDL.h>

#include <memory>
#include <string>
#include <vector>

struct CustomSDLRect : SDL_Rect {
  CustomSDLRect(SDL_Rect *rect);
  ~CustomSDLRect();
  const SDL_Point getPoint();
  const SDL_Point getCenter();
  std::unique_ptr<CustomSDLRect> createInsideMiddleRect(
      uint8_t reductionProportion);
  bool xPointIsInBounds(int x);
  bool yPointIsInBounds(int y);
  int xGetNearestBoundary(int x);
  int yGetNearestBoundary(int y);
  std::vector<const SDL_Point> getVertices();

  void setPoint(SDL_Point point);
  void setRect(SDL_Rect rect);
  void setX(int x);
  void setY(int y);
  void setW(int w);
  void setH(int h);
};

class GlobalPositionalSDLObject {
 private:
  CustomSDLRect *destination;

 public:
  GlobalPositionalSDLObject();
  GlobalPositionalSDLObject(SDL_Rect *destination);
  virtual ~GlobalPositionalSDLObject();
  CustomSDLRect *getGlobalDestination();
  void setDestination(SDL_Rect *destination);
  CustomSDLRect *getDestination(CustomSDLRect *referenceRect);
};

class CustomSDLMaterialObject : public GlobalPositionalSDLObject {
 private:
  SDL_Texture *texture;
  CustomSDLRect *srcRect;

  void setLastDestination(CustomSDLRect *lastDestination);

 public:
  CustomSDLMaterialObject(SDL_Texture *texture, CustomSDLRect *srcRect,
                          CustomSDLRect *destination);
  CustomSDLMaterialObject(CustomSDLRect *srcRect, CustomSDLRect *destination);
  virtual ~CustomSDLMaterialObject();

  void setTexture(SDL_Texture *texture);
  SDL_Texture *getTexture();

  virtual CustomSDLRect *getSrcRect();
};

class CustomMovableSDLMaterialObject : public CustomSDLMaterialObject {
 public:
  CustomMovableSDLMaterialObject(SDL_Texture *texture, CustomSDLRect *srcRect,
                                 CustomSDLRect *destination);
  CustomMovableSDLMaterialObject(CustomSDLRect *srcRect,
                                 CustomSDLRect *destination);
  virtual ~CustomMovableSDLMaterialObject();

  void move(SDL_Rect *destination);
};

struct CustomSDLAnimation {};

class CustomAnimatedSDLMaterialObject : public CustomSDLMaterialObject {
 private:
  int typesOfAnimation;
  int stepsOfAnimation;

  Uint64 animationFrameGap;
  Uint64 lastTick;
//TODO
 public:
  CustomAnimatedSDLMaterialObject(SDL_Texture *texture, CustomSDLRect *srcRect,
                                  CustomSDLRect *destination,
                                  int typesOfAnimation, int stepsOfAnimation);
  CustomAnimatedSDLMaterialObject(CustomSDLRect *srcRect,
                                  CustomSDLRect *destination,
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