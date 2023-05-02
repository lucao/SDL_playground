#include <SDL2/SDL.h>

#include <CustomSDLObject.hpp>
#include <Stage.hpp>
#include <algorithm>
#include <memory>
#include <tuple>

DynamicRegion::DynamicRegion(
    std::unordered_set<CustomSDLMaterialObject*> objectsOnRegion,
    CustomSDLRect* rect, SDL_Renderer* renderer)
    : Region(objectsOnRegion, rect, new BackgroundSDLTexture(renderer)) {}
DynamicRegion::~DynamicRegion() {}

Region::DirectionMatrixMap Region::directionMatrixMap = {
    {TOP, std::make_tuple<int, int>(0, 1)},
    {BOTTOM, std::make_tuple<int, int>(0, -1)},
    {LEFT, std::make_tuple<int, int>(-1, 0)},
    {RIGHT, std::make_tuple<int, int>(1, 0)},
    {TOPLEFT, std::make_tuple<int, int>(-1, 1)},
    {TOPRIGHT, std::make_tuple<int, int>(1, 1)},
    {BOTTOMLEFT, std::make_tuple<int, int>(-1, -1)},
    {BOTTOMRIGHT, std::make_tuple<int, int>(1, -1)}};
Region::Region(std::unordered_set<CustomSDLMaterialObject*> objectsOnRegion,
               CustomSDLRect* rect, BackgroundSDLTexture* background) {
  this->rect = rect;
  this->objectsOnRegion = objectsOnRegion;
  this->background = background;
}
Region::~Region() {
  delete this->rect;
  delete this->background;
}
void Region::addObjectToRegion(CustomSDLMaterialObject* object) {
  this->objectsOnRegion.insert(object);
}
void Region::removeObjectFromRegion(CustomSDLMaterialObject* object) {
  this->objectsOnRegion.erase(object);
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

  SDL_IntersectRect(this->rect, referenceRect, srcRect.get());

  srcRect->x -= this->rect->x;
  srcRect->y -= this->rect->y;

  return srcRect;
}
BackgroundSDLTexture* Region::getBackground() { return this->background; }
std::shared_ptr<Region> Region::getSideRegion(Region::Direction direction) {
  return this->sideRegions[direction];
}
void Region::setSideRegion(Region::Direction direction,
                           std::shared_ptr<Region> region) {
  this->sideRegions[direction] = region;
}

Stage::Stage(CustomSDLRect* rect) {
  this->rect = rect;
  // load active region
  // this->activeRegion = NULL;
}
Stage::~Stage() { delete this->rect; }
std::shared_ptr<Region> Stage::getActiveRegion(SDL_Point* cameraCenter,
                                               SDL_Renderer* renderer) {
  std::tuple<int, int> key =
      std::make_tuple<int, int>(cameraCenter->x / Region::fixedRegionWidth,
                                cameraCenter->y / Region::fixedRegionHeight);
  if (this->regionsMatrix.count(key)) {
    this->activeRegion = this->regionsMatrix[key];
    /*
    if (!SDL_PointInRect(cameraCenter, this->activeRegion->getRect().get()))
      return this->activeRegion; TODO verificar erros */
  } else {
    // não deveria ser DynamicRegion, carregar region de arquivo
    std::shared_ptr<Region> region(new DynamicRegion(
        {},
        new CustomSDLRect(new SDL_Rect(
            {std::get<0>(key) * Region::fixedRegionWidth,
             std::get<1>(key) * Region::fixedRegionHeight,
             Region::fixedRegionWidth, Region::fixedRegionHeight})),
        renderer));
    std::pair<std::tuple<int, int>, std::shared_ptr<Region>> pair =
        std::pair<std::tuple<int, int>, std::shared_ptr<Region>>({key, region});

    this->regionsMatrix.insert(pair);
    this->activeRegion = region;
    /*
      if (!SDL_PointInRect(cameraCenter, this->activeRegion->getRect().get()))
        return this->activeRegion;// TODO verificar erros*/
  }

  for (Region::Direction direction : Region::directions) {
    std::tuple<int, int> sideRegionKey = std::make_tuple<int, int>(
        std::get<0>(key) + std::get<0>(Region::directionMatrixMap[direction]),
        std::get<1>(key) + std::get<1>(Region::directionMatrixMap[direction]));
    if (this->regionsMatrix.count(sideRegionKey)) {
      this->activeRegion->setSideRegion(direction,
                                        this->regionsMatrix[sideRegionKey]);
    } else {
      // não deveria ser DynamicRegion, carregar region de arquivo
      std::shared_ptr<Region> sideRegion(new DynamicRegion(
          {},
          new CustomSDLRect(new SDL_Rect(
              {(std::get<0>(key) * Region::fixedRegionWidth) +
                   std::get<0>(sideRegionKey) * Region::fixedRegionWidth,
               (std::get<1>(key) * Region::fixedRegionHeight) +
                   std::get<1>(sideRegionKey) * Region::fixedRegionHeight,
               Region::fixedRegionWidth, Region::fixedRegionHeight})),
          renderer));
      this->regionsMatrix[sideRegionKey] = sideRegion;
      this->activeRegion->setSideRegion(direction, sideRegion);
    }
  }

  return this->activeRegion;
}
std::vector<CustomSDLMaterialObject*> Stage::getMaterialObjectsNear(
      GlobalPositionalSDLObject* object){return {};}
std::shared_ptr<Stage> Stage::getNextStage() { return this->nextStage; }
std::shared_ptr<Stage> Stage::getPreviousStage() { return this->previousStage; }