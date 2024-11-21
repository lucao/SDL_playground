#include <SDL_image.h>

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
  this->mainPlayer = new CustomPlayer(texturesManager, animationSprites,
                                      CustomSDLRect({0, 0, 40, 50}), 10, 4);
  this->physicsControl = new PhysicsControl();
}

World::~World() {}

Stage* World::createBlankStage(SDL_Renderer* renderer) {
  Stage* blankStage = new Stage(
      {0}, CustomSDLRect(SDL_Rect({-40000, -40000, 80000, 80000})), renderer);

  this->stages[blankStage->getId()] = blankStage;

  return blankStage;
}

Stage* World::getStage(Stage::StageId id) { return this->stages.at(id); }

std::vector<CustomSDLMaterialObject*> World::getMaterialObjects() {
  return this->materialObjects;
}

CustomGroundPlane* World::createDefaultGround(SDL_Texture* static_texture) {
  CustomTextureManager* textureManager =
      new CustomTextureManager(static_texture);
  return new CustomGroundPlane(textureManager, SDL_Rect({20, 200, 200, 200}));
}