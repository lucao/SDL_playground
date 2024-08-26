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

//TODO enum of shapes


class CustomPhysicalObject {
 private:
  btCollisionShape * shape;
  btDefaultMotionState * motionState;
  btRigidBody *rigidBody;

  btScalar mass;

  CollisionMasks collisionMask;
  CollisionFilters collisionFilter;

 public:
  CustomPhysicalObject(CollisionMasks collisionMask,
                       CollisionFilters collisionFilter,
                       btCollisionShape *shape, btScalar mass);
  virtual ~CustomPhysicalObject();

  CollisionMasks getCollisionMask();
  CollisionFilters getCollisionFilter();
  virtual void doPhysics(Uint64 startTick, Uint64 endTick);
};

class CustomDynamicPhysicalObject : public CustomPhysicalObject {
 protected:
  std::list<Movement*> movementList;

 private:
  btTransform transform;

 public:
  CustomDynamicPhysicalObject(CollisionMasks collisionMask,
                              CollisionFilters collisionFilter);

  virtual ~CustomDynamicPhysicalObject();
  void move(Movement* movement);
};

class PhysicsControl {
 private:
  btBroadphaseInterface *broadphase;  // = new btDbvtBroadphase();
  btDefaultCollisionConfiguration
      *collisionConfiguration;  // = new btDefaultCollisionConfiguration();
  btCollisionDispatcher
      *dispatcher;  // = new btCollisionDispatcher(collisionConfiguration);
  btSequentialImpulseConstraintSolver
      *solver;  // = new btSequentialImpulseConstraintSolver();
  btDiscreteDynamicsWorld
      *dynamicsWorld;  // = new btDiscreteDynamicsWorld(dispatcher, broadphase,
  // solver, collisionConfiguration);

  void addPhysicalObject(CustomPhysicalObject *object);

 public:
  PhysicsControl();
  virtual ~PhysicsControl();

  void doPhysics(std::vector<CustomPhysicalObject *> physicalObjects,
                 Uint64 startTick, Uint64 endTick) noexcept;
};

#endif
