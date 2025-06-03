#include <CustomGameComponents.hpp>
#include <SDL2_Box2D_Utils.hpp>

#include "CustomPhysics.hpp"

CustomGroundPlane::CustomGroundPlane(CustomTextureManager* textureManager,
                                     CustomSDLRect position, b2BodyDef bodyDef,
                                     b2ShapeDef shapeDef, b2WorldId worldId)
    : PositionObject(convertPixelsToMetersX(position.x),
                     -convertPixelsToMetersY(position.y),
                     convertPixelsToMetersX(position.w),
                     convertPixelsToMetersY(position.h)),
      CustomPhysicalObject(
          bodyDef, shapeDef,
          b2Vec2({convertPixelsToMetersX(position.getCenter().x),
                  -convertPixelsToMetersY(position.getCenter().y)}),
          b2MakeBox(convertPixelsToMetersX(position.w) / 2,
                    convertPixelsToMetersY(position.h) / 2),
          worldId),
      CustomSDLMaterialObject(textureManager, CustomSDLRect({0, 0, 1, 1}),
                              position),
      GameObject(GAME_TAGS::OBJECT) {}

CustomGroundPlane::CustomGroundPlane(CustomTextureManager* textureManager,
                                     CustomSDLRect position, b2WorldId worldId)
    : PositionObject(convertPixelsToMetersX(position.x),
                     -convertPixelsToMetersY(position.y),
                     convertPixelsToMetersX(position.w),
                     convertPixelsToMetersY(position.h)),
      CustomPhysicalObject(
          b2DefaultBodyDef(), b2DefaultShapeDef(),
          b2Vec2({convertPixelsToMetersX(position.getCenter().x),
                  -convertPixelsToMetersY(position.getCenter().y)}),
          b2MakeBox(convertPixelsToMetersX(position.w) / 2,
                    convertPixelsToMetersY(position.h) / 2),
          worldId),
      CustomSDLMaterialObject(textureManager, CustomSDLRect({0, 0, 1, 1}),
                              position),
      GameObject(GAME_TAGS::OBJECT) {}

void CustomGroundPlane::doPhysics(Uint64 startTick, Uint64 endTick) {
  // TODO
}

void CustomGroundPlane::afterSimulation(Uint64 startTick, Uint64 endTick) {
  // TODO
}

const CustomSDLRect CustomGroundPlane::getDestination() {
  // TODO
}
