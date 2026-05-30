#ifndef ZLAY_COMPONENTS_THEME_ZLAY_THEME_PROVIDER_HPP
#define ZLAY_COMPONENTS_THEME_ZLAY_THEME_PROVIDER_HPP

#include <zlay/components/runtime/zlay_component.hpp>
#include <zlay/components/theme/zlay_theme_data.hpp>

namespace zlay {

struct ThemeProviderProps {
  ThemeData value;
  Widget child;
  Key key;
};

inline Widget ThemeProvider(const ThemeProviderProps& props) {
  return Widget::stateless(type_id<ThemeProviderProps>(), "ThemeProvider", props.key,
                           [props](BuildContext&) { return props.child; });
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_THEME_ZLAY_THEME_PROVIDER_HPP
