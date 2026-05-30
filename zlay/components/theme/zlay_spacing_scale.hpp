#ifndef ZLAY_COMPONENTS_THEME_ZLAY_SPACING_SCALE_HPP
#define ZLAY_COMPONENTS_THEME_ZLAY_SPACING_SCALE_HPP

namespace zlay {

struct SpacingScale {
  float xs;
  float sm;
  float md;
  float lg;
  float xl;

  SpacingScale() : xs(4.0f), sm(8.0f), md(12.0f), lg(16.0f), xl(24.0f) {}
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_THEME_ZLAY_SPACING_SCALE_HPP
