#ifndef ZLAY_COMPONENTS_STYLE_ZLAY_STYLE_SHEET_HPP
#define ZLAY_COMPONENTS_STYLE_ZLAY_STYLE_SHEET_HPP

#include <map>
#include <string>

#include <zlay/components/style/zlay_style.hpp>

namespace zlay {

class StyleSheet {
public:
  void set(const std::string& selector, const Style& style) {
    styles_[selector] = style;
  }

  const Style* find(const std::string& selector) const {
    std::map<std::string, Style>::const_iterator it = styles_.find(selector);
    return it == styles_.end() ? 0 : &it->second;
  }

private:
  std::map<std::string, Style> styles_;
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_STYLE_ZLAY_STYLE_SHEET_HPP
