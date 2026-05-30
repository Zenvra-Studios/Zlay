#ifndef ZLAY_COMPONENTS_TOOLING_ZLAY_DOCK_SPACE_HPP
#define ZLAY_COMPONENTS_TOOLING_ZLAY_DOCK_SPACE_HPP

#include <string>

#include <zlay/components/layout/zlay_row.hpp>
#include <zlay/components/tooling/zlay_tooling_panel.hpp>

namespace zlay {

inline Widget DockTab(const std::string& title, const Widget& child) {
  std::vector<Widget> body;
  body.push_back(child);
  return ToolingPanel(title, body);
}

inline Widget DockSpace(std::initializer_list<Widget> tabs) {
  FlexProps props;
  props.direction = FlexDirection::Row;
  props.gap = 10.0f;
  props.children.assign(tabs.begin(), tabs.end());
  return Row(props);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_TOOLING_ZLAY_DOCK_SPACE_HPP
