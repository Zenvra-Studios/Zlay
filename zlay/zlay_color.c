#include <zlay_color.h>

static float ZLay__Clamp01(float value) {
  if (value < 0.0f) return 0.0f;
  if (value > 1.0f) return 1.0f;
  return value;
}

static float ZLay__Max3(float a, float b, float c) {
  float result = a > b ? a : b;
  return result > c ? result : c;
}

static float ZLay__Min3(float a, float b, float c) {
  float result = a < b ? a : b;
  return result < c ? result : c;
}

static float ZLay__Abs(float value) {
  return value < 0.0f ? -value : value;
}

static float ZLay__WrapHue(float hue) {
  while (hue < 0.0f) hue += 360.0f;
  while (hue >= 360.0f) hue -= 360.0f;
  return hue;
}

static uint8_t ZLay__FloatToByte(float value) {
  value = ZLay__Clamp01(value);
  return (uint8_t)(value * 255.0f + 0.5f);
}

static float ZLay__ByteToFloat(uint8_t value) {
  return (float)value / 255.0f;
}

ZLay_Color ZLay_ColorRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  return (ZLay_Color){r, g, b, a};
}

ZLay_Color ZLay_ColorRGB(uint8_t r, uint8_t g, uint8_t b) {
  return ZLay_ColorRGBA(r, g, b, 255);
}

ZLay_Color ZLay_ColorTransparent(void) {
  return ZLay_ColorRGBA(0, 0, 0, 0);
}

ZLay_Color ZLay_ColorWithAlpha(ZLay_Color color, uint8_t alpha) {
  color.a = alpha;
  return color;
}

ZLay_Color ZLay_ColorMix(ZLay_Color a, ZLay_Color b, float t) {
  t = ZLay__Clamp01(t);
  return ZLay_ColorRGBA(
    (uint8_t)((float)a.r + ((float)b.r - (float)a.r) * t),
    (uint8_t)((float)a.g + ((float)b.g - (float)a.g) * t),
    (uint8_t)((float)a.b + ((float)b.b - (float)a.b) * t),
    (uint8_t)((float)a.a + ((float)b.a - (float)a.a) * t)
  );
}

ZLay_Color ZLay_ColorFromRGBA32(uint32_t rgba) {
  return ZLay_ColorRGBA(
    (uint8_t)((rgba >> 24u) & 0xFFu),
    (uint8_t)((rgba >> 16u) & 0xFFu),
    (uint8_t)((rgba >> 8u) & 0xFFu),
    (uint8_t)(rgba & 0xFFu)
  );
}

ZLay_Color ZLay_ColorFromARGB32(uint32_t argb) {
  return ZLay_ColorRGBA(
    (uint8_t)((argb >> 16u) & 0xFFu),
    (uint8_t)((argb >> 8u) & 0xFFu),
    (uint8_t)(argb & 0xFFu),
    (uint8_t)((argb >> 24u) & 0xFFu)
  );
}

ZLay_Color ZLay_ColorFromRGB24(uint32_t rgb) {
  return ZLay_ColorRGBA(
    (uint8_t)((rgb >> 16u) & 0xFFu),
    (uint8_t)((rgb >> 8u) & 0xFFu),
    (uint8_t)(rgb & 0xFFu),
    255
  );
}

uint32_t ZLay_ColorToRGBA32(ZLay_Color color) {
  return ((uint32_t)color.r << 24u) | ((uint32_t)color.g << 16u) | ((uint32_t)color.b << 8u) | (uint32_t)color.a;
}

uint32_t ZLay_ColorToARGB32(ZLay_Color color) {
  return ((uint32_t)color.a << 24u) | ((uint32_t)color.r << 16u) | ((uint32_t)color.g << 8u) | (uint32_t)color.b;
}

uint32_t ZLay_ColorToRGB24(ZLay_Color color) {
  return ((uint32_t)color.r << 16u) | ((uint32_t)color.g << 8u) | (uint32_t)color.b;
}

ZLay_RGB ZLay_RGBColor(float r, float g, float b) {
  return (ZLay_RGB){ZLay__Clamp01(r), ZLay__Clamp01(g), ZLay__Clamp01(b)};
}

ZLay_RGBA ZLay_RGBAColor(float r, float g, float b, float a) {
  return (ZLay_RGBA){ZLay__Clamp01(r), ZLay__Clamp01(g), ZLay__Clamp01(b), ZLay__Clamp01(a)};
}

