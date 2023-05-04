#include <SDL.h>
#include <SDL_image.h>

#include <CustomCollision.hpp>
#include <CustomGameCharacters.hpp>
#include <CustomGameObjects.hpp>
#include <CustomSDLObject.hpp>
#include <queue>
#include <unordered_set>

std::unordered_set<SDL_Scancode> movementScanCodes(
    {SDL_SCANCODE_W, SDL_SCANCODE_UP, SDL_SCANCODE_A, SDL_SCANCODE_LEFT,
     SDL_SCANCODE_S, SDL_SCANCODE_DOWN, SDL_SCANCODE_D, SDL_SCANCODE_RIGHT});

CustomGameCharacter::CustomGameCharacter(SDL_Texture *texture,
                                         CustomSDLRect *srcRect,
                                         CustomSDLRect *position,
                                         int lifePoints)
    : CustomSDLMaterialObject(texture, srcRect, position) {
  this->lifePoints = lifePoints;
}
CustomGameCharacter::CustomGameCharacter(CustomSDLRect *srcRect,
                                         CustomSDLRect *position,
                                         int lifePoints)
    : CustomSDLMaterialObject(srcRect, position) {
  this->lifePoints = lifePoints;
}
CustomGameCharacter::~CustomGameCharacter() {}

CustomEvent::CustomEvent(Action action, long timeSinceLastEventProcess) {
  this->action = action;
  this->timeSinceLastEventProcess = timeSinceLastEventProcess;
}
CustomEvent::~CustomEvent() {}
long CustomEvent::getTimeSinceLastEventProcess() {
  return this->timeSinceLastEventProcess;
}
Action CustomEvent::getAction() { return this->action; }

bool EventListener::handleEvent(CustomEvent *event) {
  // handle general events
  return true;
}

/// @brief
/// @param texture
/// @param srcRect
/// @param position
/// @param lifePoints
/// @param speed in pixels per milisecond
CustomPlayer::CustomPlayer(SDL_Texture *texture, CustomSDLRect *srcRect,
                           CustomSDLRect *position, int lifePoints, int speed)
    : CustomGameCharacter(texture, srcRect, position, lifePoints) {
  this->speed = speed;
}
CustomPlayer::CustomPlayer(CustomSDLRect *srcRect, CustomSDLRect *position,
                           int lifePoints, int speed)
    : CustomGameCharacter(srcRect, position, lifePoints) {
  this->speed = speed;
}
CustomPlayer::~CustomPlayer() {}

bool CustomPlayer::handleEvent(CustomEvent *event) {
  SDL_Rect *destination = this->getGlobalDestination();
  const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);

  int pixelsMoved =
      speed * (SDL_GetTicks() - event->getTimeSinceLastEventProcess());

  if (event->getAction() == Action::PLAYER_MOVE_UP) {
    destination->y -= speed;
  } else if (event->getAction() == Action::PLAYER_MOVE_DOWN) {
    destination->y += speed;
  }
  if (event->getAction() == Action::PLAYER_MOVE_LEFT) {
    destination->x -= speed;
  } else if (event->getAction() == Action::PLAYER_MOVE_RIGHT) {
    destination->x += speed;
  }

  return true;
}

bool CustomPlayer::colideWith(Platform *collider) { 
  return false;
//this->getDestination()  TODO
}

