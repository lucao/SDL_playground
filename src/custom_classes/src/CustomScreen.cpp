#include <SDL.h>

#include <CustomSDLObject.hpp>
#include <CustomScreen.hpp>
#include <Stage.hpp>
#include <algorithm>
#include <map>
#include <memory>
#include <vector>

Camera::Camera(SDL_Window* window, int* speed) {
  Uint32 render_flags = SDL_RENDERER_ACCELERATED;
  this->renderer = SDL_CreateRenderer(window, -1, render_flags);
  int w, h;
  SDL_GetWindowSize(window, &w, &h);
  this->cameraRect = new CustomSDLRect(new SDL_Rect({0, 0, w, h}));
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
void Camera::follow(SDL_Point* point) {
  std::unique_ptr<SDL_Point> cameraCenterPoint = this->cameraRect->getCenter();
  std::unique_ptr<SDL_Rect> insideRect =
      this->cameraRect->getInsideMiddleRect(reduction);
  SDL_IntersectRectAndLine(
      this->cameraRect->getInsideMiddleRect(reduction).get(),
      &cameraCenterPoint->x, &cameraCenterPoint->y, &point->x, &point->y);
}
void Camera::renderStage(Stage* stage) {
  // camera fixando nela mesma caso não esteja seguindo nenhum objeto
  std::unique_ptr<SDL_Point> followedPoint;
  if (this->followedObject) {
    followedPoint = this->followedObject->getDestination()->getCenter();
  } else {
    followedPoint = this->cameraRect->getCenter();
  }

  // adjust camera

  if (!SDL_PointInRect(
          followedPoint.get(),
          this->cameraRect->getInsideMiddleRect(reduction).get())) {
    this->follow(followedPoint.get());
  }

  // find main region being filmed
  Region* filmedRegion;
  bool regionFound = false;
  for (Region* region : stage->getRegionsOnStage()) {
    if (SDL_PointInRect(this->cameraRect->getCenter().get(),
                        region->getRect())) {
      filmedRegion = region;
      regionFound = true;
    }
  }

  if (regionFound) {  // lembrando que a Region não pode ser menor do que o
                      // SDL_Rect da camera

    // get the part of the main region being filmed
    CustomSDLRect* filmedRect = new CustomSDLRect(new SDL_Rect());
    SDL_IntersectRect(this->cameraRect->getRect(), filmedRegion->getRect(),
                      filmedRect->getRect());

    std::pair<CustomSDLRect*, Region*> pair;
    pair.first = filmedRect;
    pair.second = filmedRegion;
    std::map<CustomSDLRect*, Region*> clips;
    clips.insert(pair);

    if (!SDL_RectEquals(
            filmedRect->getRect(),
            this->getCameraRect()
                ->getRect())) {  // carregar outras 3 regions faltantes
      int x_filmed = filmedRect->getRect()->x;
      int y_filmed = filmedRect->getRect()->y;
      int h_filmed = filmedRect->getRect()->h;
      int w_filmed = filmedRect->getRect()->w;

      int x_camera = this->getCameraRect()->getRect()->x;
      int y_camera = this->getCameraRect()->getRect()->y;
      int h_camera = this->getCameraRect()->getRect()->h;
      int w_camera = this->getCameraRect()->getRect()->w;

      if (x_filmed == x_camera) {
        // TODO carregar region da direita
        if (y_filmed == y_camera) {
          // TODO carregar region debaixo e direita-baixo
        } else {
          // TODO carregar region de cima e direita-cima
        }
      } else {
        // TODO carregar region da esquerda
        if (y_filmed == y_camera) {
          // TODO carregar region debaixo e esquerda-baixo
        } else {
          // TODO carregar region de cima e esquerda-cima
        }
      }
    }
    SDL_RenderClear(this->renderer);
    std::vector<CustomSDLMaterialObject*> drawableObjects;
    for (std::pair<CustomSDLRect*, Region*> key_value : clips) {
      SDL_RenderCopy(this->renderer,
                     key_value.second->getBackground()->getTexture(),
                     key_value.second->getBackground()->getSrcRect()->getRect(),
                     this->getRelativeDestinationRect(key_value.first).get());
      delete key_value.first;
      for (CustomSDLMaterialObject* object :
           key_value.second->getObjectsOnRegion()) {
        if (SDL_HasIntersection(object->getDestination()->getRect(),
                                this->getCameraRect()->getRect())) {
          drawableObjects.push_back(object);
        }
      }
    }
    for (CustomSDLMaterialObject* object : drawableObjects) {
      SDL_RenderCopy(
          this->renderer, object->getTexture(), object->getSrcRect()->getRect(),
          this->getRelativeDestinationRect(object->getDestination()).get());
    }

    SDL_RenderPresent(this->renderer);

  } else {
    // error (may need to reload stage)
  }
}
std::unique_ptr<SDL_Rect> Camera::getRelativeDestinationRect(
    CustomSDLRect* destination) {
  int x_relative = destination->getRect()->x - this->cameraRect->getRect()->x;
  int y_relative = destination->getRect()->y - this->cameraRect->getRect()->y;
  std::unique_ptr<SDL_Rect> relativeRect(
      new SDL_Rect({x_relative, y_relative, destination->getRect()->w,
                    destination->getRect()->h}));
  return relativeRect;
}

Screen::Screen(int resolution_w, int resolution_h) {
  this->window =
      SDL_CreateWindow("GAME",  // creates a window
                       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       resolution_w, resolution_h, 0);
}
Screen::~Screen() { SDL_DestroyWindow(this->window); }
SDL_Window* Screen::getWindow() { return this->window; }
