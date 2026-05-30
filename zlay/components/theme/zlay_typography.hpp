#ifndef ZLAY_COMPONENTS_THEME_ZLAY_TYPOGRAPHY_HPP
#define ZLAY_COMPONENTS_THEME_ZLAY_TYPOGRAPHY_HPP

#include <string>

namespace zlay {

struct Typography {
  std::string family;
  float body;
  float title;
  float caption;

  Typography() : family("Inter"), body(14.0f), title(18.0f), caption(12.0f) {}
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_THEME_ZLAY_TYPOGRAPHY_HPP
