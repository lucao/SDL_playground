#ifndef CUSTOM_GAME_UTILS_H
#define CUSTOM_GAME_UTILS_H

#include <SDL.h>

#include <deque>
#include <unordered_set>

enum GAME_ENTITY_TYPE {
  PLAYABLE_OBJECT,
  PHYSICAL_OBJECT,
  MATERIAL_OBJECT,
  LOGIC_OBJECT
};

enum ANIMATION_TYPE { NO_ANIMATION, IDLE, WALKING, RUNNING };

enum PLAYER_ACTION {
  PLAYER_IDLE_INPUT = 0,

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

// TODO destructors
class Movement {
 protected:
  SDL_Point startPoint;
  Uint64 startTick;
  Uint64 endTick;
  SDL_Point endPoint;

  Movement(SDL_Point startPoint, Uint64 startTick, Uint64 endTick);

 public:
  Movement(SDL_Point startPoint, Uint64 startTick, Uint64 endTick,
           SDL_Point endPoint);
  Movement(const Movement& other) = default;
  Movement& operator=(const Movement& other) = default;

  SDL_Point getStartPoint() const { return this->startPoint; }
  SDL_Point getEndPoint() const { return this->endPoint; }
  Uint64 getStartTick() const { return this->startTick; }
  Uint64 getEndTick() const { return this->endTick; }

  Uint64 getDeltaTick() const { return this->endTick - this->startTick; }

  // velocidade em pixels/milisegundos
  // TODO testar isso
  float getVelocityX();
  float getVelocityY();

  virtual MOVEMENT_TYPE getType() const { return MOVEMENT_TYPE::MOVEMENT; }

  virtual bool operator==(const Movement& other) const {
    return this == &other;
  }
};
// TODO remove pointers
class CustomEvent {
 private:
  PLAYER_ACTION action;
  Uint64 initialTick;
  Uint64 endTick;

 public:
  CustomEvent(PLAYER_ACTION action, Uint64 initialTick, Uint64 endTick);
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

class FPSControl {
 private:
  Uint64 frameTick;
  Uint64 lastFrameTick;

  Uint64 lastSecondTick;

  Uint16 frameCounter;
  std::deque<Uint64> ticks;
  std::deque<Uint16> samples;

 public:
  static const Uint32 secondinMS = 1000;
  FPSControl() {
    this->frameTick = SDL_GetTicks64();
    this->lastFrameTick = SDL_GetTicks64();
    this->lastSecondTick = 0;
    this->ticks = std::deque<Uint64>(20, -1);
    this->samples = std::deque<Uint16>(20, -1);
  }

  void tick() {
    this->lastFrameTick = this->frameTick;
    this->frameTick = SDL_GetTicks64();

    if (this->ticks.size() > 19) this->ticks.pop_back();
    this->ticks.push_front(this->frameTick);

    frameCounter++;
    if (SDL_GetTicks64() - this->lastSecondTick >= secondinMS) {
      this->lastSecondTick = SDL_GetTicks64();

      if (this->samples.size() > 19) this->samples.pop_back();
      this->samples.push_front(frameCounter);

      frameCounter = 0;
    }
  }

  Uint64 getLastFrameTick() { return this->lastFrameTick; }
  Uint64 getFrameTick() { return this->frameTick; }
  std::deque<Uint16> getFpsSamples() { return this->samples; }
};

#endif