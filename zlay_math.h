#ifndef ZLAY_MATH_H
#define ZLAY_MATH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef ZLAY_MATH_EPSILON
#  define ZLAY_MATH_EPSILON 0.000001f
#endif

#ifndef ZLAY_MATH_PI
#  define ZLAY_MATH_PI 3.14159265358979323846f
#endif

#ifndef ZLAY_MATH_TAU
#  define ZLAY_MATH_TAU 6.28318530717958647692f
#endif

typedef struct ZLay_Vec2 {
  float x;
  float y;
} ZLay_Vec2;

typedef struct ZLay_Vec3 {
  float x;
  float y;
  float z;
} ZLay_Vec3;

typedef struct ZLay_Vec4 {
  float x;
  float y;
  float z;
  float w;
} ZLay_Vec4;

typedef struct ZLay_Quat {
  float x;
  float y;
  float z;
  float w;
} ZLay_Quat;

typedef struct ZLay_Mat4 {
  float m00;
  float m01;
  float m02;
  float m03;
  float m10;
  float m11;
  float m12;
  float m13;
  float m20;
  float m21;
  float m22;
  float m23;
  float m30;
  float m31;
  float m32;
  float m33;
} ZLay_Mat4;

typedef struct ZLay_Dimensions {
  float width;
  float height;
} ZLay_Dimensions;

typedef struct ZLay_Rect {
  float x;
  float y;
  float width;
  float height;
} ZLay_Rect;

typedef struct ZLay_Color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;
} ZLay_Color;

static inline float ZLay_Radians(float degrees) {
  return degrees * (ZLAY_MATH_PI / 180.0f);
}

static inline float ZLay_Degrees(float radians) {
  return radians * (180.0f / ZLAY_MATH_PI);
}

static inline float ZLay_MinFloat(float a, float b) {
  return a < b ? a : b;
}

static inline float ZLay_MaxFloat(float a, float b) {
  return a > b ? a : b;
}

static inline float ZLay_ClampFloat(float value, float min_value, float max_value) {
  return ZLay_MinFloat(ZLay_MaxFloat(value, min_value), max_value);
}

static inline float ZLay_SaturateFloat(float value) {
  return ZLay_ClampFloat(value, 0.0f, 1.0f);
}

static inline float ZLay_LerpFloat(float a, float b, float t) {
  return a + ((b - a) * t);
}

static inline float ZLay_InverseLerpFloat(float a, float b, float value) {
  const float delta = b - a;
  if (fabsf(delta) <= ZLAY_MATH_EPSILON) {
    return 0.0f;
  }
  return (value - a) / delta;
}

static inline bool ZLay_NearlyEqualFloat(float a, float b, float epsilon) {
  return fabsf(a - b) <= epsilon;
}

static inline ZLay_Vec2 ZLay_Vec2Make(float x, float y) {
  ZLay_Vec2 result;
  result.x = x;
  result.y = y;
  return result;
}

static inline ZLay_Vec2 ZLay_Vec2Zero(void) {
  return ZLay_Vec2Make(0.0f, 0.0f);
}

static inline ZLay_Vec2 ZLay_Vec2Add(ZLay_Vec2 a, ZLay_Vec2 b) {
  return ZLay_Vec2Make(a.x + b.x, a.y + b.y);
}

static inline ZLay_Vec2 ZLay_Vec2Sub(ZLay_Vec2 a, ZLay_Vec2 b) {
  return ZLay_Vec2Make(a.x - b.x, a.y - b.y);
}

static inline ZLay_Vec2 ZLay_Vec2Scale(ZLay_Vec2 value, float scale) {
  return ZLay_Vec2Make(value.x * scale, value.y * scale);
}

static inline ZLay_Vec2 ZLay_Vec2Hadamard(ZLay_Vec2 a, ZLay_Vec2 b) {
  return ZLay_Vec2Make(a.x * b.x, a.y * b.y);
}

static inline float ZLay_Vec2Dot(ZLay_Vec2 a, ZLay_Vec2 b) {
  return (a.x * b.x) + (a.y * b.y);
}

static inline float ZLay_Vec2LengthSq(ZLay_Vec2 value) {
  return ZLay_Vec2Dot(value, value);
}

static inline float ZLay_Vec2Length(ZLay_Vec2 value) {
  return sqrtf(ZLay_Vec2LengthSq(value));
}

static inline ZLay_Vec2 ZLay_Vec2Normalize(ZLay_Vec2 value) {
  const float length = ZLay_Vec2Length(value);
  if (length <= ZLAY_MATH_EPSILON) {
    return ZLay_Vec2Zero();
  }
  return ZLay_Vec2Scale(value, 1.0f / length);
}

static inline float ZLay_Vec2Distance(ZLay_Vec2 a, ZLay_Vec2 b) {
  return ZLay_Vec2Length(ZLay_Vec2Sub(a, b));
}

