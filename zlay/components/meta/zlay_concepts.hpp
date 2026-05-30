#ifndef ZLAY_COMPONENTS_META_ZLAY_CONCEPTS_HPP
#define ZLAY_COMPONENTS_META_ZLAY_CONCEPTS_HPP

#include <type_traits>

#include <zlay/components/meta/zlay_type_traits.hpp>

namespace zlay {

#if __cplusplus >= 202002L
template <class T>
concept ComponentLike = is_component<T>::value;

template <class T>
concept WidgetLike = is_widget<T>::value;
#endif

template <class T>
struct component_like : is_component<T> {};

template <class T>
struct widget_like : is_widget<T> {};

} // namespace zlay

#endif // ZLAY_COMPONENTS_META_ZLAY_CONCEPTS_HPP
