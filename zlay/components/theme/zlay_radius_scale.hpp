#ifndef ZLAY_COMPONENTS_THEME_ZLAY_RADIUS_SCALE_HPP
#define ZLAY_COMPONENTS_THEME_ZLAY_RADIUS_SCALE_HPP

namespace zlay {

struct RadiusScale {
  float sm;
  float md;
  float lg;
  float pill;

  RadiusScale() : sm(4.0f), md(6.0f), lg(8.0f), pill(999.0f) {}
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_THEME_ZLAY_RADIUS_SCALE_HPP
