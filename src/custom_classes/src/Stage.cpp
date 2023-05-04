#include <SDL.h>
#include <SDL_image.h>

#include <CustomSDLObject.hpp>
#include <Stage.hpp>
#include <algorithm>
#include <memory>
#include <tuple>

DynamicRegion::DynamicRegion(
    std::unordered_set<CustomSDLMaterialObject*> objectsOnRegion,
    CustomSDLRect* rect, SDL_Renderer* renderer, SDL_Texture* texture)
    : Region(objectsOnRegion, rect, new BackgroundSDLTexture(texture)) {
    }
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
CustomSDLRect* Region::getRect() {
  return this->rect;
}
CustomSDLRect* Region::getDestinationRect(
    CustomSDLRect* referenceRect) {
  std::unique_ptr<CustomSDLRect> referenceDestinationRect =
      std::make_unique<CustomSDLRect>(
          new SDL_Rect({0, 0, referenceRect->w, referenceRect->h}));

    std::unique_ptr<CustomSDLRect> thisDestinationRect = std::make_unique<CustomSDLRect>(new SDL_Rect(
          {this->rect->x - referenceRect->x, this->rect->y - referenceRect->y,
           this->rect->w, this->rect->h}));

  CustomSDLRect* destinationRect = new CustomSDLRect(new SDL_Rect());
  if (SDL_IntersectRect(thisDestinationRect.get(),
                        referenceDestinationRect.get(),
                        destinationRect)) {
    return destinationRect;
  } else {  // throw exception
    return new CustomSDLRect(new SDL_Rect());
  }
}
CustomSDLRect* Region::getSrcRect(
    CustomSDLRect* referenceRect) {
  CustomSDLRect* srcRect =
      new CustomSDLRect(new SDL_Rect());

  SDL_IntersectRect(this->rect, referenceRect, srcRect);

  srcRect->x -= this->rect->x;
  srcRect->y -= this->rect->y;

  return srcRect;
}
BackgroundSDLTexture* Region::getBackground() { return this->background; }
Region* Region::getSideRegion(Region::Direction direction) {
  return this->sideRegions[direction].get();
}
void Region::setSideRegion(Region::Direction direction,
                           std::shared_ptr<Region> region) {
  this->sideRegions[direction] = region;
}

Stage::Stage(CustomSDLRect* rect, SDL_Renderer* renderer) {
  this->rect = rect;
  this->renderer = renderer;
  this->default_dynamic_texture = IMG_LoadTextureTyped_RW(
      this->renderer,
       SDL_RWFromFile("C:/Users/lucas/git/SDL_playground/media/img/praia.jpg",
                          "r"), 1, "jpeg");
  // load active region
  // this->activeRegion = NULL;
}
Stage::~Stage() { delete this->rect; }

SDL_Renderer* Stage::getRenderer() {
  return this->renderer;
}
Region* Stage::getActiveRegion(SDL_Point* cameraCenter) {
  std::tuple<int, int> key =
      std::make_tuple<int, int>(cameraCenter->x / Region::fixedRegionWidth,
                                cameraCenter->y / Region::fixedRegionHeight);
  if (this->regionsMatrix.count(key)) {
    this->activeRegion = this->regionsMatrix[key].get();
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
        this->renderer, this->default_dynamic_texture));
    std::pair<std::tuple<int, int>, std::shared_ptr<Region>> pair =
        std::pair<std::tuple<int, int>, std::shared_ptr<Region>>({key, region});

    this->regionsMatrix.insert(pair);
    this->activeRegion = region.get();
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
          this->renderer, this->default_dynamic_texture));
      this->regionsMatrix[sideRegionKey] = sideRegion;
      this->activeRegion->setSideRegion(direction, sideRegion);
    }
  }

  return this->activeRegion;
}
std::vector<CustomSDLMaterialObject*> Stage::getMaterialObjectsNear(
      GlobalPositionalSDLObject* object){return {};}
Stage* Stage::getNextStage() { return this->nextStage; }
Stage* Stage::getPreviousStage() { return this->previousStage; }