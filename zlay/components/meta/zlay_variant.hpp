#ifndef ZLAY_COMPONENTS_META_ZLAY_VARIANT_HPP
#define ZLAY_COMPONENTS_META_ZLAY_VARIANT_HPP

#if __cplusplus >= 201703L
#  include <variant>
#endif

namespace zlay {

#if __cplusplus >= 201703L
template <class... T>
using Variant = std::variant<T...>;
#endif

struct VariantUnavailable {};

} // namespace zlay

#endif // ZLAY_COMPONENTS_META_ZLAY_VARIANT_HPP
