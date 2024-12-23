#ifndef CUSTOM_GAME_OBJECT_H
#define CUSTOM_GAME_OBJECT_H

#include<boost/uuid/uuid.hpp>

class GameObject {
 private:
  boost::uuids::uuid uuid;

 public:

  GameObject();
  boost::uuids::uuid getUUID();
};

#endif