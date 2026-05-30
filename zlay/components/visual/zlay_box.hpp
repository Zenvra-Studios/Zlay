#ifndef ZLAY_COMPONENTS_VISUAL_ZLAY_BOX_HPP
#define ZLAY_COMPONENTS_VISUAL_ZLAY_BOX_HPP

#include <zlay/components/layout/zlay_container.hpp>

namespace zlay {

inline Widget Box(float width, float height, ZLay_Color color) {
  ContainerProps props;
  props.width = width;
  props.height = height;
  props.background = color;
  return Container(props);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_VISUAL_ZLAY_BOX_HPP
