#ifndef ZLAY_COMPONENTS_PAINTING_ZLAY_CANVAS_VIEW_HPP
#define ZLAY_COMPONENTS_PAINTING_ZLAY_CANVAS_VIEW_HPP

#include <functional>

#include <zlay/components/runtime/zlay_widget.hpp>

namespace zlay {

struct CanvasViewProps {
  float width = 320.0f;
  float height = 180.0f;
  std::function<void(ZLay_Canvas&, const Rect&)> onPaint;
  Key key;
};

class CanvasViewRenderObject : public RenderObject {
public:
  explicit CanvasViewRenderObject(const CanvasViewProps& props) : props_(props) {}

  void setProps(const CanvasViewProps& props) { props_ = props; }

  void layout(const BoxConstraints& constraints) override {
    setSize(constraints.constrain(Size(props_.width, props_.height)));
  }

  void paint(ZLay_Canvas* canvas) override {
    const bool pushed = pushLocalTransform(canvas);
#if defined(ZLAY_ENABLE_CANVAS)
    if (canvas && props_.onPaint) {
      props_.onPaint(*canvas, Rect(0.0f, 0.0f, size().width, size().height));
    }
#else
    (void)canvas;
#endif
    popLocalTransform(canvas, pushed);
  }

private:
  CanvasViewProps props_;
};

inline Widget CanvasView(const CanvasViewProps& props) {
  CanvasViewProps captured = props;
  return Widget::render(
    type_id<CanvasViewProps>(), "CanvasView", captured.key, std::vector<Widget>(),
    [captured](BuildContext&) {
      return detail::make_unique<CanvasViewRenderObject>(captured);
    },
    [captured](RenderObject& object, BuildContext&) {
      CanvasViewRenderObject* view =
        dynamic_cast<CanvasViewRenderObject*>(&object);
      if (view) {
        view->setProps(captured);
      }
    });
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_PAINTING_ZLAY_CANVAS_VIEW_HPP
