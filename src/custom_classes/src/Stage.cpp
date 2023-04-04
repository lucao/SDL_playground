#include <SDL.h>

#include <CustomSDLObject.hpp>
#include <Stage.hpp>
#include <algorithm>

Stage::Stage(std::unordered_set<Region*> regionsOnStage, CustomSDLRect* rect) {
  this->rect = rect;
  this->regionsOnStage = regionsOnStage;
}
Stage::~Stage() {
  delete this->rect;
  for (Region* region : this->regionsOnStage) {
    delete region;
  }
}
std::unordered_set<Region*> Stage::getRegionsOnStage() {
  return this->regionsOnStage;
}

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
SDL_Rect* Region::getRect() { return this->rect->getRect(); }
BackgroundSDLObject* Region::getBackground() { return this->background; }
void Region::loadRegion(Direction direction, SDL_Renderer* renderer) {
  // Carregar Region dinâmica, TODO verificar qual a melhor forma de fazer
  // (estou pensando em apenas repetir o background da Region atual)
  SDL_Rect atual = *this->rect->getRect();
  SDL_Rect* newRect;
  std::pair<Direction, std::shared_ptr<Region>> pair;
  switch (direction) {
    case Region::TOP:
      newRect = new SDL_Rect({atual.x, atual.y * 2, atual.w, atual.h});
      if (false) {
        // TODO carregar Region de um arquivo se não estiver em memória
      } else {
        pair.first = Region::TOP;
        pair.second = std::shared_ptr<Region>(
            new DynamicRegion({}, new CustomSDLRect(newRect), renderer));
      }
      break;
    case Region::BOTTOM:
      newRect = new SDL_Rect({atual.x, atual.y * -2, atual.w, atual.h});
      if (false) {
        // TODO carregar Region de um arquivo se não estiver em memória
      } else {
        pair.first = Region::BOTTOM;
        pair.second = std::shared_ptr<Region>(
            new DynamicRegion({}, new CustomSDLRect(newRect), renderer));
      }
      break;
    case Region::LEFT:
      newRect = new SDL_Rect({atual.x * -2, atual.y, atual.w, atual.h});
      if (false) {
        // TODO carregar Region de um arquivo se não estiver em memória
      } else {
        pair.first = Region::LEFT;
        pair.second = std::shared_ptr<Region>(
            new DynamicRegion({}, new CustomSDLRect(newRect), renderer));
      }
      break;
    case Region::RIGHT:
      newRect = new SDL_Rect({atual.x * 2, atual.y, atual.w, atual.h});
      if (false) {
        // TODO carregar Region de um arquivo se não estiver em memória
      } else {
        pair.first = Region::RIGHT;
        pair.second = std::shared_ptr<Region>(
            new DynamicRegion({}, new CustomSDLRect(newRect), renderer));
      }
      break;
    case Region::TOPLEFT:
      newRect = new SDL_Rect({atual.x * -2, atual.y * 2, atual.w, atual.h});
      if (false) {
        // TODO carregar Region de um arquivo se não estiver em memória
      } else {
        pair.first = Region::TOPLEFT;
        pair.second = std::shared_ptr<Region>(
            new DynamicRegion({}, new CustomSDLRect(newRect), renderer));
      }
      break;
    case Region::TOPRIGHT:
      newRect = new SDL_Rect({atual.x * 2, atual.y * 2, atual.w, atual.h});
      if (false) {
        // TODO carregar Region de um arquivo se não estiver em memória
      } else {
        pair.first = Region::TOPRIGHT;
        pair.second = std::shared_ptr<Region>(
            new DynamicRegion({}, new CustomSDLRect(newRect), renderer));
      }
      break;
    case Region::BOTTOMLEFT:
      newRect = new SDL_Rect({atual.x * -2, atual.y * -2, atual.w, atual.h});
      if (false) {
        // TODO carregar Region de um arquivo se não estiver em memória
      } else {
        pair.first = Region::BOTTOMLEFT;
        pair.second = std::shared_ptr<Region>(
            new DynamicRegion({}, new CustomSDLRect(newRect), renderer));
      }
      break;
    case Region::BOTTOMRIGHT:
      newRect = new SDL_Rect({atual.x * 2, atual.y * -2, atual.w, atual.h});
      if (false) {
        // TODO carregar Region de um arquivo se não estiver em memória
      } else {
        pair.first = Region::BOTTOMRIGHT;
        pair.second = std::shared_ptr<Region>(
            new DynamicRegion({}, new CustomSDLRect(newRect), renderer));
      }
      break;
    default:
      // ERROR
      break;
  }
}

DynamicRegion::DynamicRegion(std::set<CustomSDLMaterialObject*> objectsOnRegion,
                             CustomSDLRect* rect, SDL_Renderer* renderer)
    : Region(objectsOnRegion, rect, new BackgroundSDLObject(renderer, rect)) {}
DynamicRegion::~DynamicRegion() {}
