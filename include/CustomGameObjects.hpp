#ifndef CUSTOM_GAME_OBJECT_H
#define CUSTOM_GAME_OBJECT_H

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>

class GameObject {
 private:
  boost::uuids::uuid uuid;

 public:
  GameObject();
  const boost::uuids::uuid& getUUID() const;

  bool operator==(const GameObject& other) const {
    return this->uuid == other.uuid;
  }
  bool operator<(const GameObject& other) const {
    return this->uuid < other.uuid;
  }
};

// Especializando std::hash for GameObject class
namespace std {
template <>
struct hash<GameObject> {
  size_t operator()(const GameObject& gameObject) const {
    return boost::hash<boost::uuids::uuid>()(gameObject.getUUID());
  }
};
}  // namespace std

#endif