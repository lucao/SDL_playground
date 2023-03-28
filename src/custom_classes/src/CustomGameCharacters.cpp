#include "../CustomGameCharacters.h"

#include <SDL.h>

#include "../CustomSDLObject.h"

std::set<SDL_Scancode> movementScanCodes({SDL_SCANCODE_W, SDL_SCANCODE_UP,
                                          SDL_SCANCODE_A, SDL_SCANCODE_LEFT,
                                          SDL_SCANCODE_S, SDL_SCANCODE_DOWN,
                                          SDL_SCANCODE_D, SDL_SCANCODE_RIGHT});

CustomGameCharacter::CustomGameCharacter(SDL_Surface *surface,
                                         SDL_Rect position, int lifePoints)
    : CustomSDLObject(surface, position) {
  this->lifePoints = lifePoints;
}

CustomPlayer::CustomPlayer(SDL_Surface *surface, SDL_Rect position,
                           int lifePoints, int speed)
    : CustomGameCharacter(surface, position, lifePoints) {
  this->speed = speed;
}
void CustomPlayer::move(SDL_Scancode code) {
  if (movementScanCodes.find(code) != movementScanCodes.end()) {
    SDL_Rect destination = this->getDestination();
    switch (code) {
      case SDL_SCANCODE_W:
      case SDL_SCANCODE_UP:
        destination.y -= speed;
        break;
      case SDL_SCANCODE_A:
      case SDL_SCANCODE_LEFT:
        destination.x -= speed / 30;
        break;
      case SDL_SCANCODE_S:
      case SDL_SCANCODE_DOWN:
        destination.y += speed / 30;
        break;
      case SDL_SCANCODE_D:
      case SDL_SCANCODE_RIGHT:
        destination.x += speed / 30;
        break;
      default:
        break;
    }
    this->setDestination(destination);
    // TODO setRect for this player
  }
}
