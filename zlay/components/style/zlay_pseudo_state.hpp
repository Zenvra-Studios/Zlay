#ifndef ZLAY_COMPONENTS_STYLE_ZLAY_PSEUDO_STATE_HPP
#define ZLAY_COMPONENTS_STYLE_ZLAY_PSEUDO_STATE_HPP

#include <stdint.h>

namespace zlay {

enum PseudoState {
  PseudoStateNormal = 0,
  PseudoStateHover = 1 << 0,
  PseudoStateActive = 1 << 1,
  PseudoStateFocus = 1 << 2,
  PseudoStateDisabled = 1 << 3,
  PseudoStateSelected = 1 << 4,
  PseudoStateDragging = 1 << 5,
  PseudoStateError = 1 << 6
};

typedef uint32_t PseudoStateFlags;

} // namespace zlay

#endif // ZLAY_COMPONENTS_STYLE_ZLAY_PSEUDO_STATE_HPP
