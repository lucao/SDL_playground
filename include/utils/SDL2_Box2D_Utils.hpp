#ifndef SDL2_BOX2D_UTILS
#define SDL2_BOX2D_UTILS

//TODO check for monitor ratio
//const int pixelsRatio = 16; // 16 pixels is equivalent to 1 meter

const int pixelToMeterProportionHeight = 16 ;  
const int pixelToMeterProportionWidth =  9; 

constexpr float convertMetersToPixelsX(float meters) {
  return meters * pixelToMeterProportionWidth;
}

constexpr float convertMetersToPixelsY(float meters) {
  return meters * pixelToMeterProportionHeight;
}

constexpr float convertPixelsToMetersX(int pixels) {
  return static_cast<float>(pixels) / pixelToMeterProportionWidth;
}

constexpr float convertPixelsToMetersY(int pixels) {
  return static_cast<float>(pixels) / pixelToMeterProportionHeight;
}

#endif