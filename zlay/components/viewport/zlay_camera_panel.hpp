#ifndef ZLAY_COMPONENTS_VIEWPORT_ZLAY_CAMERA_PANEL_HPP
#define ZLAY_COMPONENTS_VIEWPORT_ZLAY_CAMERA_PANEL_HPP

#include <zlay/components/tooling/zlay_tooling_panel.hpp>

namespace zlay {

inline Widget CameraPanel() {
  std::vector<Widget> body;
  body.push_back(Text("FOV 60"));
  body.push_back(Text("Near 0.1"));
  body.push_back(Text("Far 1000"));
  return ToolingPanel("Camera", body);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_VIEWPORT_ZLAY_CAMERA_PANEL_HPP
