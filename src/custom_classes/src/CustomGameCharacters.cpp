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

CustomEvent::CustomEvent(Action action) { this->action = action; }
CustomEvent::~CustomEvent() {}
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
/// @param maxSpeed
CustomPlayer::CustomPlayer(SDL_Texture *texture, CustomSDLRect *srcRect,
                           CustomSDLRect *position, int typesOfAnimation,
                           int stepsOfAnimation, int lifePoints, int maxSpeed)
    : CustomGameCharacter(texture, srcRect, position, typesOfAnimation,
                          stepsOfAnimation, lifePoints) {
  this->maxSpeed = maxSpeed;
  this->speed = 0;
}
CustomPlayer::CustomPlayer(CustomSDLRect *srcRect, CustomSDLRect *position,
                           int typesOfAnimation, int stepsOfAnimation,
                           int lifePoints, int maxSpeed)
    : CustomGameCharacter(srcRect, position, typesOfAnimation, stepsOfAnimation,
                          lifePoints) {
  this->maxSpeed = maxSpeed;
  this->speed = 0;
}
CustomPlayer::~CustomPlayer() {}

bool CustomPlayer::handleEvent(CustomEvent *event) {
  if (event->getAction() == Action::PLAYER_LEFT_KEY_PRESSED) {
    this->move(Direction::LEFT);
  } else if (event->getAction() == Action::PLAYER_RIGHT_KEY_PRESSED) {
    this->move(Direction::RIGHT);
  } else if (event->getAction() == Action::PLAYER_RIGHT_KEY_RELEASED ||
             event->getAction() == Action::PLAYER_LEFT_KEY_RELEASED) {
    this->stopMovement();
  }

  return true;
}

void CustomPlayer::move(Direction direction) {
  if (Direction::LEFT == direction) {
    if (this->isMovingRight || this->startedMovingRight) {
      this->stopMovement();
      return;
    } else if (this->isStopped()) {
      this->startedMovingLeft = true;
      this->startedMovingLeftTick = SDL_GetTicks64();

      this->stopped = false;
    } else if (this->startedMovingLeft) {
      Uint64 startedMovingLeftInterval =
          SDL_GetTicks64() - this->startedMovingLeftTick;
      if (startedMovingLeftInterval > this->getStartMovingDelay()) {
        this->startedMovingLeft = false;
        this->isMovingLeft = true;
      }
      this->speed = (this->maxSpeed * this->getStartMovingDelay()) /
                    startedMovingLeftInterval;

    } else if (this->isMovingLeft) {
      this->speed = this->maxSpeed;
    }
  } else if (Direction::RIGHT == direction) {
    if (this->isMovingLeft || this->startedMovingLeft) {
      this->stopMovement();
      return;
    } else if (this->isStopped()) {
      this->startedMovingRight = true;
      this->startedMovingRightTick = SDL_GetTicks64();

      this->stopped = false;
    } else if (this->startedMovingRight) {
      Uint64 startedMovingRightInterval =
          SDL_GetTicks64() - this->startedMovingRightTick;
      if (startedMovingRightInterval > this->getStartMovingDelay()) {
        this->startedMovingRight = false;
        this->isMovingRight = true;
      }
      this->speed = ((this->maxSpeed * this->getStartMovingDelay()) /
                     startedMovingRightInterval) *
                    -1;

    } else if (this->isMovingLeft) {
      this->speed = this->maxSpeed * -1;
    }
  }

  this->getGlobalDestination()->x += this->speed;
}

void CustomPlayer::stopMovement() {
  if (this->stoppingMovement) {
    Uint64 stoppingMovementInterval =
        SDL_GetTicks64() - this->stoppingMovementTick;

    Uint64 stopMovingTickAdvantage =
        (this->speedWhenStartedStopping * this->getStopMovingDelay()) /
        this->speedWhenStartedStopping;
    if (stoppingMovementInterval >
        this->getStopMovingDelay() - stopMovingTickAdvantage) {
      this->speed = 0;
      this->stopped = true;
      return;
    } else {
      Uint64 brakeSpeed = (this->maxSpeed * this->getStopMovingDelay()) /
                          stoppingMovementInterval;
      this->speed += this->speed > 0 ? brakeSpeed * -1 : brakeSpeed;
    }
    this->getGlobalDestination()->x += this->speed;
  } else if (this->isStopped()) {
    return;
  } else {
    this->speedWhenStartedStopping = this->speed;
    this->stoppingMovement = true;
    this->stoppingMovementTick = SDL_GetTicks64();

    this->getGlobalDestination()->x += this->speed;
  }
}

bool CustomPlayer::isStopped() { return this->stopped; }

bool CustomPlayer::colideWith(Platform *collider) {
  return false;
  // this->getDestination()  TODO
}
