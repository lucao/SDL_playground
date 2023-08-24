#ifndef STAGE_H
#define STAGE_H

#include <SDL.h>

#include <CustomSDLObject.hpp>
#include <CustomUtils.hpp>
#include <future>
#include <memory>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <valarray>
#include<vector>

class Stage;

class Region {
 public:
  struct RegionID {
    static const std::valarray<std::valarray<int>> peripheralRegionsModifiers;
    
    std::tuple<int, int> id;
    std::vector<std::tuple<int, int>> neighbourRegionsIDs;

    static RegionID valueFrom(std::tuple<int, int> tuple) {
      std::valarray<int> tuple_asvalarray {std::get<0>(tuple), std::get<1>(tuple)};
      std::valarray<std::valarray<int>> neighbourIDs_asvalarray = tuple_asvalarray + Region::RegionID::peripheralRegionsModifiers;

      std::vector<std::tuple<int, int>> neighbourRegionsIDs;
      std::for_each(std::begin(neighbourIDs_asvalarray), std::end(neighbourIDs_asvalarray), [&neighbourRegionsIDs] (const std::valarray<int> n) {
        neighbourRegionsIDs.push_back({n[0], n[1]});
      });

      return RegionID{tuple, neighbourRegionsIDs};
    }
  };

  struct RegionID_key_hash {
    std::size_t operator()(const Region::RegionID &k) const {
      return std::get<0>(k.id) ^ std::get<1>(k.id);
    }
  };
  struct RegionID_key_equal {
    bool operator()(const Region::RegionID &v0,
                    const Region::RegionID &v1) const {
      return (std::get<0>(v0.id) == std::get<0>(v1.id) &&
              std::get<1>(v0.id) == std::get<1>(v1.id));
    }
  };

 private:
  RegionID regionID;

  BackgroundSDLTexture *background;
  std::unordered_set<CustomSDLMaterialObject *> objectsOnRegion;
  CustomSDLRect *rect;

 public:
  static const int fixedRegionWidth = 1920;
  static const int fixedRegionHeight = 1080;

 public:
  Region(RegionID regionID, std::unordered_set<CustomSDLMaterialObject *> objectsOnRegion,
         CustomSDLRect *rect, BackgroundSDLTexture *background);
  virtual ~Region();
  void addObjectToRegion(CustomSDLMaterialObject *object);
  void removeObjectFromRegion(CustomSDLMaterialObject *object);
  RegionID getRegionId();
  CustomSDLRect *getRect();
  CustomSDLRect *getDestinationRect(CustomSDLRect *referenceRect);
  BackgroundSDLTexture *getBackground();
  CustomSDLRect *getSrcRect(CustomSDLRect *referenceRect);
};

class BlankRegion : public Region {
 public:
  BlankRegion(Region::RegionID regionId, std::unordered_set<CustomSDLMaterialObject *> objectsOnRegion,
              CustomSDLRect *rect, BackgroundSDLTexture *background)
      : Region(regionId, objectsOnRegion, rect, background){};
  virtual ~BlankRegion(){};
};

class Stage {
 private:
  std::string id;  // ainda não estou usando isso
  CustomSDLRect *rect;
  Stage *nextStage;
  Stage *previousStage;

  SDL_Texture *default_dynamic_texture;

  SDL_Renderer *renderer;

  LazyLoadMatrix<Region::RegionID, Region*,
                             Region::RegionID_key_hash, Region::RegionID_key_equal> regionsMatrix;    
  LazyLoadMatrix<Region::RegionID, Region*,
                             Region::RegionID_key_hash, Region::RegionID_key_equal> blankRegionsMatrix;             

 public:
  Stage(CustomSDLRect *rect, SDL_Renderer *renderer);
  ~Stage();
  Stage *getNextStage();
  Stage *getPreviousStage();
  Region *getRegion(SDL_Point point);
  std::vector<CustomSDLMaterialObject *> getMaterialObjectsNear(
      GlobalPositionalSDLObject *object);
  SDL_Renderer *getRenderer();
};

class StageOutOfBounds : public std::exception {
 public:
  char *what() {
    return const_cast<char *>(
        "Game API Error, Tried to access point outside the current stage");
  }
};

class StagesLoadError : public std::exception {
 public:
  char *what() {
    return const_cast<char *>("Game API Error, Stages not properly loaded");
  }
};

class RegionLoadError : public std::exception {
 private:
  std::tuple<int, int> regionKey;

 public:
  RegionLoadError(std::tuple<int, int> regionKey) {
    this->regionKey = regionKey;
  };
  std::tuple<int, int> getRegionKey() { return regionKey; };
  char *what() {
    return const_cast<char *>("Game API Error, Region not properly loaded");
  }
};

class RegionNotLoaded : public RegionLoadError {
 public:
  RegionNotLoaded(std::tuple<int, int> regionKey)
      : RegionLoadError(regionKey){};
  char *what() {
    return const_cast<char *>("Game API Error, Region not loaded");
  }
};
class RegionNotLoadedYet : public RegionLoadError {
 public:
  RegionNotLoadedYet(std::tuple<int, int> regionKey)
      : RegionLoadError(regionKey){};
  char *what() {
    return const_cast<char *>("Game API Error, Region load is not finished");
  }
};

class DynamicRegion : public Region {
 public:
  static SDL_RWops *DEFAULT_TEXTURE_RWOPS;

  DynamicRegion(Region::RegionID regionId, std::unordered_set<CustomSDLMaterialObject *> objectsOnRegion,
                CustomSDLRect *rect, SDL_Renderer *renderer,
                SDL_Texture *texture);
  ~DynamicRegion();
};
#endif