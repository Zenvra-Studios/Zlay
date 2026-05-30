#include <renderer/canvas/zlay_canvas_gradient.h>

static void ZLay__CanvasGradientInit(
  ZLay_CanvasGradient* gradient,
  ZLay_CanvasGradientType type,
  ZLay_CanvasGradientStop* stops,
  uint32_t stop_capacity
) {
  if (!gradient) return;
  gradient->type = type;
  gradient->start = (ZLay_Vec2){0.0f, 0.0f};
  gradient->end = (ZLay_Vec2){0.0f, 0.0f};
  gradient->radius = 0.0f;
  gradient->stops = stops;
  gradient->stop_count = 0u;
  gradient->stop_capacity = stop_capacity;
}

void ZLay_CanvasGradientInitLinear(
  ZLay_CanvasGradient* gradient,
  ZLay_CanvasGradientStop* stops,
  uint32_t stop_capacity,
  ZLay_Vec2 start,
  ZLay_Vec2 end
) {
  ZLay__CanvasGradientInit(gradient, ZLAY_CANVAS_GRADIENT_LINEAR, stops, stop_capacity);
  if (!gradient) return;
  gradient->start = start;
  gradient->end = end;
}

void ZLay_CanvasGradientInitRadial(
  ZLay_CanvasGradient* gradient,
  ZLay_CanvasGradientStop* stops,
  uint32_t stop_capacity,
  ZLay_Vec2 center,
  float radius
) {
  ZLay__CanvasGradientInit(gradient, ZLAY_CANVAS_GRADIENT_RADIAL, stops, stop_capacity);
  if (!gradient) return;
  gradient->start = center;
  gradient->end = center;
  gradient->radius = radius;
}

bool ZLay_CanvasGradientAddStop(ZLay_CanvasGradient* gradient, float offset, ZLay_Color color) {
  if (!gradient || !gradient->stops || gradient->stop_count >= gradient->stop_capacity) return false;
  gradient->stops[gradient->stop_count++] = (ZLay_CanvasGradientStop){ZLay_SaturateFloat(offset), color};
  return true;
}
