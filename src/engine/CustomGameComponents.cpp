#include <CustomGameComponents.hpp>
#include <SDL2_Box2D_Utils.hpp>

#include "CustomPhysics.hpp"

CustomGroundPlane::CustomGroundPlane(CustomTextureManager* textureManager,
                                     CustomSDLRect destination,
                                     b2BodyDef bodyDef,
                                     b2ShapeDef shapeDef, b2WorldId worldId)
    : CustomPhysicalObject(
          bodyDef, shapeDef,
          b2Vec2({convertPixelsToMeters(destination.getCenter().x),
                  -convertPixelsToMeters(destination.getCenter().y)}),
          b2MakeBox(convertPixelsToMeters(destination.w) / 2,
                    convertPixelsToMeters(destination.h) / 2),
          worldId),
      CustomSDLMaterialObject(textureManager, CustomSDLRect({0, 0, 1, 1}),
                              destination),
      GameObject(GAME_TAGS::OBJECT) {}

CustomGroundPlane::CustomGroundPlane(CustomTextureManager* textureManager,
                                     CustomSDLRect destination,
                                     b2WorldId worldId)
    : CustomPhysicalObject(
          b2DefaultBodyDef(), b2DefaultShapeDef(),
          b2Vec2({convertPixelsToMeters(destination.getCenter().x),
                  -convertPixelsToMeters(destination.getCenter().y)}),
          b2MakeBox(convertPixelsToMeters(destination.w) / 2,
                    convertPixelsToMeters(destination.h) / 2),
          worldId),
      CustomSDLMaterialObject(textureManager, CustomSDLRect({0, 0, 1, 1}),
                              destination),
      GameObject(GAME_TAGS::OBJECT) {}

void CustomGroundPlane::doPhysics(Uint64 startTick, Uint64 endTick) {
  // TODO
}

void CustomGroundPlane::afterSimulation(Uint64 startTick, Uint64 endTick) {
  // TODO
}
