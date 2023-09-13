#ifndef CUSTOM_GAME_CHARACTERS_H
#define CUSTOM_GAME_CHARACTERS_H

#include <SDL.h>

#include <CustomCollision.hpp>
#include <CustomGameObjects.hpp>
#include <CustomSDLObject.hpp>
#include <queue>

class CustomGameCharacter : public CustomAnimatedSDLMaterialObject {
 private:
  int lifePoints;

 public:
  CustomGameCharacter(SDL_Texture *texture, CustomSDLRect *srcRect,
                      CustomSDLRect *position, int typesOfAnimation,
                      int stepsOfAnimation, int lifePoints);
  CustomGameCharacter(CustomSDLRect *srcRect, CustomSDLRect *position,
                      int typesOfAnimation, int stepsOfAnimation,
                      int lifePoints);
  virtual ~CustomGameCharacter();
};

enum Action {
  PLAYER_UP_KEY_PRESSED = 1011,
  PLAYER_UP_KEY_RELEASED = 1010,
  PLAYER_DOWN_KEY_PRESSED = 1021,
  PLAYER_DOWN_KEY_RELEASED = 1020,
  PLAYER_LEFT_KEY_PRESSED = 1031,
  PLAYER_LEFT_KEY_RELEASED = 1030,
  PLAYER_RIGHT_KEY_PRESSED = 1041,
  PLAYER_RIGHT_KEY_RELEASED = 1040,
  PLAYER_INTERACT_KEY_PRESSED = 1000
};

class CustomEvent {
 private:
  Action action;

 public:
  CustomEvent(Action action);
  virtual ~CustomEvent();
  Action getAction();
};

class EventListener {
 public:
  virtual bool handleEvent(CustomEvent *event);
};

enum Direction {
  LEFT, RIGHT
};

class CustomPlayer : public CustomGameCharacter,
                     public EventListener,
                     public Collider<Platform> {
 private:
  int maxSpeed;
  int speed;

  bool startedMovingLeft;
  Uint64 startedMovingLeftTick;
  bool startedMovingRight;
  Uint64 startedMovingRightTick;

  bool isMovingLeft;
  Uint64 isMovingLeftTick;
  bool isMovingRight;
  Uint64 isMovingRightTick;

  int speedWhenStartedStopping;
  bool stoppingMovement;
  Uint64 stoppingMovementTick;
  bool stopped;

 public:
  CustomPlayer(SDL_Texture *texture, CustomSDLRect *srcRect,
               CustomSDLRect *position, int typesOfAnimation,
               int stepsOfAnimation, int lifePoints, int maxSpeed);
  CustomPlayer(CustomSDLRect *srcRect, CustomSDLRect *position,
               int typesOfAnimation, int stepsOfAnimation, int lifePoints,
               int maxSpeed);
  ~CustomPlayer();
  void move(Direction direction);
  void stopMovement();
  bool isStopped();
  virtual int getStartMovingDelay();
  virtual int getStopMovingDelay();
  bool handleEvent(CustomEvent *event) override;
  bool colideWith(Platform *collider) override;
};

#endif