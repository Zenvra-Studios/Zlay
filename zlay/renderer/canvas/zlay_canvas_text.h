#ifndef ZLAY_RENDERER_CANVAS_ZLAY_CANVAS_TEXT_H
#define ZLAY_RENDERER_CANVAS_ZLAY_CANVAS_TEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <zlay.h>

typedef enum ZLay_TextAlign {
  ZLAY_TEXT_ALIGN_LEFT = 0,
  ZLAY_TEXT_ALIGN_CENTER,
  ZLAY_TEXT_ALIGN_RIGHT
} ZLay_TextAlign;

typedef struct ZLay_TextStyle {
  void* font;
  ZLay_String font_family;
  float size;
  ZLay_Color color;
  ZLay_TextAlign align;
} ZLay_TextStyle;

ZLAY_API ZLay_TextStyle ZLay_TextStyleDefault(void);
ZLAY_API ZLay_TextStyle ZLay_TextStyleMake(ZLay_Color color, float size);
ZLAY_API ZLay_TextStyle ZLay_TextStyleWithFontFamily(ZLay_TextStyle style, const char* family);
ZLAY_API ZLay_TextStyle ZLay_TextStyleWithFontFamilyLen(ZLay_TextStyle style, const char* family, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif
