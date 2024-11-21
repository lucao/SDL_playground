#ifndef WORLD_H
#define WORLD_H

#include <CustomGameCharacters.hpp>
#include <CustomGameComponents.hpp>
#include <Stage.hpp>
#include <unordered_map>

class World {
 private:
  CustomPlayer* mainPlayer;
  CustomGroundPlane* ground;

  std::vector<CustomSDLMaterialObject*> materialObjects;
  PhysicsControl* physicsControl;

  std::unordered_map<Stage::StageId, Stage*, Stage::StageId> stages;

 public:
  World(SDL_Renderer* const renderer);
  ~World();

  Stage* createBlankStage(SDL_Renderer* renderer);
  Stage* getStage(Stage::StageId id);
  std::vector<CustomSDLMaterialObject*> getMaterialObjects();
  CustomPlayer* createLocalPlayer(
      std::unordered_map<ANIMATION_TYPE, SDL_Texture*> textures,
      std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>>
          animationSprites);
  CustomGroundPlane* createDefaultGround(SDL_Texture* static_texture);
};

#endif