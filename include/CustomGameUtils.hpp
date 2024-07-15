#ifndef CUSTOM_GAME_UTILS_H
#define CUSTOM_GAME_UTILS_H

#include <SDL.h>

#include <unordered_set>

enum PLAYER_ACTION {
  PLAYER_UP_KEY_PRESSED = 1011,
  PLAYER_UP_KEY_RELEASED = 1010,
  PLAYER_DOWN_KEY_PRESSED = 1021,
  PLAYER_DOWN_KEY_RELEASED = 1020,
  PLAYER_LEFT_KEY_PRESSED = 1031,
  PLAYER_LEFT_KEY_RELEASED = 1030,
  PLAYER_RIGHT_KEY_PRESSED = 1041,
  PLAYER_RIGHT_KEY_RELEASED = 1040,
  PLAYER_INTERACT_KEY_PRESSED = 1000,

  PLAYER_JUMP_KEY_PRESSED = 1051,
  PLAYER_JUMP_KEY_RELEASED = 1050,

  PLAYER_RUNNING_KEY_PRESSED = 1061,
  PLAYER_RUNNING_KEY_RELEASED = 1060
};

extern std::unordered_set<SDL_Scancode> movementScanCodes;

enum Direction { LEFT, RIGHT };

enum MOVEMENT_TYPE { MOVEMENT, WALK, RUN, JUMP };

class Movement {
 private:
  SDL_Point startPoint;
  Uint64 startTick;
  Uint64 endTick;
  SDL_Point endPoint;

 public:
  Movement(SDL_Point startPoint, Uint64 startTick, Uint64 endTick);
  Movement(const Movement& other) = default;
  Movement& operator=(const Movement& other) = default;

  virtual MOVEMENT_TYPE getType() const { return MOVEMENT_TYPE::MOVEMENT; }

  bool operator==(const Movement& other) const {
    return other.getType() == MOVEMENT_TYPE::MOVEMENT;
  }
};

namespace std {
template <>
struct hash<Movement> {
  std::size_t operator()(const Movement& k) const {
    return static_cast<std::size_t>(k.getType());
  }
};
}  // namespace std

class CustomEvent {
 private:
  PLAYER_ACTION action;
  Uint64 initialTick;
  Uint64 endTick;

 public:
  CustomEvent(PLAYER_ACTION action, Uint64 initialTick, Uint64 endTick);
  virtual ~CustomEvent();
  PLAYER_ACTION getAction();
  Uint64 getInitialTick();
  Uint64 getEndTick();
};

class EventListener {
 public:
  virtual void handleEvent(CustomEvent* event);
};

class Walk : public Movement {
 private:
  int speed;

 public:
  Walk(int speed, Direction direction, SDL_Point startPoint, Uint64 startTick,
       Uint64 endTick);

  virtual MOVEMENT_TYPE getType() const override { return MOVEMENT_TYPE::WALK; }
};

class Jump : public Movement {
 private:
  int jumpForce;

 public:
  Jump(int jumpForce, SDL_Point startPoint, Uint64 startTick, Uint64 endTick);
  virtual MOVEMENT_TYPE getType() const override { return MOVEMENT_TYPE::JUMP; }
};

#endif