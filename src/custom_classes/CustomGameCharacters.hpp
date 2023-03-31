#ifndef CUSTOM_GAME_CHARACTERS_H
#define CUSTOM_GAME_CHARACTERS_H

#include <SDL.h>

#include <CustomSDLObject.hpp>

class CustomGameCharacter : public CustomSDLMaterialObject {
 private:
  int lifePoints;
  // hitbox

 public:
  CustomGameCharacter(SDL_Surface *surface, SDL_Rect position, int lifePoints);
};

class CustomPlayer : public CustomGameCharacter {
 private:
  int speed;
  void move(SDL_Rect destination);

 public:
  CustomPlayer(SDL_Surface *surface, SDL_Rect position, int lifePoints,
               int speed);
  CustomPlayer(char *imgPath, int x, int y, int lifePoints, int speed);
  void handleEvent(SDL_Event event);
};

#endif