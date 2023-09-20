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
                                         int typesOfAnimation,
                                         int stepsOfAnimation, int lifePoints)
    : CustomAnimatedSDLMaterialObject(texture, srcRect, position,
                                      typesOfAnimation, stepsOfAnimation) {
  this->lifePoints = lifePoints;
}
CustomGameCharacter::CustomGameCharacter(CustomSDLRect *srcRect,
                                         CustomSDLRect *position,
                                         int typesOfAnimation,
                                         int stepsOfAnimation, int lifePoints)
    : CustomAnimatedSDLMaterialObject(srcRect, position, typesOfAnimation,
                                      stepsOfAnimation) {
  this->lifePoints = lifePoints;
}
CustomGameCharacter::~CustomGameCharacter() {}

CustomEvent::CustomEvent(Action action, Uint64 initialTick, Uint64 endTick) {
  this->action = action;
  this->initialTick = initialTick;
  this->endTick = endTick;
}
CustomEvent::~CustomEvent() {}
Action CustomEvent::getAction() { return this->action; }
Uint64 CustomEvent::getInitialTick() { return this->initialTick; }
Uint64 CustomEvent::getEndTick() { return this->endTick; }

void EventListener::handleEvent(CustomEvent *event) {
  // handle general events
}

Movement::Movement(SDL_Point startPoint, SDL_Point endPoint, bool jumping,
                   Uint64 startTick, Uint64 endTick) {
  this->startPoint = startPoint;
  this->endPoint = endPoint;
  this->jumping = jumping;
  this->startTick = startTick;
  this->endTick = endTick;
}

/// @brief
/// @param texture
/// @param srcRect
/// @param position
/// @param lifePoints
/// @param normalSpeed
CustomPlayer::CustomPlayer(SDL_Texture *texture, CustomSDLRect *srcRect,
                           CustomSDLRect *position, int typesOfAnimation,
                           int stepsOfAnimation, int lifePoints,
                           int normalSpeed)
    : CustomGameCharacter(texture, srcRect, position, typesOfAnimation,
                          stepsOfAnimation, lifePoints) {
  this->normalSpeed = normalSpeed;
}
CustomPlayer::CustomPlayer(CustomSDLRect *srcRect, CustomSDLRect *position,
                           int typesOfAnimation, int stepsOfAnimation,
                           int lifePoints, int normalSpeed)
    : CustomGameCharacter(srcRect, position, typesOfAnimation, stepsOfAnimation,
                          lifePoints) {
  this->normalSpeed = normalSpeed;
}
CustomPlayer::~CustomPlayer() {}

bool CustomPlayer::canJump() { return true; }

void CustomPlayer::handleEvent(CustomEvent *event) {
  bool jumping = this->isJumping();
  if (event->getAction() == Action::PLAYER_JUMP_KEY_PRESSED) {
    if (this->canJump()) jumping = true;
  }
  if (event->getAction() == Action::PLAYER_RIGHT_KEY_PRESSED ||
      event->getAction() == Action::PLAYER_LEFT_KEY_PRESSED) {
    const SDL_Point startPosition = this->getGlobalDestination()->getPoint();

    int xMoving = startPosition.x;
    if (event->getAction() == Action::PLAYER_RIGHT_KEY_PRESSED) {
      xMoving += this->normalSpeed;
    } else if (event->getAction() == Action::PLAYER_LEFT_KEY_PRESSED) {
      xMoving -= this->normalSpeed;
    }

    const SDL_Point endPosition{xMoving, endPosition.y};

    this->movingIntentList.push_front(
        new Movement(startPosition, endPosition, jumping,
                     event->getInitialTick(), event->getEndTick()));
  }
}

bool CustomPlayer::isJumping() {
  // TODO
}

bool CustomPlayer::colideWith(Platform *collider) {
  return false;
  // this->getDestination()  TODO
}
