#include <CustomPhysics.hpp>
#include <unordered_map>

CustomPhysicalObject::CustomPhysicalObject(b2BodyDef bodyDef,
                                           b2ShapeDef shapeDef, b2Vec2 position,
                                           b2Polygon polygon,
                                           b2WorldId worldId) {
  bodyDef.position = position;
  bodyDef.type = b2_staticBody;
  shapeDef.density = 1.0f;
  shapeDef.friction = 0.3f;
  this->bodyId = b2CreateBody(worldId, &bodyDef);
  b2CreatePolygonShape(this->bodyId, &shapeDef, &polygon);
}

CustomPhysicalObject::CustomPhysicalObject(b2BodyDef bodyDef,
                                           b2ShapeDef shapeDef, b2Vec2 position,
                                           b2Capsule capsule,
                                           b2WorldId worldId) {
  bodyDef.position = position;
  bodyDef.type = b2_staticBody;
  shapeDef.density = 1.0f;
  shapeDef.friction = 0.3f;
  this->bodyId = b2CreateBody(worldId, &bodyDef);
  b2CreateCapsuleShape(this->bodyId, &shapeDef, &capsule);
}

CustomPhysicalObject::CustomPhysicalObject() {
  // TODO
}

CustomPhysicalObject::~CustomPhysicalObject() {}

b2BodyId CustomPhysicalObject::getBodyId() { return this->bodyId; }

CustomDynamicPhysicalObject::CustomDynamicPhysicalObject(b2BodyDef bodyDef,
                                                         b2ShapeDef shapeDef,
                                                         b2Vec2 position,
                                                         b2Polygon polygon,
                                                         b2WorldId worldId) {
  bodyDef.position = position;
  bodyDef.type = b2_dynamicBody;
  shapeDef.density = 1.0f;
  shapeDef.friction = 0.3f;
  this->bodyId = b2CreateBody(worldId, &bodyDef);
  b2CreatePolygonShape(this->bodyId, &shapeDef, &polygon);
}

CustomDynamicPhysicalObject::CustomDynamicPhysicalObject(b2BodyDef bodyDef,
                                                         b2ShapeDef shapeDef,
                                                         b2Vec2 position,
                                                         b2Capsule capsule,
                                                         b2WorldId worldId) {
  bodyDef.position = position;
  bodyDef.type = b2_dynamicBody;
  shapeDef.density = 1.0f;
  shapeDef.friction = 0.3f;
  this->bodyId = b2CreateBody(worldId, &bodyDef);
  b2CreateCapsuleShape(this->bodyId, &shapeDef, &capsule);
}

CustomDynamicPhysicalObject::~CustomDynamicPhysicalObject() {}

void CustomDynamicPhysicalObject::addMovement(Movement* movement) {
  // TODO verificar tamanho máximo da lista e lógica de inserção de movements
  this->movementList.push_back(movement);
}

Box2DPhysicsControl::Box2DPhysicsControl(float frameRate) {
  this->frameRate = frameRate;
  b2WorldDef worldDef = b2DefaultWorldDef();
  worldDef.gravity = b2Vec2({0.0f, -9.8f});
  this->worldId = b2CreateWorld(&worldDef);
}

Box2DPhysicsControl::~Box2DPhysicsControl() {}

b2WorldId Box2DPhysicsControl::getWorldId() { return this->worldId; }

void Box2DPhysicsControl::changeFrameRate(float frameRate) {
  this->frameRate = frameRate;
}

void Box2DPhysicsControl::doPhysics(
    std::unordered_map<GameObject, CustomPhysicalObject*> objects,
    Uint64 startTick, Uint64 endTick) noexcept {
  std::unordered_map<GameObject, CustomPhysicalObject*> allPhysicalObjects;
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
      }
    } else {
      if (objects.find(gameObject) != objects.end()) {
        this->physicalObjects[gameObject] = physicalObject;
      } else {
        // ERROR
      }
    }
  }

  for (auto pair : this->physicalObjects) {
    pair.second->doPhysics(startTick, endTick);
  }

  int subStepCount = 4;
  b2World_Step(worldId, static_cast<float>(endTick - startTick) / 1000.0,
               subStepCount);

  for (auto pair : this->physicalObjects) {
    // TODO implement callback for each CustomPhysicalObject

    pair.second->afterSimulation(startTick, endTick);
  }
}
