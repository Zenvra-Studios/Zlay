#ifndef ZLAY_COMPONENTS_INSPECTOR_ZLAY_PROPERTY_ROW_HPP
#define ZLAY_COMPONENTS_INSPECTOR_ZLAY_PROPERTY_ROW_HPP

#include <string>

#include <zlay/components/layout/zlay_row.hpp>
#include <zlay/components/text/zlay_text.hpp>

namespace zlay {

inline Widget PropertyRow(const std::string& name, const std::string& value) {
  return Row({Text(name), Text(value)});
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_INSPECTOR_ZLAY_PROPERTY_ROW_HPP
