#pragma once

#if defined(__linux__)

#include <cstdint>
#include <cstdio>
#include <string>
#include <string_view>
#include <vector>

#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>

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
  Window() = default;

  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;

  ~Window() {
    shutdown();
  }

  bool open(const WindowDesc& desc) {
    shutdown();

    width_ = desc.width;
    height_ = desc.height;
    display_ = XOpenDisplay(nullptr);
    if (display_ == nullptr) {
      std::fprintf(stderr, "ZLay X11 window: unable to open display. Is DISPLAY set?\n");
      shouldClose_ = true;
      return false;
    }

    screen_ = DefaultScreen(display_);

    XSetWindowAttributes attributes{};
    attributes.background_pixel = BlackPixel(display_, screen_);
    attributes.event_mask = ExposureMask |
                            StructureNotifyMask |
                            KeyPressMask |
                            ButtonPressMask |
                            ButtonReleaseMask |
                            PointerMotionMask |
                            EnterWindowMask |
                            LeaveWindowMask;

    window_ = XCreateWindow(
        display_,
        RootWindow(display_, screen_),
        0,
        0,
        static_cast<unsigned int>(width_),
        static_cast<unsigned int>(height_),
        0,
        CopyFromParent,
        InputOutput,
        CopyFromParent,
        CWBackPixel | CWEventMask,
        &attributes);

    if (window_ == 0) {
      shutdown();
      shouldClose_ = true;
      return false;
    }

    XStoreName(display_, window_, desc.title ? desc.title : "ZLay");
    wmDeleteWindow_ = XInternAtom(display_, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display_, window_, &wmDeleteWindow_, 1);

    gc_ = XCreateGC(display_, window_, 0, nullptr);
    arrowCursor_ = XCreateFontCursor(display_, XC_left_ptr);
    handCursor_ = XCreateFontCursor(display_, XC_hand2);
    textCursor_ = XCreateFontCursor(display_, XC_xterm);
    if (arrowCursor_ != 0) {
      XDefineCursor(display_, window_, arrowCursor_);
    }

    setFont(desc.font);

    XMapWindow(display_, window_);
    XFlush(display_);
    shouldClose_ = false;
    return true;
  }

  void shutdown() {
    if (display_ == nullptr) {
      return;
    }

    destroyBackBuffer();

    clearFontCache();
    if (font_ != nullptr) {
      XFreeFont(display_, font_);
      font_ = nullptr;
    }
    if (gc_ != 0) {
      XFreeGC(display_, gc_);
      gc_ = 0;
    }
    if (arrowCursor_ != 0) {
      XFreeCursor(display_, arrowCursor_);
      arrowCursor_ = 0;
    }
    if (handCursor_ != 0) {
      XFreeCursor(display_, handCursor_);
      handCursor_ = 0;
    }
    if (textCursor_ != 0) {
      XFreeCursor(display_, textCursor_);
      textCursor_ = 0;
    }
    if (window_ != 0) {
      XDestroyWindow(display_, window_);
      window_ = 0;
    }
    XCloseDisplay(display_);
    display_ = nullptr;
  }

  bool pollEvents() {
    if (display_ == nullptr) {
      shouldClose_ = true;
      return false;
    }

    pointer_.left_pressed = false;
    pointer_.left_released = false;
    pointer_.moved = false;

    while (XPending(display_) > 0) {
      XEvent event{};
      XNextEvent(display_, &event);

      switch (event.type) {
        case ClientMessage:
          if (static_cast<Atom>(event.xclient.data.l[0]) == wmDeleteWindow_) {
            shouldClose_ = true;
          }
          break;
        case ConfigureNotify:
          width_ = event.xconfigure.width;
          height_ = event.xconfigure.height;
          break;
        case MotionNotify:
          pointer_.x = static_cast<float>(event.xmotion.x);
          pointer_.y = static_cast<float>(event.xmotion.y);
          pointer_.inside = true;
          pointer_.moved = true;
          break;
        case EnterNotify:
          pointer_.x = static_cast<float>(event.xcrossing.x);
          pointer_.y = static_cast<float>(event.xcrossing.y);
          pointer_.inside = true;
          pointer_.moved = true;
          break;
        case LeaveNotify:
          pointer_.inside = false;
          pointer_.left_down = false;
          pointer_.moved = true;
          break;
        case ButtonPress:
          pointer_.x = static_cast<float>(event.xbutton.x);
          pointer_.y = static_cast<float>(event.xbutton.y);
          pointer_.inside = true;
          if (event.xbutton.button == Button1) {
            pointer_.left_down = true;
            pointer_.left_pressed = true;
          }
          break;
        case ButtonRelease:
          pointer_.x = static_cast<float>(event.xbutton.x);
          pointer_.y = static_cast<float>(event.xbutton.y);
          pointer_.inside = true;
          if (event.xbutton.button == Button1) {
            pointer_.left_down = false;
            pointer_.left_released = true;
          }
          break;
        case KeyPress: {
          KeySym key = XLookupKeysym(&event.xkey, 0);
          if (key == XK_Escape) {
            shouldClose_ = true;
          }
          break;
        }
        default:
          break;
      }
    }

    return !shouldClose_;
  }

  void render(ZLay_RenderCommandList commands) {
    if (display_ == nullptr || window_ == 0 || gc_ == 0) {
      return;
    }

    Drawable target = ensureBackBuffer();
    if (target == 0) {
      target = window_;
    }

    for (uint32_t i = 0; i < commands.count; ++i) {
      const ZLay_RenderCommand& command = commands.commands[i];
      if (command.type == ZLAY_CMD_RECT) {
        drawRect(target, command);
      } else if (command.type == ZLAY_CMD_TEXT) {
        drawText(target, command);
      }
    }

    if (target != window_) {
      XCopyArea(
          display_,
          target,
          window_,
          gc_,
          0,
          0,
          static_cast<unsigned int>(width_ > 0 ? width_ : 1),
          static_cast<unsigned int>(height_ > 0 ? height_ : 1),
          0,
          0);
    }

    XFlush(display_);
  }

  bool shouldClose() const {
    return shouldClose_;
  }

  int width() const {
    return width_;
  }

  int height() const {
    return height_;
  }

  bool setFont(const char* name) {
    if (display_ == nullptr || gc_ == 0) {
      return false;
    }

    const char* requested = (name != nullptr && name[0] != '\0') ? name : ZLAY_DEFAULT_FONT_FAMILY;
    XFontStruct* next = loadFontFamilyList(requested);
    if (next == nullptr) {
      return false;
    }

    clearFontCache();
    if (font_ != nullptr) {
      XFreeFont(display_, font_);
    }
    font_ = next;
    XSetFont(display_, gc_, font_->fid);
    return true;
  }

  PointerState pointer() const {
    return pointer_;
  }

  void setPointerCursor(bool hand) {
    setPointerCursor(hand ? ZLAY_POINTER_CURSOR_POINTER : ZLAY_POINTER_CURSOR_DEFAULT);
  }

  void setPointerCursor(ZLay_PointerCursor kind) {
    if (display_ == nullptr || window_ == 0 || currentCursor_ == kind) {
      return;
    }

    Cursor cursor = arrowCursor_;
    if (kind == ZLAY_POINTER_CURSOR_POINTER) {
      cursor = handCursor_;
    } else if (kind == ZLAY_POINTER_CURSOR_TEXT) {
      cursor = textCursor_;
    }
    if (cursor != 0) {
      XDefineCursor(display_, window_, cursor);
      XFlush(display_);
      currentCursor_ = kind;
    }
  }

  ZLay_Dimensions measureText(ZLay_String text) const {
    const int length = text.length > static_cast<uint32_t>(INT32_MAX) ? INT32_MAX : static_cast<int>(text.length);
    if (font_ != nullptr && text.chars != nullptr) {
      return ZLay_DimensionsMake(
          static_cast<float>(XTextWidth(font_, text.chars, length)),
          static_cast<float>(font_->ascent + font_->descent));
    }

    return ZLay_DimensionsMake(8.0f * static_cast<float>(text.length), 16.0f);
  }

  static ZLay_Dimensions measureTextCallback(ZLay_String text, void* user) {
    Window* self = static_cast<Window*>(user);
    return self != nullptr ? self->measureText(text) : ZLay_DimensionsMake(8.0f * static_cast<float>(text.length), 16.0f);
  }

