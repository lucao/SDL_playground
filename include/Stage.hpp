#ifndef STAGE_H
#define STAGE_H

#include <SDL.h>

#include <CustomSDLObject.hpp>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_set>

class Region {
 private:
  BackgroundSDLObject* background;
  std::set<CustomSDLMaterialObject*> objectsOnRegion;
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
  std::map<Region::Direction, std::shared_ptr<Region>> sideRegions;

 public:
  Region(std::set<CustomSDLMaterialObject*> objectsOnRegion,
         CustomSDLRect* rect, BackgroundSDLObject* background);
  virtual ~Region();
  void addObjectToRegion(CustomSDLMaterialObject* object);
  void removeObjectFromRegion(CustomSDLMaterialObject* object);
  SDL_Rect* getRect();
  std::set<CustomSDLMaterialObject*> getObjectsOnRegion();
  std::shared_ptr<Region> loadRegion(Stage* stage, Region::Direction direction,
                                     SDL_Renderer* renderer);
  std::shared_ptr<CustomSDLRect> clipRegion(CustomSDLRect* cameraRect);
  BackgroundSDLObject* getBackground();
};

class Stage {
 private:
  std::string id;  // ainda não estou usando isso
  CustomSDLRect* rect;
  std::shared_ptr<Stage> nextStage;
  std::shared_ptr<Stage> previousStage;

  std::unordered_set<std::shared_ptr<Region>> regionsOnStage;

 public:
  Stage(std::unordered_set<std::shared_ptr<Region>> regionsOnStage,
        CustomSDLRect* rect);
  ~Stage();
  std::shared_ptr<Stage> getNextStage();
  std::shared_ptr<Stage> getPreviousStage();
  std::unordered_set<std::shared_ptr<Region>> getRegionsOnStage();
  std::shared_ptr<Region> getRegionFromPoint(std::shared_ptr<SDL_Point> point,
                                             SDL_Renderer* renderer);
};

class StageOutOfBounds : public std::exception {
 public:
  char* what() { return "Tried to access point outside the current stage"; }
};

class StagesLoadError : public std::exception {
 public:
  char* what() { return "Stages not properly loaded"; }
};

class DynamicRegion : public Region {
 public:
  DynamicRegion(std::set<CustomSDLMaterialObject*> objectsOnRegion,
                CustomSDLRect* rect, SDL_Renderer* renderer);
  ~DynamicRegion();
};
#endif