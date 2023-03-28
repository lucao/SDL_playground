#ifndef CUSTOM_GAME_CHARACTERS_H
#define CUSTOM_GAME_CHARACTERS_H

#include <SDL.h>

#include "CustomSDLObject.h"

class CustomGameCharacter : public CustomSDLObject {
 private:
  int lifePoints;

 public:
  CustomGameCharacter(SDL_Surface *surface, SDL_Rect position, int lifePoints);
};

class CustomPlayer : public CustomGameCharacter {
 private:
  int speed;

 public:
  CustomPlayer(SDL_Surface *surface, SDL_Rect position, int lifePoints,
               int speed);
  void move(SDL_Scancode code);
};

#endif