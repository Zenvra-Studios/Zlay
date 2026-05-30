#ifndef ZLAY_COMPONENTS_LAYOUT_ZLAY_COLUMN_HPP
#define ZLAY_COMPONENTS_LAYOUT_ZLAY_COLUMN_HPP

#include <zlay/components/layout/zlay_flex.hpp>

namespace zlay {

inline Widget Column(const FlexProps& props) {
  FlexProps next = props;
  next.direction = FlexDirection::Column;
  return Flex(next);
}

inline Widget Column(std::initializer_list<Widget> children) {
  FlexProps props;
  props.direction = FlexDirection::Column;
  props.children.assign(children.begin(), children.end());
  return Flex(props);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_LAYOUT_ZLAY_COLUMN_HPP
