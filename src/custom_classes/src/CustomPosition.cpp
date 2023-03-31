#include <SDL.h>

#include <CustomPosition.hpp>

CustomPosition::CustomPosition(int x, int y) {
  this->position.x = x;
  this->position.y = y;
}

void CustomPosition::changePosition(CustomPosition newPosition) {
  // TODO
}

SDL_Point CustomPosition::getPosition() { return this->position; }
