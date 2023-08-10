#ifndef STAGE_H
#define STAGE_H

#include <SDL.h>

#include <CustomSDLObject.hpp>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <future>
#include<Eigen/Dense>


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

  std::unordered_map<Region::Direction,  std::future<Region*>> sideRegions;

 public:
  Region(std::unordered_set<CustomSDLMaterialObject*> objectsOnRegion,
         CustomSDLRect* rect, BackgroundSDLTexture* background);
  virtual ~Region();
  void addObjectToRegion(CustomSDLMaterialObject* object);
  void removeObjectFromRegion(CustomSDLMaterialObject* object);
  CustomSDLRect* getRect();
  CustomSDLRect* getDestinationRect(
      CustomSDLRect* referenceRect);
  Region* getSideRegion(Region::Direction direction);
  BackgroundSDLTexture* getBackground();
  CustomSDLRect* getSrcRect(CustomSDLRect* referenceRect);
};

class Stage {
 private:
  std::string id;  // ainda não estou usando isso
  CustomSDLRect* rect;
  Stage* nextStage;
  Stage* previousStage;

  Region* activeRegion;

  SDL_Texture* default_dynamic_texture;

  SDL_Renderer* renderer;

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
  typedef std::unordered_map<CustomMatrixKey, std::future<Region*>,
                             RegionMatrix_key_hash, RegionMatrix_key_equal>
      CustomMatrix;
  CustomMatrix regionsMatrix;

 public:
  Stage(CustomSDLRect* rect, SDL_Renderer* renderer);
  ~Stage();
  Stage* getNextStage();
  Stage* getPreviousStage();
  Region* getActiveRegion(SDL_Point* cameraCenter);
  std::vector<CustomSDLMaterialObject*> getMaterialObjectsNear(
      GlobalPositionalSDLObject* object);
  SDL_Renderer* getRenderer();
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
 static SDL_RWops* DEFAULT_TEXTURE_RWOPS;

  DynamicRegion(std::unordered_set<CustomSDLMaterialObject*> objectsOnRegion,
                CustomSDLRect* rect, SDL_Renderer* renderer, SDL_Texture* texture);
  ~DynamicRegion();
};
#endif