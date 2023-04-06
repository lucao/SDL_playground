#include <SDL.h>

#include <CustomSDLObject.hpp>
#include <Stage.hpp>
#include <algorithm>

Stage::Stage(std::unordered_set<std::shared_ptr<Region>> regionsOnStage,
             CustomSDLRect* rect) {
  this->rect = rect;
  this->regionsOnStage = regionsOnStage;
}
Stage::~Stage() { delete this->rect; }
std::unordered_set<std::shared_ptr<Region>> Stage::getRegionsOnStage() {
  return this->regionsOnStage;
}
std::shared_ptr<Region> Stage::getRegionFromPoint(
    std::shared_ptr<SDL_Point> point, SDL_Renderer* renderer) {
  if (!SDL_PointInRect(point.get(), this->rect)) {
    throw StageOutOfBounds();
  }
  for (std::shared_ptr<Region> region : this->regionsOnStage) {
    if (SDL_PointInRect(point.get(), region->getRect())) {
      return region;
      for (Region::Direction direction : Region::directions) {
        region->loadRegion(this, direction, renderer);
      }
    }
  }
}
std::shared_ptr<Stage> Stage::getNextStage() { return this->nextStage; }
std::shared_ptr<Stage> Stage::getPreviousStage() { return this->previousStage; }

Region::Region(std::set<CustomSDLMaterialObject*> objectsOnRegion,
               CustomSDLRect* rect, BackgroundSDLObject* background) {
  this->rect = rect;
  this->objectsOnRegion = objectsOnRegion;
  this->background = background;
}
Region::~Region() {
  delete this->rect;
  delete this->background;
  for (CustomSDLMaterialObject* object : this->objectsOnRegion) {
    delete object;
  }
}
void Region::addObjectToRegion(CustomSDLMaterialObject* object) {
  this->objectsOnRegion.insert(object);
}
void Region::removeObjectFromRegion(CustomSDLMaterialObject* object) {
  this->objectsOnRegion.erase(object);
}
std::set<CustomSDLMaterialObject*> Region::getObjectsOnRegion() {
  return this->objectsOnRegion;
}
SDL_Rect* Region::getRect() { return this->rect; }
BackgroundSDLObject* Region::getBackground() { return this->background; }
std::shared_ptr<Region> Region::loadRegion(Stage* stage,
                                           Region::Direction direction,
                                           SDL_Renderer* renderer) {
  // Carregar Region dinâmica, TODO verificar qual a melhor forma de fazer
  // (estou pensando em apenas repetir o background da Region atual)

  if (this->sideRegions.count(direction)) {
    return this->sideRegions[direction];
  }
  SDL_Rect atual = *this->rect;
  CustomSDLRect* newRect;
  std::pair<Region::Direction, std::shared_ptr<Region>> pair;
  switch (direction) {
    case Region::TOP:
      newRect = new CustomSDLRect(
          new SDL_Rect({atual.x, atual.y * 2, atual.w, atual.h}));
      if (false) {
        // TODO carregar Region de um arquivo se não estiver em memória
      } else {
        pair.first = Region::TOP;
        pair.second =
            std::shared_ptr<Region>(new DynamicRegion({}, newRect, renderer));

        this->sideRegions.insert(pair);
        return pair.second;
      }
      break;
    case Region::BOTTOM:
      newRect = new CustomSDLRect(
          new SDL_Rect({atual.x, atual.y * -2, atual.w, atual.h}));
      if (false) {
        // TODO carregar Region de um arquivo se não estiver em memória
      } else {
        pair.first = Region::BOTTOM;
        pair.second =
            std::shared_ptr<Region>(new DynamicRegion({}, newRect, renderer));

        this->sideRegions.insert(pair);
        return pair.second;
      }
      break;
    case Region::LEFT:
      newRect = new CustomSDLRect(
          new SDL_Rect({atual.x * -2, atual.y, atual.w, atual.h}));
      if (false) {
        // TODO carregar Region de um arquivo se não estiver em memória
      } else {
        pair.first = Region::LEFT;
        pair.second =
            std::shared_ptr<Region>(new DynamicRegion({}, newRect, renderer));

        this->sideRegions.insert(pair);
        return pair.second;
      }
      break;
    case Region::RIGHT:
      newRect = new CustomSDLRect(
          new SDL_Rect({atual.x * 2, atual.y, atual.w, atual.h}));
      if (false) {
        // TODO carregar Region de um arquivo se não estiver em memória
      } else {
        pair.first = Region::RIGHT;
        pair.second =
            std::shared_ptr<Region>(new DynamicRegion({}, newRect, renderer));

        this->sideRegions.insert(pair);
        return pair.second;
      }
      break;
    case Region::TOPLEFT:
      newRect = new CustomSDLRect(
          new SDL_Rect({atual.x * -2, atual.y * 2, atual.w, atual.h}));
      if (false) {
        // TODO carregar Region de um arquivo se não estiver em memória
      } else {
        pair.first = Region::TOPLEFT;
        pair.second =
            std::shared_ptr<Region>(new DynamicRegion({}, newRect, renderer));

        this->sideRegions.insert(pair);
        return pair.second;
      }
      break;
    case Region::TOPRIGHT:
      newRect = new CustomSDLRect(
          new SDL_Rect({atual.x * 2, atual.y * 2, atual.w, atual.h}));
      if (false) {
        // TODO carregar Region de um arquivo se não estiver em memória
      } else {
        pair.first = Region::TOPRIGHT;
        pair.second =
            std::shared_ptr<Region>(new DynamicRegion({}, newRect, renderer));

        this->sideRegions.insert(pair);
        return pair.second;
      }
      break;
    case Region::BOTTOMLEFT:
      newRect = new CustomSDLRect(
          new SDL_Rect({atual.x * -2, atual.y * -2, atual.w, atual.h}));
      if (false) {
        // TODO carregar Region de um arquivo se não estiver em memória
      } else {
        pair.first = Region::BOTTOMLEFT;
        pair.second =
            std::shared_ptr<Region>(new DynamicRegion({}, newRect, renderer));

        this->sideRegions.insert(pair);
        return pair.second;
      }
      break;
    case Region::BOTTOMRIGHT:
      newRect = new CustomSDLRect(
          new SDL_Rect({atual.x * 2, atual.y * -2, atual.w, atual.h}));
      if (false) {
        // TODO carregar Region de um arquivo se não estiver em memória
      } else {
        pair.first = Region::BOTTOMRIGHT;
        pair.second =
            std::shared_ptr<Region>(new DynamicRegion({}, newRect, renderer));

        this->sideRegions.insert(pair);
        return pair.second;
      }
      break;
  }
}
std::shared_ptr<CustomSDLRect> Region::clipRegion(CustomSDLRect* cameraRect) {
  std::shared_ptr<CustomSDLRect> filmedRect(new CustomSDLRect(new SDL_Rect()));
  SDL_IntersectRect(cameraRect, this->getRect(), filmedRect.get());
  return filmedRect;
}

DynamicRegion::DynamicRegion(std::set<CustomSDLMaterialObject*> objectsOnRegion,
                             CustomSDLRect* rect, SDL_Renderer* renderer)
    : Region(objectsOnRegion, rect, new BackgroundSDLObject(renderer, rect)) {}
DynamicRegion::~DynamicRegion() {}
