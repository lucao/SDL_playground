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

class CustomPlayerMovement {
  virtual bool canMove() const = 0;

  virtual bool canJump() const = 0;
  virtual bool isJumping() const = 0;
  virtual int getJumpForce() const = 0;
};

#endif