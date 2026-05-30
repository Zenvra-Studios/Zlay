#ifndef ZLAY_BACKENDS_PLATFORM_GTK_ZLAY_GTK_BACKEND_H
#define ZLAY_BACKENDS_PLATFORM_GTK_ZLAY_GTK_BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <os/zlay_os.h>

#ifndef ZLAY_GTK_EVENT_QUEUE_CAPACITY
#  define ZLAY_GTK_EVENT_QUEUE_CAPACITY 256u
#endif

typedef enum ZLay_GTKDisplayServer {
  ZLAY_GTK_DISPLAY_UNKNOWN = 0,
  ZLAY_GTK_DISPLAY_WAYLAND,
  ZLAY_GTK_DISPLAY_X11,
  ZLAY_GTK_DISPLAY_BROADWAY,
  ZLAY_GTK_DISPLAY_HEADLESS
} ZLay_GTKDisplayServer;

typedef float (*ZLay_GTKGetDpiScaleFn)(void* widget, void* user);
typedef ZLay_OSStyleInfo (*ZLay_GTKGetStyleInfoFn)(void* user);
typedef bool (*ZLay_GTKClipboardSetTextFn)(const char* text, void* user);
typedef const char* (*ZLay_GTKClipboardGetTextFn)(void* user);
typedef void (*ZLay_GTKSetCursorFn)(void* widget, ZLay_Cursor cursor, void* user);
typedef void (*ZLay_GTKPollEventsFn)(void* user);

typedef struct ZLay_GTKInitInfo {
  void* widget;
  void* gdk_display;
  void* gdk_surface;
  void* wl_display;
  void* wl_surface;
  void* x11_display;
  unsigned long x11_window;
  int32_t width;
  int32_t height;
  float scale;
  ZLay_GTKDisplayServer display_server;
  bool prefer_dark_theme;
  const char* font_family;
  ZLay_GTKGetDpiScaleFn get_dpi_scale;
  ZLay_GTKGetStyleInfoFn get_style_info;
  ZLay_GTKClipboardSetTextFn clipboard_set_text;
  ZLay_GTKClipboardGetTextFn clipboard_get_text;
  ZLay_GTKSetCursorFn set_cursor;
  ZLay_GTKPollEventsFn poll_events;
  void* user;
} ZLay_GTKInitInfo;

struct ZLay_Window {
  void* widget;
  void* gdk_display;
  void* gdk_surface;
  void* wl_display;
  void* wl_surface;
  void* x11_display;
  unsigned long x11_window;
  uint32_t width;
  uint32_t height;
  float scale;
  ZLay_GTKDisplayServer display_server;
  bool should_close;
  bool events_enabled;
  void* user_data;
  ZLay_Event events[ZLAY_GTK_EVENT_QUEUE_CAPACITY];
  uint32_t event_head;
  uint32_t event_count;
  float last_mouse_x;
  float last_mouse_y;
  bool has_last_mouse;
  ZLay_Cursor cursor;
  struct ZLay_Window* next;
};

typedef struct ZLay_GTK_State {
  void* widget;
  void* gdk_display;
  void* gdk_surface;
  void* wl_display;
  void* wl_surface;
  void* x11_display;
  unsigned long x11_window;
  int32_t width;
  int32_t height;
  float scale;
  ZLay_GTKDisplayServer display_server;
  bool prefer_dark_theme;
  const char* font_family;
  ZLay_GTKGetDpiScaleFn get_dpi_scale;
  ZLay_GTKGetStyleInfoFn get_style_info;
  ZLay_GTKClipboardSetTextFn clipboard_set_text;
  ZLay_GTKClipboardGetTextFn clipboard_get_text;
  ZLay_GTKSetCursorFn set_cursor;
  ZLay_GTKPollEventsFn poll_events;
  void* user;
  ZLay_Window* windows;
  char* clipboard_text;
} ZLay_GTK_State;

ZLAY_API bool ZLay_GTK_Init(const ZLay_GTKInitInfo* info);
ZLAY_API void ZLay_GTK_Shutdown(void);
ZLAY_API void ZLay_GTK_NewFrame(ZLay_Context* ctx, int32_t width, int32_t height);
ZLAY_API float ZLay_GTK_GetDpiScale(void* widget);
ZLAY_API ZLay_OSStyleInfo ZLay_GTK_GetStyleInfo(void);
ZLAY_API ZLay_GTKDisplayServer ZLay_GTK_GetDisplayServer(void);
ZLAY_API const char* ZLay_GTK_DisplayServerName(ZLay_GTKDisplayServer display_server);
ZLAY_API ZLay_NativeWindowHandle ZLay_GTK_GetNativeHandle(void);

ZLay_GTK_State* ZLay_GTK_StateGet(void);
uint64_t ZLay_GTK_NowNs(void);
double ZLay_GTK_EventTimestamp(void);
ZLay_Platform ZLay_GTK_PlatformForDisplayServer(ZLay_GTKDisplayServer display_server);
ZLay_GTKDisplayServer ZLay_GTK_DetectDisplayServer(const ZLay_GTKInitInfo* info);
void ZLay_GTK_LinkWindow(ZLay_Window* window);
void ZLay_GTK_UnlinkWindow(ZLay_Window* window);
ZLay_Window* ZLay_GTK_FirstWindow(void);

ZLAY_API ZLay_Window* ZLay_GTK_WindowAttachNative(const ZLay_WindowAttachDesc* desc);
ZLAY_API void ZLay_GTK_WindowDetach(ZLay_Window* window);
ZLAY_API void ZLay_GTK_WindowShow(ZLay_Window* window);
ZLAY_API void ZLay_GTK_WindowHide(ZLay_Window* window);
ZLAY_API void ZLay_GTK_WindowSetTitle(ZLay_Window* window, const char* title);
ZLAY_API bool ZLay_GTK_WindowShouldClose(ZLay_Window* window);
ZLAY_API void ZLay_GTK_WindowRequestClose(ZLay_Window* window);
ZLAY_API void ZLay_GTK_WindowGetSize(ZLay_Window* window, uint32_t* width, uint32_t* height);
ZLAY_API void ZLay_GTK_WindowGetFramebufferSize(ZLay_Window* window, uint32_t* width, uint32_t* height);
ZLAY_API float ZLay_GTK_WindowGetScale(ZLay_Window* window);
ZLAY_API ZLay_NativeWindowHandle ZLay_GTK_WindowGetNativeHandle(ZLay_Window* window);
ZLAY_API void ZLay_GTK_WindowSetCursor(ZLay_Window* window, ZLay_Cursor cursor);
ZLAY_API void ZLay_GTK_PollEvents(void);
ZLAY_API bool ZLay_GTK_WindowNextEvent(ZLay_Window* window, ZLay_Event* out_event);
ZLAY_API void ZLay_GTK_WindowPushEvent(ZLay_Window* window, const ZLay_Event* event);
ZLAY_API uint32_t ZLay_GTK_GetMonitorCount(void);
ZLAY_API bool ZLay_GTK_GetMonitorInfo(uint32_t index, ZLay_MonitorInfo* out_info);
ZLAY_API bool ZLay_GTK_ClipboardSetText(const char* text);
ZLAY_API const char* ZLay_GTK_ClipboardGetText(void);

#ifdef __cplusplus
}
#endif

#endif
