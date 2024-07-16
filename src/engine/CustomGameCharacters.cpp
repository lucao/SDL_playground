
#include <CustomGameCharacters.hpp>

CustomGameCharacter::CustomGameCharacter(SDL_Texture* texture,
                                         CustomSDLRect* srcRect,
                                         CustomSDLRect* position,
                                         int typesOfAnimation,
                                         int stepsOfAnimation, int lifePoints)
    : CustomAnimatedSDLMaterialObject(texture, srcRect, position,
                                      typesOfAnimation, stepsOfAnimation),
      CustomDynamicPhysicalObject(CollisionMasks::CHARACTER,
                                  CollisionFilters::CHARACTERS,
                                  new btCollisionObject()) {
  this->lifePoints = lifePoints;
}
CustomGameCharacter::CustomGameCharacter(CustomSDLRect* srcRect,
                                         CustomSDLRect* position,
                                         int typesOfAnimation,
                                         int stepsOfAnimation, int lifePoints)
    : CustomAnimatedSDLMaterialObject(srcRect, position, typesOfAnimation,
                                      stepsOfAnimation),
      CustomDynamicPhysicalObject(CollisionMasks::CHARACTER,
                                  CollisionFilters::CHARACTERS,
                                  new btCollisionObject()) {
  this->lifePoints = lifePoints;
}
CustomGameCharacter::~CustomGameCharacter() {}

/// @brief
/// @param texture
/// @param srcRect
/// @param position
/// @param lifePoints
/// @param normalSpeed
CustomPlayer::CustomPlayer(SDL_Texture* texture, CustomSDLRect* srcRect,
                           CustomSDLRect* position, int typesOfAnimation,
                           int stepsOfAnimation, int lifePoints,
                           int normalSpeed)
    : CustomGameCharacter(texture, srcRect, position, typesOfAnimation,
                          stepsOfAnimation, lifePoints) {
  this->normalSpeed = normalSpeed;
}
CustomPlayer::CustomPlayer(CustomSDLRect* srcRect, CustomSDLRect* position,
                           int typesOfAnimation, int stepsOfAnimation,
                           int lifePoints, int normalSpeed)
    : CustomGameCharacter(srcRect, position, typesOfAnimation, stepsOfAnimation,
                          lifePoints) {
  this->normalSpeed = normalSpeed;
}
CustomPlayer::~CustomPlayer() {}

inline bool CustomPlayer::canMove() const { return true; }

inline bool CustomPlayer::canJump() const { return true; }

inline bool CustomPlayer::isJumping() const { return false; }
// calcular com base em atributos
inline int CustomPlayer::getJumpForce() const { return 10; }

void CustomPlayer::handleEvent(CustomEvent event) {
  bool jumping = this->isJumping();
  if (event.getAction() == PLAYER_ACTION::PLAYER_JUMP_KEY_PRESSED) {
    if (this->canJump()) {
      this->move(Jump(this->getJumpForce(),
                      this->getGlobalDestination()->getPoint(),
                      event.getInitialTick(), event.getEndTick()));
    }
  }

  if (this->canMove()) {
    if (event.getAction() == PLAYER_ACTION::PLAYER_RIGHT_KEY_PRESSED) {
      this->move(Walk(this->normalSpeed, Direction::RIGHT,
                      this->getGlobalDestination()->getPoint(),
                      event.getInitialTick(), event.getEndTick()));
    } else if (event.getAction() == PLAYER_ACTION::PLAYER_LEFT_KEY_PRESSED) {
      this->move(Walk(this->normalSpeed, Direction::LEFT,
                      this->getGlobalDestination()->getPoint(),
                      event.getInitialTick(), event.getEndTick()));
    }
  }
}
