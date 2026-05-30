#ifndef ZLAY_COMPONENTS_STYLE_ZLAY_STYLE_HPP
#define ZLAY_COMPONENTS_STYLE_ZLAY_STYLE_HPP

#include <zlay.h>

namespace zlay {

inline ZLay_Color Color(unsigned char r, unsigned char g, unsigned char b,
                        unsigned char a = 255u) {
  ZLay_Color color;
  color.r = r;
  color.g = g;
  color.b = b;
  color.a = a;
  return color;
}

struct Style {
  ZLay_Color background;
  ZLay_Color foreground;
  ZLay_Color border;
  float border_width;
  float radius;

  Style()
      : background(Color(0, 0, 0, 0)),
        foreground(Color(232, 237, 243, 255)),
        border(Color(92, 104, 119, 255)),
        border_width(0.0f),
        radius(6.0f) {}
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_STYLE_ZLAY_STYLE_HPP
