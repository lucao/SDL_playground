#ifndef CUSTOM_TEXTURE_H
#define CUSTOM_TEXTURE_H

#include <SDL.h>

#include <CustomGameObjects.hpp>
#include <string>
#include <unordered_map>''

class CustomTextureManager {
 private:
  std::unordered_map<ANIMATION_TYPE, SDL_Texture*> textures;

 public:
  CustomTextureManager(
      std::unordered_map<ANIMATION_TYPE, SDL_Texture*> textures);

  virtual ~CustomTextureManager();

  SDL_Texture* getTexture(ANIMATION_TYPE animationType);
  SDL_Texture* getDefaultTexture();
};

#endif
