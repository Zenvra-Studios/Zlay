#ifndef ZLAY_COMPONENTS_VISUAL_ZLAY_DIVIDER_HPP
#define ZLAY_COMPONENTS_VISUAL_ZLAY_DIVIDER_HPP

#include <zlay/components/layout/zlay_container.hpp>

namespace zlay {

inline Widget Divider(bool vertical = false) {
  ContainerProps props;
  props.width = vertical ? 1.0f : -1.0f;
  props.height = vertical ? -1.0f : 1.0f;
  props.background = Color(75, 88, 105);
  return Container(props);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_VISUAL_ZLAY_DIVIDER_HPP
