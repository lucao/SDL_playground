
#include <CustomGameCharacters.hpp>

CustomGameCharacter::CustomGameCharacter(
    SDL_Texture* texture,
    std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>> animationSprites,
    CustomSDLRect position, int lifePoints)
    : CustomAnimatedSDLMaterialObject(texture, animationSprites, position),
      CustomDynamicPhysicalObject(CollisionMasks::CHARACTER,
                                  CollisionFilters::CHARACTERS,
                                  new btCollisionObject()) {
  this->lifePoints = lifePoints;
}
CustomGameCharacter::CustomGameCharacter(
    std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>> animationSprites,
    CustomSDLRect position, int lifePoints)
    : CustomAnimatedSDLMaterialObject(animationSprites, position),
      CustomDynamicPhysicalObject(CollisionMasks::CHARACTER,
                                  CollisionFilters::CHARACTERS,
                                  new btCollisionObject()) {
  this->lifePoints = lifePoints;
}
CustomGameCharacter::~CustomGameCharacter() {}

void CustomGameCharacter::doPhysics(Uint64 startTick, Uint64 endTick) {
  std::list<Movement*>::iterator iterator = this->movementList.begin();
  while (iterator != this->movementList.end()) {
    Movement* movement = *iterator;
    if (movement->getEndTick() < startTick) {
      iterator = this->movementList.erase(iterator);
      continue;
    }
    iterator++;
    switch (movement->getType()) {
      case MOVEMENT_TYPE::WALK:
        this->setDestination(movement->getEndPoint());
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
    SDL_Texture* texture,
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

inline bool CustomPlayer::canMove() const { return true; }

inline bool CustomPlayer::canJump() const { return true; }

inline bool CustomPlayer::isJumping() const { return false; }
// calcular com base em atributos
inline int CustomPlayer::getJumpForce() const { return 10; }

void CustomPlayer::handleEvent(CustomEvent event) {
  bool jumping = this->isJumping();
  if (event.getAction() == PLAYER_ACTION::PLAYER_JUMP_KEY_PRESSED) {
    if (this->canJump()) {
      this->move(new Jump(this->getJumpForce(),
                          this->getDestination().getPoint(),
                          event.getInitialTick(), event.getEndTick()));
    }
  }

  if (this->canMove()) {
    if (event.getAction() == PLAYER_ACTION::PLAYER_RIGHT_KEY_PRESSED) {
      this->move(new Walk(this->normalSpeed, Direction::RIGHT,
                          this->getDestination().getPoint(),
                          event.getInitialTick(), event.getEndTick()));
    } else if (event.getAction() == PLAYER_ACTION::PLAYER_LEFT_KEY_PRESSED) {
      this->move(new Walk(this->normalSpeed, Direction::LEFT,
                          this->getDestination().getPoint(),
                          event.getInitialTick(), event.getEndTick()));
    }
  }
}
