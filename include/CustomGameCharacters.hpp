#ifndef CUSTOM_GAME_CHARACTERS_H
#define CUSTOM_GAME_CHARACTERS_H

#include <SDL.h>

#include <CustomCollision.hpp>
#include <CustomGameObjects.hpp>
#include <CustomSDLObject.hpp>
#include <queue>

class CustomGameCharacter : public CustomSDLMaterialObject {
 private:
  int lifePoints;

 public:
  CustomGameCharacter(SDL_Texture *texture, CustomSDLRect *srcRect,
                      CustomSDLRect *position, int lifePoints);
  CustomGameCharacter(CustomSDLRect *srcRect, CustomSDLRect *position,
                      int lifePoints);
  virtual ~CustomGameCharacter();
};

enum Action {
  PLAYER_MOVE_UP = 101,
  PLAYER_MOVE_DOWN = 102,
  PLAYER_MOVE_LEFT = 103,
  PLAYER_MOVE_RIGHT = 104,
  PLAYER_INTERACT = 100
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

class CustomPlayer : public CustomGameCharacter,
                     public EventListener,
                     public Collider<Platform> {
 private:
  int speed;

 public:
  CustomPlayer(SDL_Texture *texture, CustomSDLRect *srcRect,
               CustomSDLRect *position, int lifePoints, int speed);
  CustomPlayer(CustomSDLRect *srcRect, CustomSDLRect *position, int lifePoints,
               int speed);
  ~CustomPlayer();
  bool handleEvent(CustomEvent *event) override;
  bool colideWith(Platform *collider) override;
};

#endif