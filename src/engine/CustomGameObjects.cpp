#include "CustomGameObjects.hpp"

#include <CustomPhysics.hpp>
#include <CustomSDLObject.hpp>

GameObject::GameObject() {
  // TODO set uuid
  this->uuid = boost::uuids::uuid();
}

boost::uuids::uuid GameObject::getUUID() { return this->uuid; }
