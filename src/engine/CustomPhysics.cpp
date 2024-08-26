#include <CustomPhysics.hpp>

CustomPhysicalObject::CustomPhysicalObject(CollisionMasks collisionMask,
                                           CollisionFilters collisionFilter,
                                           btCollisionShape* shape,
                                           btScalar mass) {
  this->collisionFilter = collisionFilter;
  this->collisionMask = collisionMask;

  this->shape = shape;
  this->motionState = new btDefaultMotionState(
      btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 10, 0)));
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

CollisionFilters CustomPhysicalObject::getCollisionFilter() {
  return this->collisionFilter;
}

void CustomPhysicalObject::doPhysics(Uint64 startTick, Uint64 endTick) {}

CustomDynamicPhysicalObject::CustomDynamicPhysicalObject(
    CollisionMasks collisionMask, CollisionFilters collisionFilter,
    btCollisionObject* body)
    : CustomPhysicalObject(collisionMask, collisionFilter, body) {
  // TODO
}

CustomDynamicPhysicalObject::~CustomDynamicPhysicalObject() {
  delete this->motionState;
}

void CustomDynamicPhysicalObject::move(Movement* movement) {
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
  this->dynamicsWorld->setGravity(btVector3(0, -10, 0));
}

PhysicsControl::~PhysicsControl() {
  delete this->broadphase;
  delete this->collisionConfiguration;
  delete this->dispatcher;
  delete this->solver;
  delete this->dynamicsWorld;
}

void PhysicsControl::doPhysics(std::vector<CustomPhysicalObject*> objects,
                               Uint64 startTick, Uint64 endTick) noexcept {
  // TODO
}

//TODO
std::vector<std::pair<CustomPhysicalObject*, CustomPhysicalObject*>>
PhysicsControl::getCollisions(
    std::vector<CustomPhysicalObject*> objects) noexcept {
  /*
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
  */
  return {};  // std::vector<std::pair<CustomPhysicalObject,
              // CustomPhysicalObject>>();
}

void PhysicsControl::addPhysicalObject(CustomPhysicalObject* object) {
  // TODO add physical object by it's type (rigid body, collision object or
  // character)
  return;
}
