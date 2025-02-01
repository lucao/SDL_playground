#ifndef CUSTOM_PHYSICS_H
#define CUSTOM_PHYSICS_H

#include <SDL.h>
#include <box2d/box2d.h>

#include <CustomGameObjects.hpp>
#include <CustomGameUtils.hpp>
#include <unordered_map>

class CustomPhysicalObject {
 protected:
  b2BodyId bodyId;
  b2Polygon polygon;

  CustomPhysicalObject();
 public:
  CustomPhysicalObject(b2BodyDef bodyDef,
                       b2ShapeDef shapeDef,
                       b2Vec2 position, b2Polygon polygon, b2WorldId worldId);
  virtual ~CustomPhysicalObject();

  virtual void doPhysics(Uint64 startTick, Uint64 endTick) = 0;
  virtual void afterSimulation(Uint64 startTick, Uint64 endTick) = 0;

  b2BodyId getBodyId();
};

class CustomDynamicPhysicalObject : public CustomPhysicalObject {
 protected:
  std::list<Movement *> movementList;

 public:
  CustomDynamicPhysicalObject(b2BodyDef bodyDef,
                              b2ShapeDef shapeDef,
                              b2Vec2 position, b2Polygon polygon,
                              b2WorldId worldId);

  virtual ~CustomDynamicPhysicalObject();
  void addMovement(Movement *movement);
};

class Box2DPhysicsControl {
 private:
  b2WorldDef worldDef;
  b2WorldId worldId;

  std::unordered_map<GameObject, CustomPhysicalObject *> physicalObjects;

  float frameRate;

 public:
  Box2DPhysicsControl(float frameRate);
  virtual ~Box2DPhysicsControl();

  void changeFrameRate(float frameRate);

  b2WorldId getWorldId();

  virtual void doPhysics(
      std::unordered_map<GameObject, CustomPhysicalObject *> physicalObjects,
      Uint64 startTick, Uint64 endTick) noexcept;
};

#endif
