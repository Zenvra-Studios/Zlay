#ifndef ZLAY_COMPONENTS_LAYOUT_ZLAY_STACK_HPP
#define ZLAY_COMPONENTS_LAYOUT_ZLAY_STACK_HPP

#include <algorithm>
#include <vector>

#include <zlay/components/runtime/zlay_widget.hpp>

namespace zlay {

struct StackProps {
  std::vector<Widget> children;
  Key key;
};

class StackRenderObject : public RenderObject {
public:
  explicit StackRenderObject(const StackProps& props) : props_(props) {}

  void setProps(const StackProps& props) { props_ = props; }

  void layout(const BoxConstraints& constraints) override {
    Size desired;
    for (std::size_t i = 0; i < children().size(); ++i) {
      RenderObject* child = children()[i];
      if (!child) {
        continue;
      }
      child->layout(constraints);
      child->setOffset(Offset());
      desired.width = std::max(desired.width, child->size().width);
      desired.height = std::max(desired.height, child->size().height);
    }
    setSize(constraints.constrain(desired));
  }

  void paint(ZLay_Canvas* canvas) override {
    const bool pushed = pushLocalTransform(canvas);
    paintChildren(canvas);
    popLocalTransform(canvas, pushed);
  }

private:
  StackProps props_;
};

inline Widget Stack(const StackProps& props) {
  StackProps captured = props;
  return Widget::render(
    type_id<StackProps>(), "Stack", captured.key, captured.children,
    [captured](BuildContext&) {
      return detail::make_unique<StackRenderObject>(captured);
    },
    [captured](RenderObject& object, BuildContext&) {
      StackRenderObject* stack = dynamic_cast<StackRenderObject*>(&object);
      if (stack) {
        stack->setProps(captured);
      }
    });
}

inline Widget Stack(std::initializer_list<Widget> children) {
  StackProps props;
  props.children.assign(children.begin(), children.end());
  return Stack(props);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_LAYOUT_ZLAY_STACK_HPP
