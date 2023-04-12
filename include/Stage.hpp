#ifndef STAGE_H
#define STAGE_H

#include <SDL.h>

#include <CustomSDLObject.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

class Stage;
class Region {
 private:
  BackgroundSDLTexture* background;
  std::unordered_set<CustomSDLMaterialObject*> objectsOnRegion;
  CustomSDLRect* rect;

 public:
  enum Direction {
    TOP,
    BOTTOM,
    LEFT,
    RIGHT,
    TOPLEFT,
    TOPRIGHT,
    BOTTOMLEFT,
    BOTTOMRIGHT
  };
  static constexpr Direction directions[] = {
      TOP, BOTTOM, LEFT, RIGHT, TOPLEFT, TOPRIGHT, BOTTOMLEFT, BOTTOMRIGHT};

 private:
  std::unordered_map<Region::Direction, std::shared_ptr<Region>> sideRegions;

 public:
  Region(std::unordered_set<CustomSDLMaterialObject*> objectsOnRegion,
         CustomSDLRect* rect, BackgroundSDLTexture* background);
  virtual ~Region();
  void addObjectToRegion(CustomSDLMaterialObject* object);
  void removeObjectFromRegion(CustomSDLMaterialObject* object);
  std::shared_ptr<CustomSDLRect> getRect();
  std::shared_ptr<CustomSDLRect> getDestinationRect(
      CustomSDLRect* referenceRect);
  std::unordered_set<CustomSDLMaterialObject*> getObjectsOnRegion();
  std::shared_ptr<Region> loadRegion(Stage* stage, Region::Direction direction,
                                     SDL_Renderer* renderer);
  std::shared_ptr<Region> getSideRegion(Region::Direction direction);
  BackgroundSDLTexture* getBackground();
  std::shared_ptr<CustomSDLRect> getSrcRect(CustomSDLRect* referenceRect);
};

class Stage {
 private:
  std::string id;  // ainda não estou usando isso
  CustomSDLRect* rect;
  std::shared_ptr<Stage> nextStage;
  std::shared_ptr<Stage> previousStage;

  std::shared_ptr<Region> activeRegion;

  std::unordered_set<std::shared_ptr<Region>> regionsOnStage;

 public:
  Stage(std::unordered_set<std::shared_ptr<Region>> regionsOnStage,
        CustomSDLRect* rect);
  ~Stage();
  std::shared_ptr<Stage> getNextStage();
  std::shared_ptr<Stage> getPreviousStage();
  std::unordered_set<std::shared_ptr<Region>> getRegionsOnStage();
  std::shared_ptr<Region> getActiveRegion();
  void setActiveRegion(std::shared_ptr<Region> region);
  std::shared_ptr<Region> getRegionFromPoint(std::shared_ptr<SDL_Point> point,
                                             SDL_Renderer* renderer);
};

class StageOutOfBounds : public std::exception {
 public:
  char* what() {
    return const_cast<char*>("Tried to access point outside the current stage");
  }
};

class StagesLoadError : public std::exception {
 public:
  char* what() { return const_cast<char*>("Stages not properly loaded"); }
};

class DynamicRegion : public Region {
 public:
  DynamicRegion(std::unordered_set<CustomSDLMaterialObject*> objectsOnRegion,
                CustomSDLRect* rect, SDL_Renderer* renderer);
  ~DynamicRegion();
};
#endif