ZLay_Color ZLay_ColorFromRGB(ZLay_RGB rgb) {
  return ZLay_ColorFromRGBA(ZLay_RGBAColor(rgb.r, rgb.g, rgb.b, 1.0f));
}

ZLay_Color ZLay_ColorFromRGBA(ZLay_RGBA rgba) {
  return ZLay_ColorRGBA(
    ZLay__FloatToByte(rgba.r),
    ZLay__FloatToByte(rgba.g),
    ZLay__FloatToByte(rgba.b),
    ZLay__FloatToByte(rgba.a)
  );
}

ZLay_RGB ZLay_ColorToRGB(ZLay_Color color) {
  return ZLay_RGBColor(ZLay__ByteToFloat(color.r), ZLay__ByteToFloat(color.g), ZLay__ByteToFloat(color.b));
}

ZLay_RGBA ZLay_ColorToRGBA(ZLay_Color color) {
  return ZLay_RGBAColor(
    ZLay__ByteToFloat(color.r),
    ZLay__ByteToFloat(color.g),
    ZLay__ByteToFloat(color.b),
    ZLay__ByteToFloat(color.a)
  );
}

ZLay_CMYK ZLay_CMYKColor(float c, float m, float y, float k, float a) {
  return (ZLay_CMYK){ZLay__Clamp01(c), ZLay__Clamp01(m), ZLay__Clamp01(y), ZLay__Clamp01(k), ZLay__Clamp01(a)};
}

ZLay_Color ZLay_ColorFromCMYK(ZLay_CMYK cmyk) {
  const float c = ZLay__Clamp01(cmyk.c);
  const float m = ZLay__Clamp01(cmyk.m);
  const float y = ZLay__Clamp01(cmyk.y);
  const float k = ZLay__Clamp01(cmyk.k);
  return ZLay_ColorRGBA(
    ZLay__FloatToByte((1.0f - c) * (1.0f - k)),
    ZLay__FloatToByte((1.0f - m) * (1.0f - k)),
    ZLay__FloatToByte((1.0f - y) * (1.0f - k)),
    ZLay__FloatToByte(cmyk.a)
  );
}

ZLay_CMYK ZLay_ColorToCMYK(ZLay_Color color) {
  const float r = ZLay__ByteToFloat(color.r);
  const float g = ZLay__ByteToFloat(color.g);
  const float b = ZLay__ByteToFloat(color.b);
  const float k = 1.0f - ZLay__Max3(r, g, b);
  if (k >= 1.0f) return ZLay_CMYKColor(0.0f, 0.0f, 0.0f, 1.0f, ZLay__ByteToFloat(color.a));
  return ZLay_CMYKColor(
    (1.0f - r - k) / (1.0f - k),
    (1.0f - g - k) / (1.0f - k),
    (1.0f - b - k) / (1.0f - k),
    k,
    ZLay__ByteToFloat(color.a)
  );
}

ZLay_HSL ZLay_HSLColor(float h, float s, float l, float a) {
  return (ZLay_HSL){ZLay__WrapHue(h), ZLay__Clamp01(s), ZLay__Clamp01(l), ZLay__Clamp01(a)};
}

ZLay_Color ZLay_ColorFromHSL(ZLay_HSL hsl) {
  const float h = ZLay__WrapHue(hsl.h);
  const float s = ZLay__Clamp01(hsl.s);
  const float l = ZLay__Clamp01(hsl.l);
  const float c = (1.0f - ZLay__Abs(2.0f * l - 1.0f)) * s;
  const float hp = h / 60.0f;
  const float x = c * (1.0f - ZLay__Abs((hp - (float)((int)(hp / 2.0f) * 2)) - 1.0f));
  const float m = l - c * 0.5f;
  float r = 0.0f;
  float g = 0.0f;
  float b = 0.0f;

  if (hp < 1.0f) { r = c; g = x; }
  else if (hp < 2.0f) { r = x; g = c; }
  else if (hp < 3.0f) { g = c; b = x; }
  else if (hp < 4.0f) { g = x; b = c; }
  else if (hp < 5.0f) { r = x; b = c; }
  else { r = c; b = x; }

  return ZLay_ColorRGBA(ZLay__FloatToByte(r + m), ZLay__FloatToByte(g + m), ZLay__FloatToByte(b + m), ZLay__FloatToByte(hsl.a));
}

