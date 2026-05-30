#ifndef ZLAY_COMPONENTS_RUNTIME_ZLAY_GEOMETRY_HPP
#define ZLAY_COMPONENTS_RUNTIME_ZLAY_GEOMETRY_HPP

#include <algorithm>
#include <limits>

namespace zlay {

struct Size {
  float width;
  float height;

  Size() : width(0.0f), height(0.0f) {}
  Size(float w, float h) : width(w), height(h) {}
};

struct Offset {
  float x;
  float y;

  Offset() : x(0.0f), y(0.0f) {}
  Offset(float px, float py) : x(px), y(py) {}
};

struct Rect {
  float x;
  float y;
  float width;
  float height;

  Rect() : x(0.0f), y(0.0f), width(0.0f), height(0.0f) {}
  Rect(float px, float py, float w, float h)
      : x(px), y(py), width(w), height(h) {}

  bool contains(float px, float py) const {
    return px >= x && py >= y && px <= x + width && py <= y + height;
  }
};

struct EdgeInsets {
  float left;
  float top;
  float right;
  float bottom;

  EdgeInsets() : left(0.0f), top(0.0f), right(0.0f), bottom(0.0f) {}
  explicit EdgeInsets(float all)
      : left(all), top(all), right(all), bottom(all) {}
  EdgeInsets(float l, float t, float r, float b)
      : left(l), top(t), right(r), bottom(b) {}

  float horizontal() const { return left + right; }
  float vertical() const { return top + bottom; }
};

struct BoxConstraints {
  float min_width;
  float max_width;
  float min_height;
  float max_height;

  BoxConstraints()
      : min_width(0.0f),
        max_width(std::numeric_limits<float>::infinity()),
        min_height(0.0f),
        max_height(std::numeric_limits<float>::infinity()) {}

  BoxConstraints(float min_w, float max_w, float min_h, float max_h)
      : min_width(min_w),
        max_width(max_w),
        min_height(min_h),
        max_height(max_h) {}

  static BoxConstraints tight(Size size) {
    return BoxConstraints(size.width, size.width, size.height, size.height);
  }

  static BoxConstraints loose(Size size) {
    return BoxConstraints(0.0f, size.width, 0.0f, size.height);
  }

  Size constrain(Size size) const {
    const float w = std::max(min_width, std::min(max_width, size.width));
    const float h = std::max(min_height, std::min(max_height, size.height));
    return Size(w, h);
  }

  BoxConstraints deflate(EdgeInsets insets) const {
    const float h = insets.horizontal();
    const float v = insets.vertical();
    return BoxConstraints(
      std::max(0.0f, min_width - h),
      std::max(0.0f, max_width - h),
      std::max(0.0f, min_height - v),
      std::max(0.0f, max_height - v));
  }
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_RUNTIME_ZLAY_GEOMETRY_HPP
