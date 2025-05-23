
#include <CustomGameCharacters.hpp>
#include <SDL2_Box2D_Utils.hpp>

CustomGameCharacter::CustomGameCharacter(
    GAME_TAGS tag, CustomTextureManager* texture,
    std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>> animationSprites,
    CustomSDLRect position, b2BodyDef bodyDef, b2ShapeDef shapeDef,
    b2WorldId worldId, int lifePoints, int normalSpeed)
    : PositionObject(convertPixelsToMetersX(position.x),
                     -convertPixelsToMetersY(position.y),
                     convertPixelsToMetersX(position.w),
                     convertPixelsToMetersY(position.h)),
      CustomAnimatedSDLMaterialObject(texture, animationSprites, position),
      CustomDynamicPhysicalObject(
          bodyDef, shapeDef,
          b2Vec2({convertPixelsToMetersX(position.getCenter().x),
                  -convertPixelsToMetersY(position.getCenter().y)}),
          b2Capsule(
              {b2Vec2({0, convertPixelsToMetersY(position.h) -
                              (2 * convertPixelsToMetersX(position.w)) / 2}),
               b2Vec2({0, -(convertPixelsToMetersY(position.h) -
                              (2 * convertPixelsToMetersX(position.w)) / 2)}),
               convertPixelsToMetersX(position.w) / 2}),
          worldId),
      GameObject(tag) {  // TODO verificar se essa construção da capsula está OK
  this->lifePoints = lifePoints;
  this->normalSpeed = normalSpeed;
  this->moving = false;
  this->grounded = true;  // TODO should be false... Is setted true for testing
}

CustomGameCharacter::CustomGameCharacter(
    CustomTextureManager* texture,
    std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>> animationSprites,
    CustomSDLRect position, b2BodyDef bodyDef, b2ShapeDef shapeDef,
    b2WorldId worldId, int lifePoints, int normalSpeed)
    : PositionObject(convertPixelsToMetersX(position.x),
                     -convertPixelsToMetersY(position.y),
                     convertPixelsToMetersX(position.w),
                     convertPixelsToMetersY(position.h)),
      CustomAnimatedSDLMaterialObject(texture, animationSprites, position),
      CustomDynamicPhysicalObject(
          bodyDef, shapeDef,
          b2Vec2({convertPixelsToMetersX(position.getCenter().x),
                  -convertPixelsToMetersY(position.getCenter().y)}),
          b2MakeBox(convertPixelsToMetersX(position.w) / 2,
                    convertPixelsToMetersY(position.h) / 2),
          worldId),
      GameObject(GAME_TAGS::NPC) {
  this->lifePoints = lifePoints;
  this->normalSpeed = normalSpeed;
}

CustomGameCharacter::~CustomGameCharacter() {}

const CustomSDLRect CustomGameCharacter::getDestination() {
  return CustomSDLRect(
      static_cast<int>(convertMetersToPixelsX(std::round(this->x))),
      static_cast<int>(convertMetersToPixelsY(std::round(this->y))),
      static_cast<int>(convertMetersToPixelsX(std::round(this->w))),
      static_cast<int>(convertMetersToPixelsY(std::round(this->h))));
}

void CustomGameCharacter::doPhysics(Uint64 startTick, Uint64 endTick) {
  std::list<Movement*>::iterator iterator = this->movementList.begin();
  auto velocity = b2Body_GetLinearVelocity(this->bodyId);

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
        // TODO
        // b2Body_ApplyForceToCenter(myBodyId, force, wake);
        // b2Body_ApplyLinearImpulseToCenter(myBodyId, linearImpulse, wake);
        if (this->moving) {
          b2Body_ApplyForceToCenter(
              this->bodyId,
              b2Body_GetMass(this->bodyId) *
                  b2Vec2({this->normalSpeed - velocity.x, 0}),
              true);
        } else {
          this->moving = true;
          b2Body_ApplyLinearImpulseToCenter(
              this->bodyId,
              b2Body_GetMass(this->bodyId) *
                  b2Vec2({this->normalSpeed - velocity.x, 0}),
              true);
        }
        /*
         * Use ApplyForce or ApplyLinearImpulse to move the character
         *horizontally.
         *
         *Use ApplyLinearImpulse for instant changes in velocity (e.g.,
         *jumping).
         */
        // TODO jump
        return;
    }
  }
  this->moving = false;
  // erase movements that are past the ticks
}

void CustomGameCharacter::afterSimulation(Uint64 startTick, Uint64 endTick) {
  b2Vec2 position = b2Body_GetPosition(this->bodyId);
  this->x += position.x - this->getDestination().getCenter().x;
  this->y += position.y - this->getDestination().getCenter().y;
}

CustomPlayer::CustomPlayer(
    CustomTextureManager* texture,
    std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>> animationSprites,
    CustomSDLRect position, b2WorldId worldId)
    : CustomGameCharacter(GAME_TAGS::PLAYER, texture, animationSprites,
                          position, b2DefaultBodyDef(), b2DefaultShapeDef(),
                          worldId, 10, 100) {}
CustomPlayer::~CustomPlayer() {}

void CustomPlayer::handleEvent(CustomEvent* event) {
  // TODO interaction key pressed
  if (event->getAction() == PLAYER_ACTION::PLAYER_JUMP_KEY_PRESSED) {
    if (this->grounded) {
      /* TODO
      this->addMovement(new Jump(this->getJumpForce(),
                                 this->getDestination().getPoint(),
                                 event->getInitialTick(), event->getEndTick()));
                                 */
    }
  }
  if (event->getAction() == PLAYER_ACTION::PLAYER_IDLE_INPUT) {
    if (this->moving) {
      this->moving = false;
      // TODO enviar evento de parar personagem
    } else {
      this->changeAnimation(ANIMATION_TYPE::IDLE,
                            this->currentAnimationDirection);
    }

  } else {
    this->moving = true;
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
