#ifndef ZLAY_COMPONENTS_THEME_ZLAY_SHADOW_SCALE_HPP
#define ZLAY_COMPONENTS_THEME_ZLAY_SHADOW_SCALE_HPP

namespace zlay {

struct ShadowScale {
  float sm;
  float md;
  float lg;

  ShadowScale() : sm(4.0f), md(10.0f), lg(18.0f) {}
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_THEME_ZLAY_SHADOW_SCALE_HPP
