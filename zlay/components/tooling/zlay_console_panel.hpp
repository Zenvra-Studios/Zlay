#ifndef ZLAY_COMPONENTS_TOOLING_ZLAY_CONSOLE_PANEL_HPP
#define ZLAY_COMPONENTS_TOOLING_ZLAY_CONSOLE_PANEL_HPP

#include <zlay/components/tooling/zlay_tooling_panel.hpp>

namespace zlay {

inline Widget ConsolePanel() {
  std::vector<Widget> lines;
  lines.push_back(Text("> zlay components ready"));
  lines.push_back(Text("> renderer backend clean"));
  return ToolingPanel("Console", lines);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_TOOLING_ZLAY_CONSOLE_PANEL_HPP
