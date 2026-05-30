#ifndef ZLAY_RENDERER_CANVAS_ZLAY_CANVAS_TRANSFORM_H
#define ZLAY_RENDERER_CANVAS_ZLAY_CANVAS_TRANSFORM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <zlay.h>

typedef struct ZLay_CanvasTransform {
  float m00;
  float m01;
  float m02;
  float m10;
  float m11;
  float m12;
} ZLay_CanvasTransform;

ZLAY_API ZLay_CanvasTransform ZLay_CanvasTransformIdentity(void);
ZLAY_API ZLay_CanvasTransform ZLay_CanvasTransformTranslate(float x, float y);
ZLAY_API ZLay_CanvasTransform ZLay_CanvasTransformScale(float sx, float sy);
ZLAY_API ZLay_CanvasTransform ZLay_CanvasTransformRotate(float radians);
ZLAY_API ZLay_CanvasTransform ZLay_CanvasTransformMultiply(ZLay_CanvasTransform a, ZLay_CanvasTransform b);
ZLAY_API ZLay_Vec2 ZLay_CanvasTransformPoint(ZLay_CanvasTransform transform, ZLay_Vec2 point);
ZLAY_API ZLay_Rect ZLay_CanvasTransformRect(ZLay_CanvasTransform transform, ZLay_Rect rect);
ZLAY_API bool ZLay_CanvasTransformIsIdentity(ZLay_CanvasTransform transform);

#ifdef __cplusplus
}
#endif

#endif
