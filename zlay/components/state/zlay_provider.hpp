#ifndef ZLAY_COMPONENTS_STATE_ZLAY_PROVIDER_HPP
#define ZLAY_COMPONENTS_STATE_ZLAY_PROVIDER_HPP

#include <zlay/components/runtime/zlay_widget.hpp>

namespace zlay {

template <class T>
struct ProviderProps {
  T value;
  Widget child;
  Key key;
};

template <class T>
inline Widget Provider(const ProviderProps<T>& props) {
  return Widget::stateless(type_id<ProviderProps<T> >(), "Provider", props.key,
                           [props](BuildContext&) { return props.child; });
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_STATE_ZLAY_PROVIDER_HPP
