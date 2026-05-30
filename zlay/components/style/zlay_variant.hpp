#ifndef ZLAY_COMPONENTS_STYLE_ZLAY_VARIANT_HPP
#define ZLAY_COMPONENTS_STYLE_ZLAY_VARIANT_HPP

#include <string>

namespace zlay {

struct StyleVariant {
  std::string name;

  StyleVariant() {}
  explicit StyleVariant(const std::string& value) : name(value) {}
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_STYLE_ZLAY_VARIANT_HPP
