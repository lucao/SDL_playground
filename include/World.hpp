#ifndef WORLD_H
#define WORLD_H

#include <CustomGameCharacters.hpp>

#include <Stage.hpp>
#include <unordered_map>

class World {
 private:
  CustomPlayer* mainPlayer;
  //TODO physical Objects
  Box2DPhysicsControl* physicsControl;

  std::unordered_map<Stage::StageId, Stage*, Stage::StageId> stages;

 public:
  World(SDL_Renderer* const renderer);
  ~World();

  Stage* createBlankStage(SDL_Renderer* renderer);
  Stage* getStage(Stage::StageId id);
  
};

#endif