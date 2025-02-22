#ifndef CUSTOM_GAME_CHARACTERS_H
#define CUSTOM_GAME_CHARACTERS_H

#include <CustomGameObjects.hpp>
#include <CustomGameUtils.hpp>
#include <CustomPhysics.hpp>
#include <CustomSDLObjects.hpp>

class CustomGameCharacter : public PositionObject,
                            public CustomAnimatedSDLMaterialObject,
                            public CustomDynamicPhysicalObject,
                            public GameObject {
 protected:
  std::string name;
  int lifePoints;
  int normalSpeed;

  bool still;
  bool grounded;

 public:
  CustomGameCharacter(CustomTextureManager* texture,
                      std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>>
                          animationSprites,
                      CustomSDLRect position, b2BodyDef bodyDef,
                      b2ShapeDef shapeDef, b2WorldId worldId, int lifePoints,
                      int normalSpeed);
  CustomGameCharacter(GAME_TAGS tag, CustomTextureManager* texture,
                      std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>>
                          animationSprites,
                      CustomSDLRect position, b2BodyDef bodyDef,
                      b2ShapeDef shapeDef, b2WorldId worldId, int lifePoints,
                      int normalSpeed);
  virtual ~CustomGameCharacter();

  virtual const CustomSDLRect getDestination() override;

  void doPhysics(Uint64 startTick, Uint64 endTick) override;
  void afterSimulation(Uint64 startTick, Uint64 endTick) override;

  virtual bool operator==(const CustomGameCharacter& other) const {
    return this == &other;
  }
};

class CustomPlayer : public CustomGameCharacter, public EventListener {
 public:
  CustomPlayer(CustomTextureManager* textureManager,
               std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>>
                   animationSprites,
               CustomSDLRect position, b2WorldId worldId);
  ~CustomPlayer();

  void handleEvent(CustomEvent* event) override;
};

#endif