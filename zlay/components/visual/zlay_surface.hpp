#ifndef ZLAY_COMPONENTS_VISUAL_ZLAY_SURFACE_HPP
#define ZLAY_COMPONENTS_VISUAL_ZLAY_SURFACE_HPP

#include <zlay/components/visual/zlay_panel.hpp>

namespace zlay {

inline Widget Surface(std::initializer_list<Widget> children) {
  return Panel(children);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_VISUAL_ZLAY_SURFACE_HPP
