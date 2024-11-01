
#include <CustomGameCharacters.hpp>

CustomGameCharacter::CustomGameCharacter(
    CustomTextureManager texture,
    std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>> animationSprites,
    CustomSDLRect position, int lifePoints)
    : CustomAnimatedSDLMaterialObject(texture, animationSprites, position),
      CustomDynamicPhysicalObject(CollisionMasks::CHARACTER,
                                  CollisionFilters::CHARACTERS,
                                  new btSphereShape(1), 1) {
  this->lifePoints = lifePoints;
}
CustomGameCharacter::CustomGameCharacter(
    std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>> animationSprites,
    CustomSDLRect position, int lifePoints)
    : CustomAnimatedSDLMaterialObject(animationSprites, position),
      CustomDynamicPhysicalObject(CollisionMasks::CHARACTER,
                                  CollisionFilters::CHARACTERS,
                                  new btSphereShape(1), 1) {
  this->lifePoints = lifePoints;
}
CustomGameCharacter::~CustomGameCharacter() {}

void CustomGameCharacter::doPhysics(Uint64 startTick, Uint64 endTick) {
  std::list<Movement* const>::iterator iterator = this->movementList.begin();
  while (iterator != this->movementList.end()) {
    Movement* movement = *iterator;
    if (movement->getEndTick() < startTick) {
      iterator = this->movementList.erase(iterator);
      delete movement;
      continue;
    }
    iterator++;
    switch (movement->getType()) {
      case MOVEMENT_TYPE::WALK:
        this->rigidBody->setLinearVelocity(btVector3(movement->getVelocityX(),
                      this->rigidBody->getLinearVelocity().getY(), 0));
        break;
    }
  }
  // erase movements that are past the ticks
}

/// @brief
/// @param texture
/// @param srcRect
/// @param position
/// @param lifePoints
/// @param normalSpeed
CustomPlayer::CustomPlayer(
    CustomTextureManager texture,
    std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>> animationSprites,
    CustomSDLRect position,

    int lifePoints, int normalSpeed)
    : CustomGameCharacter(texture, animationSprites, position, lifePoints) {
  this->normalSpeed = normalSpeed;
}
CustomPlayer::CustomPlayer(
    std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>> animationSprites,
    CustomSDLRect position, int lifePoints, int normalSpeed)
    : CustomGameCharacter(animationSprites, position, lifePoints) {
  this->normalSpeed = normalSpeed;
}
CustomPlayer::~CustomPlayer() {}

inline bool CustomGameCharacter::canMove() const { return true; }

inline bool CustomGameCharacter::canJump() const { return true; }

inline bool CustomGameCharacter::isJumping() const { return false; }
// calcular com base em atributos
inline int CustomGameCharacter::getJumpForce() const { return 10; }

void CustomPlayer::handleEvent(CustomEvent event) {
  bool jumping = this->isJumping();
  if (event.getAction() == PLAYER_ACTION::PLAYER_JUMP_KEY_PRESSED) {
    if (this->canJump()) {
      this->addMovement(new Jump(this->getJumpForce(),
                          this->getDestination().getPoint(),
                          event.getInitialTick(), event.getEndTick()));
    }
  }
  if (event.getAction() == PLAYER_ACTION::PLAYER_IDLE_INPUT) {
    this->changeAnimation(ANIMATION_TYPE::IDLE,
                          this->currentAnimationDirection);
  } else {
    if (event.getAction() == PLAYER_ACTION::PLAYER_RIGHT_KEY_PRESSED) {
      this->addMovement(new Walk(this->normalSpeed, Direction::RIGHT,
                          this->getDestination().getPoint(),
                          event.getInitialTick(), event.getEndTick()));
      this->changeAnimation(ANIMATION_TYPE::WALKING, Direction::RIGHT);
    } else if (event.getAction() == PLAYER_ACTION::PLAYER_LEFT_KEY_PRESSED) {
      this->addMovement(new Walk(this->normalSpeed, Direction::LEFT,
                          this->getDestination().getPoint(),
                          event.getInitialTick(), event.getEndTick()));
      this->changeAnimation(ANIMATION_TYPE::WALKING, Direction::LEFT);
    }
  }
}
