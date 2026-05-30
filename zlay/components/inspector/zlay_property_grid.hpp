#ifndef ZLAY_COMPONENTS_INSPECTOR_ZLAY_PROPERTY_GRID_HPP
#define ZLAY_COMPONENTS_INSPECTOR_ZLAY_PROPERTY_GRID_HPP

#include <zlay/components/inspector/zlay_property_row.hpp>
#include <zlay/components/tooling/zlay_tooling_panel.hpp>

namespace zlay {

inline Widget PropertyGrid() {
  std::vector<Widget> rows;
  rows.push_back(PropertyRow("Position", "0, 0, 0"));
  rows.push_back(PropertyRow("Rotation", "0, 0, 0"));
  rows.push_back(PropertyRow("Scale", "1, 1, 1"));
  return ToolingPanel("Properties", rows);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_INSPECTOR_ZLAY_PROPERTY_GRID_HPP
