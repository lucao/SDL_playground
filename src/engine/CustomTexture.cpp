#include "CustomTexture.hpp"

#include <unordered_map>

#include "CustomGameUtils.hpp"
#include "SDL_render.h"

CustomTextureManager::CustomTextureManager(SDL_Texture* static_texture) {
  this->textures[ANIMATION_TYPE::NO_ANIMATION] = static_texture;
}

CustomTextureManager::CustomTextureManager(
    std::unordered_map<ANIMATION_TYPE, SDL_Texture*> textures) {
  this->textures = textures;
}

CustomTextureManager::~CustomTextureManager() {
  for (auto texture : this->textures) {
    SDL_DestroyTexture(texture.second);
  }
}

SDL_Texture* CustomTextureManager::getTexture(ANIMATION_TYPE animationType) {
  return this->textures.at(animationType);
}

SDL_Texture* CustomTextureManager::getTexture() {
  return this->textures.at(ANIMATION_TYPE::NO_ANIMATION);
}
