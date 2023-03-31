#ifndef CUSTOM_POSITION_H
#define CUSTOM_POSITION_H

#include <SDL.h>

class CustomPosition {
 private:
  SDL_Point position;
  CustomPosition *lastPosition;

 public:
  CustomPosition(int x, int y);
  SDL_Point getPosition();
  void changePosition(CustomPosition newPosition);
};

#endif