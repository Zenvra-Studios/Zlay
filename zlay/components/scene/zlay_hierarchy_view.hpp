#ifndef ZLAY_COMPONENTS_SCENE_ZLAY_HIERARCHY_VIEW_HPP
#define ZLAY_COMPONENTS_SCENE_ZLAY_HIERARCHY_VIEW_HPP

#include <zlay/components/scene/zlay_scene_outliner.hpp>

namespace zlay {

inline Widget HierarchyView() {
  return SceneOutliner();
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_SCENE_ZLAY_HIERARCHY_VIEW_HPP
