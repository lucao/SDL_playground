#include <SDL.h>
#include <SDL_image.h>

#include <CustomGameCharacters.hpp>
#include <CustomSDLObject.hpp>

std::set<SDL_Scancode> movementScanCodes({SDL_SCANCODE_W, SDL_SCANCODE_UP,
                                          SDL_SCANCODE_A, SDL_SCANCODE_LEFT,
                                          SDL_SCANCODE_S, SDL_SCANCODE_DOWN,
                                          SDL_SCANCODE_D, SDL_SCANCODE_RIGHT});

CustomGameCharacter::CustomGameCharacter(SDL_Surface *surface,
                                         SDL_Rect position, int lifePoints)
    : CustomSDLMaterialObject(surface, position) {
  this->lifePoints = lifePoints;
}

CustomPlayer::CustomPlayer(SDL_Surface *surface, SDL_Rect position,
                           int lifePoints, int speed)
    : CustomGameCharacter(surface, position, lifePoints) {
  this->speed = speed;
}

const int player_w = 100;
const int player_h = 100;
CustomPlayer::CustomPlayer(char *imgPath, int x, int y, int lifePoints,
                           int speed)
    : CustomGameCharacter(IMG_Load(imgPath), SDL_Rect{x, y, player_w, player_h},
                          lifePoints) {
  this->speed = speed;
}

void CustomPlayer::handleEvent(SDL_Event event) {
  if (event.type == SDL_KEYDOWN) {
    SDL_Scancode code = event.key.keysym.scancode;
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
    }
  }
}

void CustomPlayer::move(SDL_Rect destination) {
  this->setlastDestination(this->getDestination());
  this->setDestination(destination);
}
