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

 private:
  BackgroundSDLObject* background;
  std::set<CustomSDLMaterialObject*> objectsOnRegion;
  CustomSDLRect* rect;
  std::map<Direction, std::shared_ptr<Region>> sideRegions;

 public:
  Region(std::set<CustomSDLMaterialObject*> objectsOnRegion,
         CustomSDLRect* rect, BackgroundSDLObject* background);
  virtual ~Region();
  void addObjectToRegion(CustomSDLMaterialObject* object);
  void removeObjectFromRegion(CustomSDLMaterialObject* object);
  SDL_Rect* getRect();
  std::set<CustomSDLMaterialObject*> getObjectsOnRegion();
  BackgroundSDLObject* getBackground();
  void loadRegion(Direction direction, SDL_Renderer* renderer);
};

class Stage {
 private:
  std::string id;  // ainda não estou usando isso
  CustomSDLRect* rect;
  std::unordered_set<Region*> regionsOnStage;
  std::shared_ptr<Stage> nextStage;
  std::shared_ptr<Stage> previousStage;

 public:
  Stage(std::unordered_set<Region*> regionsOnStage, CustomSDLRect* rect);
  ~Stage();
  std::unordered_set<Region*> getRegionsOnStage();
};

class DynamicRegion : public Region {
 public:
  DynamicRegion(std::set<CustomSDLMaterialObject*> objectsOnRegion,
                CustomSDLRect* rect, SDL_Renderer* renderer);
  ~DynamicRegion();
};
#endif