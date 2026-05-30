#ifndef ZLAY_COMPONENTS_META_ZLAY_TYPE_ID_HPP
#define ZLAY_COMPONENTS_META_ZLAY_TYPE_ID_HPP

#include <stdint.h>
#include <typeinfo>

namespace zlay {

typedef uint64_t TypeId;

namespace detail {

inline TypeId fnv1a_64(const char* text) {
  TypeId hash = 14695981039346656037ull;
  if (!text) {
    return hash;
  }
  while (*text) {
    hash ^= static_cast<unsigned char>(*text++);
    hash *= 1099511628211ull;
  }
  return hash;
}

template <class T>
inline const char* type_name() {
#if defined(__clang__) || defined(__GNUC__)
  return __PRETTY_FUNCTION__;
#elif defined(_MSC_VER)
  return __FUNCSIG__;
#else
  return typeid(T).name();
#endif
}

#if __cplusplus >= 202002L
consteval TypeId consteval_fnv1a_64(const char* text) {
  TypeId hash = 14695981039346656037ull;
  while (*text) {
    hash ^= static_cast<unsigned char>(*text++);
    hash *= 1099511628211ull;
  }
  return hash;
}
#endif

} // namespace detail

template <class T>
inline TypeId type_id() {
  static const TypeId id = detail::fnv1a_64(detail::type_name<T>());
  return id;
}

#if __cplusplus >= 202002L
template <class T>
consteval TypeId static_type_id() {
#if defined(__clang__) || defined(__GNUC__)
  return detail::consteval_fnv1a_64(__PRETTY_FUNCTION__);
#elif defined(_MSC_VER)
  return detail::consteval_fnv1a_64(__FUNCSIG__);
#else
  return detail::consteval_fnv1a_64("zlay::unknown_static_type_id");
#endif
}
#endif

} // namespace zlay

#endif // ZLAY_COMPONENTS_META_ZLAY_TYPE_ID_HPP
