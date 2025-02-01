#ifndef CUSTOM_SDL_OBJECT_H
#define CUSTOM_SDL_OBJECT_H

#include <CustomTexture.hpp>
#include <CyclicIterator.hpp>
#include <unordered_map>
#include <vector>

#include "CustomGameObjects.hpp"
#include "CustomGameUtils.hpp"
#include "SDL_rect.h"
#include "SDL_render.h"
#include "SDL_stdinc.h"

//TODO verificar formas mais performáticas de calcular 
struct CustomSDLRect : SDL_Rect {
  CustomSDLRect(int x, int y, int w, int h);
  CustomSDLRect();
  CustomSDLRect(SDL_Rect rect);
  ~CustomSDLRect();
  SDL_Point getPoint() const;
  SDL_Point getCenter() const;
  bool xPointIsInBounds(int x) const;
  bool yPointIsInBounds(int y) const;
  int xGetNearestBoundary(int x) const;
  int yGetNearestBoundary(int y) const;
  std::vector<SDL_Point> getVertices() const;
};

// TODO 3D rect

class GlobalPositionalObject {
 protected:
  double x;
  double y;
  double z;

  GlobalPositionalObject();
  GlobalPositionalObject(double x, double y, double z);

 public:
  virtual ~GlobalPositionalObject();
  virtual const CustomSDLRect getDestination() = 0;
};

class CustomSDLMaterialObject : public GlobalPositionalObject {
 protected:
  double w;
  double h;

  CustomTextureManager *textureManager;

 private:
  CustomSDLRect srcRect;
  

 public:
  CustomSDLMaterialObject(CustomTextureManager *textureManager,
                          CustomSDLRect srcRect, CustomSDLRect destination);
  virtual ~CustomSDLMaterialObject();
  virtual const CustomSDLRect getDestination() override;
  virtual void render(const SDL_Rect cameraRect, SDL_Renderer *renderer);
};

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
      CustomTextureManager *textureManager,
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