static inline ZLay_Vec2 ZLay_Vec2Lerp(ZLay_Vec2 a, ZLay_Vec2 b, float t) {
  return ZLay_Vec2Make(ZLay_LerpFloat(a.x, b.x, t), ZLay_LerpFloat(a.y, b.y, t));
}

static inline ZLay_Vec3 ZLay_Vec3Make(float x, float y, float z) {
  ZLay_Vec3 result;
  result.x = x;
  result.y = y;
  result.z = z;
  return result;
}

static inline ZLay_Vec3 ZLay_Vec3Zero(void) {
  return ZLay_Vec3Make(0.0f, 0.0f, 0.0f);
}

static inline ZLay_Vec3 ZLay_Vec3Add(ZLay_Vec3 a, ZLay_Vec3 b) {
  return ZLay_Vec3Make(a.x + b.x, a.y + b.y, a.z + b.z);
}

static inline ZLay_Vec3 ZLay_Vec3Sub(ZLay_Vec3 a, ZLay_Vec3 b) {
  return ZLay_Vec3Make(a.x - b.x, a.y - b.y, a.z - b.z);
}

static inline ZLay_Vec3 ZLay_Vec3Scale(ZLay_Vec3 value, float scale) {
  return ZLay_Vec3Make(value.x * scale, value.y * scale, value.z * scale);
}

static inline float ZLay_Vec3Dot(ZLay_Vec3 a, ZLay_Vec3 b) {
  return (a.x * b.x) + (a.y * b.y) + (a.z * b.z);
}

static inline ZLay_Vec3 ZLay_Vec3Cross(ZLay_Vec3 a, ZLay_Vec3 b) {
  return ZLay_Vec3Make(
    (a.y * b.z) - (a.z * b.y),
    (a.z * b.x) - (a.x * b.z),
    (a.x * b.y) - (a.y * b.x)
  );
}

static inline float ZLay_Vec3LengthSq(ZLay_Vec3 value) {
  return ZLay_Vec3Dot(value, value);
}

static inline float ZLay_Vec3Length(ZLay_Vec3 value) {
  return sqrtf(ZLay_Vec3LengthSq(value));
}

static inline ZLay_Vec3 ZLay_Vec3Normalize(ZLay_Vec3 value) {
  const float length = ZLay_Vec3Length(value);
  if (length <= ZLAY_MATH_EPSILON) {
    return ZLay_Vec3Zero();
  }
  return ZLay_Vec3Scale(value, 1.0f / length);
}

static inline ZLay_Vec4 ZLay_Vec4Make(float x, float y, float z, float w) {
  ZLay_Vec4 result;
  result.x = x;
  result.y = y;
  result.z = z;
  result.w = w;
  return result;
}

static inline ZLay_Dimensions ZLay_DimensionsMake(float width, float height) {
  ZLay_Dimensions result;
  result.width = width;
  result.height = height;
  return result;
}

static inline ZLay_Rect ZLay_RectMake(float x, float y, float width, float height) {
  ZLay_Rect result;
  result.x = x;
  result.y = y;
  result.width = width;
  result.height = height;
  return result;
}

static inline float ZLay_RectMinX(ZLay_Rect rect) {
  return rect.x;
}

static inline float ZLay_RectMinY(ZLay_Rect rect) {
  return rect.y;
}

static inline float ZLay_RectMaxX(ZLay_Rect rect) {
  return rect.x + rect.width;
}

static inline float ZLay_RectMaxY(ZLay_Rect rect) {
  return rect.y + rect.height;
}

static inline ZLay_Vec2 ZLay_RectCenter(ZLay_Rect rect) {
  return ZLay_Vec2Make(rect.x + (rect.width * 0.5f), rect.y + (rect.height * 0.5f));
}

static inline bool ZLay_RectContainsPoint(ZLay_Rect rect, ZLay_Vec2 point) {
  return point.x >= ZLay_RectMinX(rect) &&
         point.y >= ZLay_RectMinY(rect) &&
         point.x < ZLay_RectMaxX(rect) &&
         point.y < ZLay_RectMaxY(rect);
}

static inline bool ZLay_RectOverlaps(ZLay_Rect a, ZLay_Rect b) {
  return ZLay_RectMinX(a) < ZLay_RectMaxX(b) &&
         ZLay_RectMaxX(a) > ZLay_RectMinX(b) &&
         ZLay_RectMinY(a) < ZLay_RectMaxY(b) &&
         ZLay_RectMaxY(a) > ZLay_RectMinY(b);
}