private:
  void destroyBackBuffer() {
    if (display_ != nullptr && backBuffer_ != 0) {
      XFreePixmap(display_, backBuffer_);
    }
    backBuffer_ = 0;
    backBufferWidth_ = 0;
    backBufferHeight_ = 0;
  }

  Drawable ensureBackBuffer() {
    const int width = width_ > 0 ? width_ : 1;
    const int height = height_ > 0 ? height_ : 1;

    if (backBuffer_ != 0 && backBufferWidth_ == width && backBufferHeight_ == height) {
      return backBuffer_;
    }

    destroyBackBuffer();

    backBuffer_ = XCreatePixmap(
        display_,
        window_,
        static_cast<unsigned int>(width),
        static_cast<unsigned int>(height),
        static_cast<unsigned int>(DefaultDepth(display_, screen_)));
    backBufferWidth_ = width;
    backBufferHeight_ = height;
    return backBuffer_;
  }

  unsigned long pixel(ZLay_Color color) const {
    if (display_ == nullptr) {
      return 0;
    }

    XColor xcolor{};
    xcolor.red = static_cast<uint16_t>(static_cast<uint16_t>(color.r) * 257u);
    xcolor.green = static_cast<uint16_t>(static_cast<uint16_t>(color.g) * 257u);
    xcolor.blue = static_cast<uint16_t>(static_cast<uint16_t>(color.b) * 257u);
    xcolor.flags = DoRed | DoGreen | DoBlue;

    if (XAllocColor(display_, DefaultColormap(display_, screen_), &xcolor) != 0) {
      return xcolor.pixel;
    }

    return static_cast<uint32_t>(color.r) + static_cast<uint32_t>(color.g) + static_cast<uint32_t>(color.b) > 384u
      ? WhitePixel(display_, screen_)
      : BlackPixel(display_, screen_);
  }

  void drawRect(Drawable target, const ZLay_RenderCommand& command) {
    if (command.color.a == 0u) {
      return;
    }

    XSetForeground(display_, gc_, pixel(command.color));
    XFillRectangle(
        display_,
        target,
        gc_,
        static_cast<int>(command.bounds.x),
        static_cast<int>(command.bounds.y),
        static_cast<unsigned int>(command.bounds.width < 0.0f ? 0.0f : command.bounds.width),
        static_cast<unsigned int>(command.bounds.height < 0.0f ? 0.0f : command.bounds.height));
  }

  void drawText(Drawable target, const ZLay_RenderCommand& command) {
    if (command.text.chars == nullptr || command.text.length == 0u) {
      return;
    }

    const int length = command.text.length > static_cast<uint32_t>(INT32_MAX) ? INT32_MAX : static_cast<int>(command.text.length);
    XFontStruct* commandFont = resolveFont(command.font_family);
    int baseline = static_cast<int>(command.bounds.y + command.bounds.height);

    if (commandFont != nullptr) {
      baseline = static_cast<int>(
          command.bounds.y +
          ((command.bounds.height - static_cast<float>(commandFont->ascent + commandFont->descent)) * 0.5f) +
          static_cast<float>(commandFont->ascent));
      XSetFont(display_, gc_, commandFont->fid);
    }

    XSetForeground(display_, gc_, pixel(command.color));
    XDrawString(display_, target, gc_, static_cast<int>(command.bounds.x), baseline, command.text.chars, length);
  }

  struct CachedFont {
    std::string name;
    XFontStruct* font = nullptr;
    bool owned = false;
  };

  void clearFontCache() {
    for (CachedFont& cached : fontCache_) {
      if (cached.owned && cached.font != nullptr) {
        XFreeFont(display_, cached.font);
      }
    }
    fontCache_.clear();
  }

  XFontStruct* resolveFont(ZLay_String family) {
    if (family.chars == nullptr || family.length == 0u) {
      return font_;
    }

    std::string name(family.chars, family.length);
    for (CachedFont& cached : fontCache_) {
      if (cached.name == name) {
        return cached.font != nullptr ? cached.font : font_;
      }
    }

    XFontStruct* loaded = loadFontFamilyList(name.c_str());
    bool owned = loaded != nullptr;
    if (loaded == nullptr) {
      loaded = font_;
    }

    fontCache_.push_back({name, loaded, owned});
    return loaded;
  }

  static std::string trimFontName(std::string_view value) {
    std::size_t start = 0;
    std::size_t end = value.size();
    while (start < end && (value[start] == ' ' || value[start] == '\t' || value[start] == '\'' || value[start] == '"')) {
      ++start;
    }
    while (end > start && (value[end - 1u] == ' ' || value[end - 1u] == '\t' || value[end - 1u] == '\'' || value[end - 1u] == '"')) {
      --end;
    }
    return std::string(value.substr(start, end - start));
  }

  XFontStruct* loadSingleFont(const std::string& name) const {
    if (display_ == nullptr || name.empty()) {
      return nullptr;
    }

    XFontStruct* loaded = XLoadQueryFont(display_, name.c_str());
    if (loaded != nullptr) return loaded;

    if (name == "Inter") {
      loaded = XLoadQueryFont(display_, "-*-inter-medium-r-normal--14-*-*-*-*-*-iso10646-1");
      if (loaded != nullptr) return loaded;
    }

    if (name == "Inter" || name == "Noto Sans" || name == "DejaVu Sans" || name == "sans" || name == "sans-serif") {
      loaded = XLoadQueryFont(display_, "-*-helvetica-medium-r-normal--14-*-*-*-*-*-iso10646-1");
      if (loaded != nullptr) return loaded;
      loaded = XLoadQueryFont(display_, "-*-helvetica-medium-r-normal--14-*-*-*-*-*-iso8859-1");
      if (loaded != nullptr) return loaded;
    }

    return nullptr;
  }

  XFontStruct* loadFontFamilyList(const char* family_list) const {
    std::string_view families = family_list != nullptr && family_list[0] != '\0'
      ? std::string_view(family_list)
      : std::string_view(ZLAY_DEFAULT_FONT_FAMILY);
    std::size_t start = 0;

    while (start <= families.size()) {
      const std::size_t comma = families.find(',', start);
      const std::size_t end = comma == std::string_view::npos ? families.size() : comma;
      std::string name = trimFontName(families.substr(start, end - start));
      XFontStruct* loaded = loadSingleFont(name);
      if (loaded != nullptr) return loaded;
      if (comma == std::string_view::npos) break;
      start = comma + 1u;
    }

    return XLoadQueryFont(display_, "fixed");
  }

  Display* display_ = nullptr;
  int screen_ = 0;
  ::Window window_ = 0;
  GC gc_ = 0;
  Cursor arrowCursor_ = 0;
  Cursor handCursor_ = 0;
  Cursor textCursor_ = 0;
  Pixmap backBuffer_ = 0;
  int backBufferWidth_ = 0;
  int backBufferHeight_ = 0;
  Atom wmDeleteWindow_ = 0;
  XFontStruct* font_ = nullptr;
  std::vector<CachedFont> fontCache_;
  int width_ = 0;
  int height_ = 0;
  PointerState pointer_{};
  ZLay_PointerCursor currentCursor_ = ZLAY_POINTER_CURSOR_DEFAULT;
  bool shouldClose_ = true;
};

} // namespace zlay::platform

#endif
