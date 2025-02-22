#include "CustomPlayerBuilder.hpp"

#include <SDL_pixels.h>

#include <unordered_map>
#include <vector>

#include "CustomGameCharacters.hpp"
#include "CustomGameUtils.hpp"
#include "CustomSDLObjects.hpp"
#include "CustomTexture.hpp"
#include "SDL_rect.h"
#include "SDL_render.h"

CustomPlayerBuilder::CustomPlayerBuilder(SDL_Renderer* const renderer,
                                         PLAYER_CLASS player_class,
                                         std::string name, b2WorldId worldId) {
  this->name = name;
  this->player_class = player_class;

  if (this->player_class == PLAYER_CLASS::ROGUE) {
    // default test texture for rogue
    SDL_Surface* surface = SDL_CreateRGBSurface(0, 1, 1, 32, 0, 0, 1, 1);
    SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, 1, 0, 1));
    SDL_Texture* rogueTexture = SDL_CreateTextureFromSurface(renderer, surface);

    std::unordered_map<ANIMATION_TYPE, SDL_Texture*> textures;
    textures[ANIMATION_TYPE::NO_ANIMATION] = rogueTexture;
    textures[ANIMATION_TYPE::IDLE] = rogueTexture;
    textures[ANIMATION_TYPE::WALKING] = rogueTexture;
    textures[ANIMATION_TYPE::RUNNING] = rogueTexture;

    std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>> animationSprites;
    animationSprites[ANIMATION_TYPE::IDLE] = {{0, 0, 50, 50}};
    animationSprites[ANIMATION_TYPE::NO_ANIMATION] = {{0, 0, 50, 50}};
    animationSprites[ANIMATION_TYPE::WALKING] = {{0, 0, 50, 50}};
    animationSprites[ANIMATION_TYPE::RUNNING] = {{0, 0, 50, 50}};

    this->player = new CustomRoguePlayer(new CustomTextureManager(textures),
                                         animationSprites,
                                         CustomSDLRect(0, 0, 50, 50), worldId);
  }
}

CustomPlayer* CustomPlayerBuilder::getPlayerCharacter() { return this->player; }

CustomRoguePlayer::CustomRoguePlayer(
    CustomTextureManager* customTextureManager,
    std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>> animationSprites,
    CustomSDLRect size, b2WorldId worldId)
    : CustomPlayer(customTextureManager, animationSprites, size, worldId) {
  ;
}
