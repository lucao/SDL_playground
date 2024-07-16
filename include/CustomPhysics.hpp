#ifndef CUSTOM_PHYSICS_H
#define CUSTOM_PHYSICS_H

#include <SDL.h>

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <CustomGameObjects.hpp>
#include <CustomGameUtils.hpp>
#include <unordered_set>

enum CollisionMasks { PLAYER, CHARACTER };
enum CollisionFilters { PLAYERS, CHARACTERS };

class CustomPhysicalObject {
 protected:
  btCollisionObject *body;

  CollisionMasks collisionMask;
  CollisionFilters collisionFilter;

 public:
  CustomPhysicalObject(CollisionMasks collisionMask,
                       CollisionFilters collisionFilter,
                       btCollisionObject *body);
  virtual ~CustomPhysicalObject();

  btCollisionObject *getCollisionObject();
  CollisionMasks getCollisionMask();
  CollisionFilters getCollisionFilter();
};

class CustomDynamicPhysicalObject : public CustomPhysicalObject {
 private:
  std::unordered_set<Movement> movementList;

  btTransform transform;
  btDefaultMotionState *motionState;

 public:
  CustomDynamicPhysicalObject(CollisionMasks collisionMask,
                              CollisionFilters collisionFilter,
                              btCollisionObject *body);

  virtual ~CustomDynamicPhysicalObject();

  // void setTransform(btTransform transform);
  void move(Movement movement);
};

class PhysicsControl {
 private:
  // Set up the broadphase
  btBroadphaseInterface *broadphase;  // = new btDbvtBroadphase();

  // Set up the collision configuration
  btDefaultCollisionConfiguration
      *collisionConfiguration;  // = new btDefaultCollisionConfiguration();

  // Set up the dispatcher
  btCollisionDispatcher
      *dispatcher;  // = new btCollisionDispatcher(collisionConfiguration);

  // Set up the solver
  btSequentialImpulseConstraintSolver
      *solver;  // = new btSequentialImpulseConstraintSolver();

  // Set up the dynamics world
  btDiscreteDynamicsWorld
      *dynamicsWorld;  // = new btDiscreteDynamicsWorld(dispatcher, broadphase,
  // solver, collisionConfiguration);

  void addPhysicalObject(CustomPhysicalObject object);

 public:
  PhysicsControl();
  virtual ~PhysicsControl();

  void doPhysics(std::vector<CustomPhysicalObject> objects) noexcept;
  std::vector<std::pair<CustomPhysicalObject, CustomPhysicalObject>>
  getCollisions(std::vector<CustomPhysicalObject> objects) noexcept;
};

#endif
