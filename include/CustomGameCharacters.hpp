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
  PLAYER_INTERACT_KEY_PRESSED = 1000,

  PLAYER_JUMP_KEY_PRESSED = 1051,
  PLAYER_JUMP_KEY_RELEASED = 1050
};

class CustomEvent {
 private:
  Action action;
  Uint64 initialTick;
  Uint64 endTick;

 public:
  CustomEvent(Action action, Uint64 initialTick, Uint64 endTick);
  virtual ~CustomEvent();
  Action getAction();
  Uint64 getInitialTick();
  Uint64 getEndTick();
};

class EventListener {
 public:
  virtual void handleEvent(CustomEvent *event);
};

class Movement {
 private:
  bool jumping;
  SDL_Point startPoint;
  SDL_Point endPoint;
  Uint64 startTick;
  Uint64 endTick;

 public:
  Movement(SDL_Point startPoint, SDL_Point endPoint, bool jumping,
               Uint64 startTick, Uint64 endTick);
};

class CustomPlayer : public CustomGameCharacter,
                     public EventListener,
                     public Collider<Platform> {
 private:
  int normalSpeed;

  std::list<Movement *> movingIntentList;
  std::list<Movement *> movementDeFactoList;

 public:
  CustomPlayer(SDL_Texture *texture, CustomSDLRect *srcRect,
               CustomSDLRect *position, int typesOfAnimation,
               int stepsOfAnimation, int lifePoints, int normalSpeed);
  CustomPlayer(CustomSDLRect *srcRect, CustomSDLRect *position,
               int typesOfAnimation, int stepsOfAnimation, int lifePoints,
               int normalSpeed);
  ~CustomPlayer();
  bool canJump();
  bool isJumping();
  void handleEvent(CustomEvent *event) override;
  bool colideWith(Platform *collider) override;
};

#endif