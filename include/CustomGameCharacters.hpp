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
  int lifePoints;
  int normalSpeed;

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

  bool canMove() const;

  bool canJump() const;
  bool isJumping() const;
  int getJumpForce() const;
};

class CustomPlayer : public CustomGameCharacter,
                     public EventListener { 

 public:
  CustomPlayer(CustomTextureManager textureManager,
               std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>>
                   animationSprites,
               CustomSDLRect position, int lifePoints, int normalSpeed);
  CustomPlayer(std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>>
                   animationSprites,
               CustomSDLRect position, int lifePoints, int normalSpeed);
  ~CustomPlayer();

  void handleEvent(CustomEvent event) override;
};

#endif