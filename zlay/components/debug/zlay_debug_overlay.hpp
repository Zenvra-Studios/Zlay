#ifndef ZLAY_COMPONENTS_DEBUG_ZLAY_DEBUG_OVERLAY_HPP
#define ZLAY_COMPONENTS_DEBUG_ZLAY_DEBUG_OVERLAY_HPP

#include <zlay/components/debug/zlay_render_stats.hpp>

namespace zlay {

inline Widget DebugOverlay() {
  return RenderStats();
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_DEBUG_ZLAY_DEBUG_OVERLAY_HPP
