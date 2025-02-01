#include <SDL_image.h>

#include <CustomPlayerBuilder.hpp>
#include <World.hpp>

World::World(SDL_Renderer* const renderer) {
  SDL_Surface* surface_idle = IMG_Load(
      "C:\\Users\\lucas\\git\\SDL_"
      "playground\\media\\img\\Woodcutter\\Woodcutter_idle.png");
  // preto de fundo da imagem
  Uint8 r = 255;
  Uint8 g = 255;
  Uint8 b = 255;
  SDL_SetColorKey(surface_idle, SDL_TRUE,
                  SDL_MapRGB(surface_idle->format, r, g, b));
  std::unordered_map<ANIMATION_TYPE, SDL_Texture*> textures;
  textures[ANIMATION_TYPE::IDLE] =
      SDL_CreateTextureFromSurface(renderer, surface_idle);

  SDL_FreeSurface(surface_idle);

  std::unordered_map<ANIMATION_TYPE, std::vector<SDL_Rect>> animationSprites;
  animationSprites[ANIMATION_TYPE::IDLE] = std::vector<SDL_Rect>{
      SDL_Rect{20, 14, 40, 50}, SDL_Rect{65, 14, 40, 50},
      SDL_Rect{115, 14, 40, 50}, SDL_Rect{162, 14, 40, 50}};

  CustomTextureManager* texturesManager = new CustomTextureManager(textures);

  this->physicsControl =
      new Box2DPhysicsControl(60.0f);  // TODO add frameRate to settings

  std::unique_ptr<CustomPlayerBuilder> playerBuilder =
      std::make_unique<CustomPlayerBuilder>(
          CustomPlayerBuilder(renderer, PLAYER_CLASS::ROGUE, "testName",
                              this->physicsControl->getWorldId()));
  this->mainPlayer = playerBuilder->getPlayerCharacter();

  Stage::StageId stageId({1});
  this->stages[stageId] =
      new Stage(stageId, CustomSDLRect({0, 0, 2000, 2000}), renderer);
}

World::~World() {}

Stage* World::createBlankStage(SDL_Renderer* renderer) {
  Stage* blankStage = new Stage(
      {0}, CustomSDLRect(SDL_Rect({-40000, -40000, 80000, 80000})), renderer);

  this->stages[blankStage->getId()] = blankStage;

  return blankStage;
}

Stage* World::getStage(Stage::StageId id) { return this->stages.at(id); }
