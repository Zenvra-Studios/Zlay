#ifndef ZLAY_COMPONENTS_VISUAL_ZLAY_CARD_HPP
#define ZLAY_COMPONENTS_VISUAL_ZLAY_CARD_HPP

#include <zlay/components/layout/zlay_container.hpp>

namespace zlay {

inline Widget Card(std::initializer_list<Widget> children) {
  ContainerProps props;
  props.children.assign(children.begin(), children.end());
  props.padding = EdgeInsets(16.0f);
  props.background = Color(34, 42, 54);
  props.radius = 8.0f;
  return Container(props);
}

inline Widget Card(const Widget& child) {
  return Card({child});
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_VISUAL_ZLAY_CARD_HPP
