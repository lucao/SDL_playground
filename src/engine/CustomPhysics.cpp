#include <CustomPhysics.hpp>

std::atomic_int CustomPhysicalObject::_id_counter{0};
CustomPhysicalObject::CustomPhysicalObject(
    CollisionMasks collisionMask, CollisionGroup collisionGroup,
    btCollisionShape* shape, btScalar mass,
    btDefaultMotionState* defaultMotionState) {
  this->id = CustomPhysicalObject::_id_counter.fetch_add(1);

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

int CustomPhysicalObject::getID() const { return this->id; }

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
    std::unordered_set<CustomPhysicalObject*,
                       PhysicsControl::CustomPhysicalObjectHash,
                       PhysicsControl::CustomPhysicalObjectEqual>
        objects,
    Uint64 startTick, Uint64 endTick) noexcept {
  std::unordered_set<CustomPhysicalObject*,
                     PhysicsControl::CustomPhysicalObjectHash,
                     PhysicsControl::CustomPhysicalObjectEqual>
      allPhysicalObjects;
  allPhysicalObjects.insert(objects.begin(), objects.end());
  allPhysicalObjects.insert(this->physicalObjects.begin(),
                            this->physicalObjects.end());
  for (auto it = allPhysicalObjects.begin(); it != allPhysicalObjects.end(); ++it) {
    CustomPhysicalObject* object = *it;
    if (this->physicalObjects.find(object) != this->physicalObjects.end()) {
      if (objects.find(object) == objects.end()) {
        this->physicalObjects.erase(object);
        dynamicsWorld->removeRigidBody(object->getRigidBody());
      }
    } else {
      if (objects.find(object) != objects.end()) {
        this->physicalObjects.insert(object);
        dynamicsWorld->addRigidBody(object->getRigidBody(),
                                    object->getCollisionGroup(),
                                    object->getCollisionMask());
      } else {
        // ERROR
      }
    }
  }

  for (auto object : this->physicalObjects) {
    object->doPhysics(startTick, endTick);
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
