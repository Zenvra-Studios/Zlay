#ifndef ZLAY_COMPONENTS_STATE_ZLAY_VALUE_NOTIFIER_HPP
#define ZLAY_COMPONENTS_STATE_ZLAY_VALUE_NOTIFIER_HPP

#include <zlay/components/state/zlay_signal.hpp>

namespace zlay {

template <class T>
class ValueNotifier : public Signal<T> {
public:
  ValueNotifier() : Signal<T>() {}
  explicit ValueNotifier(const T& value) : Signal<T>(value) {}
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_STATE_ZLAY_VALUE_NOTIFIER_HPP
