#ifndef SDL2_BOX2D_UTILS
#define SDL2_BOX2D_UTILS

const int pixelToMeterProportion = 25;  // 25 pixels is equivalent to 1 meter

constexpr float convertMetersToPixels(float meters) {
  return meters * pixelToMeterProportion;
}

constexpr float convertPixelsToMeters(int pixels) {
  return static_cast<float>(pixels) / pixelToMeterProportion;
}

#endif