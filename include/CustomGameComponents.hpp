#ifndef CUSTOM_GAME_COMPONENTS_H
#define CUSTOM_GAME_COMPONENTS_H

#include <CustomGameObjects.hpp>
#include <CustomPhysics.hpp>
#include <CustomSDLObjects.hpp>

#include "SDL_stdinc.h"

class CustomGroundPlane : public PositionObject,
                          public CustomSDLMaterialObject,
                          public CustomPhysicalObject,
                          public GameObject {
 private:
 public:
  CustomGroundPlane(CustomTextureManager* textureManager,
                    CustomSDLRect position, b2WorldId worldId);
  CustomGroundPlane(CustomTextureManager* textureManager,
                    CustomSDLRect position, b2BodyDef bodyDef,
                    b2ShapeDef shapeDef, b2WorldId worldId);
  void doPhysics(Uint64 startTick, Uint64 endTick) override;
  void afterSimulation(Uint64 startTick, Uint64 endTick) override;
  const CustomSDLRect getDestination() override;
};

#endif
