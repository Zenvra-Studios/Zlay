#include <zlay.h>

float ZLay_RoundedClampRadius(float radius, float width, float height) {
  const float max_radius = ZLay_MaxFloat(0.0f, ZLay_MinFloat(width, height) * 0.5f);
  return ZLay_ClampFloat(radius, 0.0f, max_radius);
}

ZLay_Rounded ZLay_RoundedMake(float radius, ZLay_RoundedCurve curve) {
  ZLay_Rounded rounded;
  rounded.radius = ZLay_MaxFloat(radius, 0.0f);
  rounded.smoothness = 0.0f;
  rounded.exponent = 2.0f;
  rounded.curve = curve;
  return ZLay_RoundedResolve(rounded);
}

ZLay_Rounded ZLay_RoundedCustom(float radius, float smoothness, float exponent) {
  ZLay_Rounded rounded;
  rounded.radius = ZLay_MaxFloat(radius, 0.0f);
  rounded.smoothness = ZLay_SaturateFloat(smoothness);
  rounded.exponent = ZLay_MaxFloat(exponent, 2.0f);
  rounded.curve = ZLAY_ROUNDED_CURVE_CUSTOM;
  return rounded;
}

ZLay_Rounded ZLay_RoundedCircular(float radius) {
  ZLay_Rounded rounded;
  rounded.radius = ZLay_MaxFloat(radius, 0.0f);
  rounded.smoothness = 0.0f;
  rounded.exponent = 2.0f;
  rounded.curve = ZLAY_ROUNDED_CURVE_CIRCULAR;
  return rounded;
}

ZLay_Rounded ZLay_RoundedContinuous(float radius) {
  ZLay_Rounded rounded;
  rounded.radius = ZLay_MaxFloat(radius, 0.0f);
  rounded.smoothness = 0.72f;
  rounded.exponent = 4.0f;
  rounded.curve = ZLAY_ROUNDED_CURVE_CONTINUOUS;
  return rounded;
}

ZLay_Rounded ZLay_RoundedMacOS(float radius) {
  ZLay_Rounded rounded;
  rounded.radius = ZLay_MaxFloat(radius, 0.0f);
  rounded.smoothness = 0.86f;
  rounded.exponent = 5.0f;
  rounded.curve = ZLAY_ROUNDED_CURVE_MACOS;
  return rounded;
}

ZLay_Rounded ZLay_RoundedPresetValue(ZLay_RoundedPreset preset) {
  switch (preset) {
    case ZLAY_ROUNDED_SM: return ZLay_RoundedCircular(6.0f);
    case ZLAY_ROUNDED_MD: return ZLay_RoundedCircular(10.0f);
    case ZLAY_ROUNDED_LG: return ZLay_RoundedCircular(16.0f);
    case ZLAY_ROUNDED_XL: return ZLay_RoundedCircular(24.0f);
    case ZLAY_ROUNDED_2XL: return ZLay_RoundedCircular(32.0f);
    case ZLAY_ROUNDED_PILL: return ZLay_RoundedCircular(999.0f);
    case ZLAY_ROUNDED_MACOS_SM: return ZLay_RoundedMacOS(8.0f);
    case ZLAY_ROUNDED_MACOS_MD: return ZLay_RoundedMacOS(14.0f);
    case ZLAY_ROUNDED_MACOS_LG: return ZLay_RoundedMacOS(20.0f);
    case ZLAY_ROUNDED_NONE:
    default: return ZLay_RoundedCircular(0.0f);
  }
}

ZLay_Rounded ZLay_RoundedWithRadius(ZLay_Rounded rounded, float radius) {
  rounded.radius = ZLay_MaxFloat(radius, 0.0f);
  return ZLay_RoundedResolve(rounded);
}

ZLay_Rounded ZLay_RoundedWithSmoothness(ZLay_Rounded rounded, float smoothness) {
  rounded.smoothness = ZLay_SaturateFloat(smoothness);
  if (rounded.curve != ZLAY_ROUNDED_CURVE_CUSTOM) {
    rounded.curve = ZLAY_ROUNDED_CURVE_CUSTOM;
  }
  return ZLay_RoundedResolve(rounded);
}

