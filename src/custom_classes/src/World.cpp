#include <Stage.hpp>
#include <World.hpp>

World::World() {}
World::~World() {
  for (Stage* stage : this->stages) {
    delete stage;
  }
}
Stage* World::getActiveStage() { return this->activeStage; }
void World::loadStage(Stage* stage) { this->stages.insert(stage); }