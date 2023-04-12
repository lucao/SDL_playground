#include <SDL.h>
#include <assert.h>
#include <stdio.h>

#include <CustomSDLObject.hpp>
#include <Stage.hpp>
#include <iostream>
#include <platform.hpp>

int custom_main(int, char**);

#ifdef RUNNING_ON_WINDOWS
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
  return custom_main(0, 0);
}
#else

int main(int, char**) { return custom_main(0, 0); }
#endif

void testRegionGetSrc1();
void testRegionGetSrc2();
void testRegionGetDest();

int custom_main(int, char**) {
  testRegionGetSrc1();
  // testRegionGetSrc2();
  return 0;
}

/*std::unordered_set<CustomSDLMaterialObject*> objectsOnRegion,
         CustomSDLRect* rect, BackgroundSDLTexture* background*/
void testRegionGetSrc1() {
  Region* region = new Region(
      {}, new CustomSDLRect(new SDL_Rect({-1000, -1000, 2000, 2000})), NULL);

  std::shared_ptr<CustomSDLRect> resultRect = region->getSrcRect(
      new CustomSDLRect(new SDL_Rect({-500, -500, 1000, 1000})));

  assert((resultRect->x == 500));
  assert((resultRect->y == 500));
  assert((resultRect->w == 1000));
  assert((resultRect->h == 1000));

  delete region;
}
void testRegionGetSrc2() {
  Region* region = new Region(
      {}, new CustomSDLRect(new SDL_Rect({-1000, -1000, 2000, 2000})), NULL);

  std::shared_ptr<CustomSDLRect> resultRect = region->getSrcRect(
      new CustomSDLRect(new SDL_Rect({500, 500, 1000, 1000})));

  assert((resultRect->x == 1500));
  assert((resultRect->y == 1500));
  assert((resultRect->w == 500));
  assert((resultRect->h == 500));

  delete region;
}

void testRegionGetDest() {
  Region* region = new Region(
      {}, new CustomSDLRect(new SDL_Rect({-1000, -1000, 2000, 2000})), NULL);

  std::shared_ptr<CustomSDLRect> resultRect = region->getDestinationRect(
      new CustomSDLRect(new SDL_Rect({-500, -500, 1000, 1000})));

  assert(resultRect->x == 0);
  assert(resultRect->y == 0);
  assert(resultRect->w == 1000);
  assert(resultRect->h == 1000);

  delete region;
}