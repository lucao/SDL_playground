#ifndef CUSTOM_GAME_CHARACTERS_H
#define CUSTOM_GAME_CHARACTERS_H

#include <CustomGameObjects.hpp>
#include <CustomGameUtils.hpp>
#include <CustomPhysics.hpp>
#include <CustomSDLObject.hpp>

class CustomGameCharacter : public CustomAnimatedSDLMaterialObject,
                            public CustomDynamicPhysicalObject,
                            public GameObject {
 protected:
  std::string name;
  int lifePoints;
  int normalSpeed;

 public:
  CustomGameCharacter(CustomTextureManager* texture,
                      std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>>
                          animationSprites,
                      CustomSDLRect position, int lifePoints, int normalSpeed);
  virtual ~CustomGameCharacter();

  void doPhysics(Uint64 startTick, Uint64 endTick) override;

  bool canMove() const;

  bool canJump() const;
  bool isJumping() const;
  int getJumpForce() const;

  virtual CustomSDLRect getDestination() override;
};

class CustomPlayer : public CustomGameCharacter, public EventListener {
 public:
  CustomPlayer(CustomTextureManager* textureManager,
               std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>>
                   animationSprites,
               CustomSDLRect position);
  ~CustomPlayer();

  void handleEvent(CustomEvent event) override;
};

#endif