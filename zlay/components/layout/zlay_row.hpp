#ifndef ZLAY_COMPONENTS_LAYOUT_ZLAY_ROW_HPP
#define ZLAY_COMPONENTS_LAYOUT_ZLAY_ROW_HPP

#include <zlay/components/layout/zlay_flex.hpp>

namespace zlay {

inline Widget Row(const FlexProps& props) {
  FlexProps next = props;
  next.direction = FlexDirection::Row;
  return Flex(next);
}

inline Widget Row(std::initializer_list<Widget> children) {
  FlexProps props;
  props.direction = FlexDirection::Row;
  props.children.assign(children.begin(), children.end());
  return Flex(props);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_LAYOUT_ZLAY_ROW_HPP
