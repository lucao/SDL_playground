#ifndef CUSTOM_SDL_OBJECT_H
#define CUSTOM_SDL_OBJECT_H

#include <SDL.h>

#include <memory>
#include <set>
#include <string>

class CustomSDLRect : public SDL_Rect {
 public:
  CustomSDLRect(SDL_Rect *rect);
  CustomSDLRect(SDL_Rect *rect, SDL_Texture *texture, Uint32 *format,
                int *access);
  ~CustomSDLRect();
  std::shared_ptr<SDL_Point> createCenter();
  std::shared_ptr<CustomSDLRect> createRectWith0Axis();
  std::shared_ptr<CustomSDLRect> createInsideMiddleRect(
      uint8_t reductionProportion);
  bool xPointIsInBounds(int x);
  bool yPointIsInBounds(int y);
  int xGetNearestBoundary(int x);
  int yGetNearestBoundary(int y);
  void assingTexture(SDL_Texture *texture, Uint32 *format, int *access);
};

class CustomSDLMaterialObject {
 private:
  SDL_Texture *texture;
  CustomSDLRect *srcRect;
  CustomSDLRect *destination;

 public:
  CustomSDLMaterialObject(SDL_Texture *texture, CustomSDLRect *srcRect,
                          CustomSDLRect *destination);
  CustomSDLMaterialObject(CustomSDLRect *srcRect, CustomSDLRect *destination);
  virtual ~CustomSDLMaterialObject();

  void setTexture(SDL_Texture *texture);
  SDL_Texture *getTexture();

  void setDestination(SDL_Rect *destination);
  CustomSDLRect *getDestination();

  void setSrcRect(SDL_Rect *srcRect);
  CustomSDLRect *getSrcRect();
};

class BackgroundSDLObject {
 private:
  SDL_Texture *texture;
  CustomSDLRect *srcRect;
  CustomSDLRect *destination;

 public:
  BackgroundSDLObject(SDL_Renderer *renderer, CustomSDLRect *destination);
  BackgroundSDLObject(SDL_Surface *surface, CustomSDLRect *destination);
  virtual ~BackgroundSDLObject();
  SDL_Texture *getTexture();
  CustomSDLRect *getSrcRect();
  CustomSDLRect *getDestination();
};

#endif