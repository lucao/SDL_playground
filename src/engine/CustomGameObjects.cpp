#include "CustomGameObjects.hpp"

#include <CustomPhysics.hpp>
#include <CustomSDLObject.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

GameObject::GameObject() : uuid(boost::uuids::random_generator()()) {}

const boost::uuids::uuid& GameObject::getUUID() const { return this->uuid; }
