#ifndef ZLAY_COMPONENTS_INPUT_ZLAY_BUTTON_HPP
#define ZLAY_COMPONENTS_INPUT_ZLAY_BUTTON_HPP

#include <functional>
#include <string>

#include <zlay/components/layout/zlay_container.hpp>
#include <zlay/components/style/zlay_pseudo_state.hpp>
#include <zlay/components/text/zlay_text.hpp>

namespace zlay {

enum class ButtonVariant {
  Default,
  Primary,
  Secondary,
  Ghost,
  Danger
};

struct ButtonProps {
  std::string text;
  ButtonVariant variant = ButtonVariant::Default;
  std::function<void()> onClick;
  bool disabled = false;
  Key key;
};

class ButtonRenderObject : public RenderObject {
public:
  explicit ButtonRenderObject(const ButtonProps& props) : props_(props) {}

  void setProps(const ButtonProps& props) { props_ = props; }

  void layout(const BoxConstraints& constraints) override {
    const float font_size = 14.0f;
    const float width = std::max(64.0f,
      static_cast<float>(props_.text.size()) * font_size * 0.56f + 24.0f);
    const float height = 32.0f;
    setSize(constraints.constrain(Size(width, height)));
  }

  void paint(ZLay_Canvas* canvas) override {
    const bool pushed = pushLocalTransform(canvas);
#if defined(ZLAY_ENABLE_CANVAS)
    if (canvas) {
      ZLay_Color bg = background();
      ZLay_CanvasRoundedRect(canvas, 0.0f, 0.0f, size().width, size().height,
                             6.0f, ZLay_CanvasPaintSolid(bg));
      ZLay_TextStyle style = ZLay_TextStyleMake(foreground(), 14.0f);
      const float text_x = 12.0f;
      const float text_y = 21.0f;
      ZLay_CanvasTextLen(canvas, text_x, text_y, props_.text.c_str(),
                         static_cast<uint32_t>(props_.text.size()), style);
    }
#else
    (void)canvas;
#endif
    popLocalTransform(canvas, pushed);
  }

  void click() const {
    if (!props_.disabled && props_.onClick) {
      props_.onClick();
    }
  }

private:
  ZLay_Color background() const {
    if (props_.disabled) {
      return Color(66, 74, 86);
    }
    switch (props_.variant) {
      case ButtonVariant::Primary:
        return Color(82, 148, 255);
      case ButtonVariant::Secondary:
        return Color(52, 63, 78);
      case ButtonVariant::Ghost:
        return Color(0, 0, 0, 0);
      case ButtonVariant::Danger:
        return Color(218, 74, 85);
      case ButtonVariant::Default:
      default:
        return Color(41, 50, 63);
    }
  }

  ZLay_Color foreground() const {
    return props_.disabled ? Color(164, 174, 188) : Color(244, 248, 252);
  }

  ButtonProps props_;
};

inline Widget Button(const ButtonProps& props) {
  ButtonProps captured = props;
  return Widget::render(
    type_id<ButtonProps>(), "Button", captured.key, std::vector<Widget>(),
    [captured](BuildContext&) {
      return detail::make_unique<ButtonRenderObject>(captured);
    },
    [captured](RenderObject& object, BuildContext&) {
      ButtonRenderObject* button = dynamic_cast<ButtonRenderObject*>(&object);
      if (button) {
        button->setProps(captured);
      }
    });
}

inline Widget Button(const std::string& label) {
  ButtonProps props;
  props.text = label;
  return Button(props);
}

inline Widget Button(const char* label) {
  return Button(std::string(label ? label : ""));
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_INPUT_ZLAY_BUTTON_HPP
