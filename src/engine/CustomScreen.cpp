#include <SDL.h>

#include <CustomScreen.hpp>
#include <cmath>
#include <memory>

CameraSDL::CameraSDL(SDL_Window* window, int speed) {
  Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  this->renderer = SDL_CreateRenderer(window, -1, render_flags);
  int w, h;
  SDL_GetWindowSize(window, &w, &h);
  SDL_RenderSetLogicalSize(this->renderer, w, h);
  this->cameraRectResize_w = w;
  this->cameraRectResize_h = h;
  this->cameraRect = CustomSDLRect(SDL_Rect({0, 0, w, h}));
  this->speed = speed;
}
CameraSDL::~CameraSDL() { SDL_DestroyRenderer(this->renderer); }
void CameraSDL::setFollowedObject(CustomSDLMaterialObject* object) {
  this->followedObject = object;
}
CustomSDLRect CameraSDL::getCameraRect() { return this->cameraRect; }

void CameraSDL::resize(int w, int h) {  // TODO
  this->cameraRectResize_w = w;
  this->cameraRectResize_h = h;
}
SDL_Renderer* CameraSDL::getRenderer() { return this->renderer; }

const static std::pair<int, int> cameraToleranceBounds =
    std::make_pair<int, int>(-10, 10);
const static float cameraSmoothLerp = 0.005f;
void CameraSDL::moveCamera(Uint64 startTick, Uint64 endTick) {
  // zoom camera
  if (!(this->cameraRectResize_w == this->cameraRect.w &&
        this->cameraRectResize_h == this->cameraRect.h)) {
    if (this->cameraRectResize_w > this->cameraRect.w) {
      this->cameraRect.w +=
          (int)log2(abs(this->cameraRectResize_w - this->cameraRect.w));
    } else {
      this->cameraRect.w -=
          (int)log2(abs(this->cameraRectResize_w - this->cameraRect.w));
    }
    if (this->cameraRectResize_h > this->cameraRect.h) {
      this->cameraRect.h +=
          (int)log2(abs(this->cameraRectResize_h - this->cameraRect.h));
    } else {
      this->cameraRect.h -=
          (int)log2(abs(this->cameraRectResize_h - this->cameraRect.h));
    }
    SDL_RenderSetLogicalSize(this->renderer, this->cameraRect.w,
                             this->cameraRect.h);
  }

  const SDL_Point followedPoint =
      this->followedObject->getDestination().getCenter();
  const SDL_Point cameraCenter = this->cameraRect.getCenter();

  const Uint64 deltaTime = (endTick - startTick);

  int delta_x = (followedPoint.x - cameraCenter.x);
  int delta_y = (followedPoint.y - cameraCenter.y);
  if ((delta_x < cameraToleranceBounds.first) ||
      (delta_x > cameraToleranceBounds.second))
    this->cameraRect.x +=
        static_cast<int>(delta_x * cameraSmoothLerp * deltaTime);
  if ((delta_y < cameraToleranceBounds.first) ||
      (delta_y > cameraToleranceBounds.second))
    this->cameraRect.y +=
        static_cast<int>(delta_y * cameraSmoothLerp * deltaTime);
}
SDL_Renderer* CameraSDL::film(Stage* stage, Uint64 startTick, Uint64 endTick) {
  SDL_SetRenderDrawColor(this->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  SDL_RenderClear(this->renderer);

  // render regions
  std::vector<Region*> regionsToRender;
  for (SDL_Point point : this->cameraRect.getVertices()) {
    Region* region = stage->getRegion(point);
    if (std::find(regionsToRender.begin(), regionsToRender.end(), region) ==
        regionsToRender.end()) {
      regionsToRender.push_back(region);
    }
  }
  for (Region* regionToRender : regionsToRender) {
    SDL_Rect srcRegioRect = regionToRender->getSrcRect(this->cameraRect);
    SDL_Rect destRegionRect = regionToRender->cropRectInside(this->cameraRect);
    SDL_RenderCopy(this->renderer,
                   regionToRender->getBackground()->getTexture(), &srcRegioRect,
                   &destRegionRect);
    // render objects
    for (CustomSDLMaterialObject* object :
         regionToRender->getMaterialObjects()) {
      SDL_Rect destObjectRect = object->getDestination();
      if (SDL_HasIntersection(&destObjectRect, &this->cameraRect)) {
        object->render({object->getDestination().x - this->cameraRect.x,
                        object->getDestination().y - this->cameraRect.y,
                        object->getDestination().w, object->getDestination().h},
                       this->renderer);
      }
    }
  }

  SDL_RenderPresent(this->renderer);
  return this->renderer;
}

Screen::Screen(int resolution_w, int resolution_h) {
  this->window =
      SDL_CreateWindow("GAME",  // creates a window
                       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       resolution_w, resolution_h, 0);
}
Screen::~Screen() { SDL_DestroyWindow(this->window); }
SDL_Window* Screen::getWindow() { return this->window; }
