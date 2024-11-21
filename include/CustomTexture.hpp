#ifndef CUSTOM_TEXTURE_H
#define CUSTOM_TEXTURE_H


#include <unordered_map>
#include "CustomGameUtils.hpp"
#include "SDL_render.h"

class CustomTextureManager {
 private:
  std::unordered_map<ANIMATION_TYPE, SDL_Texture*> textures;

 public:
  CustomTextureManager(SDL_Texture* static_texture);
  CustomTextureManager(
      std::unordered_map<ANIMATION_TYPE, SDL_Texture*> textures);

  virtual ~CustomTextureManager();

  virtual SDL_Texture* getTexture(ANIMATION_TYPE animationType);
  virtual SDL_Texture* getTexture();
};



#endif
