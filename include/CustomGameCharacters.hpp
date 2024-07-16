#ifndef CUSTOM_GAME_CHARACTERS_H
#define CUSTOM_GAME_CHARACTERS_H

#include <CustomGameObjects.hpp>
#include <CustomPhysics.hpp>
#include <CustomSDLObject.hpp>
#include <CustomGameUtils.hpp>

class CustomGameCharacter : public CustomAnimatedSDLMaterialObject,
                            public CustomDynamicPhysicalObject,
                            public GameObject {
	private:
	int lifePoints;

	public:
	CustomGameCharacter(SDL_Texture* texture, CustomSDLRect* srcRect,
		CustomSDLRect* position, int typesOfAnimation,
		int stepsOfAnimation, int lifePoints);
	CustomGameCharacter(CustomSDLRect* srcRect, CustomSDLRect* position,
		int typesOfAnimation, int stepsOfAnimation,
		int lifePoints);
	virtual ~CustomGameCharacter();
};

class CustomPlayer : public CustomGameCharacter,
                     public EventListener,
                     public CustomPlayerMovement {
       private:
        int normalSpeed;

       public:
        CustomPlayer(SDL_Texture* texture, CustomSDLRect* srcRect,
                     CustomSDLRect* position, int typesOfAnimation,
                     int stepsOfAnimation, int lifePoints, int normalSpeed);
        CustomPlayer(CustomSDLRect* srcRect, CustomSDLRect* position,
                     int typesOfAnimation, int stepsOfAnimation, int lifePoints,
                     int normalSpeed);
        ~CustomPlayer();

        bool canMove() const override;

        bool canJump() const override;
        bool isJumping() const override;
        int getJumpForce() const override;

        void handleEvent(CustomEvent event) override;
};

#endif