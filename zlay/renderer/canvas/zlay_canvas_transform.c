#include <renderer/canvas/zlay_canvas_transform.h>

#include <math.h>

ZLay_CanvasTransform ZLay_CanvasTransformIdentity(void) {
  return (ZLay_CanvasTransform){
    1.0f, 0.0f, 0.0f,
    0.0f, 1.0f, 0.0f
  };
}

ZLay_CanvasTransform ZLay_CanvasTransformTranslate(float x, float y) {
  return (ZLay_CanvasTransform){
    1.0f, 0.0f, x,
    0.0f, 1.0f, y
  };
}

ZLay_CanvasTransform ZLay_CanvasTransformScale(float sx, float sy) {
  return (ZLay_CanvasTransform){
    sx, 0.0f, 0.0f,
    0.0f, sy, 0.0f
  };
}

ZLay_CanvasTransform ZLay_CanvasTransformRotate(float radians) {
  const float c = cosf(radians);
  const float s = sinf(radians);
  return (ZLay_CanvasTransform){
    c, -s, 0.0f,
    s, c, 0.0f
  };
}

ZLay_CanvasTransform ZLay_CanvasTransformMultiply(ZLay_CanvasTransform a, ZLay_CanvasTransform b) {
  ZLay_CanvasTransform result;
  result.m00 = a.m00 * b.m00 + a.m01 * b.m10;
  result.m01 = a.m00 * b.m01 + a.m01 * b.m11;
  result.m02 = a.m00 * b.m02 + a.m01 * b.m12 + a.m02;
  result.m10 = a.m10 * b.m00 + a.m11 * b.m10;
  result.m11 = a.m10 * b.m01 + a.m11 * b.m11;
  result.m12 = a.m10 * b.m02 + a.m11 * b.m12 + a.m12;
  return result;
}

ZLay_Vec2 ZLay_CanvasTransformPoint(ZLay_CanvasTransform transform, ZLay_Vec2 point) {
  return (ZLay_Vec2){
    transform.m00 * point.x + transform.m01 * point.y + transform.m02,
    transform.m10 * point.x + transform.m11 * point.y + transform.m12
  };
}

ZLay_Rect ZLay_CanvasTransformRect(ZLay_CanvasTransform transform, ZLay_Rect rect) {
  const ZLay_Vec2 p0 = ZLay_CanvasTransformPoint(transform, (ZLay_Vec2){rect.x, rect.y});
  const ZLay_Vec2 p1 = ZLay_CanvasTransformPoint(transform, (ZLay_Vec2){rect.x + rect.width, rect.y});
  const ZLay_Vec2 p2 = ZLay_CanvasTransformPoint(transform, (ZLay_Vec2){rect.x + rect.width, rect.y + rect.height});
  const ZLay_Vec2 p3 = ZLay_CanvasTransformPoint(transform, (ZLay_Vec2){rect.x, rect.y + rect.height});
  const float min_x = ZLay_MinFloat(ZLay_MinFloat(p0.x, p1.x), ZLay_MinFloat(p2.x, p3.x));
  const float min_y = ZLay_MinFloat(ZLay_MinFloat(p0.y, p1.y), ZLay_MinFloat(p2.y, p3.y));
  const float max_x = ZLay_MaxFloat(ZLay_MaxFloat(p0.x, p1.x), ZLay_MaxFloat(p2.x, p3.x));
  const float max_y = ZLay_MaxFloat(ZLay_MaxFloat(p0.y, p1.y), ZLay_MaxFloat(p2.y, p3.y));
  return (ZLay_Rect){min_x, min_y, max_x - min_x, max_y - min_y};
}

bool ZLay_CanvasTransformIsIdentity(ZLay_CanvasTransform transform) {
  const ZLay_CanvasTransform identity = ZLay_CanvasTransformIdentity();
  return ZLay_NearlyEqualFloat(transform.m00, identity.m00, ZLAY_MATH_EPSILON) &&
         ZLay_NearlyEqualFloat(transform.m01, identity.m01, ZLAY_MATH_EPSILON) &&
         ZLay_NearlyEqualFloat(transform.m02, identity.m02, ZLAY_MATH_EPSILON) &&
         ZLay_NearlyEqualFloat(transform.m10, identity.m10, ZLAY_MATH_EPSILON) &&
         ZLay_NearlyEqualFloat(transform.m11, identity.m11, ZLAY_MATH_EPSILON) &&
         ZLay_NearlyEqualFloat(transform.m12, identity.m12, ZLAY_MATH_EPSILON);
}
