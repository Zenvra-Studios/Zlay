#include <os/zlay_dpi.h>

float ZLay_DpiScaleFromDpi(float dpi) {
  return dpi > 0.0f ? dpi / ZLAY_DPI_BASELINE : 1.0f;
}

float ZLay_DpiFromScale(float scale) {
  return scale > 0.0f ? scale * ZLAY_DPI_BASELINE : ZLAY_DPI_BASELINE;
}

uint32_t ZLay_DpiScalePixels(uint32_t pixels, float scale) {
  if (scale <= 0.0f) {
    scale = 1.0f;
  }
  return (uint32_t)((float)pixels * scale + 0.5f);
}
