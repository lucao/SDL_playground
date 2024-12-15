#ifndef STAGE_H
#define STAGE_H

#include <SDL.h>
#include <CustomGameComponents.hpp>
#include <CustomGameObjects.hpp>
#include <CustomPhysics.hpp>
#include <CustomSDLObject.hpp>
#include <LazyLoadMatrix.hpp>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <valarray>
#include <vector>

class Region {
 public:
  struct RegionID {
    static const std::valarray<std::valarray<int>> peripheralRegionsModifiers;

    std::pair<int, int> id;
    std::vector<std::pair<int, int>> neighbourRegionsIDs;

    static RegionID valueFrom(std::pair<int, int> pair) {
      std::valarray<int> pair_asvalarray{pair.first, pair.second};
      std::valarray<std::valarray<int>> neighbourIDs_asvalarray =
          pair_asvalarray + Region::RegionID::peripheralRegionsModifiers;

      std::vector<std::pair<int, int>> neighbourRegionsIDs;
      std::for_each(std::begin(neighbourIDs_asvalarray),
                    std::end(neighbourIDs_asvalarray),
                    [&neighbourRegionsIDs](const std::valarray<int> n) {
                      neighbourRegionsIDs.push_back({n[0], n[1]});
                    });

      return RegionID{pair, neighbourRegionsIDs};
    }

    static constexpr std::pair<int, int> getIdFrom(int x, int y) {
      return std::make_pair<int, int>(
          x > 0 ? x / Region::fixedRegionWidth
                : (x - Region::fixedRegionWidth) / Region::fixedRegionWidth,
          y > 0 ? y / Region::fixedRegionHeight
                : (y - Region::fixedRegionHeight) / Region::fixedRegionHeight);
    }
  };

  struct RegionID_key_hash {
    std::size_t operator()(const Region::RegionID& k) const {
      return k.id.first ^ k.id.second;
    }
  };
  struct RegionID_key_equal {
    bool operator()(const Region::RegionID& v0,
                    const Region::RegionID& v1) const {
      return ((v0.id.first == v1.id.first) && v0.id.second == v1.id.second);
    }
  };

 private:
  RegionID regionID;

  BackgroundSDLTexture* background;
  std::unordered_set<CustomSDLMaterialObject*> objectsOnRegion;
  CustomSDLRect rect;

 public:
  static const int fixedRegionWidth = 1920;
  static const int fixedRegionHeight = 1080;

 public:
  Region(RegionID regionID,
         std::unordered_set<CustomSDLMaterialObject*> objectsOnRegion,
         CustomSDLRect rect, BackgroundSDLTexture* background);
  virtual ~Region();
  void addObjectToRegion(CustomSDLMaterialObject* object);
  void removeObjectFromRegion(CustomSDLMaterialObject* object);
  RegionID getRegionId();
  CustomSDLRect getRect();
  BackgroundSDLTexture* getBackground();
  CustomSDLRect getSrcRect(CustomSDLRect referenceRect);

  CustomSDLRect cropRectInside(CustomSDLRect referenceRect);
};

class BlankRegion : public Region {
 public:
  BlankRegion(Region::RegionID regionId,
              std::unordered_set<CustomSDLMaterialObject*> objectsOnRegion,
              CustomSDLRect rect, BackgroundSDLTexture* background)
      : Region(regionId, objectsOnRegion, rect, background){};
  virtual ~BlankRegion(){};
};

class Stage {
 public:
  struct StageId {
    int id;

    bool operator==(const StageId& other) const { return id == other.id; }
    std::size_t operator()(const Stage::StageId& s) const { return s.id; }
  };

   // Custom hash function for strings
  struct StageObject_key_hash {
    std::size_t operator()(const GlobalPositionalSDLObject*& s) const {
      auto regionId =
          Region::RegionID::getIdFrom(s->destination.x, s->destination.y);
      return regionId.first ^ regionId.second;
    }
  };

  // Custom equality function for strings (case-insensitive comparison)
  struct StageObject_key_equal {
    bool operator()(const GlobalPositionalSDLObject*& a,
                    const GlobalPositionalSDLObject*& b) const {
      return a->destination.x == b->destination.x &&
             a->destination.y == b->destination.y;
    }
  };

 private:
  Stage::StageId id;
  CustomSDLRect rect;
  Stage* nextStage;
  Stage* previousStage;

  //TODO verificar melhor forma de armazenar os gameObjects
  //TODO usar std::variant
  std::unordered_multiset<CustomSDLMaterialObject*, StageObject_key_hash,
                          StageObject_key_equal>
      materialObjects;

  //std::unordered_map<id_game_object, PhysicalObjects>
  //std::unordered_map<id_game_object, MaterialObjects>

  SDL_Texture* default_dynamic_texture;

  LazyLoadMatrix<Region::RegionID, Region*, Region::RegionID_key_hash,
                 Region::RegionID_key_equal>
      regionsMatrix;

 public:
  Stage(Stage::StageId stageId, CustomSDLRect rect, SDL_Renderer* renderer);
  ~Stage();

  Stage::StageId getId();
  Stage* getNextStage();
  Stage* getPreviousStage();
  Region* getRegion(SDL_Point point);
  void placeGameObject(GameObject* gameObject);
  std::vector<CustomSDLMaterialObject*> getMaterialObjects();
  CustomGroundPlane* createDefaultGround(SDL_Texture* static_texture);
};

class StageOutOfBounds : public std::exception {
 public:
  char* what() {
    return const_cast<char*>(
        "Game API Error, Tried to access point outside the current stage");
  }
};

class StagesLoadError : public std::exception {
 public:
  char* what() {
    return const_cast<char*>("Game API Error, Stages not properly loaded");
  }
};

class RegionLoadError : public std::exception {
 private:
  std::pair<int, int> regionKey;

 public:
  RegionLoadError(std::pair<int, int> regionKey) {
    this->regionKey = regionKey;
  };
  std::pair<int, int> getRegionKey() { return regionKey; };
  char* what() {
    return const_cast<char*>("Game API Error, Region not properly loaded");
  }
};

class RegionNotLoaded : public RegionLoadError {
 public:
  RegionNotLoaded(std::pair<int, int> regionKey) : RegionLoadError(regionKey){};
  char* what() {
    return const_cast<char*>("Game API Error, Region not loaded");
  }
};
class RegionNotLoadedYet : public RegionLoadError {
 public:
  RegionNotLoadedYet(std::pair<int, int> regionKey)
      : RegionLoadError(regionKey){};
  char* what() {
    return const_cast<char*>("Game API Error, Region load is not finished");
  }
};

class DynamicRegion : public Region {
 public:
  static SDL_RWops* DEFAULT_TEXTURE_RWOPS;

  DynamicRegion(Region::RegionID regionId,
                std::unordered_set<CustomSDLMaterialObject*> objectsOnRegion,
                CustomSDLRect rect, SDL_Renderer* renderer,
                SDL_Texture* texture);
  ~DynamicRegion();
};
#endif
