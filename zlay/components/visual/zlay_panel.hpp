#ifndef ZLAY_COMPONENTS_VISUAL_ZLAY_PANEL_HPP
#define ZLAY_COMPONENTS_VISUAL_ZLAY_PANEL_HPP

#include <zlay/components/layout/zlay_container.hpp>

namespace zlay {

inline Widget Panel(std::initializer_list<Widget> children) {
  ContainerProps props;
  props.children.assign(children.begin(), children.end());
  props.padding = EdgeInsets(12.0f);
  props.background = Color(25, 31, 40);
  props.radius = 6.0f;
  return Container(props);
}

inline Widget Panel(const Widget& child) {
  return Panel({child});
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_VISUAL_ZLAY_PANEL_HPP
