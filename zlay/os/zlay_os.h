#ifndef ZLAY_ZLAY_OS_H
#define ZLAY_ZLAY_OS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <zlay.h>
#include <os/zlay_dpi.h>
#include <os/zlay_native.h>
#include <os/zlay_platform.h>
#include <os/event/zlay_event.h>
#include <os/event/zlay_input.h>
#include <os/event/zlay_keycode.h>
#include <os/event/zlay_mouse.h>
#include <os/system/zlay_clipboard.h>
#include <os/system/zlay_file.h>
#include <os/system/zlay_mutex.h>
#include <os/system/zlay_path.h>
#include <os/system/zlay_thread.h>
#include <os/system/zlay_timer.h>
#include <os/window/zlay_cursor.h>
#include <os/window/zlay_monitor.h>
#include <os/window/zlay_window.h>

typedef enum ZLay_OSBackendType {
  ZLAY_OS_BACKEND_NULL = 0,
  ZLAY_OS_BACKEND_WIN32,
  ZLAY_OS_BACKEND_WINRT, /* reserved: WinRT is exposed as Windows modern support, not a window backend */
  ZLAY_OS_BACKEND_COCOA,
  ZLAY_OS_BACKEND_GTK,
  ZLAY_OS_BACKEND_X11,
  ZLAY_OS_BACKEND_WAYLAND,
  ZLAY_OS_BACKEND_POSIX,
  ZLAY_OS_BACKEND_UNIX_BSD,
  ZLAY_OS_BACKEND_SOLARIS,
  ZLAY_OS_BACKEND_ANDROID,
  ZLAY_OS_BACKEND_IOS,
  ZLAY_OS_BACKEND_EMSCRIPTEN
} ZLay_OSBackendType;

typedef enum ZLay_OSCapabilityFlags {
  ZLAY_OS_CAP_NONE = 0,
  ZLAY_OS_CAP_MONOTONIC_TIME = 1 << 0,
  ZLAY_OS_CAP_DPI_SCALE = 1 << 1,
  ZLAY_OS_CAP_NATIVE_THEME = 1 << 2,
  ZLAY_OS_CAP_CLIPBOARD = 1 << 3,
  ZLAY_OS_CAP_MOUSE_CURSOR = 1 << 4,
  ZLAY_OS_CAP_TEXT_INPUT = 1 << 5
} ZLay_OSCapabilityFlags;

typedef enum ZLay_OSThemeMode {
  ZLAY_OS_THEME_LIGHT = 0,
  ZLAY_OS_THEME_DARK = 1,
  ZLAY_OS_THEME_HIGH_CONTRAST = 2
} ZLay_OSThemeMode;

typedef struct ZLay_OSStyleInfo {
  ZLay_OSThemeMode mode;
  ZLay_Color window_bg;
  ZLay_Color panel_bg;
  ZLay_Color text;
  ZLay_Color accent;
  float corner_radius;
  float control_height;
  const char* font_family;
} ZLay_OSStyleInfo;

typedef struct ZLay_OSFrameInfo {
  void* native_window;
  int32_t width;
  int32_t height;
  float dpi_scale;
  uint64_t time_ns;
} ZLay_OSFrameInfo;

ZLAY_API const char* ZLay_OS_BackendName(void);
ZLAY_API ZLay_OSBackendType ZLay_OS_BackendType(void);
ZLAY_API uint32_t ZLay_OS_Capabilities(void);
ZLAY_API uint64_t ZLay_OS_NowNs(void);
ZLAY_API float ZLay_OS_GetDpiScale(void* native_window);
ZLAY_API ZLay_OSStyleInfo ZLay_OS_GetStyleInfo(void);
ZLAY_API void ZLay_OS_NewFrame(ZLay_Context* ctx, const ZLay_OSFrameInfo* frame);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ZLAY_ZLAY_OS_H
