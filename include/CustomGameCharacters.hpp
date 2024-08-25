#ifndef CUSTOM_GAME_CHARACTERS_H
#define CUSTOM_GAME_CHARACTERS_H

#include <CustomGameObjects.hpp>
#include <CustomGameUtils.hpp>
#include <CustomPhysics.hpp>
#include <CustomSDLObject.hpp>

class CustomGameCharacter : public CustomAnimatedSDLMaterialObject,
                            public CustomDynamicPhysicalObject,
                            public GameObject {
 private:
  int lifePoints;

 public:
  CustomGameCharacter(CustomTextureManager texture,
                      std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>>
                          animationSprites,
                      CustomSDLRect position, int lifePoints);
  CustomGameCharacter(std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>>
                          animationSprites,
                      CustomSDLRect position, int lifePoints);
  virtual ~CustomGameCharacter();

  void doPhysics(Uint64 startTick, Uint64 endTick) override;
};

class CustomPlayer : public CustomGameCharacter,
                     public EventListener,
                     public CustomPlayerMovement {
 private:
  int normalSpeed;

 public:
  CustomPlayer(CustomTextureManager textureManager,
               std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>>
                   animationSprites,
               CustomSDLRect position, int lifePoints, int normalSpeed);
  CustomPlayer(std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>>
                   animationSprites,
               CustomSDLRect position, int lifePoints, int normalSpeed);
  ~CustomPlayer();

  bool canMove() const override;

  bool canJump() const override;
  bool isJumping() const override;
  int getJumpForce() const override;

  void handleEvent(CustomEvent event) override;
};

#endif