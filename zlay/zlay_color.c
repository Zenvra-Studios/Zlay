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

static float ZLay__MaxFloat(float a, float b) {
  return a > b ? a : b;
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

static uint8_t ZLay__ScaleByte(uint8_t value, float scale) {
  return ZLay__FloatToByte(ZLay__ByteToFloat(value) * scale);
}

static bool ZLay__IsSpace(char c) {
  return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

static int32_t ZLay__HexValue(char c) {
  if (c >= '0' && c <= '9') return (int32_t)(c - '0');
  if (c >= 'a' && c <= 'f') return (int32_t)(c - 'a') + 10;
  if (c >= 'A' && c <= 'F') return (int32_t)(c - 'A') + 10;
  return -1;
}

static uint8_t ZLay__HexPairToByte(int32_t hi, int32_t lo) {
  return (uint8_t)(((uint32_t)hi << 4u) | (uint32_t)lo);
}

static char ZLay__HexDigit(uint8_t value) {
  static const char digits[] = "0123456789ABCDEF";
  return digits[value & 0x0Fu];
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

ZLay_Color ZLay_ColorWhite(void) {
  return ZLay_ColorRGBA(255, 255, 255, 255);
}

ZLay_Color ZLay_ColorBlack(void) {
  return ZLay_ColorRGBA(0, 0, 0, 255);
}

ZLay_Color ZLay_ColorWithAlpha(ZLay_Color color, uint8_t alpha) {
  color.a = alpha;
  return color;
}

ZLay_Color ZLay_ColorMakeOpaque(ZLay_Color color) {
  color.a = 255;
  return color;
}

ZLay_Color ZLay_ColorMultiplyAlpha(ZLay_Color color, float alpha_scale) {
  color.a = ZLay__ScaleByte(color.a, ZLay__MaxFloat(alpha_scale, 0.0f));
  return color;
}

ZLay_Color ZLay_ColorMix(ZLay_Color a, ZLay_Color b, float t) {
  t = ZLay__Clamp01(t);
  return ZLay_ColorRGBA(
    ZLay__FloatToByte(ZLay__ByteToFloat(a.r) + (ZLay__ByteToFloat(b.r) - ZLay__ByteToFloat(a.r)) * t),
    ZLay__FloatToByte(ZLay__ByteToFloat(a.g) + (ZLay__ByteToFloat(b.g) - ZLay__ByteToFloat(a.g)) * t),
    ZLay__FloatToByte(ZLay__ByteToFloat(a.b) + (ZLay__ByteToFloat(b.b) - ZLay__ByteToFloat(a.b)) * t),
    ZLay__FloatToByte(ZLay__ByteToFloat(a.a) + (ZLay__ByteToFloat(b.a) - ZLay__ByteToFloat(a.a)) * t)
  );
}

bool ZLay_ColorIsTransparent(ZLay_Color color) {
  return color.a == 0u;
}

bool ZLay_ColorIsOpaque(ZLay_Color color) {
  return color.a == 255u;
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

ZLay_Color ZLay_ColorFromHEX(ZLay_HEX hex, ZLay_Color fallback) {
  ZLay_Color color;
  if (!ZLay_ColorParseHex(hex.hex, &color)) {
    return fallback;
  }

  return color;
}

bool ZLay_ColorParseHex(const char* hex, ZLay_Color* out_color) {
  int32_t digits[8];
  uint32_t count = 0u;

  if (hex == NULL || out_color == NULL) {
    return false;
  }

  while (ZLay__IsSpace(*hex)) {
    ++hex;
  }

  if (*hex == '#') {
    ++hex;
  } else if (hex[0] == '0' && (hex[1] == 'x' || hex[1] == 'X')) {
    hex += 2;
  }

  while (*hex != '\0' && !ZLay__IsSpace(*hex)) {
    int32_t value;
    if (count >= 8u) {
      return false;
    }

    value = ZLay__HexValue(*hex++);
    if (value < 0) {
      return false;
    }

    digits[count++] = value;
  }

  while (ZLay__IsSpace(*hex)) {
    ++hex;
  }

  if (*hex != '\0') {
    return false;
  }

  if (count == 3u || count == 4u) {
    const uint8_t r = ZLay__HexPairToByte(digits[0], digits[0]);
    const uint8_t g = ZLay__HexPairToByte(digits[1], digits[1]);
    const uint8_t b = ZLay__HexPairToByte(digits[2], digits[2]);
    const uint8_t a = count == 4u ? ZLay__HexPairToByte(digits[3], digits[3]) : 255u;
    *out_color = ZLay_ColorRGBA(r, g, b, a);
    return true;
  }

  if (count == 6u || count == 8u) {
    const uint8_t r = ZLay__HexPairToByte(digits[0], digits[1]);
    const uint8_t g = ZLay__HexPairToByte(digits[2], digits[3]);
    const uint8_t b = ZLay__HexPairToByte(digits[4], digits[5]);
    const uint8_t a = count == 8u ? ZLay__HexPairToByte(digits[6], digits[7]) : 255u;
    *out_color = ZLay_ColorRGBA(r, g, b, a);
    return true;
  }

  return false;
}

bool ZLay_ColorToHex(ZLay_Color color, char* out_hex, size_t out_capacity, ZLay_ColorHexFormat format) {
  const bool include_alpha = format == ZLAY_COLOR_HEX_RGBA;
  const size_t required = include_alpha ? 10u : 8u;

  if (out_hex == NULL || out_capacity < required) {
    return false;
  }

  out_hex[0] = '#';
  out_hex[1] = ZLay__HexDigit((uint8_t)(color.r >> 4u));
  out_hex[2] = ZLay__HexDigit(color.r);
  out_hex[3] = ZLay__HexDigit((uint8_t)(color.g >> 4u));
  out_hex[4] = ZLay__HexDigit(color.g);
  out_hex[5] = ZLay__HexDigit((uint8_t)(color.b >> 4u));
  out_hex[6] = ZLay__HexDigit(color.b);

  if (include_alpha) {
    out_hex[7] = ZLay__HexDigit((uint8_t)(color.a >> 4u));
    out_hex[8] = ZLay__HexDigit(color.a);
    out_hex[9] = '\0';
  } else {
    out_hex[7] = '\0';
  }

  return true;
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

ZLay_Color ZLay_ColorGrayscale(ZLay_Color color) {
  const uint8_t gray = ZLay__FloatToByte(ZLay_ColorLuminance(color));
  return ZLay_ColorRGBA(gray, gray, gray, color.a);
}

ZLay_Color ZLay_ColorInvert(ZLay_Color color) {
  return ZLay_ColorRGBA(
    (uint8_t)(255u - color.r),
    (uint8_t)(255u - color.g),
    (uint8_t)(255u - color.b),
    color.a
  );
}

ZLay_Color ZLay_ColorLighten(ZLay_Color color, float amount) {
  ZLay_HSL hsl = ZLay_ColorToHSL(color);
  hsl.l = ZLay__Clamp01(hsl.l + amount);
  return ZLay_ColorFromHSL(hsl);
}

ZLay_Color ZLay_ColorDarken(ZLay_Color color, float amount) {
  return ZLay_ColorLighten(color, -amount);
}

ZLay_Color ZLay_ColorBrighten(ZLay_Color color, float amount) {
  const float factor = 1.0f / (1.0f + ZLay__MaxFloat(amount, 0.0f));
  return ZLay_ColorRGBA(
    ZLay__FloatToByte(1.0f - (1.0f - ZLay__ByteToFloat(color.r)) * factor),
    ZLay__FloatToByte(1.0f - (1.0f - ZLay__ByteToFloat(color.g)) * factor),
    ZLay__FloatToByte(1.0f - (1.0f - ZLay__ByteToFloat(color.b)) * factor),
    color.a
  );
}

ZLay_Color ZLay_ColorAddContrast(ZLay_Color color, float amount) {
  return ZLay_ColorRGBA(
    ZLay__FloatToByte((ZLay__ByteToFloat(color.r) - 0.5f) * amount + 0.5f),
    ZLay__FloatToByte((ZLay__ByteToFloat(color.g) - 0.5f) * amount + 0.5f),
    ZLay__FloatToByte((ZLay__ByteToFloat(color.b) - 0.5f) * amount + 0.5f),
    color.a
  );
}

ZLay_Color ZLay_ColorSaturate(ZLay_Color color, float amount) {
  ZLay_HSL hsl = ZLay_ColorToHSL(color);
  hsl.s = ZLay__Clamp01(hsl.s + amount);
  return ZLay_ColorFromHSL(hsl);
}

ZLay_Color ZLay_ColorDesaturate(ZLay_Color color, float amount) {
  return ZLay_ColorSaturate(color, -amount);
}

ZLay_Color ZLay_ColorPremultiplyAlpha(ZLay_Color color) {
  const float alpha = ZLay__ByteToFloat(color.a);
  return ZLay_ColorRGBA(
    ZLay__ScaleByte(color.r, alpha),
    ZLay__ScaleByte(color.g, alpha),
    ZLay__ScaleByte(color.b, alpha),
    color.a
  );
}

ZLay_Color ZLay_ColorUnpremultiplyAlpha(ZLay_Color color) {
  const float alpha = ZLay__ByteToFloat(color.a);
  if (alpha <= 0.0f) {
    return ZLay_ColorTransparent();
  }

  return ZLay_ColorRGBA(
    ZLay__FloatToByte(ZLay__ByteToFloat(color.r) / alpha),
    ZLay__FloatToByte(ZLay__ByteToFloat(color.g) / alpha),
    ZLay__FloatToByte(ZLay__ByteToFloat(color.b) / alpha),
    color.a
  );
}

ZLay_Color ZLay_ColorBlendOver(ZLay_Color src, ZLay_Color dst) {
  const float src_a = ZLay__ByteToFloat(src.a);
  const float dst_a = ZLay__ByteToFloat(dst.a);
  const float out_a = src_a + dst_a * (1.0f - src_a);

  if (out_a <= 0.0f) {
    return ZLay_ColorTransparent();
  }

  return ZLay_ColorRGBA(
    ZLay__FloatToByte((ZLay__ByteToFloat(src.r) * src_a + ZLay__ByteToFloat(dst.r) * dst_a * (1.0f - src_a)) / out_a),
    ZLay__FloatToByte((ZLay__ByteToFloat(src.g) * src_a + ZLay__ByteToFloat(dst.g) * dst_a * (1.0f - src_a)) / out_a),
    ZLay__FloatToByte((ZLay__ByteToFloat(src.b) * src_a + ZLay__ByteToFloat(dst.b) * dst_a * (1.0f - src_a)) / out_a),
    ZLay__FloatToByte(out_a)
  );
}

float ZLay_ColorDistanceSquared(ZLay_Color a, ZLay_Color b) {
  const float dr = ZLay__ByteToFloat(a.r) - ZLay__ByteToFloat(b.r);
  const float dg = ZLay__ByteToFloat(a.g) - ZLay__ByteToFloat(b.g);
  const float db = ZLay__ByteToFloat(a.b) - ZLay__ByteToFloat(b.b);
  const float da = ZLay__ByteToFloat(a.a) - ZLay__ByteToFloat(b.a);
  return dr * dr + dg * dg + db * db + da * da;
}
