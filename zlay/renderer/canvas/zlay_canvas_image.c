#include <renderer/canvas/zlay_canvas_image.h>

ZLay_ImageHandle ZLay_ImageHandleMake(uint64_t id) {
  ZLay_ImageHandle handle;
  handle.id = id;
  return handle;
}

bool ZLay_ImageHandleIsValid(ZLay_ImageHandle image) {
  return image.id != 0u;
}

ZLay_CanvasImageOptions ZLay_CanvasImageOptionsDefault(void) {
  ZLay_CanvasImageOptions options;
  options.source_rect = (ZLay_Rect){0};
  options.tint = (ZLay_Color){255u, 255u, 255u, 255u};
  return options;
}
