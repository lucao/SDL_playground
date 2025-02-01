#ifndef CUSTOM_PLAYER_BUILDER_H
#define CUSTOM_PLAYER_BUILDER_H

#include <CustomGameCharacters.hpp>
#include <string>

enum PLAYER_CLASS { BARBARIAN, HUNTER, CLERIC, MAGE, DRUID, ROGUE };

class CustomPlayerBuilder {
 private:
  std::string name;
  PLAYER_CLASS player_class;

  CustomPlayer* player;

 public:
  CustomPlayerBuilder(SDL_Renderer* renderer, PLAYER_CLASS player_class,
                      std::string name, b2WorldId worldId);
  CustomPlayer* getPlayerCharacter();
};

class CustomRoguePlayer : public CustomPlayer {

 public:
  CustomRoguePlayer(CustomTextureManager* customTextureManager,
                    std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>>
                        animationSprites,
                    CustomSDLRect size, b2WorldId worldId);
};
#endif