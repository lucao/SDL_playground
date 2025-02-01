#ifndef CUSTOM_GAME_COMPONENTS_H
#define CUSTOM_GAME_COMPONENTS_H

#include <CustomGameObjects.hpp>
#include <CustomPhysics.hpp>
#include <CustomSDLObject.hpp>

#include "SDL_stdinc.h"

class CustomGroundPlane : public CustomSDLMaterialObject,
                          public CustomPhysicalObject,
                          public GameObject {
 private:
 public:
  CustomGroundPlane(CustomTextureManager* textureManager, CustomSDLRect destination,
                    b2WorldId worldId);
  CustomGroundPlane(CustomTextureManager* textureManager,
                    CustomSDLRect destination,
                    b2BodyDef bodyDef, b2ShapeDef shapeDef, b2WorldId worldId);
  void doPhysics(Uint64 startTick, Uint64 endTick) override;
  void afterSimulation(Uint64 startTick, Uint64 endTick) override;
};

#endif
