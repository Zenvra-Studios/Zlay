#ifndef ZLAY_COMPONENTS_TEXT_ZLAY_TEXT_HPP
#define ZLAY_COMPONENTS_TEXT_ZLAY_TEXT_HPP

#include <algorithm>
#include <string>

#include <zlay/components/runtime/zlay_widget.hpp>
#include <zlay/components/style/zlay_style.hpp>

namespace zlay {

struct TextProps {
  std::string text;
  float font_size = 14.0f;
  ZLay_Color color = Color(232, 237, 243, 255);
  std::string font_family = "Inter";
  Key key;
};

class TextRenderObject : public RenderObject {
public:
  explicit TextRenderObject(const TextProps& props) : props_(props) {}

  void setProps(const TextProps& props) { props_ = props; }

  void layout(const BoxConstraints& constraints) override {
    const float width =
      static_cast<float>(props_.text.size()) * props_.font_size * 0.56f;
    const float height = props_.font_size * 1.35f;
    setSize(constraints.constrain(Size(width, height)));
  }

  void paint(ZLay_Canvas* canvas) override {
    const bool pushed = pushLocalTransform(canvas);
#if defined(ZLAY_ENABLE_CANVAS)
    if (canvas) {
      ZLay_TextStyle style = ZLay_TextStyleMake(props_.color, props_.font_size);
      if (!props_.font_family.empty()) {
        style = ZLay_TextStyleWithFontFamilyLen(
          style, props_.font_family.c_str(),
          static_cast<uint32_t>(props_.font_family.size()));
      }
      ZLay_CanvasTextLen(canvas, 0.0f, props_.font_size, props_.text.c_str(),
                         static_cast<uint32_t>(props_.text.size()), style);
    }
#else
    (void)canvas;
#endif
    paintChildren(canvas);
    popLocalTransform(canvas, pushed);
  }

private:
  TextProps props_;
};

inline Widget Text(const TextProps& props) {
  TextProps captured = props;
  return Widget::render(
    type_id<TextProps>(), "Text", captured.key, std::vector<Widget>(),
    [captured](BuildContext&) {
      return detail::make_unique<TextRenderObject>(captured);
    },
    [captured](RenderObject& object, BuildContext&) {
      TextRenderObject* text = dynamic_cast<TextRenderObject*>(&object);
      if (text) {
        text->setProps(captured);
      }
    });
}

inline Widget Text(const std::string& value) {
  TextProps props;
  props.text = value;
  return Text(props);
}

inline Widget Text(const char* value) {
  return Text(std::string(value ? value : ""));
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_TEXT_ZLAY_TEXT_HPP
