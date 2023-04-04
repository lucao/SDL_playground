#ifndef CUSTOM_SDL_OBJECT_H
#define CUSTOM_SDL_OBJECT_H

#include <SDL.h>

#include <memory>
#include <set>
#include <string>

class CustomSDLRect {
 private:
  SDL_Rect *rect;

 public:
  CustomSDLRect(SDL_Rect *rect);
  CustomSDLRect(SDL_Rect *rect, SDL_Texture *texture, Uint32 *format,
                int *access);
  ~CustomSDLRect();
  std::unique_ptr<SDL_Point> getCenter();
  SDL_Rect *getRect();
  std::unique_ptr<SDL_Rect> getRectWith0Axis();
  std::unique_ptr<SDL_Rect> getInsideMiddleRect(uint8_t reductionProportion);
  void assingTexture(SDL_Texture *texture, Uint32 *format, int *access);
};

class CustomSDLObject {
 private:
  SDL_Point *position;

 public:
  CustomSDLObject(SDL_Point *position);
  virtual ~CustomSDLObject();
  SDL_Point *getPosition();
};

class CustomSDLMaterialObject {
 private:
  SDL_Texture *texture;
  CustomSDLRect *srcRect;
  CustomSDLRect *destination;

 public:
  CustomSDLMaterialObject(SDL_Texture *texture, CustomSDLRect *srcRect,
                          CustomSDLRect *destination);
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