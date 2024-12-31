
#include <CustomGameCharacters.hpp>

CustomGameCharacter::CustomGameCharacter(
    CustomTextureManager* texture,
    std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>> animationSprites,
    CustomSDLRect position, int lifePoints, int normalSpeed)
    : CustomAnimatedSDLMaterialObject(texture, animationSprites, position),
      CustomDynamicPhysicalObject(
          new btSphereShape(1), btScalar(10),
          new btDefaultMotionState(
              btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 10, 0)))) {
  this->lifePoints = lifePoints;
  this->normalSpeed = normalSpeed;

  // characters cant rotate
  this->rigidBody->setAngularFactor(btVector3(0, 0, 0));
}

CustomGameCharacter::~CustomGameCharacter() {}

void CustomGameCharacter::doPhysics(Uint64 startTick, Uint64 endTick) {
  std::list<Movement*>::iterator iterator = this->movementList.begin();

  this->rigidBody->setLinearVelocity(
      btVector3(static_cast<btScalar>(0),
                this->rigidBody->getLinearVelocity().getY(), 0));

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
        this->rigidBody->setLinearVelocity(
            btVector3(static_cast<btScalar>(movement->getVelocityX()*10),
                      this->rigidBody->getLinearVelocity().getY(), 0));
        continue;
        // TODO jump
    }
  }

  // erase movements that are past the ticks
}

CustomPlayer::CustomPlayer(
    CustomTextureManager* texture,
    std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>> animationSprites,
    CustomSDLRect position)
    : CustomGameCharacter(texture, animationSprites, position, 10, 100) {}
CustomPlayer::~CustomPlayer() {}

bool CustomGameCharacter::canMove() const { return true; }

bool CustomGameCharacter::canJump() const { return true; }

bool CustomGameCharacter::isJumping() const { return false; }

int CustomGameCharacter::getJumpForce() const { return 10; }

CustomSDLRect CustomGameCharacter::getDestination() {
  auto transform = this->getTransform();
  // TODO check for max value for int
  return CustomSDLRect(
      {this->destination.x + static_cast<int>(transform.getOrigin().getX()),
       this->destination.y + static_cast<int>(transform.getOrigin().getY()),
       this->destination.w, this->destination.h});
}

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
