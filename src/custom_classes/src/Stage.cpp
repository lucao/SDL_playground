#include <SDL.h>

#include <CustomSDLObject.hpp>
#include <Stage.hpp>

Stage::Stage(std::unordered_set<Region*> regionsOnStage, SDL_Rect rect) {
  this->rect = rect;
  this->regionsOnStage = regionsOnStage;
}

Region::Region(std::unordered_set<CustomSDLMaterialObject*> objectsOnRegion,
               SDL_Rect rect, BackgroundSDLObject* background) {
  this->rect = rect;
  this->objectsOnRegion = objectsOnRegion;
  this->background = *background;
}
void Region::addObjectToRegion(CustomSDLMaterialObject* object) {}
void Region::removeObjectFromRegion(CustomSDLMaterialObject* object) {}
std::unordered_set<CustomSDLMaterialObject*> Region::getObjectsOnRegion() {}
SDL_Rect Region::getRect() { return this->rect; }
BackgroundSDLObject Region::getBackground() { return this->background; }
void Region::loadRegion(Direction direction) {
  // Carregar Region dinâmica, TODO verificar qual a melhor forma de fazer
  // (estou pensando em apenas repetir o background da Region atual)
  SDL_Rect atual = this->rect;
  SDL_Rect newRect;
  switch (direction) {
    case Region::TOP:
      newRect = {atual.x, atual.y * 2, atual.w, atual.h};
      if (false) {
        // TODO carregar Region de um arquivo se não estiver em memória
      } else {
        this->top = new DynamicRegion({}, newRect);
      }
      break;
    case Region::BOTTOM:
      newRect = {atual.x, atual.y * -2, atual.w, atual.h};
      if (false) {
        // TODO carregar Region de um arquivo se não estiver em memória
      } else {
        this->top = new DynamicRegion({}, newRect);
      }
      break;
    case Region::LEFT:
      newRect = {atual.x * -2, atual.y, atual.w, atual.h};
      if (false) {
        // TODO carregar Region de um arquivo se não estiver em memória
      } else {
        this->top = new DynamicRegion({}, newRect);
      }
      break;
    case Region::RIGHT:
      newRect = {atual.x * 2, atual.y, atual.w, atual.h};
      if (false) {
        // TODO carregar Region de um arquivo se não estiver em memória
      } else {
        this->top = new DynamicRegion({}, newRect);
      }
      break;
    case Region::TOPLEFT:
      newRect = {atual.x * -2, atual.y * 2, atual.w, atual.h};
      if (false) {
        // TODO carregar Region de um arquivo se não estiver em memória
      } else {
        this->top = new DynamicRegion({}, newRect);
      }
      break;
    case Region::TOPRIGHT:
      newRect = {atual.x * 2, atual.y * 2, atual.w, atual.h};
      if (false) {
        // TODO carregar Region de um arquivo se não estiver em memória
      } else {
        this->top = new DynamicRegion({}, newRect);
      }
      break;
    case Region::BOTTOMLEFT:
      newRect = {atual.x * -2, atual.y * -2, atual.w, atual.h};
      if (false) {
        // TODO carregar Region de um arquivo se não estiver em memória
      } else {
        this->top = new DynamicRegion({}, newRect);
      }
      break;
    case Region::BOTTOMRIGHT:
      newRect = {atual.x * 2, atual.y * -2, atual.w, atual.h};
      if (false) {
        // TODO carregar Region de um arquivo se não estiver em memória
      } else {
        this->top = new DynamicRegion({}, newRect);
      }
      break;
    default:
      // ERROR
      break;
  }
}

DynamicRegion::DynamicRegion(
    std::unordered_set<CustomSDLMaterialObject*> objectsOnRegion, SDL_Rect rect)
    : Region(objectsOnRegion, rect, new BackgroundSDLObject()) {}

void World::loadStage(Stage* stage) { this->stages.insert(stage); }