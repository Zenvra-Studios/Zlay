#ifndef ZLAY_COMPONENTS_HOOKS_ZLAY_DIRTY_HPP
#define ZLAY_COMPONENTS_HOOKS_ZLAY_DIRTY_HPP

#include <zlay/components/runtime/zlay_element.hpp>

namespace zlay {

inline void MarkDirty(Element& element) {
  element.markDirty();
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_HOOKS_ZLAY_DIRTY_HPP
