#ifndef ZLAY_COMPONENTS_DEBUG_ZLAY_RENDER_STATS_HPP
#define ZLAY_COMPONENTS_DEBUG_ZLAY_RENDER_STATS_HPP

#include <zlay/components/tooling/zlay_tooling_panel.hpp>

namespace zlay {

inline Widget RenderStats() {
  std::vector<Widget> body;
  body.push_back(Text("commands: 0"));
  body.push_back(Text("draw calls: 0"));
  return ToolingPanel("Render Stats", body);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_DEBUG_ZLAY_RENDER_STATS_HPP
