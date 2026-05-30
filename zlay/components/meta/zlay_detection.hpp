#ifndef ZLAY_COMPONENTS_META_ZLAY_DETECTION_HPP
#define ZLAY_COMPONENTS_META_ZLAY_DETECTION_HPP

#include <type_traits>

namespace zlay {

template <class...>
struct make_void {
  typedef void type;
};

template <class... T>
using void_t = typename make_void<T...>::type;

template <class T, class = void>
struct is_detected : std::false_type {};

template <class T>
struct is_detected<T, void_t<T> > : std::true_type {};

} // namespace zlay

#endif // ZLAY_COMPONENTS_META_ZLAY_DETECTION_HPP
