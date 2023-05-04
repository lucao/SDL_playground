#include <SDL2/SDL.h>

#include <CustomSDLObject.hpp>
#include <CustomScreen.hpp>
#include <Stage.hpp>
#include <algorithm>
#include <cmath>
#include <map>
#include <memory>
#include <stdexcept>
#include <vector>

Camera::Camera(SDL_Window* window, CustomSDLMaterialObject* followedObject,
               int* speed) {
  Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  this->renderer = SDL_CreateRenderer(window, -1, render_flags);
  int w, h;
  SDL_GetWindowSize(window, &w, &h);
  SDL_RenderSetLogicalSize(this->renderer, w, h);
  this->cameraRectResize_w = w;
  this->cameraRectResize_h = h;
  this->cameraRect = new CustomSDLRect(new SDL_Rect({0, 0, w, h}));
  this->followedObject = followedObject;
  this->speed = *speed;
}
Camera::~Camera() {
  delete this->cameraRect;
  delete this->followedObject;
  SDL_DestroyRenderer(this->renderer);
}
void Camera::setFollowedObject(CustomSDLMaterialObject* object) {
  this->followedObject = object;
}
CustomSDLRect* Camera::getCameraRect() { return this->cameraRect; }
void Camera::resize(int w, int h) {
  this->cameraRectResize_w = w;
  this->cameraRectResize_h = h;
}
SDL_Renderer* Camera::getRenderer() { return this->renderer; }

const uint8_t reduction = 3;
void Camera::moveCamera() {
  // zoom camera
  if (!(this->cameraRectResize_w == this->cameraRect->w &&
        this->cameraRectResize_h == this->cameraRect->h)) {
    if (this->cameraRectResize_w > this->cameraRect->w) {
      this->cameraRect->w +=
         (int) log2(abs(this->cameraRectResize_w - this->cameraRect->w));
    } else {
      this->cameraRect->w -=
         (int) log2(abs(this->cameraRectResize_w - this->cameraRect->w));
    }
    if (this->cameraRectResize_h > this->cameraRect->h) {
      this->cameraRect->h +=
         (int) log2(abs(this->cameraRectResize_h - this->cameraRect->h));
    } else {
      this->cameraRect->h -=
         (int) log2(abs(this->cameraRectResize_h - this->cameraRect->h));
    }
    SDL_RenderSetLogicalSize(this->renderer, this->cameraRect->w,
                             this->cameraRect->h);
  }

  std::shared_ptr<SDL_Point> followedPoint;
  followedPoint = this->followedObject->getGlobalDestination()->createCenter();

  std::shared_ptr<SDL_Rect> cameraInsideRect =
      this->cameraRect->createInsideMiddleRect(
          reduction);  // TODO modificar inside rect para ser proporcional ao
                       // followed object

  std::shared_ptr<SDL_Point> cameraCenterPoint =
      this->cameraRect->createCenter();

  // moving camera following the object
  if (followedPoint->x > cameraInsideRect->x + cameraInsideRect->w ||
      followedPoint->x < cameraInsideRect->x) {
    int x_distance = followedPoint->x - cameraCenterPoint->x;
    int x_smoothDistance = (int) log2(abs(x_distance));

    if (x_distance > 0) {
      if (x_smoothDistance > this->speed) {
        this->cameraRect->x += this->speed;
      } else {
        this->cameraRect->x += x_smoothDistance;
      }
    } else if (x_distance < 0) {
      if (x_smoothDistance > this->speed) {
        this->cameraRect->x -= this->speed;
      } else {
        this->cameraRect->x -= x_smoothDistance;
      }
    }
  }
  if (followedPoint->y > cameraInsideRect->y + cameraInsideRect->h ||
      followedPoint->y < cameraInsideRect->y) {
    int y_distance = followedPoint->y - cameraCenterPoint->y;
    int y_smoothDistance = (int) log2(abs(y_distance));

    if (y_distance > 0) {
      if (y_smoothDistance > this->speed) {
        this->cameraRect->y += this->speed;
      } else {
        this->cameraRect->y += y_smoothDistance;
      }
    } else if (y_distance < 0) {
      if (y_smoothDistance > this->speed) {
        this->cameraRect->y -= this->speed;
      } else {
        this->cameraRect->y -= y_smoothDistance;
      }
    }
  }
}
SDL_Renderer* Camera::film(Stage* stage) {
  std::shared_ptr<SDL_Point> cameraCenter = this->cameraRect->createCenter();
  Region* activeRegion = stage->getActiveRegion(
      this->cameraRect->createCenter().get(), this->getRenderer());

  std::vector<Region*> regionsToRender = {};
  regionsToRender.push_back(activeRegion);

  for (Region::Direction direction : Region::directions) {
    Region* sideRegion = activeRegion->getSideRegion(direction);
    if (SDL_HasIntersection(sideRegion->getRect(), this->cameraRect)) {
      regionsToRender.push_back(sideRegion);
    }
  }
  if (regionsToRender.size() > 4) {
    throw std::logic_error(
        "Regions were not correctly clipped. only 4 or less regions can be "
        "renderer at the same time on the screen.");
  }
  std::vector<CustomSDLMaterialObject*> drawableObjects =
      stage->getMaterialObjectsNear(this->followedObject);

  for (Region* region : regionsToRender) {
    SDL_RenderCopy(this->renderer, region->getBackground()->getTexture(),
                   std::make_unique<CustomSDLRect>(region->getSrcRect(this->cameraRect)).get(),
                   std::make_unique<CustomSDLRect>(region->getDestinationRect(this->cameraRect)).get());
    for (CustomSDLMaterialObject* object : drawableObjects) {
      if (SDL_HasIntersection(object->getGlobalDestination(),
                              this->cameraRect)) {
        drawableObjects.push_back(object);
      }
    }
  }
  for (CustomSDLMaterialObject* object : drawableObjects) {
    SDL_RenderCopy(this->renderer, object->getTexture(), object->getSrcRect(),
                   object->getDestination(this->cameraRect));
  }

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
