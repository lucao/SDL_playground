#ifndef CUSTOM_GAME_OBJECT_H
#define CUSTOM_GAME_OBJECT_H

#include <CustomSDLObject.hpp>

enum GAME_ENTITY_TYPE {
  PLAYABLE_OBJECT,
  PHYSICAL_OBJECT,
  MATERIAL_OBJECT,
  LOGIC_OBJECT
};

const enum ANIMATION_TYPE { IDLE = 0, WALKING = 1, RUNNING = 2 };

class GameObject {};

#endif