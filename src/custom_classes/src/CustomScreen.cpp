#include <SDL.h>

#include <CustomScreen.hpp>
#include <algorithm>
#include <map>

Camera::Camera(SDL_Window* window) {
  Uint32 render_flags = SDL_RENDERER_ACCELERATED;
  this->renderer = SDL_CreateRenderer(window, -1, render_flags);
  this->x = 0;
  this->y = 0;
  this->h = 100;
}
void Camera::setFollowedObject(CustomSDLMaterialObject* object) {
  this->followedObject = object;
}
SDL_Renderer* Camera::getRenderer() { return this->renderer; }
void Camera::filmStage(Stage* stage) {
  SDL_Rect objectPosition = this->followedObject->getDestination();

  SDL_Point pointCenterOfObject = {objectPosition.x + (objectPosition.w / 2),
                                   objectPosition.y + (objectPosition.h / 2)};
  SDL_Rect centerOfCamera = {this->x, this->y, this->h, this->h};

  if (!SDL_PointInRect(&pointCenterOfObject, &centerOfCamera)) {
    this->x = pointCenterOfObject.x;
    this->y - pointCenterOfObject.y;
  }

  SDL_Point pointCenterOfCamera = {this->x + (this->h / 2),
                                   this->y + (this->h / 2)};
  Region* filmedRegion = *std::find_if(
      stage->getRegionsOnStage().begin(), stage->getRegionsOnStage().end(),
      [&pointCenterOfCamera](Region& region) {
        return SDL_PointInRect(&pointCenterOfCamera, &region.getRect());
      });
  if (filmedRegion) {
    SDL_Rect cameraRect = this->getCameraRect();
    std::map<SDL_Rect*, SDL_Surface**> clips =
        clipRegions(filmedRegion, cameraRect);

    SDL_Surface* cameraSurface = this->surface;
    std::for_each(
        clips.begin(), clips.end(),
        [&cameraSurface,
         &cameraRect](const std::pair<SDL_Rect*, SDL_Surface**>& pair) {
          SDL_Surface* surface = *(pair.second);
          SDL_BlitSurface(surface, pair.first, cameraSurface, &cameraRect);
          // SDL_BlitSurface( source, clip, destination, &camera );
          SDL_FreeSurface(surface);
        });

  } else {
    // error (may need to reload stage)
  }
}

std::map<SDL_Rect*, SDL_Surface**> clipRegions(Region* region,
                                               SDL_Rect cameraSpace) {
  std::map<SDL_Rect*, SDL_Surface**> clips{};

  SDL_Point intersectionBetweenRegions;
  SDL_IntersectRect;
  if (cameraSpace.x >= region->getRect().x) {
    intersectionBetweenRegions.x = cameraSpace.x;
  } else {
  }
  if (cameraSpace.y >= region->getRect().y) {
  } else {
  }
  return clips;
}

Screen::Screen(int resolution_w, int resolution_h) {
  this->window =
      SDL_CreateWindow("GAME",  // creates a window
                       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       resolution_w, resolution_h, 0);
}
SDL_Window* Screen::getWindow() { return this->window; }
