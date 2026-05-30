#ifndef ZLAY_COMPONENTS_STYLE_ZLAY_STYLE_TOKEN_HPP
#define ZLAY_COMPONENTS_STYLE_ZLAY_STYLE_TOKEN_HPP

#include <string>

namespace zlay {

struct StyleToken {
  std::string name;

  StyleToken() {}
  explicit StyleToken(const std::string& value) : name(value) {}
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_STYLE_ZLAY_STYLE_TOKEN_HPP
