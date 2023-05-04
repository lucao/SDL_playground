#ifndef CUSTOM_SDL_OBJECT_H
#define CUSTOM_SDL_OBJECT_H

#include <SDL.h>

#include <memory>
#include <string>

struct CustomSDLRect : SDL_Rect {
  CustomSDLRect(SDL_Rect *rect);
  ~CustomSDLRect();
  std::shared_ptr<SDL_Point> createCenter();
  std::shared_ptr<CustomSDLRect> createRectWith0Axis();
  std::shared_ptr<CustomSDLRect> createInsideMiddleRect(
      uint8_t reductionProportion);
  bool xPointIsInBounds(int x);
  bool yPointIsInBounds(int y);
  int xGetNearestBoundary(int x);
  int yGetNearestBoundary(int y);
};

class GlobalPositionalSDLObject {
 private:
  CustomSDLRect *destination;
  GlobalPositionalSDLObject *lastDestination;
  void setLastDestination(GlobalPositionalSDLObject *lastDestination);

 public:
  GlobalPositionalSDLObject();
  GlobalPositionalSDLObject(SDL_Rect *destination);
  virtual ~GlobalPositionalSDLObject();
  CustomSDLRect *getGlobalDestination();
  void setDestination(SDL_Rect *destination);
  CustomSDLRect *getDestination(CustomSDLRect *referenceRect);
  GlobalPositionalSDLObject *getLastGlobalDestination();

  void move(SDL_Rect *destination);
};

class CustomSDLMaterialObject : public GlobalPositionalSDLObject {
 private:
  SDL_Texture *texture;
  CustomSDLRect *srcRect;

 public:
  CustomSDLMaterialObject(SDL_Texture *texture, CustomSDLRect *srcRect,
                          CustomSDLRect *destination);
  CustomSDLMaterialObject(CustomSDLRect *srcRect, CustomSDLRect *destination);
  virtual ~CustomSDLMaterialObject();

  void setTexture(SDL_Texture *texture);
  SDL_Texture *getTexture();

  void setSrcRect(SDL_Rect *srcRect);
  CustomSDLRect *getSrcRect();
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