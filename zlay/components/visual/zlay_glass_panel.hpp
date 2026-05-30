#ifndef ZLAY_COMPONENTS_VISUAL_ZLAY_GLASS_PANEL_HPP
#define ZLAY_COMPONENTS_VISUAL_ZLAY_GLASS_PANEL_HPP

#include <zlay/components/visual/zlay_panel.hpp>

namespace zlay {

inline Widget GlassPanel(std::initializer_list<Widget> children) {
  ContainerProps props;
  props.children.assign(children.begin(), children.end());
  props.padding = EdgeInsets(12.0f);
  props.background = Color(32, 42, 54, 196);
  props.radius = 8.0f;
  return Container(props);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_VISUAL_ZLAY_GLASS_PANEL_HPP
