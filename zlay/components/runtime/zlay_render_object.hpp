#ifndef ZLAY_COMPONENTS_RUNTIME_ZLAY_RENDER_OBJECT_HPP
#define ZLAY_COMPONENTS_RUNTIME_ZLAY_RENDER_OBJECT_HPP

#include <vector>

#include <renderer/canvas/zlay_canvas.h>
#include <zlay/components/runtime/zlay_geometry.hpp>

namespace zlay {

class RenderObject {
public:
  virtual ~RenderObject() {}

  virtual void layout(const BoxConstraints& constraints) {
    setSize(constraints.constrain(Size()));
  }

  virtual void paint(ZLay_Canvas* canvas) {
    paintChildren(canvas);
  }

  virtual bool hitTest(float x, float y) const {
    const Offset p = offset();
    const Size s = size();
    return x >= p.x && y >= p.y && x <= p.x + s.width && y <= p.y + s.height;
  }

  Size size() const { return size_; }
  Offset offset() const { return offset_; }

  void setSize(Size value) { size_ = value; }
  void setOffset(Offset value) { offset_ = value; }

  void setChildren(const std::vector<RenderObject*>& children) {
    children_ = children;
  }

protected:
  const std::vector<RenderObject*>& children() const { return children_; }

  bool pushLocalTransform(ZLay_Canvas* canvas) const {
#if defined(ZLAY_ENABLE_CANVAS)
    if (!canvas) {
      return false;
    }
    if (!ZLay_CanvasPushTransform(canvas)) {
      return false;
    }
    ZLay_CanvasTranslate(canvas, offset_.x, offset_.y);
    return true;
#else
    (void)canvas;
    return false;
#endif
  }

  void popLocalTransform(ZLay_Canvas* canvas, bool pushed) const {
#if defined(ZLAY_ENABLE_CANVAS)
    if (canvas && pushed) {
      ZLay_CanvasPopTransform(canvas);
    }
#else
    (void)canvas;
    (void)pushed;
#endif
  }

  void paintChildren(ZLay_Canvas* canvas) {
    for (std::size_t i = 0; i < children_.size(); ++i) {
      if (children_[i]) {
        children_[i]->paint(canvas);
      }
    }
  }

private:
  Size size_;
  Offset offset_;
  std::vector<RenderObject*> children_;
};

} // namespace zlay

#endif // ZLAY_COMPONENTS_RUNTIME_ZLAY_RENDER_OBJECT_HPP
