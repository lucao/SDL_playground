#ifndef CUSTOM_GAME_CHARACTERS_H
#define CUSTOM_GAME_CHARACTERS_H

#include <SDL.h>

#include <CustomSDLObject.hpp>

class CustomGameCharacter : public CustomSDLMaterialObject {
 private:
  int lifePoints;
  // hitbox

 public:
  CustomGameCharacter(SDL_Texture *texture, CustomSDLRect *srcRect,
                      CustomSDLRect *position, int lifePoints);
  virtual ~CustomGameCharacter();
};

class CustomPlayer : public CustomGameCharacter {
 private:
  int speed;
  void move(SDL_Rect *destination);

 public:
  CustomPlayer(SDL_Texture *texture, CustomSDLRect *srcRect,
               CustomSDLRect *position, int lifePoints, int speed);
  ~CustomPlayer();
  void handleEvent(SDL_Event event);
};

#endif