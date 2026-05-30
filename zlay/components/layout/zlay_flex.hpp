#ifndef ZLAY_COMPONENTS_LAYOUT_ZLAY_FLEX_HPP
#define ZLAY_COMPONENTS_LAYOUT_ZLAY_FLEX_HPP

#include <algorithm>
#include <vector>

#include <zlay/components/runtime/zlay_widget.hpp>

namespace zlay {

enum class FlexDirection {
  Row,
  Column
};

struct FlexProps {
  std::vector<Widget> children;
  FlexDirection direction = FlexDirection::Column;
  float gap = 8.0f;
  Key key;
};

class FlexRenderObject : public RenderObject {
public:
  explicit FlexRenderObject(const FlexProps& props) : props_(props) {}

  void setProps(const FlexProps& props) { props_ = props; }

  void layout(const BoxConstraints& constraints) override {
    float main = 0.0f;
    float cross = 0.0f;
    const bool row = props_.direction == FlexDirection::Row;

    for (std::size_t i = 0; i < children().size(); ++i) {
      RenderObject* child = children()[i];
      if (!child) {
        continue;
      }
      child->layout(BoxConstraints(0.0f, constraints.max_width,
                                   0.0f, constraints.max_height));
      Size child_size = child->size();
      if (row) {
        child->setOffset(Offset(main, 0.0f));
        main += child_size.width;
        cross = std::max(cross, child_size.height);
      } else {
        child->setOffset(Offset(0.0f, main));
        main += child_size.height;
        cross = std::max(cross, child_size.width);
      }
      if (i + 1u < children().size()) {
        main += props_.gap;
      }
    }

    Size desired = row ? Size(main, cross) : Size(cross, main);
    setSize(constraints.constrain(desired));
  }

  void paint(ZLay_Canvas* canvas) override {
    const bool pushed = pushLocalTransform(canvas);
    paintChildren(canvas);
    popLocalTransform(canvas, pushed);
  }

private:
  FlexProps props_;
};

inline Widget Flex(const FlexProps& props) {
  FlexProps captured = props;
  return Widget::render(
    type_id<FlexProps>(), "Flex", captured.key, captured.children,
    [captured](BuildContext&) {
      return detail::make_unique<FlexRenderObject>(captured);
    },
    [captured](RenderObject& object, BuildContext&) {
      FlexRenderObject* flex = dynamic_cast<FlexRenderObject*>(&object);
      if (flex) {
        flex->setProps(captured);
      }
    });
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_LAYOUT_ZLAY_FLEX_HPP
