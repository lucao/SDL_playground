#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <CustomSDLObject.hpp>
#include <Stage.hpp>

class EntityManager {
 private:
  const int MAX_ENTITIES = 5000;

 public:
  EntityManager();
  ~EntityManager();

  auto getObjectsNear(GlobalPositionalSDLObject* object);

  auto getFixedPositionObjectsFrom(Stage* stage);
  auto getFixedPositionObjectsFrom(Region* stage);
  auto getMovablePositionObjectsFrom(Stage* stage);
  auto getMovablePositionObjectsFrom(Region* stage);
}

#endif