static inline ZLay_Rect ZLay_RectIntersection(ZLay_Rect a, ZLay_Rect b) {
  const float min_x = ZLay_MaxFloat(ZLay_RectMinX(a), ZLay_RectMinX(b));
  const float min_y = ZLay_MaxFloat(ZLay_RectMinY(a), ZLay_RectMinY(b));
  const float max_x = ZLay_MinFloat(ZLay_RectMaxX(a), ZLay_RectMaxX(b));
  const float max_y = ZLay_MinFloat(ZLay_RectMaxY(a), ZLay_RectMaxY(b));
  if (max_x <= min_x || max_y <= min_y) {
    return ZLay_RectMake(0.0f, 0.0f, 0.0f, 0.0f);
  }
  return ZLay_RectMake(min_x, min_y, max_x - min_x, max_y - min_y);
}

static inline ZLay_Rect ZLay_RectUnion(ZLay_Rect a, ZLay_Rect b) {
  const float min_x = ZLay_MinFloat(ZLay_RectMinX(a), ZLay_RectMinX(b));
  const float min_y = ZLay_MinFloat(ZLay_RectMinY(a), ZLay_RectMinY(b));
  const float max_x = ZLay_MaxFloat(ZLay_RectMaxX(a), ZLay_RectMaxX(b));
  const float max_y = ZLay_MaxFloat(ZLay_RectMaxY(a), ZLay_RectMaxY(b));
  return ZLay_RectMake(min_x, min_y, max_x - min_x, max_y - min_y);
}

static inline ZLay_Rect ZLay_RectInset(ZLay_Rect rect, float dx, float dy) {
  return ZLay_RectMake(rect.x + dx, rect.y + dy, rect.width - (dx * 2.0f), rect.height - (dy * 2.0f));
}

static inline ZLay_Rect ZLay_RectOffset(ZLay_Rect rect, float dx, float dy) {
  return ZLay_RectMake(rect.x + dx, rect.y + dy, rect.width, rect.height);
}

static inline ZLay_Quat ZLay_QuatIdentity(void) {
  ZLay_Quat result;
  result.x = 0.0f;
  result.y = 0.0f;
  result.z = 0.0f;
  result.w = 1.0f;
  return result;
}

static inline ZLay_Quat ZLay_QuatNormalize(ZLay_Quat value) {
  const float length = sqrtf(
    (value.x * value.x) +
    (value.y * value.y) +
    (value.z * value.z) +
    (value.w * value.w)
  );
  if (length <= ZLAY_MATH_EPSILON) {
    return ZLay_QuatIdentity();
  }
  value.x /= length;
  value.y /= length;
  value.z /= length;
  value.w /= length;
  return value;
}

static inline ZLay_Quat ZLay_QuatFromAxisAngle(ZLay_Vec3 axis, float radians) {
  const ZLay_Vec3 n = ZLay_Vec3Normalize(axis);
  const float half_angle = radians * 0.5f;
  const float s = sinf(half_angle);
  ZLay_Quat result;
  result.x = n.x * s;
  result.y = n.y * s;
  result.z = n.z * s;
  result.w = cosf(half_angle);
  return ZLay_QuatNormalize(result);
}

static inline ZLay_Quat ZLay_QuatMul(ZLay_Quat a, ZLay_Quat b) {
  ZLay_Quat result;
  result.x = (a.w * b.x) + (a.x * b.w) + (a.y * b.z) - (a.z * b.y);
  result.y = (a.w * b.y) - (a.x * b.z) + (a.y * b.w) + (a.z * b.x);
  result.z = (a.w * b.z) + (a.x * b.y) - (a.y * b.x) + (a.z * b.w);
  result.w = (a.w * b.w) - (a.x * b.x) - (a.y * b.y) - (a.z * b.z);
  return result;
}

static inline ZLay_Mat4 ZLay_Mat4Identity(void) {
  ZLay_Mat4 result;
  result.m00 = 1.0f;
  result.m01 = 0.0f;
  result.m02 = 0.0f;
  result.m03 = 0.0f;
  result.m10 = 0.0f;
  result.m11 = 1.0f;
  result.m12 = 0.0f;
  result.m13 = 0.0f;
  result.m20 = 0.0f;
  result.m21 = 0.0f;
  result.m22 = 1.0f;
  result.m23 = 0.0f;
  result.m30 = 0.0f;
  result.m31 = 0.0f;
  result.m32 = 0.0f;
  result.m33 = 1.0f;
  return result;
}

static inline ZLay_Mat4 ZLay_Mat4Translation(float x, float y, float z) {
  ZLay_Mat4 result = ZLay_Mat4Identity();
  result.m03 = x;
  result.m13 = y;
  result.m23 = z;
  return result;
}

static inline ZLay_Mat4 ZLay_Mat4Scale(float x, float y, float z) {
  ZLay_Mat4 result = ZLay_Mat4Identity();
  result.m00 = x;
  result.m11 = y;
  result.m22 = z;
  return result;
}

