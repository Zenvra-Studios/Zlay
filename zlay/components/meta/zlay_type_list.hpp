#ifndef ZLAY_COMPONENTS_META_ZLAY_TYPE_LIST_HPP
#define ZLAY_COMPONENTS_META_ZLAY_TYPE_LIST_HPP

#include <cstddef>

namespace zlay {

template <class... T>
struct TypeList {
  static const std::size_t size = sizeof...(T);
};

template <class List>
struct TypeListSize;

template <class... T>
struct TypeListSize<TypeList<T...> > {
  static const std::size_t value = sizeof...(T);
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_META_ZLAY_TYPE_LIST_HPP
