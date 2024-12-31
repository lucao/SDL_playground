#include <iostream>
#include <valarray>

int main(int, char**) {
  std::valarray<std::valarray<int>> peripheralRegionsModifiers{
    //[0] = up/down, [1] = left/right
    // modifiers {0=middle, 1=up/right, -1=down/left}
    {1,0}, {0,1}, {-1,0}, {0,-1}, {1,1}, {1,-1}, {-1,1}, {-1,-1}};

    std::valarray<int> actualRegion = {100,100};
    for (std::valarray<int> peripheralRegion: (actualRegion + peripheralRegionsModifiers)) { 
      if (peripheralRegion[0] == 1){
        return 1;
      }
    }


    //TODO test learn pointers
    int* p1 = new int(1);
    int* p2 = new int(1);
    if (*p1 != *p2) return 2;
  return 0;
}