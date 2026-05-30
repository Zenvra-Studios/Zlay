#include <renderer/canvas/zlay_canvas_text.h>

#include <string.h>

ZLay_TextStyle ZLay_TextStyleDefault(void) {
  ZLay_TextStyle style;
  style.font = NULL;
  style.font_family = ZLAY_STRING_LITERAL(ZLAY_DEFAULT_FONT_FAMILY);
  style.size = 16.0f;
  style.color = (ZLay_Color){0u, 0u, 0u, 255u};
  style.align = ZLAY_TEXT_ALIGN_LEFT;
  return style;
}

ZLay_TextStyle ZLay_TextStyleMake(ZLay_Color color, float size) {
  ZLay_TextStyle style = ZLay_TextStyleDefault();
  style.color = color;
  style.size = size;
  return style;
}

ZLay_TextStyle ZLay_TextStyleWithFontFamily(ZLay_TextStyle style, const char* family) {
  if (!family) {
    style.font_family = (ZLay_String){0};
    return style;
  }
  return ZLay_TextStyleWithFontFamilyLen(style, family, (uint32_t)strlen(family));
}

ZLay_TextStyle ZLay_TextStyleWithFontFamilyLen(ZLay_TextStyle style, const char* family, uint32_t length) {
  style.font_family.chars = family;
  style.font_family.length = family ? length : 0u;
  style.font_family.is_static = false;
  return style;
}
