#include <backends/platform/gtk/zlay_gtk_backend.h>

ZLay_NativeWindowHandle ZLay_GTK_WindowGetNativeHandle(ZLay_Window* window) {
  if (window == NULL) {
    return (ZLay_NativeWindowHandle){0};
  }

  if (window->display_server == ZLAY_GTK_DISPLAY_WAYLAND) {
    return (ZLay_NativeWindowHandle){
      ZLAY_OS_PLATFORM_WAYLAND,
      window->wl_display,
      window->widget,
      window->wl_surface,
      NULL,
      (uintptr_t)window->wl_surface,
      0u,
      0u
    };
  }

  if (window->display_server == ZLAY_GTK_DISPLAY_X11) {
    return (ZLay_NativeWindowHandle){
      ZLAY_OS_PLATFORM_X11,
      window->x11_display,
      (void*)(uintptr_t)window->x11_window,
      window->gdk_surface,
      NULL,
      (uintptr_t)window->x11_window,
      0u,
      0u
    };
  }

  return (ZLay_NativeWindowHandle){
    ZLAY_OS_PLATFORM_GTK,
    window->gdk_display,
    window->widget,
    window->gdk_surface,
    NULL,
    (uintptr_t)window->widget,
    (uintptr_t)window->gdk_surface,
    0u
  };
}

ZLay_NativeWindowHandle ZLay_GTK_GetNativeHandle(void) {
  ZLay_GTK_State* state = ZLay_GTK_StateGet();
  ZLay_Window* first = ZLay_GTK_FirstWindow();

  if (first != NULL) {
    return ZLay_GTK_WindowGetNativeHandle(first);
  }

  if (state->display_server == ZLAY_GTK_DISPLAY_WAYLAND) {
    return (ZLay_NativeWindowHandle){
      ZLAY_OS_PLATFORM_WAYLAND,
      state->wl_display,
      state->widget,
      state->wl_surface,
      NULL,
      (uintptr_t)state->wl_surface,
      0u,
      0u
    };
  }

  if (state->display_server == ZLAY_GTK_DISPLAY_X11) {
    return (ZLay_NativeWindowHandle){
      ZLAY_OS_PLATFORM_X11,
      state->x11_display,
      (void*)(uintptr_t)state->x11_window,
      state->gdk_surface,
      NULL,
      (uintptr_t)state->x11_window,
      0u,
      0u
    };
  }

  return (ZLay_NativeWindowHandle){
    ZLAY_OS_PLATFORM_GTK,
    state->gdk_display,
    state->widget,
    state->gdk_surface,
    NULL,
    (uintptr_t)state->widget,
    (uintptr_t)state->gdk_surface,
    0u
  };
}

uint32_t ZLay_GTK_GetMonitorCount(void) {
  return 1u;
}

bool ZLay_GTK_GetMonitorInfo(uint32_t index, ZLay_MonitorInfo* out_info) {
  ZLay_GTK_State* state = ZLay_GTK_StateGet();

  if (index != 0u || out_info == NULL) {
    return false;
  }

  *out_info = (ZLay_MonitorInfo){
    0,
    0,
    state->width > 0 ? (uint32_t)state->width : 0u,
    state->height > 0 ? (uint32_t)state->height : 0u,
    ZLay_GTK_GetDpiScale(state->widget),
    ZLay_GTK_GetDpiScale(state->widget),
    0u,
    ZLay_GTK_DisplayServerName(state->display_server)
  };
  return true;
}
