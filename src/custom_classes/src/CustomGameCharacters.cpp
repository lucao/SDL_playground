#include <SDL.h>
#include <SDL_image.h>

#include <CustomGameCharacters.hpp>
#include <CustomSDLObject.hpp>
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

void CustomPlayer::handleEvent(SDL_Event event) {
  SDL_Rect *destination = this->getDestination();
  const Uint8 *keyboardState = SDL_GetKeyboardState(NULL);
  if (keyboardState[SDL_SCANCODE_W] || keyboardState[SDL_SCANCODE_UP]) {
    destination->y -= speed;
  } else if (keyboardState[SDL_SCANCODE_S] ||
             keyboardState[SDL_SCANCODE_DOWN]) {
    destination->y += speed;
  }
  if (keyboardState[SDL_SCANCODE_A] || keyboardState[SDL_SCANCODE_LEFT]) {
    destination->x -= speed;
  } else if (keyboardState[SDL_SCANCODE_D] ||
             keyboardState[SDL_SCANCODE_RIGHT]) {
    destination->x += speed;
  }
}

// acho que nao preciso desse método
void CustomPlayer::move(SDL_Rect *destination) {
  this->setDestination(destination);
}
