#ifndef ZLAY_COMPONENTS_META_ZLAY_TYPE_TRAITS_HPP
#define ZLAY_COMPONENTS_META_ZLAY_TYPE_TRAITS_HPP

#include <type_traits>

namespace zlay {

class Component;
class Widget;

template <class T>
struct is_component : std::is_base_of<Component, T> {};

template <class T>
struct is_widget : std::is_same<typename std::decay<T>::type, Widget> {};

} // namespace zlay

#endif // ZLAY_COMPONENTS_META_ZLAY_TYPE_TRAITS_HPP
