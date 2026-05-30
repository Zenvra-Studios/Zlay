#ifndef ZLAY_COMPONENTS_HOOKS_ZLAY_REBUILD_HPP
#define ZLAY_COMPONENTS_HOOKS_ZLAY_REBUILD_HPP

#include <zlay/components/runtime/zlay_element.hpp>

namespace zlay {

inline void Rebuild(Element& element, BuildContext& ctx) {
  element.rebuild(ctx);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_HOOKS_ZLAY_REBUILD_HPP
