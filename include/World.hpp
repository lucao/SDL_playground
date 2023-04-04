#ifndef WORLD_H
#define WORLD_H

#include <Stage.hpp>
#include <unordered_set>

class World {
 private:
  std::unordered_set<Stage*> stages;
  Stage* activeStage;

 public:
  World();
  ~World();
  void loadStage(Stage* stage);
  Stage* getActiveStage();
};

#endif