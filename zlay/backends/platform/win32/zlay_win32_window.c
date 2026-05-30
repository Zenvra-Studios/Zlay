#include <backends/platform/win32/zlay_win32_backend.h>

#include <stdlib.h>
#include <string.h>

static ZLay_Window* zlay_win32_windows;

static uint32_t ZLay_Win32_EventQueueIndex(const ZLay_Window* window, uint32_t offset) {
  return (window->event_head + offset) % ZLAY_WIN32_EVENT_QUEUE_CAPACITY;
}

static LRESULT CALLBACK ZLay_Win32_SubclassProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
  LRESULT result = 0;
  ZLay_Window* window = ZLay_Win32_FindWindow(hwnd);

  if (ZLay_Win32_ProcessMessage(hwnd, message, wparam, lparam, &result)) {
    return result;
  }

  if (window != NULL && window->previous_wndproc != NULL) {
    return CallWindowProcW(window->previous_wndproc, hwnd, message, wparam, lparam);
  }

  return DefWindowProcW(hwnd, message, wparam, lparam);
}

static void ZLay_Win32_LinkWindow(ZLay_Window* window) {
  window->next = zlay_win32_windows;
  zlay_win32_windows = window;
}

static void ZLay_Win32_UnlinkWindow(ZLay_Window* window) {
  ZLay_Window** link = &zlay_win32_windows;

  while (*link != NULL) {
    if (*link == window) {
      *link = window->next;
      window->next = NULL;
      return;
    }
    link = &(*link)->next;
  }
}

ZLay_Window* ZLay_Win32_FindWindow(HWND hwnd) {
  ZLay_Window* current = zlay_win32_windows;

  while (current != NULL) {
    if (current->hwnd == hwnd) {
      return current;
    }
    current = current->next;
  }

  return NULL;
}

ZLay_Window* ZLay_Win32_WindowAttachNative(const ZLay_WindowAttachDesc* desc) {
  HWND hwnd;
  ZLay_Window* window;
  RECT rect;

  if (desc == NULL || desc->native.platform != ZLAY_OS_PLATFORM_WIN32) {
    return NULL;
  }

  hwnd = (HWND)(desc->native.window != NULL ? desc->native.window : (void*)desc->native.raw0);
  if (hwnd == NULL) {
    return NULL;
  }

  window = (ZLay_Window*)calloc(1u, sizeof(ZLay_Window));
  if (window == NULL) {
    return NULL;
  }

  window->hwnd = hwnd;
  window->instance = desc->native.display != NULL
    ? (HINSTANCE)desc->native.display
    : (HINSTANCE)(LONG_PTR)GetWindowLongPtrW(hwnd, GWLP_HINSTANCE);
  window->width = desc->width;
  window->height = desc->height;
  window->scale = desc->scale > 0.0f ? desc->scale : ZLay_Win32_DpiScaleForHwnd(hwnd);
  window->user_data = desc->user_data;

  if ((window->width == 0u || window->height == 0u) && GetClientRect(hwnd, &rect)) {
    window->width = (uint32_t)(rect.right - rect.left);
    window->height = (uint32_t)(rect.bottom - rect.top);
  }

  ZLay_Win32_LinkWindow(window);

  if (desc->select_events) {
    window->previous_wndproc = (WNDPROC)(LONG_PTR)GetWindowLongPtrW(hwnd, GWLP_WNDPROC);
    if (window->previous_wndproc != ZLay_Win32_SubclassProc) {
      SetWindowLongPtrW(hwnd, GWLP_WNDPROC, (LONG_PTR)ZLay_Win32_SubclassProc);
      window->subclassed = true;
    }
  }

#if defined(ZLAY_ENABLE_WINRT_SUPPORT) && ZLAY_ENABLE_WINRT_MODERN_UI
  ZLay_Win32_WindowEnableModern(window, NULL);
#endif

  return window;
}

void ZLay_Win32_WindowDetach(ZLay_Window* window) {
  if (window == NULL) {
    return;
  }

  if (window->subclassed && window->hwnd != NULL && window->previous_wndproc != NULL) {
    SetWindowLongPtrW(window->hwnd, GWLP_WNDPROC, (LONG_PTR)window->previous_wndproc);
  }

#if defined(ZLAY_ENABLE_WINRT_SUPPORT)
  ZLay_Win32_WindowDisableModern(window);
#endif

  ZLay_Win32_UnlinkWindow(window);
  free(window);
}

void ZLay_Win32_WindowShow(ZLay_Window* window) {
  if (window != NULL && window->hwnd != NULL) {
    ShowWindow(window->hwnd, SW_SHOW);
  }
}

void ZLay_Win32_WindowHide(ZLay_Window* window) {
  if (window != NULL && window->hwnd != NULL) {
    ShowWindow(window->hwnd, SW_HIDE);
  }
}

