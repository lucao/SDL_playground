#ifndef CUSTOM_GAME_OBJECT_H
#define CUSTOM_GAME_OBJECT_H

#include <boost/functional/hash.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

enum GAME_TAGS { PLAYER, NPC, OBJECT };

class GameObject {
 private:
  boost::uuids::uuid uuid;
  GAME_TAGS tag;

 public:
  GameObject(GAME_TAGS tag);
  const boost::uuids::uuid& getUUID() const;
  const GAME_TAGS getTag() const;

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