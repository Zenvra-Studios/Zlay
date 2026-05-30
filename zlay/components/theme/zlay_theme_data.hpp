#ifndef ZLAY_COMPONENTS_THEME_ZLAY_THEME_DATA_HPP
#define ZLAY_COMPONENTS_THEME_ZLAY_THEME_DATA_HPP

#include <zlay/components/theme/zlay_color_scheme.hpp>
#include <zlay/components/theme/zlay_radius_scale.hpp>
#include <zlay/components/theme/zlay_spacing_scale.hpp>
#include <zlay/components/theme/zlay_typography.hpp>

namespace zlay {

struct ThemeData {
  ColorScheme colors;
  SpacingScale spacing;
  RadiusScale radius;
  Typography typography;
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_THEME_ZLAY_THEME_DATA_HPP
