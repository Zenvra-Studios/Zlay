#ifndef ZLAY_COMPONENTS_TOOLING_ZLAY_TOOLING_PANEL_HPP
#define ZLAY_COMPONENTS_TOOLING_ZLAY_TOOLING_PANEL_HPP

#include <string>
#include <vector>

#include <zlay/components/layout/zlay_column.hpp>
#include <zlay/components/text/zlay_text.hpp>
#include <zlay/components/visual/zlay_panel.hpp>

namespace zlay {

inline Widget ToolingPanel(const std::string& title,
                           const std::vector<Widget>& children = std::vector<Widget>()) {
  std::vector<Widget> items;
  TextProps heading;
  heading.text = title;
  heading.font_size = 13.0f;
  heading.color = Color(185, 197, 212);
  items.push_back(Text(heading));
  items.insert(items.end(), children.begin(), children.end());

  FlexProps column;
  column.gap = 8.0f;
  column.children = items;
  return Panel(Column(column));
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_TOOLING_ZLAY_TOOLING_PANEL_HPP
