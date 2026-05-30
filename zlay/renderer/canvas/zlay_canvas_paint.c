#include <renderer/canvas/zlay_canvas_paint.h>

static uint8_t ZLay__CanvasFloatToByte(float value) {
  const float clamped = ZLay_SaturateFloat(value);
  return (uint8_t)(clamped * 255.0f + 0.5f);
}

ZLay_CanvasColor ZLay_CanvasColorRGBA(float r, float g, float b, float a) {
  ZLay_CanvasColor color;
  color.r = r;
  color.g = g;
  color.b = b;
  color.a = a;
  return color;
}

ZLay_CanvasColor ZLay_CanvasColorFromColor(ZLay_Color color) {
  return ZLay_CanvasColorRGBA(
    (float)color.r / 255.0f,
    (float)color.g / 255.0f,
    (float)color.b / 255.0f,
    (float)color.a / 255.0f
  );
}

ZLay_Color ZLay_CanvasColorToColor(ZLay_CanvasColor color) {
  return (ZLay_Color){
    ZLay__CanvasFloatToByte(color.r),
    ZLay__CanvasFloatToByte(color.g),
    ZLay__CanvasFloatToByte(color.b),
    ZLay__CanvasFloatToByte(color.a)
  };
}

ZLay_CanvasPaint ZLay_CanvasPaintNone(void) {
  return (ZLay_CanvasPaint){0};
}

ZLay_CanvasPaint ZLay_CanvasPaintSolid(ZLay_Color color) {
  ZLay_CanvasPaint paint;
  paint.type = ZLAY_PAINT_SOLID;
  paint.color = color;
  paint.image = (ZLay_ImageHandle){0};
  paint.gradient = NULL;
  return paint;
}

ZLay_CanvasPaint ZLay_CanvasPaintSolidRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  return ZLay_CanvasPaintSolid((ZLay_Color){r, g, b, a});
}

ZLay_CanvasPaint ZLay_CanvasPaintFromCanvasColor(ZLay_CanvasColor color) {
  return ZLay_CanvasPaintSolid(ZLay_CanvasColorToColor(color));
}

ZLay_CanvasPaint ZLay_CanvasPaintImage(ZLay_ImageHandle image, ZLay_Color tint) {
  ZLay_CanvasPaint paint;
  paint.type = ZLAY_PAINT_IMAGE;
  paint.color = tint;
  paint.image = image;
  paint.gradient = NULL;
  return paint;
}

bool ZLay_CanvasPaintIsVisible(ZLay_CanvasPaint paint) {
  return paint.type != ZLAY_PAINT_NONE && paint.color.a != 0u;
}

ZLay_CanvasStroke ZLay_CanvasStrokeMake(ZLay_Color color, float width) {
  return ZLay_CanvasStrokeFromPaint(ZLay_CanvasPaintSolid(color), width);
}

ZLay_CanvasStroke ZLay_CanvasStrokeFromPaint(ZLay_CanvasPaint paint, float width) {
  ZLay_CanvasStroke stroke;
  stroke.paint = paint;
  stroke.width = width;
  return stroke;
}
