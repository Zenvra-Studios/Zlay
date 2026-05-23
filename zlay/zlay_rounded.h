#ifndef ZLAY_ROUNDED_H
#define ZLAY_ROUNDED_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zlay_math.h>

#ifndef ZLAY_ROUNDED_API
#  ifdef ZLAY_API
#    define ZLAY_ROUNDED_API ZLAY_API
#  else
#    define ZLAY_ROUNDED_API
#  endif
#endif

typedef enum ZLay_RoundedCurve {
  ZLAY_ROUNDED_CURVE_CIRCULAR = 0,
  ZLAY_ROUNDED_CURVE_CONTINUOUS = 1,
  ZLAY_ROUNDED_CURVE_MACOS = 2,
  ZLAY_ROUNDED_CURVE_CUSTOM = 3
} ZLay_RoundedCurve;

typedef struct ZLay_Rounded {
  float radius;
  float smoothness;
  float exponent;
  ZLay_RoundedCurve curve;
} ZLay_Rounded;

typedef enum ZLay_RoundedPreset {
  ZLAY_ROUNDED_NONE = 0,
  ZLAY_ROUNDED_SM = 1,
  ZLAY_ROUNDED_MD = 2,
  ZLAY_ROUNDED_LG = 3,
  ZLAY_ROUNDED_XL = 4,
  ZLAY_ROUNDED_2XL = 5,
  ZLAY_ROUNDED_PILL = 6,
  ZLAY_ROUNDED_MACOS_SM = 7,
  ZLAY_ROUNDED_MACOS_MD = 8,
  ZLAY_ROUNDED_MACOS_LG = 9
} ZLay_RoundedPreset;

ZLAY_ROUNDED_API float ZLay_RoundedClampRadius(float radius, float width, float height);
ZLAY_ROUNDED_API ZLay_Rounded ZLay_RoundedMake(float radius, ZLay_RoundedCurve curve);
ZLAY_ROUNDED_API ZLay_Rounded ZLay_RoundedCustom(float radius, float smoothness, float exponent);
ZLAY_ROUNDED_API ZLay_Rounded ZLay_RoundedCircular(float radius);
ZLAY_ROUNDED_API ZLay_Rounded ZLay_RoundedContinuous(float radius);
ZLAY_ROUNDED_API ZLay_Rounded ZLay_RoundedMacOS(float radius);
ZLAY_ROUNDED_API ZLay_Rounded ZLay_RoundedPresetValue(ZLay_RoundedPreset preset);
ZLAY_ROUNDED_API ZLay_Rounded ZLay_RoundedWithRadius(ZLay_Rounded rounded, float radius);
ZLAY_ROUNDED_API ZLay_Rounded ZLay_RoundedWithSmoothness(ZLay_Rounded rounded, float smoothness);
ZLAY_ROUNDED_API ZLay_Rounded ZLay_RoundedWithExponent(ZLay_Rounded rounded, float exponent);
ZLAY_ROUNDED_API ZLay_Rounded ZLay_RoundedWithCurve(ZLay_Rounded rounded, ZLay_RoundedCurve curve);
ZLAY_ROUNDED_API ZLay_Rounded ZLay_RoundedResolve(ZLay_Rounded rounded);
ZLAY_ROUNDED_API float ZLay_RoundedEffectiveRadius(ZLay_Rounded rounded, float width, float height);
ZLAY_ROUNDED_API float ZLay_RoundedEffectiveExponent(ZLay_Rounded rounded);
ZLAY_ROUNDED_API float ZLay_RoundedSDF(ZLay_Vec2 local, ZLay_Vec2 half_size, ZLay_Rounded rounded);
ZLAY_ROUNDED_API ZLay_Vec2 ZLay_RoundedPointOnCorner(float angle_radians, ZLay_Rounded rounded);

#ifdef __cplusplus
}
#endif

#endif
