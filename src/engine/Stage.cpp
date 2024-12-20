#include <SDL.h>
#include <SDL_image.h>

#include <Stage.hpp>

DynamicRegion::DynamicRegion(Region::RegionID regionId, CustomSDLRect rect,
                             SDL_Renderer* renderer, SDL_Texture* texture)
    : Region(regionId, rect, new BackgroundSDLTexture(texture)) {}
DynamicRegion::~DynamicRegion() {}

const std::valarray<std::valarray<int>>
    Region::RegionID::peripheralRegionsModifiers{
        //[0] = up/down, [1] = left/right
        // modifiers {0=middle, 1=up/right, -1=down/left}
        {1, 0}, {0, 1}, {-1, 0}, {0, -1}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

Region::Region(Region::RegionID regionId, CustomSDLRect rect,
               BackgroundSDLTexture* background) {
  this->regionID = regionId;
  this->rect = rect;
  this->background = background;
}
Region::~Region() { delete this->background; }
CustomSDLRect Region::getRect() { return this->rect; }

CustomSDLRect Region::cropRectInside(CustomSDLRect rect) {
  CustomSDLRect referenceDestinationRect =
      CustomSDLRect(SDL_Rect({0, 0, rect.w, rect.h}));

  CustomSDLRect thisDestinationRect =
      CustomSDLRect(SDL_Rect({this->rect.x - rect.x, this->rect.y - rect.y,
                              this->rect.w, this->rect.h}));

  CustomSDLRect destinationRect;
  if (SDL_IntersectRect(&thisDestinationRect, &referenceDestinationRect,
                        &destinationRect)) {
    return destinationRect;
  } else {  // throw exception
    return CustomSDLRect(SDL_Rect());
  }
}
CustomSDLRect Region::getSrcRect(CustomSDLRect referenceRect) {
  CustomSDLRect srcRect;
  SDL_IntersectRect(&this->rect, &referenceRect, &srcRect);

  srcRect.x -= this->rect.x;
  srcRect.y -= this->rect.y;

  return srcRect;
}
BackgroundSDLTexture* Region::getBackground() { return this->background; }
Region::RegionID Region::getRegionId() { return this->regionID; }

Region* Stage::getRegion(SDL_Point point) {
  Region::RegionID regionId = Region::RegionID::valueFrom(
      Region::RegionID::getIdFrom(point.x, point.y));

  Region* region;
  try {
    region = this->regionsMatrix.getElement(regionId);
  } catch (ElementNotFountException&) {
    // load new region
    region = new Region(
        regionId,
        CustomSDLRect(
            SDL_Rect({regionId.id.first * Region::fixedRegionWidth,
                      regionId.id.second * Region::fixedRegionHeight,
                      Region::fixedRegionWidth, Region::fixedRegionHeight})),
        new BackgroundSDLTexture(this->default_dynamic_texture));

    this->regionsMatrix.addElement(
        regionId, [this, region](Region::RegionID regionId) { return region; });
  }

  std::for_each(
      regionId.neighbourRegionsIDs.begin(), regionId.neighbourRegionsIDs.end(),
      [this](std::pair<int, int> key) {
        Region::RegionID neighbourRegionID = Region::RegionID::valueFrom(key);

        if (!this->regionsMatrix.contains(neighbourRegionID)) {
          this->regionsMatrix.addElement(
              neighbourRegionID, [this](Region::RegionID neighbourRegionIDKey) {
                return new Region(
                    neighbourRegionIDKey,
                    CustomSDLRect(SDL_Rect({neighbourRegionIDKey.id.first *
                                                Region::fixedRegionWidth,
                                            neighbourRegionIDKey.id.second *
                                                Region::fixedRegionHeight,
                                            Region::fixedRegionWidth,
                                            Region::fixedRegionHeight})),
                    new BackgroundSDLTexture(this->default_dynamic_texture));
              });
        }
      });

  // TODO delete faraway regions

  return region;
}

Stage::~Stage() {}
Stage::Stage(Stage::StageId stageId, CustomSDLRect rect,
             SDL_Renderer* renderer) {
  this->rect = rect;
  this->id = stageId;
  this->default_dynamic_texture = IMG_LoadTextureTyped_RW(
      renderer,
      SDL_RWFromFile("C:/Users/lucas/git/SDL_playground/media/img/praia.jpg",
                     "r"),
      1, "jpeg");
}
void Stage::placeMaterialObject(CustomSDLMaterialObject* const materialObject) {
  this->materialObjects[Region::RegionID::getIdFrom(
                            materialObject->getDestination().x,
                            materialObject->getDestination().y)]
      .insert(materialObject);
}

void Stage::updateMaterialObject(CustomSDLMaterialObject* const materialObject,
                                 const std::pair<int, int> oldRegionId) {
  const std::pair<int, int> materialObjectNewRegionId =
      Region::RegionID::getIdFrom(materialObject->getDestination().x,
                                  materialObject->getDestination().y);
  if (materialObjectNewRegionId != oldRegionId) {
    this->materialObjects[oldRegionId].erase(materialObject);
    this->materialObjects[materialObjectNewRegionId].insert(materialObject);
  }
}

const std::vector<
    std::pair<CustomSDLMaterialObject* const, const Region::RegionID>>
Stage::getMaterialObjects(const std::vector<Region::RegionID> regionsIds) {
  std::vector<std::pair<CustomSDLMaterialObject* const, const Region::RegionID>>
      materialObjects;

  for (const auto regionId : regionsIds) {
    std::vector<
        std::pair<CustomSDLMaterialObject* const, const Region::RegionID>>
        materialObjectsTransform;

    std::transform(std::begin(this->materialObjects[regionId]),
                   std::end(this->materialObjects[regionId]),
                   std::begin(materialObjectsTransform),
                   [regionId](CustomSDLMaterialObject* const materialObject) {
                     return std::make_pair(materialObject, regionId);
                   });

    materialObjects.insert(std::end(materialObjects),
                           std::begin(materialObjectsTransform),
                           std::end(materialObjectsTransform));
  }

  return materialObjects;
}

CustomGroundPlane* Stage::createDefaultGround(SDL_Texture* static_texture) {
  CustomTextureManager* textureManager =
      new CustomTextureManager(static_texture);
  return new CustomGroundPlane(textureManager, SDL_Rect({20, 200, 200, 200}));
}

Stage::StageId Stage::getId() { return Stage::StageId(); }
Stage* Stage::getNextStage() { return this->nextStage; }
Stage* Stage::getPreviousStage() { return this->previousStage; }
