#include <SDL.h>

#include <CustomGameUtils.hpp>

std::unordered_set<SDL_Scancode> movementScanCodes(
    {SDL_SCANCODE_W, SDL_SCANCODE_UP, SDL_SCANCODE_A, SDL_SCANCODE_LEFT,
     SDL_SCANCODE_S, SDL_SCANCODE_DOWN, SDL_SCANCODE_D, SDL_SCANCODE_RIGHT});

Movement::Movement(SDL_Point startPoint, Uint64 startTick, Uint64 endTick) {
  this->startPoint = startPoint;
  this->startTick = startTick;
  this->endTick = endTick;
  this->endPoint = startPoint;
}

Walk::Walk(int speed, Direction direction, SDL_Point startPoint,
           Uint64 startTick, Uint64 endTick)
    : Movement(startPoint, startTick, endTick) {}

Jump::Jump(int jumpForce, SDL_Point startPoint, Uint64 startTick,
           Uint64 endTick)
    : Movement(startPoint, startTick, endTick) {}

CustomEvent::CustomEvent(PLAYER_ACTION action, Uint64 initialTick,
                         Uint64 endTick) {
  this->action = action;
  this->initialTick = initialTick;
  this->endTick = endTick;
}
CustomEvent::~CustomEvent() {}
PLAYER_ACTION CustomEvent::getAction() { return this->action; }
Uint64 CustomEvent::getInitialTick() { return this->initialTick; }
Uint64 CustomEvent::getEndTick() { return this->endTick; }

void EventListener::handleEvent(CustomEvent event) {
  // handle general events
}
