#include "CustomPlayerBuilder.hpp"

#include <unordered_map>
#include <vector>

#include "CustomGameCharacters.hpp"
#include "CustomGameUtils.hpp"
#include "CustomSDLObject.hpp"
#include "CustomTexture.hpp"
#include "SDL_rect.h"
#include "SDL_render.h"
#include <SDL_pixels.h>

CustomPlayerBuilder::CustomPlayerBuilder(SDL_Renderer* const renderer,
                                         PLAYER_CLASS player_class,
                                         std::string name) {
  this->name = name;
  this->player_class = player_class;

  if (this->player_class == PLAYER_CLASS::ROGUE) {
    // default test texture for rogue
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 1, 1, 32, 0, 0, 1, 0);
    SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, 1, 1, 1));
    SDL_Texture* rogueTexture = SDL_CreateTextureFromSurface(renderer, surface);

    std::unordered_map<ANIMATION_TYPE, SDL_Texture*> textures;
    textures[ANIMATION_TYPE::IDLE] = rogueTexture;
    textures[ANIMATION_TYPE::WALKING] = rogueTexture;
    textures[ANIMATION_TYPE::RUNNING] = rogueTexture;

    std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>> animationSprites;
    animationSprites[ANIMATION_TYPE::IDLE] = {{0,0,10,10}};

    this->player = new CustomRoguePlayer(new CustomTextureManager(textures),
                                         animationSprites);
  }
}

CustomPlayer* CustomPlayerBuilder::getPlayerCharacter() { return this->player; }

CustomRoguePlayer::CustomRoguePlayer(
    CustomTextureManager* customTextureManager,
    std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>> animationSprites)
    : CustomPlayer(
          customTextureManager,
          animationSprites,
          CustomSDLRect(0, 0, this->size.w, this->size.h)) {
  ;
}
