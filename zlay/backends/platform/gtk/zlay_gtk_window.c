#include <backends/platform/gtk/zlay_gtk_backend.h>

#include <stdlib.h>
#include <string.h>

static ZLay_GTKDisplayServer ZLay_GTK_DisplayFromNative(const ZLay_NativeWindowHandle* native) {
  if (native == NULL) return ZLay_GTK_GetDisplayServer();

  switch (native->platform) {
    case ZLAY_OS_PLATFORM_WAYLAND:
      return ZLAY_GTK_DISPLAY_WAYLAND;
    case ZLAY_OS_PLATFORM_X11:
      return ZLAY_GTK_DISPLAY_X11;
    case ZLAY_OS_PLATFORM_GTK:
    case ZLAY_OS_PLATFORM_UNKNOWN:
    default:
      return ZLay_GTK_GetDisplayServer();
  }
}

ZLay_Window* ZLay_GTK_WindowAttachNative(const ZLay_WindowAttachDesc* desc) {
  ZLay_GTK_State* state = ZLay_GTK_StateGet();
  ZLay_Window* window;
  ZLay_GTKDisplayServer display_server;

  if (desc == NULL) {
    return NULL;
  }

  display_server = ZLay_GTK_DisplayFromNative(&desc->native);
  window = (ZLay_Window*)calloc(1u, sizeof(ZLay_Window));
  if (window == NULL) {
    return NULL;
  }

  window->widget = desc->native.window != NULL ? desc->native.window : state->widget;
  window->gdk_display = desc->native.display != NULL ? desc->native.display : state->gdk_display;
  window->gdk_surface = desc->native.surface != NULL ? desc->native.surface : state->gdk_surface;
  window->wl_display = state->wl_display;
  window->wl_surface = state->wl_surface;
  window->x11_display = state->x11_display;
  window->x11_window = state->x11_window;
  window->width = desc->width != 0u ? desc->width : (uint32_t)(state->width > 0 ? state->width : 0);
  window->height = desc->height != 0u ? desc->height : (uint32_t)(state->height > 0 ? state->height : 0);
  window->scale = desc->scale > 0.0f ? desc->scale : ZLay_GTK_GetDpiScale(window->widget);
  window->display_server = display_server;
  window->events_enabled = desc->select_events;
  window->user_data = desc->user_data;
  window->cursor = ZLAY_CURSOR_ARROW;

  if (display_server == ZLAY_GTK_DISPLAY_WAYLAND) {
    window->wl_display = desc->native.display != NULL ? desc->native.display : state->wl_display;
    window->wl_surface = desc->native.surface != NULL ? desc->native.surface : state->wl_surface;
  } else if (display_server == ZLAY_GTK_DISPLAY_X11) {
    window->x11_display = desc->native.display != NULL ? desc->native.display : state->x11_display;
    window->x11_window = desc->native.raw0 != 0u
      ? (unsigned long)desc->native.raw0
      : (unsigned long)(uintptr_t)desc->native.window;
    if (window->x11_window == 0ul) {
      window->x11_window = state->x11_window;
    }
  }

  ZLay_GTK_LinkWindow(window);
  return window;
}

void ZLay_GTK_WindowDetach(ZLay_Window* window) {
  if (window == NULL) {
    return;
  }

  ZLay_GTK_UnlinkWindow(window);
  free(window);
}

void ZLay_GTK_WindowShow(ZLay_Window* window) {
  (void)window;
}

void ZLay_GTK_WindowHide(ZLay_Window* window) {
  (void)window;
}

void ZLay_GTK_WindowSetTitle(ZLay_Window* window, const char* title) {
  (void)window;
  (void)title;
}

bool ZLay_GTK_WindowShouldClose(ZLay_Window* window) {
  return window == NULL || window->should_close;
}

void ZLay_GTK_WindowRequestClose(ZLay_Window* window) {
  ZLay_Event event;

  if (window == NULL) {
    return;
  }

  memset(&event, 0, sizeof(event));
  event.type = ZLAY_EVENT_WINDOW_CLOSE;
  event.timestamp = ZLay_GTK_EventTimestamp();
  ZLay_GTK_WindowPushEvent(window, &event);
}

void ZLay_GTK_WindowGetSize(ZLay_Window* window, uint32_t* width, uint32_t* height) {
  if (width != NULL) {
    *width = window != NULL ? window->width : 0u;
  }
  if (height != NULL) {
    *height = window != NULL ? window->height : 0u;
  }
}

void ZLay_GTK_WindowGetFramebufferSize(ZLay_Window* window, uint32_t* width, uint32_t* height) {
  const float scale = window != NULL && window->scale > 0.0f ? window->scale : 1.0f;

  if (width != NULL) {
    *width = window != NULL ? ZLay_DpiScalePixels(window->width, scale) : 0u;
  }
  if (height != NULL) {
    *height = window != NULL ? ZLay_DpiScalePixels(window->height, scale) : 0u;
  }
}

float ZLay_GTK_WindowGetScale(ZLay_Window* window) {
  return window != NULL ? window->scale : ZLay_GTK_GetDpiScale(NULL);
}

void ZLay_GTK_WindowSetCursor(ZLay_Window* window, ZLay_Cursor cursor) {
  ZLay_GTK_State* state = ZLay_GTK_StateGet();

  if (window == NULL) {
    return;
  }

  window->cursor = cursor;
  if (state->set_cursor != NULL) {
    state->set_cursor(window->widget != NULL ? window->widget : state->widget, cursor, state->user);
  }
}
