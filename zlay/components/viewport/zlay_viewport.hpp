#ifndef ZLAY_COMPONENTS_VIEWPORT_ZLAY_VIEWPORT_HPP
#define ZLAY_COMPONENTS_VIEWPORT_ZLAY_VIEWPORT_HPP

#include <zlay/components/painting/zlay_canvas_view.hpp>
#include <zlay/components/tooling/zlay_tooling_panel.hpp>

namespace zlay {

inline Widget Viewport() {
  CanvasViewProps canvas;
  canvas.width = 420.0f;
  canvas.height = 260.0f;
  canvas.onPaint = [](ZLay_Canvas& c, const Rect& r) {
#if defined(ZLAY_ENABLE_CANVAS)
    ZLay_CanvasRect(&c, 0.0f, 0.0f, r.width, r.height,
                    ZLay_CanvasPaintSolid(Color(12, 16, 23)));
    ZLay_CanvasLine(&c, 0.0f, r.height * 0.5f, r.width, r.height * 0.5f,
                    ZLay_CanvasStrokeMake(Color(55, 68, 84), 1.0f));
    ZLay_CanvasLine(&c, r.width * 0.5f, 0.0f, r.width * 0.5f, r.height,
                    ZLay_CanvasStrokeMake(Color(55, 68, 84), 1.0f));
#else
    (void)c;
    (void)r;
#endif
  };
  std::vector<Widget> body;
  body.push_back(CanvasView(canvas));
  return ToolingPanel("Viewport", body);
}

} // namespace zlay

#endif // ZLAY_COMPONENTS_VIEWPORT_ZLAY_VIEWPORT_HPP
