#ifndef STAGE_H
#define STAGE_H

#include <SDL.h>

#include <CustomSDLObject.hpp>
#include <unordered_set>

class Region {
 private:
  BackgroundSDLObject background;
  std::unordered_set<CustomSDLMaterialObject*> objectsOnRegion;
  SDL_Rect rect;

  Region* top;
  Region* bottom;
  Region* left;
  Region* right;
  Region* topLeft;
  Region* topRight;
  Region* bottomLeft;
  Region* bottomRight;

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
  Region(std::unordered_set<CustomSDLMaterialObject*> objectsOnRegion,
         SDL_Rect rect, BackgroundSDLObject* background);
  void addObjectToRegion(CustomSDLMaterialObject* object);
  void removeObjectFromRegion(CustomSDLMaterialObject* object);
  SDL_Rect getRect();
  std::unordered_set<CustomSDLMaterialObject*> getObjectsOnRegion();
  BackgroundSDLObject getBackground();
  void loadRegion(Direction direction);
};

class DynamicRegion : public Region {
 public:
  DynamicRegion(std::unordered_set<CustomSDLMaterialObject*> objectsOnRegion,
                SDL_Rect rect);
};

class Stage {
 private:
  SDL_Rect rect;
  std::unordered_set<Region*> regionsOnStage;
  Stage* nextStage;
  Stage* previousStage;

 public:
  Stage(std::unordered_set<Region*> regionsOnStage, SDL_Rect rect);
  std::unordered_set<Region*> getRegionsOnStage();
};

class World {
 private:
  std::unordered_set<Stage*> stages;

 public:
  World();
  void loadStage(Stage* stage);
};

#endif