ZLay_Rounded ZLay_RoundedWithExponent(ZLay_Rounded rounded, float exponent) {
  rounded.exponent = ZLay_MaxFloat(exponent, 2.0f);
  if (rounded.curve != ZLAY_ROUNDED_CURVE_CUSTOM) {
    rounded.curve = ZLAY_ROUNDED_CURVE_CUSTOM;
  }
  return ZLay_RoundedResolve(rounded);
}

ZLay_Rounded ZLay_RoundedWithCurve(ZLay_Rounded rounded, ZLay_RoundedCurve curve) {
  rounded.curve = curve;
  return ZLay_RoundedResolve(rounded);
}

ZLay_Rounded ZLay_RoundedResolve(ZLay_Rounded rounded) {
  if (rounded.radius < 0.0f) {
    rounded.radius = 0.0f;
  }

  switch (rounded.curve) {
    case ZLAY_ROUNDED_CURVE_MACOS:
      if (rounded.exponent < 2.0f) rounded.exponent = 5.0f;
      if (rounded.smoothness <= 0.0f) rounded.smoothness = 0.86f;
      break;
    case ZLAY_ROUNDED_CURVE_CONTINUOUS:
      if (rounded.exponent < 2.0f) rounded.exponent = 4.0f;
      if (rounded.smoothness <= 0.0f) rounded.smoothness = 0.72f;
      break;
    case ZLAY_ROUNDED_CURVE_CUSTOM:
      rounded.exponent = ZLay_MaxFloat(rounded.exponent, 2.0f);
      rounded.smoothness = ZLay_SaturateFloat(rounded.smoothness);
      break;
    case ZLAY_ROUNDED_CURVE_CIRCULAR:
    default:
      rounded.curve = ZLAY_ROUNDED_CURVE_CIRCULAR;
      rounded.exponent = 2.0f;
      rounded.smoothness = 0.0f;
      break;
  }

  return rounded;
}

float ZLay_RoundedEffectiveRadius(ZLay_Rounded rounded, float width, float height) {
  rounded = ZLay_RoundedResolve(rounded);
  return ZLay_RoundedClampRadius(rounded.radius, width, height);
}

float ZLay_RoundedEffectiveExponent(ZLay_Rounded rounded) {
  return ZLay_RoundedResolve(rounded).exponent;
}

float ZLay_RoundedSDF(ZLay_Vec2 local, ZLay_Vec2 half_size, ZLay_Rounded rounded) {
  const ZLay_Rounded resolved = ZLay_RoundedResolve(rounded);
  const float radius = ZLay_RoundedClampRadius(
    resolved.radius,
    half_size.x * 2.0f,
    half_size.y * 2.0f
  );
  const float exponent = ZLay_MaxFloat(resolved.exponent, 2.0f);
  ZLay_Vec2 q;
  ZLay_Vec2 outside;
  float outside_len;
  float inside;

  q.x = fabsf(local.x) - half_size.x + radius;
  q.y = fabsf(local.y) - half_size.y + radius;
  outside.x = ZLay_MaxFloat(q.x, 0.0f);
  outside.y = ZLay_MaxFloat(q.y, 0.0f);

  if (exponent <= 2.0001f) {
    outside_len = ZLay_Vec2Length(outside);
  } else {
    outside_len = powf(powf(outside.x, exponent) + powf(outside.y, exponent), 1.0f / exponent);
  }

  inside = ZLay_MinFloat(ZLay_MaxFloat(q.x, q.y), 0.0f);
  return outside_len + inside - radius;
}

ZLay_Vec2 ZLay_RoundedPointOnCorner(float angle_radians, ZLay_Rounded rounded) {
  const ZLay_Rounded resolved = ZLay_RoundedResolve(rounded);
  const float exponent = ZLay_MaxFloat(resolved.exponent, 2.0f);
  const float c = cosf(angle_radians);
  const float s = sinf(angle_radians);
  const float inv = 2.0f / exponent;
  ZLay_Vec2 point;

  point.x = resolved.radius * (c < 0.0f ? -1.0f : 1.0f) * powf(fabsf(c), inv);
  point.y = resolved.radius * (s < 0.0f ? -1.0f : 1.0f) * powf(fabsf(s), inv);
  return point;
}
