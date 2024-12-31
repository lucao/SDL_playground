#include <CustomPhysics.hpp>
#include <unordered_map>

CustomPhysicalObject::CustomPhysicalObject(
    CollisionMasks collisionMask, CollisionGroup collisionGroup,
    btCollisionShape* shape, btScalar mass,
    btDefaultMotionState* defaultMotionState) {
  this->collisionGroup = collisionGroup;
  this->collisionMask = collisionMask;

  this->shape = shape;
  this->motionState = defaultMotionState;
  btRigidBody::btRigidBodyConstructionInfo collisionInfo(
      mass, this->motionState, this->shape, btVector3(0, 0, 0));
  this->rigidBody = new btRigidBody(collisionInfo);
}

CustomPhysicalObject::~CustomPhysicalObject() {
  delete this->shape;
  delete this->motionState;
  delete this->rigidBody;
}

CollisionMasks CustomPhysicalObject::getCollisionMask() {
  return this->collisionMask;
}

CollisionGroup CustomPhysicalObject::getCollisionGroup() {
  return this->collisionGroup;
}

btRigidBody* CustomPhysicalObject::getRigidBody() { return this->rigidBody; }

btTransform CustomPhysicalObject::getTransform() {
  btTransform transform;
  this->rigidBody->getMotionState()->getWorldTransform(transform);
  return transform;
}

CustomDynamicPhysicalObject::CustomDynamicPhysicalObject(
    btCollisionShape* shape, btScalar mass,
    btDefaultMotionState* defaultMotionState)
    : CustomPhysicalObject(CollisionMasks::DYNAMIC_OBJECT,
                           CollisionGroup::DYNAMIC_OBJECTS, shape, mass,
                           defaultMotionState) {
  // TODO
}

CustomDynamicPhysicalObject::~CustomDynamicPhysicalObject() {}

void CustomDynamicPhysicalObject::addMovement(Movement* movement) {
  // TODO verificar tamanho máximo da lista e lógica de inserção de movements
  this->movementList.push_back(movement);
}

PhysicsControl::PhysicsControl() {
  // Set up the broadphase
  this->broadphase = new btDbvtBroadphase();

  // Set up the collision configuration
  this->collisionConfiguration = new btDefaultCollisionConfiguration();

  // Set up the dispatcher
  this->dispatcher = new btCollisionDispatcher(collisionConfiguration);

  // Set up the solver
  this->solver = new btSequentialImpulseConstraintSolver();

  // Set up the dynamics world
  this->dynamicsWorld = new btDiscreteDynamicsWorld(
      dispatcher, broadphase, solver, collisionConfiguration);

  // Set the gravity in the world
  this->dynamicsWorld->setGravity(btVector3(0, 10, 0));
}

PhysicsControl::~PhysicsControl() {
  delete this->broadphase;
  delete this->collisionConfiguration;
  delete this->dispatcher;
  delete this->solver;
  delete this->dynamicsWorld;
}

void PhysicsControl::doPhysics(
    std::unordered_map<GameObject, CustomPhysicalObject*> objects,
    Uint64 startTick, Uint64 endTick) noexcept {
  std::unordered_map<GameObject, CustomPhysicalObject*>
      allPhysicalObjects;
  allPhysicalObjects.insert(objects.begin(), objects.end());
  allPhysicalObjects.insert(this->physicalObjects.begin(),
                            this->physicalObjects.end());
  for (auto it = allPhysicalObjects.begin(); it != allPhysicalObjects.end();
       ++it) {
    auto pair = *it;
    GameObject gameObject = pair.first;
    CustomPhysicalObject* physicalObject = pair.second;

    if (this->physicalObjects.find(gameObject) != this->physicalObjects.end()) {
      if (objects.find(gameObject) == objects.end()) {
        this->physicalObjects.erase(gameObject);
        dynamicsWorld->removeRigidBody(physicalObject->getRigidBody());
      }
    } else {
      if (objects.find(gameObject) != objects.end()) {
        this->physicalObjects[gameObject] = physicalObject;
        dynamicsWorld->addRigidBody(physicalObject->getRigidBody(),
                                    physicalObject->getCollisionGroup(),
                                    physicalObject->getCollisionMask());
      } else {
        // ERROR
      }
    }
  }

  for (auto pair : this->physicalObjects) {
    pair.second->doPhysics(startTick, endTick);
  }
  this->dynamicsWorld->stepSimulation(btScalar(endTick - startTick / 1000.0),
                                      1);
}

// TODO
/*
std::vector<std::pair<CustomPhysicalObject*, CustomPhysicalObject*>>
PhysicsControl::getCollisions(
    std::vector<CustomPhysicalObject*> objects) noexcept {

  for (CustomPhysicalObject object : objects) {
    this->addPhysicalObject(object);
  }


  this->dynamicsWorld->stepSimulation(1 / 60.f, 10);

  this->dynamicsWorld->performDiscreteCollisionDetection();

  int numManifolds = this->dynamicsWorld->getDispatcher()->getNumManifolds();

  for (int i = 0; i < numManifolds; i++) {
    btPersistentManifold* contactManifold =
        this->dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);

    const btCollisionObject* objectA = contactManifold->getBody0();
    const btCollisionObject* objectB = contactManifold->getBody1();

    int numContacts = contactManifold->getNumContacts();

    if (numContacts > 0) {
      // has hit
    }
  }

}*/

// void PhysicsControl::addPhysicalObject(CustomPhysicalObject* object) {
//  TODO add physical object by it's type (rigid body, collision object or
//  character)
//  return;
//}
