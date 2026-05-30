#ifndef ZLAY_COMPONENTS_SCENE_ZLAY_SCENE_OUTLINER_HPP
#define ZLAY_COMPONENTS_SCENE_ZLAY_SCENE_OUTLINER_HPP

#include <zlay/components/tooling/zlay_tooling_panel.hpp>

namespace zlay {

inline Widget EntityRow(const std::string& name) {
  return Text(name);
}

inline Widget SceneOutliner() {
  std::vector<Widget> rows;
  rows.push_back(EntityRow("Root"));
  rows.push_back(EntityRow("Camera"));
  rows.push_back(EntityRow("Light"));
  return ToolingPanel("Scene", rows);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_SCENE_ZLAY_SCENE_OUTLINER_HPP
