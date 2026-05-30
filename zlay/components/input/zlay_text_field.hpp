#ifndef ZLAY_COMPONENTS_INPUT_ZLAY_TEXT_FIELD_HPP
#define ZLAY_COMPONENTS_INPUT_ZLAY_TEXT_FIELD_HPP

#include <string>

#include <zlay/components/layout/zlay_container.hpp>
#include <zlay/components/text/zlay_text.hpp>

namespace zlay {

struct TextFieldProps {
  std::string value;
  std::string placeholder;
  bool disabled = false;
  Key key;
};

inline Widget TextField(const TextFieldProps& props) {
  TextProps label;
  label.text = props.value.empty() ? props.placeholder : props.value;
  label.color = props.value.empty() ? Color(151, 162, 177) : Color(232, 237, 243);

  ContainerProps box;
  box.key = props.key;
  box.children.push_back(Text(label));
  box.padding = EdgeInsets(8.0f, 6.0f, 8.0f, 6.0f);
  box.background = props.disabled ? Color(34, 39, 49) : Color(20, 25, 32);
  box.radius = 6.0f;
  return Container(box);
}

inline Widget TextField(const std::string& placeholder) {
  TextFieldProps props;
  props.placeholder = placeholder;
  return TextField(props);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_INPUT_ZLAY_TEXT_FIELD_HPP
