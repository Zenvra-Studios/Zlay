#ifndef ZLAY_COMPONENTS_HOOKS_ZLAY_UNMOUNT_HPP
#define ZLAY_COMPONENTS_HOOKS_ZLAY_UNMOUNT_HPP

#include <zlay/components/runtime/zlay_element.hpp>

namespace zlay {

inline void Unmount(Element& element) {
  element.unmount();
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_HOOKS_ZLAY_UNMOUNT_HPP
