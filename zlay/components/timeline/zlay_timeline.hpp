#ifndef ZLAY_COMPONENTS_TIMELINE_ZLAY_TIMELINE_HPP
#define ZLAY_COMPONENTS_TIMELINE_ZLAY_TIMELINE_HPP

#include <zlay/components/tooling/zlay_tooling_panel.hpp>

namespace zlay {

inline Widget Timeline() {
  std::vector<Widget> body;
  body.push_back(Text("00:00 | keyframe"));
  body.push_back(Text("00:12 | keyframe"));
  return ToolingPanel("Timeline", body);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_TIMELINE_ZLAY_TIMELINE_HPP
