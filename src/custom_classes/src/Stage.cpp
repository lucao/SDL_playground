#include <SDL.h>
#include <SDL_image.h>

#include <CustomSDLObject.hpp>
#include <CustomUtils.hpp>
#include <Stage.hpp>
#include <algorithm>
#include <list>
#include <memory>
#include <tuple>
#include <valarray>

DynamicRegion::DynamicRegion(
    Region::RegionID regionId,
    std::unordered_set<CustomSDLMaterialObject*> objectsOnRegion,
    CustomSDLRect* rect, SDL_Renderer* renderer, SDL_Texture* texture)
    : Region(regionId, objectsOnRegion, rect,
             new BackgroundSDLTexture(texture)) {}
DynamicRegion::~DynamicRegion() {}

const std::valarray<std::valarray<int>>
    Region::RegionID::peripheralRegionsModifiers{
        //[0] = up/down, [1] = left/right
        // modifiers {0=middle, 1=up/right, -1=down/left}
        {1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

Region::Region(Region::RegionID regionId,
               std::unordered_set<CustomSDLMaterialObject*> objectsOnRegion,
               CustomSDLRect* rect, BackgroundSDLTexture* background) {
  this->regionID = regionId;
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
CustomSDLRect* Region::getRect() { return this->rect; }
CustomSDLRect* Region::getDestinationRect(CustomSDLRect* referenceRect) {
  std::unique_ptr<CustomSDLRect> referenceDestinationRect =
      std::make_unique<CustomSDLRect>(
          new SDL_Rect({0, 0, referenceRect->w, referenceRect->h}));

  std::unique_ptr<CustomSDLRect> thisDestinationRect =
      std::make_unique<CustomSDLRect>(new SDL_Rect(
          {this->rect->x - referenceRect->x, this->rect->y - referenceRect->y,
           this->rect->w, this->rect->h}));

  CustomSDLRect* destinationRect = new CustomSDLRect(new SDL_Rect());
  if (SDL_IntersectRect(thisDestinationRect.get(),
                        referenceDestinationRect.get(), destinationRect)) {
    return destinationRect;
  } else {  // throw exception
    return new CustomSDLRect(new SDL_Rect());
  }
}
CustomSDLRect* Region::getSrcRect(CustomSDLRect* referenceRect) {
  CustomSDLRect* srcRect = new CustomSDLRect(new SDL_Rect());

  SDL_IntersectRect(this->rect, referenceRect, srcRect);

  srcRect->x -= this->rect->x;
  srcRect->y -= this->rect->y;

  return srcRect;
}
BackgroundSDLTexture* Region::getBackground() { return this->background; }
Region::RegionID Region::getRegionId() { return this->regionID; }

Stage::Stage(CustomSDLRect* rect, SDL_Renderer* renderer) {
  this->rect = rect;
  this->renderer = renderer;
  this->default_dynamic_texture = IMG_LoadTextureTyped_RW(
      this->renderer,
      SDL_RWFromFile("C:/Users/lucas/git/SDL_playground/media/img/praia.jpg",
                     "r"),
      1, "jpeg");
}
Stage::~Stage() { delete this->rect; }

SDL_Renderer* Stage::getRenderer() { return this->renderer; }

Region* Stage::getRegion(SDL_Point point) {
  Region::RegionID regionId = Region::RegionID::valueFrom(
      std::make_tuple<int, int>(point.x / Region::fixedRegionWidth,
                                point.y / Region::fixedRegionHeight));

  Region* region;
  try {
    region = this->regionsMatrix.getElement(regionId);
    return region;
  } catch (ElementNotFountException ex) {
    //TODO
    region = this->regionsMatrix.getElement(regionId);
  }

  std::for_each(
      regionId.neighbourRegionsIDs.begin(), regionId.neighbourRegionsIDs.end(),
      [this](std::tuple<int, int> key) {
        Region::RegionID neighbourRegionID = Region::RegionID::valueFrom(key);

        this->regionsMatrix.addElement(
            neighbourRegionID, [this, neighbourRegionID]() {
              return (Region*)new DynamicRegion(
                  neighbourRegionID, {},
                  new CustomSDLRect(new SDL_Rect(
                      {std::get<0>(neighbourRegionID.id) *
                           Region::fixedRegionWidth,
                       std::get<1>(neighbourRegionID.id) *
                           Region::fixedRegionHeight,
                       Region::fixedRegionWidth, Region::fixedRegionHeight})),
                  this->renderer, this->default_dynamic_texture);
            });
      });

  throw new RegionNotLoaded(regionId.id);
}
std::vector<CustomSDLMaterialObject*> Stage::getMaterialObjectsNear(
    GlobalPositionalSDLObject* object) {
  return {};
}
Stage* Stage::getNextStage() { return this->nextStage; }
Stage* Stage::getPreviousStage() { return this->previousStage; }