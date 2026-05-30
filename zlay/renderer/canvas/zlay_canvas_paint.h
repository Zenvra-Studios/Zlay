#ifndef ZLAY_RENDERER_CANVAS_ZLAY_CANVAS_PAINT_H
#define ZLAY_RENDERER_CANVAS_ZLAY_CANVAS_PAINT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <zlay.h>

typedef enum ZLay_PaintType {
  ZLAY_PAINT_NONE = 0,
  ZLAY_PAINT_SOLID,
  ZLAY_PAINT_LINEAR_GRADIENT,
  ZLAY_PAINT_RADIAL_GRADIENT,
  ZLAY_PAINT_IMAGE
} ZLay_PaintType;

typedef struct ZLay_CanvasColor {
  float r;
  float g;
  float b;
  float a;
} ZLay_CanvasColor;

typedef struct ZLay_CanvasPaint {
  ZLay_PaintType type;
  ZLay_Color color;
  ZLay_ImageHandle image;
  void* gradient;
} ZLay_CanvasPaint;

typedef struct ZLay_CanvasStroke {
  ZLay_CanvasPaint paint;
  float width;
} ZLay_CanvasStroke;

ZLAY_API ZLay_CanvasColor ZLay_CanvasColorRGBA(float r, float g, float b, float a);
ZLAY_API ZLay_CanvasColor ZLay_CanvasColorFromColor(ZLay_Color color);
ZLAY_API ZLay_Color ZLay_CanvasColorToColor(ZLay_CanvasColor color);

ZLAY_API ZLay_CanvasPaint ZLay_CanvasPaintNone(void);
ZLAY_API ZLay_CanvasPaint ZLay_CanvasPaintSolid(ZLay_Color color);
ZLAY_API ZLay_CanvasPaint ZLay_CanvasPaintSolidRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
ZLAY_API ZLay_CanvasPaint ZLay_CanvasPaintFromCanvasColor(ZLay_CanvasColor color);
ZLAY_API ZLay_CanvasPaint ZLay_CanvasPaintImage(ZLay_ImageHandle image, ZLay_Color tint);
ZLAY_API bool ZLay_CanvasPaintIsVisible(ZLay_CanvasPaint paint);

ZLAY_API ZLay_CanvasStroke ZLay_CanvasStrokeMake(ZLay_Color color, float width);
ZLAY_API ZLay_CanvasStroke ZLay_CanvasStrokeFromPaint(ZLay_CanvasPaint paint, float width);

#ifdef __cplusplus
}
#endif

#endif
