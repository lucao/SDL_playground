#include <CustomGameComponents.hpp>

#include "CustomPhysics.hpp"

CustomGroundPlane::CustomGroundPlane(CustomTextureManager* textureManager,
                                     SDL_Rect destination)
    : CustomPhysicalObject(
          CollisionMasks::STATIC_OBJECT, CollisionGroup::STATIC_OBJECTS,
          new btBoxShape(btVector3(btScalar(destination.w / 2),
                                   btScalar(destination.h / 2), 0)),
          btScalar(0.0),
          new btDefaultMotionState(
              btTransform(btQuaternion(0, 0, 0, 1),
                          btVector3(destination.x, destination.y, 0)))),
      CustomSDLMaterialObject(textureManager, SDL_Rect({0, 0, 1, 1}),
                              destination) {}

void CustomGroundPlane::doPhysics(Uint64 startTick, Uint64 endTick) {
  // TODO
}
