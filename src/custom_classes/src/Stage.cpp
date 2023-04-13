#include <SDL.h>

#include <CustomSDLObject.hpp>
#include <Stage.hpp>
#include <algorithm>

Stage::Stage(std::unordered_set<std::shared_ptr<Region>> regionsOnStage,
             CustomSDLRect* rect) {
  this->rect = rect;
  this->regionsOnStage = regionsOnStage;
  this->activeRegion = NULL;
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
    if (SDL_PointInRect(point.get(), region->getRect().get())) {
      for (Region::Direction direction : Region::directions) {
        region->loadRegion(this, direction, renderer);
      }
      return region;
    }
  }
  throw StageOutOfBounds();
}
std::shared_ptr<Region> Stage::getActiveRegion() { return this->activeRegion; }
void Stage::setActiveRegion(std::shared_ptr<Region> region) {
  this->activeRegion = region;
}
std::shared_ptr<Stage> Stage::getNextStage() { return this->nextStage; }
std::shared_ptr<Stage> Stage::getPreviousStage() { return this->previousStage; }

Region::Region(std::unordered_set<CustomSDLMaterialObject*> objectsOnRegion,
               CustomSDLRect* rect, BackgroundSDLTexture* background) {
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
std::unordered_set<CustomSDLMaterialObject*> Region::getObjectsOnRegion() {
  return this->objectsOnRegion;
}
std::shared_ptr<CustomSDLRect> Region::getRect() {
  return std::make_shared<CustomSDLRect>(this->rect);
}
std::shared_ptr<CustomSDLRect> Region::getDestinationRect(
    CustomSDLRect* referenceRect) {
  std::unique_ptr<CustomSDLRect> referenceDestinationRect =
      std::make_unique<CustomSDLRect>(
          new SDL_Rect({0, 0, referenceRect->w, referenceRect->h}));

  std::unique_ptr<CustomSDLRect> thisDestinationRect =
      std::make_unique<CustomSDLRect>(new SDL_Rect(
          {this->rect->x - referenceRect->x, this->rect->y - referenceRect->y,
           this->rect->w, this->rect->h}));

  std::shared_ptr<CustomSDLRect> destinationRect =
      std::make_shared<CustomSDLRect>(new SDL_Rect());
  if (SDL_IntersectRect(thisDestinationRect.get(),
                        referenceDestinationRect.get(),
                        destinationRect.get())) {
    return destinationRect;
  } else {  // throw exception
    return std::make_shared<CustomSDLRect>(new SDL_Rect());
  }
}
std::shared_ptr<CustomSDLRect> Region::getSrcRect(
    CustomSDLRect* referenceRect) {
  std::shared_ptr<CustomSDLRect> srcRect =
      std::make_shared<CustomSDLRect>(new SDL_Rect());

  int pointWH_x = this->rect->x + this->rect->w;
  int pointWH_y = this->rect->y + this->rect->h;

  srcRect->x = referenceRect->x - this->rect->x;
  if (srcRect->x < 0) srcRect->x = 0;
  srcRect->y = referenceRect->y - this->rect->y;
  if (srcRect->y < 0) srcRect->y = 0;

  if (pointWH_x > referenceRect->x &&
      pointWH_x < referenceRect->x + referenceRect->w) {
    srcRect->w = pointWH_x - referenceRect->x;
  } else {
    srcRect->w = referenceRect->w;
  }
  if (pointWH_y > referenceRect->y &&
      pointWH_y < referenceRect->y + referenceRect->h) {
    srcRect->h = pointWH_y - referenceRect->y;
  } else {
    srcRect->h = referenceRect->h;
  }

  return srcRect;
}
BackgroundSDLTexture* Region::getBackground() { return this->background; }
std::shared_ptr<Region> Region::getSideRegion(Region::Direction direction) {
  return this->sideRegions[direction];
}
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
  throw StageOutOfBounds();
}

DynamicRegion::DynamicRegion(
    std::unordered_set<CustomSDLMaterialObject*> objectsOnRegion,
    CustomSDLRect* rect, SDL_Renderer* renderer)
    : Region(objectsOnRegion, rect, new BackgroundSDLTexture(renderer)) {}
DynamicRegion::~DynamicRegion() {}
