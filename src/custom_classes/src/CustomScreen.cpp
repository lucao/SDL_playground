#include <SDL.h>

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
void Camera::setCameraRect(SDL_Rect* rect) {
  this->cameraRect = new CustomSDLRect(rect);
}
SDL_Renderer* Camera::getRenderer() { return this->renderer; }

const uint8_t reduction = 3;
void Camera::followObject() {
  // camera fixando nela mesma caso não esteja seguindo nenhum objeto
  std::shared_ptr<SDL_Point> followedPoint;
  followedPoint = this->followedObject->getGlobalDestination()->createCenter();

  // adjust camera
  std::shared_ptr<SDL_Rect> cameraInsideRect =
      this->cameraRect->createInsideMiddleRect(reduction);

  std::shared_ptr<SDL_Point> cameraCenterPoint =
      this->cameraRect->createCenter();

  if (followedPoint->x > cameraInsideRect->x + cameraInsideRect->w ||
      followedPoint->x < cameraInsideRect->x) {
    int x_distance = followedPoint->x - cameraCenterPoint->x;
    int x_smoothDistance = log2(abs(x_distance));

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
    int y_smoothDistance = log2(abs(y_distance));

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
std::vector<std::shared_ptr<Region>> Camera::getRegionsToFilm(Stage* stage) {
  std::vector<std::shared_ptr<Region>> regionsToRender = {};
  std::shared_ptr<SDL_Point> cameraCenter = this->cameraRect->createCenter();
  std::shared_ptr<Region> activeRegion = stage->getActiveRegion(
      this->cameraRect->createCenter().get(), this->getRenderer());

  regionsToRender.push_back(activeRegion);

  for (Region::Direction direction : Region::directions) {
    std::unique_ptr<CustomSDLRect> intersectedRect =
        std::make_unique<CustomSDLRect>(new SDL_Rect());
    std::shared_ptr<Region> sideRegion = activeRegion->getSideRegion(direction);
    if (SDL_IntersectRect(sideRegion->getRect().get(), this->getCameraRect(),
                          intersectedRect.get())) {
      regionsToRender.push_back(sideRegion);
    }
  }
  if (regionsToRender.size() > 4) {
    throw std::logic_error(
        "Regions were not correctly clipped. only 4 or less regions can be "
        "renderer at the same time on the screen.");
  }
  return regionsToRender;
}

Screen::Screen(int resolution_w, int resolution_h) {
  this->window =
      SDL_CreateWindow("GAME",  // creates a window
                       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       resolution_w, resolution_h, 0);
}
Screen::~Screen() { SDL_DestroyWindow(this->window); }
SDL_Window* Screen::getWindow() { return this->window; }
