#include <SDL.h>

#include <CustomGameUtils.hpp>

std::unordered_set<SDL_Scancode> movementScanCodes(
    {SDL_SCANCODE_W, SDL_SCANCODE_UP, SDL_SCANCODE_A, SDL_SCANCODE_LEFT,
     SDL_SCANCODE_S, SDL_SCANCODE_DOWN, SDL_SCANCODE_D, SDL_SCANCODE_RIGHT});

Movement::Movement(SDL_Point startPoint, Uint64 startTick, Uint64 endTick,
                   SDL_Point endPoint) {
  this->startPoint = startPoint;
  this->startTick = startTick;
  this->endTick = endTick;
  this->endPoint = endPoint;
}

Movement::Movement(SDL_Point startPoint, Uint64 startTick, Uint64 endTick) {
  this->startPoint = startPoint;
  this->startTick = startTick;
  this->endTick = endTick;
}
//TODO o que está errado aqui?
float Movement::getVelocityX() {
  return static_cast<float>(this->endPoint.x -
         this->startPoint.x) /
             static_cast<int>(this->endTick - this->startTick);
}
float Movement::getVelocityY() {
  return static_cast<float>(this->startPoint.y -
         this->endPoint.y) /
             static_cast<int>(this->endTick - this->startTick);
}

Walk::Walk(int speed, Direction direction, SDL_Point startPoint,
           Uint64 startTick, Uint64 endTick)
    : Movement(startPoint, startTick, endTick) {
  this->speed = speed;

  if (direction == Direction::RIGHT) {
    this->endPoint = SDL_Point({startPoint.x + this->speed, startPoint.y});
  } else {  // if (direction == Direction::LEFT) {
    this->endPoint = SDL_Point({startPoint.x - this->speed, startPoint.y});
  }
}

Jump::Jump(int jumpForce, SDL_Point startPoint, Uint64 startTick,
           Uint64 endTick)
    : Movement(startPoint, startTick, endTick) {}

CustomEvent::CustomEvent(PLAYER_ACTION action, Uint64 initialTick,
                         Uint64 endTick) {
  this->action = action;
  this->initialTick = initialTick;
  this->endTick = endTick;
}
PLAYER_ACTION CustomEvent::getAction() { return this->action; }
Uint64 CustomEvent::getInitialTick() { return this->initialTick; }
Uint64 CustomEvent::getEndTick() { return this->endTick; }

void EventListener::handleEvent(CustomEvent* event) {
  // handle general events
}
