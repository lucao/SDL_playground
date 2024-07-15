#ifndef WORLD_H
#define WORLD_H

#include <Stage.hpp>
#include <unordered_map>

class World {
	private:

	std::unordered_map<Stage::StageId, Stage*, Stage::StageId> stages;

	public:
	World();
	~World();
	Stage* createBlankStage(SDL_Renderer* renderer);
	Stage* getStage(Stage::StageId id);
};

#endif