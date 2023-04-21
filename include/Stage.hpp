#ifndef STAGE_H
#define STAGE_H

#include <SDL.h>

#include <CustomSDLObject.hpp>
#include <memory>
#include <string>
#include <tuple>
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

  typedef std::unordered_map<Direction, std::tuple<int, int>>
      DirectionMatrixMap;
  static DirectionMatrixMap directionMatrixMap;

  static const int fixedRegionWidth = 1920;
  static const int fixedRegionHeight = 1080;

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
  std::shared_ptr<Region> getSideRegion(Region::Direction direction);
  void setSideRegion(Region::Direction direction,
                     std::shared_ptr<Region> region);
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

  typedef std::tuple<int, int> CustomMatrixKey;
  struct RegionMatrix_key_hash {
    std::size_t operator()(const CustomMatrixKey& k) const {
      return std::get<0>(k) ^ std::get<1>(k);
    }
  };
  struct RegionMatrix_key_equal {
    bool operator()(const CustomMatrixKey& v0,
                    const CustomMatrixKey& v1) const {
      return (std::get<0>(v0) == std::get<0>(v1) &&
              std::get<1>(v0) == std::get<1>(v1));
    }
  };
  typedef std::unordered_map<CustomMatrixKey, std::shared_ptr<Region>,
                             RegionMatrix_key_hash, RegionMatrix_key_equal>
      CustomMatrix;
  CustomMatrix regionsMatrix;

 public:
  Stage(CustomSDLRect* rect);
  ~Stage();
  std::shared_ptr<Stage> getNextStage();
  std::shared_ptr<Stage> getPreviousStage();
  std::shared_ptr<Region> getActiveRegion(SDL_Point* cameraCenter,
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