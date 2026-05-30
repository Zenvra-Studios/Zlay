#ifndef ZLAY_COMPONENTS_TOOLING_ZLAY_COMMAND_PALETTE_HPP
#define ZLAY_COMPONENTS_TOOLING_ZLAY_COMMAND_PALETTE_HPP

#include <zlay/components/input/zlay_text_field.hpp>
#include <zlay/components/tooling/zlay_tooling_panel.hpp>

namespace zlay {

inline Widget CommandPalette() {
  std::vector<Widget> body;
  body.push_back(TextField("Search commands..."));
  body.push_back(Text("Open Viewport"));
  body.push_back(Text("Toggle Render Stats"));
  return ToolingPanel("Command Palette", body);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_TOOLING_ZLAY_COMMAND_PALETTE_HPP