ZLay_HSL ZLay_ColorToHSL(ZLay_Color color) {
  const float r = ZLay__ByteToFloat(color.r);
  const float g = ZLay__ByteToFloat(color.g);
  const float b = ZLay__ByteToFloat(color.b);
  const float max = ZLay__Max3(r, g, b);
  const float min = ZLay__Min3(r, g, b);
  const float delta = max - min;
  float h = 0.0f;
  float s = 0.0f;
  const float l = (max + min) * 0.5f;

  if (delta > 0.0f) {
    if (max == r) h = 60.0f * ((g - b) / delta);
    else if (max == g) h = 60.0f * (((b - r) / delta) + 2.0f);
    else h = 60.0f * (((r - g) / delta) + 4.0f);
    s = delta / (1.0f - ZLay__Abs(2.0f * l - 1.0f));
  }

  return ZLay_HSLColor(h, s, l, ZLay__ByteToFloat(color.a));
}

ZLay_HSV ZLay_HSVColor(float h, float s, float v, float a) {
  return (ZLay_HSV){ZLay__WrapHue(h), ZLay__Clamp01(s), ZLay__Clamp01(v), ZLay__Clamp01(a)};
}

ZLay_Color ZLay_ColorFromHSV(ZLay_HSV hsv) {
  const float h = ZLay__WrapHue(hsv.h);
  const float s = ZLay__Clamp01(hsv.s);
  const float v = ZLay__Clamp01(hsv.v);
  const float c = v * s;
  const float hp = h / 60.0f;
  const float x = c * (1.0f - ZLay__Abs((hp - (float)((int)(hp / 2.0f) * 2)) - 1.0f));
  const float m = v - c;
  float r = 0.0f;
  float g = 0.0f;
  float b = 0.0f;

  if (hp < 1.0f) { r = c; g = x; }
  else if (hp < 2.0f) { r = x; g = c; }
  else if (hp < 3.0f) { g = c; b = x; }
  else if (hp < 4.0f) { g = x; b = c; }
  else if (hp < 5.0f) { r = x; b = c; }
  else { r = c; b = x; }

  return ZLay_ColorRGBA(ZLay__FloatToByte(r + m), ZLay__FloatToByte(g + m), ZLay__FloatToByte(b + m), ZLay__FloatToByte(hsv.a));
}

ZLay_HSV ZLay_ColorToHSV(ZLay_Color color) {
  const float r = ZLay__ByteToFloat(color.r);
  const float g = ZLay__ByteToFloat(color.g);
  const float b = ZLay__ByteToFloat(color.b);
  const float max = ZLay__Max3(r, g, b);
  const float min = ZLay__Min3(r, g, b);
  const float delta = max - min;
  float h = 0.0f;
  const float s = max <= 0.0f ? 0.0f : delta / max;

  if (delta > 0.0f) {
    if (max == r) h = 60.0f * ((g - b) / delta);
    else if (max == g) h = 60.0f * (((b - r) / delta) + 2.0f);
    else h = 60.0f * (((r - g) / delta) + 4.0f);
  }

  return ZLay_HSVColor(h, s, max, ZLay__ByteToFloat(color.a));
}

float ZLay_ColorLuminance(ZLay_Color color) {
  return 0.2126f * ZLay__ByteToFloat(color.r) + 0.7152f * ZLay__ByteToFloat(color.g) + 0.0722f * ZLay__ByteToFloat(color.b);
}

float ZLay_ColorContrastRatio(ZLay_Color a, ZLay_Color b) {
  float la = ZLay_ColorLuminance(a);
  float lb = ZLay_ColorLuminance(b);
  if (la < lb) {
    const float tmp = la;
    la = lb;
    lb = tmp;
  }
  return (la + 0.05f) / (lb + 0.05f);
}

ZLay_Color ZLay_ColorLighten(ZLay_Color color, float amount) {
  ZLay_HSL hsl = ZLay_ColorToHSL(color);
  hsl.l = ZLay__Clamp01(hsl.l + amount);
  return ZLay_ColorFromHSL(hsl);
}

ZLay_Color ZLay_ColorDarken(ZLay_Color color, float amount) {
  return ZLay_ColorLighten(color, -amount);
}

ZLay_Color ZLay_ColorSaturate(ZLay_Color color, float amount) {
  ZLay_HSL hsl = ZLay_ColorToHSL(color);
  hsl.s = ZLay__Clamp01(hsl.s + amount);
  return ZLay_ColorFromHSL(hsl);
}

ZLay_Color ZLay_ColorDesaturate(ZLay_Color color, float amount) {
  return ZLay_ColorSaturate(color, -amount);
}
