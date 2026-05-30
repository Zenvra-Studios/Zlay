#ifndef ZLAY_COMPONENTS_HOOKS_ZLAY_UPDATE_HPP
#define ZLAY_COMPONENTS_HOOKS_ZLAY_UPDATE_HPP

#include <zlay/components/runtime/zlay_element.hpp>

namespace zlay {

inline void Update(Element& element, const Widget& widget, BuildContext& ctx) {
  element.update(widget, ctx);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_HOOKS_ZLAY_UPDATE_HPP
