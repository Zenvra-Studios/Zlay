#ifndef ZLAY_COMPONENTS_THEME_ZLAY_COLOR_SCHEME_HPP
#define ZLAY_COMPONENTS_THEME_ZLAY_COLOR_SCHEME_HPP

#include <zlay/components/style/zlay_style.hpp>

namespace zlay {

struct ColorScheme {
  ZLay_Color canvas;
  ZLay_Color surface;
  ZLay_Color surface_raised;
  ZLay_Color border;
  ZLay_Color text;
  ZLay_Color text_muted;
  ZLay_Color primary;

  ColorScheme()
      : canvas(Color(14, 18, 24)),
        surface(Color(25, 31, 40)),
        surface_raised(Color(34, 42, 54)),
        border(Color(75, 88, 105)),
        text(Color(235, 240, 247)),
        text_muted(Color(158, 170, 186)),
        primary(Color(82, 148, 255)) {}
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_THEME_ZLAY_COLOR_SCHEME_HPP