void ZLay_Win32_WindowSetTitle(ZLay_Window* window, const char* title) {
  int length;
  WCHAR* wide;

  if (window == NULL || window->hwnd == NULL) {
    return;
  }

  if (title == NULL) {
    title = "";
  }

  length = MultiByteToWideChar(CP_UTF8, 0, title, -1, NULL, 0);
  if (length <= 0) {
    return;
  }

  wide = (WCHAR*)calloc((size_t)length, sizeof(WCHAR));
  if (wide == NULL) {
    return;
  }

  MultiByteToWideChar(CP_UTF8, 0, title, -1, wide, length);
  SetWindowTextW(window->hwnd, wide);
  free(wide);
}

bool ZLay_Win32_WindowShouldClose(ZLay_Window* window) {
  return window == NULL || window->should_close;
}

void ZLay_Win32_WindowRequestClose(ZLay_Window* window) {
  ZLay_Event event;

  if (window == NULL) {
    return;
  }

  event = (ZLay_Event){0};
  event.type = ZLAY_EVENT_WINDOW_CLOSE;
  event.timestamp = ZLay_TimeNow();
  ZLay_Win32_WindowPushEvent(window, &event);
}

void ZLay_Win32_WindowGetSize(ZLay_Window* window, uint32_t* width, uint32_t* height) {
  if (width != NULL) {
    *width = window != NULL ? window->width : 0u;
  }
  if (height != NULL) {
    *height = window != NULL ? window->height : 0u;
  }
}

void ZLay_Win32_WindowGetFramebufferSize(ZLay_Window* window, uint32_t* width, uint32_t* height) {
  const float scale = window != NULL ? window->scale : 1.0f;

  if (width != NULL) {
    *width = window != NULL ? ZLay_DpiScalePixels(window->width, scale) : 0u;
  }
  if (height != NULL) {
    *height = window != NULL ? ZLay_DpiScalePixels(window->height, scale) : 0u;
  }
}

float ZLay_Win32_WindowGetScale(ZLay_Window* window) {
  return window != NULL ? window->scale : 1.0f;
}

ZLay_NativeWindowHandle ZLay_Win32_WindowGetNativeHandle(ZLay_Window* window) {
  if (window == NULL) {
    return (ZLay_NativeWindowHandle){0};
  }

  return (ZLay_NativeWindowHandle){
    ZLAY_OS_PLATFORM_WIN32,
    window->instance,
    window->hwnd,
    NULL,
    NULL,
    (uintptr_t)window->hwnd,
    (uintptr_t)window->instance,
    0u
  };
}

#if defined(ZLAY_ENABLE_WINRT_SUPPORT)
bool ZLay_Win32_WindowEnableModern(ZLay_Window* window, const ZLay_WinRTModernDesc* desc) {
  ZLay_WinRTModernDesc modern_desc;

  if (window == NULL || window->hwnd == NULL) {
    return false;
  }

  if (desc != NULL) {
    modern_desc = *desc;
  } else {
    modern_desc = (ZLay_WinRTModernDesc){0};
    modern_desc.flags = ZLAY_WINRT_MODERN_THEME |
      ZLAY_WINRT_MODERN_DPI |
      ZLAY_WINRT_MODERN_COMPOSITION;
    modern_desc.enable_mica = true;
    modern_desc.enable_dark_mode = true;
  }

  if (modern_desc.native_window == NULL) {
    modern_desc.native_window = window->hwnd;
  }

  window->modern_enabled = ZLay_WinRTModernEnable(&modern_desc);
  return window->modern_enabled;
}

void ZLay_Win32_WindowDisableModern(ZLay_Window* window) {
  if (window == NULL || !window->modern_enabled) {
    return;
  }

  ZLay_WinRTModernDisable();
  window->modern_enabled = false;
}
#endif

void ZLay_Win32_WindowPushEvent(ZLay_Window* window, const ZLay_Event* event) {
  uint32_t index;

  if (window == NULL || event == NULL) {
    return;
  }

  if (window->event_count == ZLAY_WIN32_EVENT_QUEUE_CAPACITY) {
    window->event_head = ZLay_Win32_EventQueueIndex(window, 1u);
    window->event_count -= 1u;
  }

  index = ZLay_Win32_EventQueueIndex(window, window->event_count);
  window->events[index] = *event;
  window->event_count += 1u;

  if (event->type == ZLAY_EVENT_WINDOW_CLOSE) {
    window->should_close = true;
  }
}

bool ZLay_Win32_WindowNextEvent(ZLay_Window* window, ZLay_Event* out_event) {
  if (window == NULL || out_event == NULL || window->event_count == 0u) {
    return false;
  }

  *out_event = window->events[window->event_head];
  window->event_head = ZLay_Win32_EventQueueIndex(window, 1u);
  window->event_count -= 1u;
  return true;
}
