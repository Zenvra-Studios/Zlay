#ifndef ZLAY_COMPONENTS_TOOLING_ZLAY_PROFILER_PANEL_HPP
#define ZLAY_COMPONENTS_TOOLING_ZLAY_PROFILER_PANEL_HPP

#include <zlay/components/tooling/zlay_tooling_panel.hpp>

namespace zlay {

inline Widget ProfilerPanel() {
  std::vector<Widget> body;
  body.push_back(Text("Frame: 0.00 ms"));
  body.push_back(Text("Layout: 0.00 ms"));
  body.push_back(Text("Paint: 0.00 ms"));
  return ToolingPanel("Profiler", body);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_TOOLING_ZLAY_PROFILER_PANEL_HPP
