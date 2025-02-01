
#include <CustomGameCharacters.hpp>
#include <SDL2_Box2D_Utils.hpp>

CustomGameCharacter::CustomGameCharacter(
    GAME_TAGS tag, CustomTextureManager* texture,
    std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>> animationSprites,
    CustomSDLRect position, b2BodyDef bodyDef, b2ShapeDef shapeDef,
    b2WorldId worldId, int lifePoints, int normalSpeed)
    : CustomAnimatedSDLMaterialObject(texture, animationSprites, position),
      CustomDynamicPhysicalObject(
          bodyDef, shapeDef,
          b2Vec2({convertPixelsToMeters(position.getCenter().x),
                  -convertPixelsToMeters(position.getCenter().y)}),
          b2MakeBox(convertPixelsToMeters(position.w) / 2,
                    convertPixelsToMeters(position.h) / 2),
          worldId),
      GameObject(tag) {
  this->lifePoints = lifePoints;
  this->normalSpeed = normalSpeed;
}

CustomGameCharacter::CustomGameCharacter(
    CustomTextureManager* texture,
    std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>> animationSprites,
    CustomSDLRect position, b2BodyDef bodyDef, b2ShapeDef shapeDef,
    b2WorldId worldId, int lifePoints, int normalSpeed)
    : CustomAnimatedSDLMaterialObject(texture, animationSprites, position),
      CustomDynamicPhysicalObject(
          bodyDef, shapeDef,
          b2Vec2({convertPixelsToMeters(position.getCenter().x),
                  -convertPixelsToMeters(position.getCenter().y)}),
          b2MakeBox(convertPixelsToMeters(position.w) / 2,
                    convertPixelsToMeters(position.h) / 2),
          worldId),
      GameObject(GAME_TAGS::NPC) {
  this->lifePoints = lifePoints;
  this->normalSpeed = normalSpeed;
}

CustomGameCharacter::~CustomGameCharacter() {}

void CustomGameCharacter::doPhysics(Uint64 startTick, Uint64 endTick) {
  std::list<Movement*>::iterator iterator = this->movementList.begin();

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
          //TODO walk
        continue;
        // TODO jump
    }
  }
  // erase movements that are past the ticks
}

void CustomGameCharacter::afterSimulation(Uint64 startTick, Uint64 endTick) {
  b2Vec2 position = b2Body_GetPosition(this->bodyId);
  this->x +=
      convertMetersToPixels(position.x) - this->getDestination().getCenter().x;
  this->y +=
      -convertMetersToPixels(position.y) - this->getDestination().getCenter().y;
}

CustomPlayer::CustomPlayer(
    CustomTextureManager* texture,
    std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>> animationSprites,
    CustomSDLRect position, b2WorldId worldId)
    : CustomGameCharacter(GAME_TAGS::PLAYER, texture, animationSprites,
                          position, b2DefaultBodyDef(), b2DefaultShapeDef(),
                          worldId, 10, 100) {}
CustomPlayer::~CustomPlayer() {}

bool CustomGameCharacter::canMove() const { return true; }

bool CustomGameCharacter::canJump() const { return true; }

bool CustomGameCharacter::isJumping() const { return false; }

int CustomGameCharacter::getJumpForce() const { return 10; }

void CustomPlayer::handleEvent(CustomEvent* event) {
  bool jumping = this->isJumping();
  if (event->getAction() == PLAYER_ACTION::PLAYER_JUMP_KEY_PRESSED) {
    if (this->canJump()) {
      this->addMovement(new Jump(this->getJumpForce(),
                                 this->getDestination().getPoint(),
                                 event->getInitialTick(), event->getEndTick()));
    }
  }
  if (event->getAction() == PLAYER_ACTION::PLAYER_IDLE_INPUT) {
    this->changeAnimation(ANIMATION_TYPE::IDLE,
                          this->currentAnimationDirection);
  } else {
    if (event->getAction() == PLAYER_ACTION::PLAYER_RIGHT_KEY_PRESSED) {
      this->addMovement(new Walk(this->normalSpeed, Direction::RIGHT,
                                 this->getDestination().getPoint(),
                                 event->getInitialTick(), event->getEndTick()));
      this->changeAnimation(ANIMATION_TYPE::WALKING, Direction::RIGHT);
    } else if (event->getAction() == PLAYER_ACTION::PLAYER_LEFT_KEY_PRESSED) {
      this->addMovement(new Walk(this->normalSpeed, Direction::LEFT,
                                 this->getDestination().getPoint(),
                                 event->getInitialTick(), event->getEndTick()));
      this->changeAnimation(ANIMATION_TYPE::WALKING, Direction::LEFT);
    }
  }
}
