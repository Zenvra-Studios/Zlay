#ifndef ZLAY_COLOR_H
#define ZLAY_COLOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zlay.h>

typedef struct ZLay_HEX {
  const char* hex;
} ZLay_HEX;

typedef struct ZLay_CMYK {
  float c;
  float m;
  float y;
  float k;
  float a;
} ZLay_CMYK;

typedef struct ZLay_RGB {
  float r;
  float g;
  float b;
} ZLay_RGB;

typedef struct ZLay_RGBA {
  float r;
  float g;
  float b;
  float a;
} ZLay_RGBA;

typedef struct ZLay_HSL {
  float h;
  float s;
  float l;
  float a;
} ZLay_HSL;

typedef struct ZLay_HSV {
  float h;
  float s;
  float v;
  float a;
} ZLay_HSV;

typedef enum ZLay_ColorHexFormat {
  ZLAY_COLOR_HEX_RGB = 0,
  ZLAY_COLOR_HEX_RGBA = 1
} ZLay_ColorHexFormat;

ZLAY_API ZLay_Color ZLay_ColorRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
ZLAY_API ZLay_Color ZLay_ColorRGB(uint8_t r, uint8_t g, uint8_t b);
ZLAY_API ZLay_Color ZLay_ColorTransparent(void);
ZLAY_API ZLay_Color ZLay_ColorWhite(void);
ZLAY_API ZLay_Color ZLay_ColorBlack(void);
ZLAY_API ZLay_Color ZLay_ColorWithAlpha(ZLay_Color color, uint8_t alpha);
ZLAY_API ZLay_Color ZLay_ColorMakeOpaque(ZLay_Color color);
ZLAY_API ZLay_Color ZLay_ColorMultiplyAlpha(ZLay_Color color, float alpha_scale);
ZLAY_API ZLay_Color ZLay_ColorMix(ZLay_Color a, ZLay_Color b, float t);
ZLAY_API bool ZLay_ColorIsTransparent(ZLay_Color color);
ZLAY_API bool ZLay_ColorIsOpaque(ZLay_Color color);

ZLAY_API ZLay_Color ZLay_ColorFromRGBA32(uint32_t rgba);
ZLAY_API ZLay_Color ZLay_ColorFromARGB32(uint32_t argb);
ZLAY_API ZLay_Color ZLay_ColorFromRGB24(uint32_t rgb);
ZLAY_API uint32_t ZLay_ColorToRGBA32(ZLay_Color color);
ZLAY_API uint32_t ZLay_ColorToARGB32(ZLay_Color color);
ZLAY_API uint32_t ZLay_ColorToRGB24(ZLay_Color color);
ZLAY_API ZLay_Color ZLay_ColorFromHEX(ZLay_HEX hex, ZLay_Color fallback);
ZLAY_API bool ZLay_ColorParseHex(const char* hex, ZLay_Color* out_color);
ZLAY_API bool ZLay_ColorToHex(ZLay_Color color, char* out_hex, size_t out_capacity, ZLay_ColorHexFormat format);

ZLAY_API ZLay_RGB ZLay_RGBColor(float r, float g, float b);
ZLAY_API ZLay_RGBA ZLay_RGBAColor(float r, float g, float b, float a);
ZLAY_API ZLay_Color ZLay_ColorFromRGB(ZLay_RGB rgb);
ZLAY_API ZLay_Color ZLay_ColorFromRGBA(ZLay_RGBA rgba);
ZLAY_API ZLay_RGB ZLay_ColorToRGB(ZLay_Color color);
ZLAY_API ZLay_RGBA ZLay_ColorToRGBA(ZLay_Color color);

ZLAY_API ZLay_CMYK ZLay_CMYKColor(float c, float m, float y, float k, float a);
ZLAY_API ZLay_Color ZLay_ColorFromCMYK(ZLay_CMYK cmyk);
ZLAY_API ZLay_CMYK ZLay_ColorToCMYK(ZLay_Color color);

ZLAY_API ZLay_HSL ZLay_HSLColor(float h, float s, float l, float a);
ZLAY_API ZLay_Color ZLay_ColorFromHSL(ZLay_HSL hsl);
ZLAY_API ZLay_HSL ZLay_ColorToHSL(ZLay_Color color);

ZLAY_API ZLay_HSV ZLay_HSVColor(float h, float s, float v, float a);
ZLAY_API ZLay_Color ZLay_ColorFromHSV(ZLay_HSV hsv);
ZLAY_API ZLay_HSV ZLay_ColorToHSV(ZLay_Color color);

ZLAY_API float ZLay_ColorLuminance(ZLay_Color color);
ZLAY_API float ZLay_ColorContrastRatio(ZLay_Color a, ZLay_Color b);
ZLAY_API ZLay_Color ZLay_ColorGrayscale(ZLay_Color color);
ZLAY_API ZLay_Color ZLay_ColorInvert(ZLay_Color color);
ZLAY_API ZLay_Color ZLay_ColorLighten(ZLay_Color color, float amount);
ZLAY_API ZLay_Color ZLay_ColorDarken(ZLay_Color color, float amount);
ZLAY_API ZLay_Color ZLay_ColorBrighten(ZLay_Color color, float amount);
ZLAY_API ZLay_Color ZLay_ColorAddContrast(ZLay_Color color, float amount);
ZLAY_API ZLay_Color ZLay_ColorSaturate(ZLay_Color color, float amount);
ZLAY_API ZLay_Color ZLay_ColorDesaturate(ZLay_Color color, float amount);
ZLAY_API ZLay_Color ZLay_ColorPremultiplyAlpha(ZLay_Color color);
ZLAY_API ZLay_Color ZLay_ColorUnpremultiplyAlpha(ZLay_Color color);
ZLAY_API ZLay_Color ZLay_ColorBlendOver(ZLay_Color src, ZLay_Color dst);
ZLAY_API float ZLay_ColorDistanceSquared(ZLay_Color a, ZLay_Color b);

#ifdef __cplusplus
}
#endif

#endif
