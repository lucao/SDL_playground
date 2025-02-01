#include "CustomGameObjects.hpp"

#include <CustomPhysics.hpp>
#include <CustomSDLObject.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

GameObject::GameObject(GAME_TAGS tag)
    : uuid(boost::uuids::random_generator()()) {
  this->tag = tag;
}

const boost::uuids::uuid& GameObject::getUUID() const { return this->uuid; }

const GAME_TAGS GameObject::getTag() const { return this->tag; }
