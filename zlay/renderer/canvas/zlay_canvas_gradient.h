#ifndef ZLAY_RENDERER_CANVAS_ZLAY_CANVAS_GRADIENT_H
#define ZLAY_RENDERER_CANVAS_ZLAY_CANVAS_GRADIENT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <zlay.h>

typedef enum ZLay_CanvasGradientType {
  ZLAY_CANVAS_GRADIENT_LINEAR = 0,
  ZLAY_CANVAS_GRADIENT_RADIAL
} ZLay_CanvasGradientType;

typedef struct ZLay_CanvasGradientStop {
  float offset;
  ZLay_Color color;
} ZLay_CanvasGradientStop;

typedef struct ZLay_CanvasGradient {
  ZLay_CanvasGradientType type;
  ZLay_Vec2 start;
  ZLay_Vec2 end;
  float radius;
  ZLay_CanvasGradientStop* stops;
  uint32_t stop_count;
  uint32_t stop_capacity;
} ZLay_CanvasGradient;

ZLAY_API void ZLay_CanvasGradientInitLinear(
  ZLay_CanvasGradient* gradient,
  ZLay_CanvasGradientStop* stops,
  uint32_t stop_capacity,
  ZLay_Vec2 start,
  ZLay_Vec2 end
);
ZLAY_API void ZLay_CanvasGradientInitRadial(
  ZLay_CanvasGradient* gradient,
  ZLay_CanvasGradientStop* stops,
  uint32_t stop_capacity,
  ZLay_Vec2 center,
  float radius
);
ZLAY_API bool ZLay_CanvasGradientAddStop(ZLay_CanvasGradient* gradient, float offset, ZLay_Color color);

#ifdef __cplusplus
}
#endif

#endif
