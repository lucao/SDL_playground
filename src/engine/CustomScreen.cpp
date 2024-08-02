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

const uint8_t reduction = 3;
void CameraSDL::moveCamera() {
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

  const SDL_Rect cameraInsideRect =
      this->cameraRect.getInsideMiddleRect(reduction);

  const SDL_Point cameraCenterPoint = this->cameraRect.getCenter();

  // moving camera following the object
  if (followedPoint.x > cameraInsideRect.x + cameraInsideRect.w ||
      followedPoint.x < cameraInsideRect.x) {
    int x_distance = followedPoint.x - cameraCenterPoint.x;
    int x_smoothDistance = (int)log2(abs(x_distance));

    if (x_distance > 0) {
      if (x_smoothDistance > this->speed) {
        this->cameraRect.x += this->speed;
      } else {
        this->cameraRect.x += x_smoothDistance;
      }
    } else if (x_distance < 0) {
      if (x_smoothDistance > this->speed) {
        this->cameraRect.x -= this->speed;
      } else {
        this->cameraRect.x -= x_smoothDistance;
      }
    }
  }

  if (followedPoint.y > cameraInsideRect.y + cameraInsideRect.h ||
      followedPoint.y < cameraInsideRect.y) {
    int y_distance = followedPoint.y - cameraCenterPoint.y;
    int y_smoothDistance = (int)log2(abs(y_distance));

    if (y_distance > 0) {
      if (y_smoothDistance > this->speed) {
        this->cameraRect.y += this->speed;
      } else {
        this->cameraRect.y += y_smoothDistance;
      }
    } else if (y_distance < 0) {
      if (y_smoothDistance > this->speed) {
        this->cameraRect.y -= this->speed;
      } else {
        this->cameraRect.y -= y_smoothDistance;
      }
    }
  }
}
SDL_Renderer* CameraSDL::film(Stage* stage) {
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
    SDL_RenderCopy(
        this->renderer, regionToRender->getBackground()->getTexture(),
        &CustomSDLRect(regionToRender->getSrcRect(this->cameraRect)),
        &CustomSDLRect(regionToRender->cropRectInside(this->cameraRect)));
  }
  // render objects
  for (CustomSDLMaterialObject* object : stage->getMaterialObjects()) {
    if (SDL_HasIntersection(&object->getDestination(), &this->cameraRect)) {
      object->render({object->getDestination().x - this->cameraRect.x,
                      object->getDestination().y - this->cameraRect.y,
                      object->getDestination().w, object->getDestination().h},
                     this->renderer);
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
