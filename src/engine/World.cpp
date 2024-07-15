#include <World.hpp>


World::World() {}

World::~World() {}

Stage* World::createBlankStage(SDL_Renderer* renderer) {
	Stage* blankStage = new Stage({ 0 }, new CustomSDLRect(new SDL_Rect({ -40000, -40000, 80000, 80000 })),
		renderer);

	this->stages [blankStage->getId()] = blankStage;

	return blankStage;
}

Stage* World::getStage(Stage::StageId id) {
	return this->stages.at(id);
}
