#pragma once

#if defined(_WIN32)

#include <zlay.h>

namespace zlay::platform {

using PointerState = ZLay_PointerState;

struct WindowDesc {
  int width = 1280;
  int height = 720;
  const char* title = "ZLay";
  const char* font = ZLAY_DEFAULT_FONT_FAMILY;
};

class Window {
public:
  bool open(const WindowDesc&) { return false; }
  void shutdown() {}
  bool pollEvents() { return false; }
  void render(ZLay_RenderCommandList) {}
  bool shouldClose() const { return true; }
  int width() const { return 0; }
  int height() const { return 0; }
  bool setFont(const char*) { return false; }
  PointerState pointer() const { return {}; }
  void setPointerCursor(bool) {}

  ZLay_Dimensions measureText(ZLay_String text) const {
    return ZLay_DimensionsMake(8.0f * static_cast<float>(text.length), 16.0f);
  }

  static ZLay_Dimensions measureTextCallback(ZLay_String text, void* user) {
    Window* self = static_cast<Window*>(user);
    return self != nullptr ? self->measureText(text) : ZLay_DimensionsMake(8.0f * static_cast<float>(text.length), 16.0f);
  }
};

} // namespace zlay::platform

#endif
