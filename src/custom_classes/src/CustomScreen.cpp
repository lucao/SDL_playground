#include <SDL.h>

#include <CustomSDLObject.hpp>
#include <CustomScreen.hpp>
#include <Stage.hpp>
#include <algorithm>
#include <cmath>
#include <map>
#include <memory>
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
void Camera::setFilmedRegion(Region* region) {
  if (this->filmedRegion.get() != region) {
    this->filmedRegion = std::shared_ptr<Region>(region);
  }
}
std::shared_ptr<Region> Camera::getFilmedRegion() { return this->filmedRegion; }
SDL_Renderer* Camera::getRenderer() { return this->renderer; }

const uint8_t reduction = 3;
void Camera::follow(SDL_Point* point) {
  std::shared_ptr<SDL_Point> cameraCenterPoint =
      this->cameraRect->createCenter();

  int x_distance = point->x - cameraCenterPoint->x;
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

  int y_distance = point->y - cameraCenterPoint->y;
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
void Camera::renderStage(Stage* stage) {
  // camera fixando nela mesma caso não esteja seguindo nenhum objeto
  std::shared_ptr<SDL_Point> followedPoint;
  followedPoint = this->followedObject->getDestination()->createCenter();

  // adjust camera
  std::shared_ptr<SDL_Rect> cameraInsideRect =
      this->cameraRect->createInsideMiddleRect(reduction);

  if (!SDL_PointInRect(followedPoint.get(), cameraInsideRect.get())) {
    this->follow(followedPoint.get());
  }

  // find main region being filmed
  std::shared_ptr<SDL_Point> cameraCenterPoint =
      this->cameraRect->createCenter();
  bool regionFound = false;
  for (Region* region : stage->getRegionsOnStage()) {
    if (SDL_PointInRect(cameraCenterPoint.get(), region->getRect())) {
      this->setFilmedRegion(region);
      regionFound = true;  // se não for encontrada, provavelmente precisa
                           // carregar outro Stage
    }
  }

  if (regionFound) {  // lembrando que a Region não pode ser menor do que o
                      // SDL_Rect da camera

    // get the part of the main region being filmed
    std::shared_ptr<CustomSDLRect> filmedRect(
        new CustomSDLRect(new SDL_Rect()));
    SDL_IntersectRect(this->cameraRect, filmedRegion->getRect(),
                      filmedRect.get());

    std::pair<CustomSDLRect*, Region*> pair;
    pair.first = filmedRect.get();
    pair.second = this->filmedRegion.get();
    std::map<CustomSDLRect*, Region*> clips;
    clips.insert(pair);

    if (!SDL_RectEquals(
            filmedRect.get(),
            this->getCameraRect())) {  // carregar outras 3 regions faltantes
      int x_filmed = filmedRect->x;
      int y_filmed = filmedRect->y;
      int h_filmed = filmedRect->h;
      int w_filmed = filmedRect->w;

      int x_camera = this->getCameraRect()->x;
      int y_camera = this->getCameraRect()->y;
      int h_camera = this->getCameraRect()->h;
      int w_camera = this->getCameraRect()->w;

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
                     key_value.first,
                     this->getRelativeDestinationRect(key_value.first).get());
      // TODO store relative destination on map
      for (CustomSDLMaterialObject* object :
           key_value.second->getObjectsOnRegion()) {
        if (SDL_HasIntersection(object->getDestination(), this->cameraRect)) {
          drawableObjects.push_back(object);
        }
      }
    }
    for (CustomSDLMaterialObject* object : drawableObjects) {
      SDL_RenderCopy(
          this->renderer, object->getTexture(), object->getSrcRect(),
          this->getRelativeDestinationRect(object->getDestination()).get());
    }

    SDL_RenderPresent(this->renderer);

  } else {
    // error (may need to reload stage)
  }
}
std::unique_ptr<SDL_Rect> Camera::getRelativeDestinationRect(
    CustomSDLRect* destination) {
  int x_relative = destination->x - this->cameraRect->x;
  int y_relative = destination->y - this->cameraRect->y;
  std::unique_ptr<SDL_Rect> relativeRect(
      new SDL_Rect({x_relative, y_relative, destination->w, destination->h}));
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
