#ifndef ZLAY_COMPONENTS_LAYOUT_ZLAY_CONTAINER_HPP
#define ZLAY_COMPONENTS_LAYOUT_ZLAY_CONTAINER_HPP

#include <vector>

#include <zlay/components/runtime/zlay_widget.hpp>
#include <zlay/components/style/zlay_style.hpp>

namespace zlay {

struct ContainerProps {
  std::vector<Widget> children;
  float width = -1.0f;
  float height = -1.0f;
  EdgeInsets padding;
  ZLay_Color background = Color(0, 0, 0, 0);
  ZLay_Color border = Color(0, 0, 0, 0);
  float border_width = 0.0f;
  float radius = 0.0f;
  Key key;
};

class ContainerRenderObject : public RenderObject {
public:
  explicit ContainerRenderObject(const ContainerProps& props) : props_(props) {}

  void setProps(const ContainerProps& props) { props_ = props; }

  void layout(const BoxConstraints& constraints) override {
    BoxConstraints child_constraints = constraints.deflate(props_.padding);
    Size content;
    for (std::size_t i = 0; i < children().size(); ++i) {
      RenderObject* child = children()[i];
      if (!child) {
        continue;
      }
      child->layout(child_constraints);
      child->setOffset(Offset(props_.padding.left, props_.padding.top));
      Size child_size = child->size();
      content.width = std::max(content.width, child_size.width);
      content.height = std::max(content.height, child_size.height);
    }

    Size desired(
      props_.width >= 0.0f ? props_.width : content.width + props_.padding.horizontal(),
      props_.height >= 0.0f ? props_.height : content.height + props_.padding.vertical());
    setSize(constraints.constrain(desired));
  }

  void paint(ZLay_Canvas* canvas) override {
    const bool pushed = pushLocalTransform(canvas);
#if defined(ZLAY_ENABLE_CANVAS)
    if (canvas && props_.background.a > 0u) {
      ZLay_CanvasPaint paint = ZLay_CanvasPaintSolid(props_.background);
      if (props_.radius > 0.0f) {
        ZLay_CanvasRoundedRect(canvas, 0.0f, 0.0f, size().width, size().height,
                               props_.radius, paint);
      } else {
        ZLay_CanvasRect(canvas, 0.0f, 0.0f, size().width, size().height, paint);
      }
    }
#else
    (void)canvas;
#endif
    paintChildren(canvas);
    popLocalTransform(canvas, pushed);
  }

private:
  ContainerProps props_;
};

inline Widget Container(const ContainerProps& props) {
  ContainerProps captured = props;
  return Widget::render(
    type_id<ContainerProps>(), "Container", captured.key, captured.children,
    [captured](BuildContext&) {
      return detail::make_unique<ContainerRenderObject>(captured);
    },
    [captured](RenderObject& object, BuildContext&) {
      ContainerRenderObject* container =
        dynamic_cast<ContainerRenderObject*>(&object);
      if (container) {
        container->setProps(captured);
      }
    });
}

inline Widget Container(const Widget& child) {
  ContainerProps props;
  props.children.push_back(child);
  return Container(props);
}

inline Widget Container(std::initializer_list<Widget> children) {
  ContainerProps props;
  props.children.assign(children.begin(), children.end());
  return Container(props);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_LAYOUT_ZLAY_CONTAINER_HPP
