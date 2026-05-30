#ifndef ZLAY_COMPONENTS_STYLE_ZLAY_STYLE_RESOLVER_HPP
#define ZLAY_COMPONENTS_STYLE_ZLAY_STYLE_RESOLVER_HPP

#include <string>

#include <zlay/components/style/zlay_pseudo_state.hpp>
#include <zlay/components/style/zlay_style.hpp>
#include <zlay/components/style/zlay_style_sheet.hpp>

namespace zlay {

struct StyleQuery {
  std::string selector;
  std::string variant;
  PseudoStateFlags state;

  StyleQuery() : state(PseudoStateNormal) {}
};

class StyleResolver {
public:
  explicit StyleResolver(const StyleSheet* sheet = 0) : sheet_(sheet) {}

  Style resolve(const StyleQuery& query, const Style& fallback = Style()) const {
    if (sheet_) {
      const Style* style = sheet_->find(query.selector + "." + query.variant);
      if (style) {
        return *style;
      }
      style = sheet_->find(query.selector);
      if (style) {
        return *style;
      }
    }
    return fallback;
  }

private:
  const StyleSheet* sheet_;
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_STYLE_ZLAY_STYLE_RESOLVER_HPP
