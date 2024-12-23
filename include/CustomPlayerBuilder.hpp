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
                      std::string name);
  CustomPlayer* getPlayerCharacter();
};

class CustomRoguePlayer : public CustomPlayer {
 private:
  CustomSDLRect size = CustomSDLRect(0, 0, 50, 50);

 public:
  CustomRoguePlayer(CustomTextureManager* customTextureManager,
                    std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>>
                        animationSprites);
};
#endif