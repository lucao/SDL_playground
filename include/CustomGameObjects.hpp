#ifndef CUSTOM_GAME_OBJECT_H
#define CUSTOM_GAME_OBJECT_H

#include<boost>

class GameObject {
 private:
  boost::uuids::uuid uuid;

 public:
  GameObject();
};

#endif