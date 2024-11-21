#ifndef CUSTOM_PHYSICS_H
#define CUSTOM_PHYSICS_H

#include <SDL.h>
#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

#include <CustomGameObjects.hpp>
#include <CustomGameUtils.hpp>
#include <atomic>
#include <unordered_set>

//TODO adjust masks
enum CollisionMasks { STATIC_OBJECT = 1 << 1, DYNAMIC_OBJECT = 1 << 0 };
enum CollisionGroup { STATIC_OBJECTS = 1 << 0, DYNAMIC_OBJECTS = 1 << 1 };

// TODO enum of shapes

class CustomPhysicalObject {
 private:
  static std::atomic_int _id_counter;
  int id;

 protected:
  btCollisionShape *shape;
  btDefaultMotionState *motionState;
  btRigidBody *rigidBody;

  btScalar mass;

  CollisionMasks collisionMask;
  CollisionGroup collisionGroup;

  btTransform getTransform();

 public:
  CustomPhysicalObject(CollisionMasks collisionMask,
                       CollisionGroup collisionGroup, btCollisionShape *shape,
                       btScalar mass, btDefaultMotionState *defaultMotionState);
  virtual ~CustomPhysicalObject();

  int getID() const;

  CollisionMasks getCollisionMask();
  CollisionGroup getCollisionGroup();

  btRigidBody *getRigidBody();

  virtual void doPhysics(Uint64 startTick, Uint64 endTick) = 0;
};

class CustomDynamicPhysicalObject : public CustomPhysicalObject {
 protected:
  std::list<Movement *> movementList;

 public:
  CustomDynamicPhysicalObject(btCollisionShape *shape, btScalar mass,
                              btDefaultMotionState *defaultMotionState);

  virtual ~CustomDynamicPhysicalObject();
  void addMovement(Movement *movement);
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

 public:
  struct CustomPhysicalObjectHash {
    std::size_t operator()(const CustomPhysicalObject *physicalObject) const {
      return std::hash<int>()(physicalObject->getID());
    }
  };

  struct CustomPhysicalObjectEqual {
    bool operator()(const CustomPhysicalObject *physicalObject1,
                    const CustomPhysicalObject *physicalObject2) const {
      return physicalObject1->getID() == physicalObject2->getID();
    }
  };

 private:
  std::unordered_set<CustomPhysicalObject *, CustomPhysicalObjectHash,
                     CustomPhysicalObjectEqual>
      physicalObjects;

 public:
  PhysicsControl();
  virtual ~PhysicsControl();

  void doPhysics(
      std::unordered_set<CustomPhysicalObject *, CustomPhysicalObjectHash,
                         CustomPhysicalObjectEqual>
          physicalObjects,
      Uint64 startTick, Uint64 endTick) noexcept;
};

#endif
