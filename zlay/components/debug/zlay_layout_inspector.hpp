#ifndef ZLAY_COMPONENTS_DEBUG_ZLAY_LAYOUT_INSPECTOR_HPP
#define ZLAY_COMPONENTS_DEBUG_ZLAY_LAYOUT_INSPECTOR_HPP

#include <zlay/components/tooling/zlay_tooling_panel.hpp>

namespace zlay {

inline Widget LayoutInspector() {
  std::vector<Widget> body;
  body.push_back(Text("No node selected"));
  return ToolingPanel("Layout Inspector", body);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_DEBUG_ZLAY_LAYOUT_INSPECTOR_HPP