static inline ZLay_Mat4 ZLay_Mat4RotateZ(float radians) {
  const float c = cosf(radians);
  const float s = sinf(radians);
  ZLay_Mat4 result = ZLay_Mat4Identity();
  result.m00 = c;
  result.m01 = -s;
  result.m10 = s;
  result.m11 = c;
  return result;
}

static inline ZLay_Mat4 ZLay_Mat4Ortho(
  float left,
  float right,
  float bottom,
  float top,
  float near_z,
  float far_z
) {
  ZLay_Mat4 result = ZLay_Mat4Identity();
  result.m00 = 2.0f / (right - left);
  result.m11 = 2.0f / (top - bottom);
  result.m22 = -2.0f / (far_z - near_z);
  result.m03 = -((right + left) / (right - left));
  result.m13 = -((top + bottom) / (top - bottom));
  result.m23 = -((far_z + near_z) / (far_z - near_z));
  return result;
}

static inline ZLay_Mat4 ZLay_Mat4Mul(ZLay_Mat4 a, ZLay_Mat4 b) {
  ZLay_Mat4 r;
  r.m00 = (a.m00 * b.m00) + (a.m01 * b.m10) + (a.m02 * b.m20) + (a.m03 * b.m30);
  r.m01 = (a.m00 * b.m01) + (a.m01 * b.m11) + (a.m02 * b.m21) + (a.m03 * b.m31);
  r.m02 = (a.m00 * b.m02) + (a.m01 * b.m12) + (a.m02 * b.m22) + (a.m03 * b.m32);
  r.m03 = (a.m00 * b.m03) + (a.m01 * b.m13) + (a.m02 * b.m23) + (a.m03 * b.m33);
  r.m10 = (a.m10 * b.m00) + (a.m11 * b.m10) + (a.m12 * b.m20) + (a.m13 * b.m30);
  r.m11 = (a.m10 * b.m01) + (a.m11 * b.m11) + (a.m12 * b.m21) + (a.m13 * b.m31);
  r.m12 = (a.m10 * b.m02) + (a.m11 * b.m12) + (a.m12 * b.m22) + (a.m13 * b.m32);
  r.m13 = (a.m10 * b.m03) + (a.m11 * b.m13) + (a.m12 * b.m23) + (a.m13 * b.m33);
  r.m20 = (a.m20 * b.m00) + (a.m21 * b.m10) + (a.m22 * b.m20) + (a.m23 * b.m30);
  r.m21 = (a.m20 * b.m01) + (a.m21 * b.m11) + (a.m22 * b.m21) + (a.m23 * b.m31);
  r.m22 = (a.m20 * b.m02) + (a.m21 * b.m12) + (a.m22 * b.m22) + (a.m23 * b.m32);
  r.m23 = (a.m20 * b.m03) + (a.m21 * b.m13) + (a.m22 * b.m23) + (a.m23 * b.m33);
  r.m30 = (a.m30 * b.m00) + (a.m31 * b.m10) + (a.m32 * b.m20) + (a.m33 * b.m30);
  r.m31 = (a.m30 * b.m01) + (a.m31 * b.m11) + (a.m32 * b.m21) + (a.m33 * b.m31);
  r.m32 = (a.m30 * b.m02) + (a.m31 * b.m12) + (a.m32 * b.m22) + (a.m33 * b.m32);
  r.m33 = (a.m30 * b.m03) + (a.m31 * b.m13) + (a.m32 * b.m23) + (a.m33 * b.m33);
  return r;
}

static inline ZLay_Vec4 ZLay_Mat4TransformVec4(ZLay_Mat4 m, ZLay_Vec4 v) {
  return ZLay_Vec4Make(
    (m.m00 * v.x) + (m.m01 * v.y) + (m.m02 * v.z) + (m.m03 * v.w),
    (m.m10 * v.x) + (m.m11 * v.y) + (m.m12 * v.z) + (m.m13 * v.w),
    (m.m20 * v.x) + (m.m21 * v.y) + (m.m22 * v.z) + (m.m23 * v.w),
    (m.m30 * v.x) + (m.m31 * v.y) + (m.m32 * v.z) + (m.m33 * v.w)
  );
}

static inline ZLay_Vec2 ZLay_Mat4TransformPoint2(ZLay_Mat4 m, ZLay_Vec2 point) {
  const ZLay_Vec4 p = ZLay_Mat4TransformVec4(m, ZLay_Vec4Make(point.x, point.y, 0.0f, 1.0f));
  if (fabsf(p.w) <= ZLAY_MATH_EPSILON) {
    return ZLay_Vec2Make(p.x, p.y);
  }
  return ZLay_Vec2Make(p.x / p.w, p.y / p.w);
}

#ifdef __cplusplus
}
#endif

#endif
