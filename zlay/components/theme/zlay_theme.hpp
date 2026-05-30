#ifndef ZLAY_COMPONENTS_THEME_ZLAY_THEME_HPP
#define ZLAY_COMPONENTS_THEME_ZLAY_THEME_HPP

#include <zlay/components/theme/zlay_theme_data.hpp>

namespace zlay {

inline ThemeData DarkTheme() {
  return ThemeData();
}

inline ThemeData LightTheme() {
  ThemeData theme;
  theme.colors.canvas = Color(247, 249, 252);
  theme.colors.surface = Color(255, 255, 255);
  theme.colors.surface_raised = Color(242, 245, 249);
  theme.colors.border = Color(197, 207, 219);
  theme.colors.text = Color(24, 30, 38);
  theme.colors.text_muted = Color(91, 103, 119);
  theme.colors.primary = Color(39, 104, 220);
  return theme;
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_THEME_ZLAY_THEME_HPP
