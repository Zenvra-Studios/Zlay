#include <backends/platform/cocoa/zlay_cocoa_backend.h>
#import <backends/platform/cocoa/zlay_cocoa_platform.h>

#include <stdlib.h>
#include <string.h>

static ZLay_Window* zlay_cocoa_windows;

static uint32_t ZLay_Cocoa_EventQueueIndex(const ZLay_Window* window, uint32_t offset) {
  return (window->event_head + offset) % ZLAY_COCOA_EVENT_QUEUE_CAPACITY;
}

static void ZLay_Cocoa_LinkWindow(ZLay_Window* window) {
  window->next = zlay_cocoa_windows;
  zlay_cocoa_windows = window;
}

static void ZLay_Cocoa_UnlinkWindow(ZLay_Window* window) {
  ZLay_Window** link = &zlay_cocoa_windows;

  while (*link != NULL) {
    if (*link == window) {
      *link = window->next;
      window->next = NULL;
      return;
    }
    link = &(*link)->next;
  }
}

ZLay_Window* ZLay_Cocoa_FindWindow(void* ns_window, void* ns_view) {
  ZLay_Window* current = zlay_cocoa_windows;

  while (current != NULL) {
    if ((ns_window != NULL && current->ns_window == ns_window) ||
        (ns_view != NULL && current->ns_view == ns_view)) {
      return current;
    }
    current = current->next;
  }

  return NULL;
}

static void ZLay_Cocoa_ReadNativeSize(ZLay_Window* window) {
  if (window == NULL) {
    return;
  }

#if ZLAY_COCOA_TARGET_IOS
  UIView* view = (UIView*)window->ns_view;
  UIWindow* window_handle = (UIWindow*)window->ns_window;
  if (view == nil && window_handle != nil) {
    view = window_handle.rootViewController.view;
  }
  if (view != nil) {
    const CGRect bounds = view.bounds;
    window->width = (uint32_t)bounds.size.width;
    window->height = (uint32_t)bounds.size.height;
    window->scale = ZLay_Cocoa_DpiScaleForNative(window->ns_window, view, window->ca_layer);
  }
#else
  NSView* view = (NSView*)window->ns_view;
  NSWindow* window_handle = (NSWindow*)window->ns_window;
  if (view == nil && window_handle != nil) {
    view = window_handle.contentView;
  }
  if (view != nil) {
    const NSRect bounds = view.bounds;
    window->width = (uint32_t)bounds.size.width;
    window->height = (uint32_t)bounds.size.height;
    window->scale = ZLay_Cocoa_DpiScaleForNative(window->ns_window, view, window->ca_layer);
  }
#endif
}

ZLay_Window* ZLay_Cocoa_WindowAttachNative(const ZLay_WindowAttachDesc* desc) {
  ZLay_Window* window;

  if (desc == NULL ||
      (desc->native.platform != ZLAY_OS_PLATFORM_COCOA &&
       desc->native.platform != ZLAY_OS_PLATFORM_IOS)) {
    return NULL;
  }

  if (desc->native.window == NULL && desc->native.display == NULL && desc->native.surface == NULL) {
    return NULL;
  }

  window = (ZLay_Window*)calloc(1u, sizeof(ZLay_Window));
  if (window == NULL) {
    return NULL;
  }

  window->ns_window = desc->native.window != NULL ? desc->native.window : (void*)desc->native.raw0;
  window->ns_view = desc->native.display;
  window->ca_layer = desc->native.surface;
  window->width = desc->width;
  window->height = desc->height;
  window->scale = desc->scale > 0.0f
    ? desc->scale
    : ZLay_Cocoa_DpiScaleForNative(window->ns_window, window->ns_view, window->ca_layer);
  window->events_enabled = desc->select_events;
  window->user_data = desc->user_data;

  if (window->width == 0u || window->height == 0u) {
    ZLay_Cocoa_ReadNativeSize(window);
  }

  if (window->scale <= 0.0f) {
    window->scale = 1.0f;
  }

  ZLay_Cocoa_LinkWindow(window);
  return window;
}

void ZLay_Cocoa_WindowDetach(ZLay_Window* window) {
  if (window == NULL) {
    return;
  }

  ZLay_Cocoa_UnlinkWindow(window);
  free(window);
}

void ZLay_Cocoa_WindowShow(ZLay_Window* window) {
  if (window == NULL || window->ns_window == NULL) {
    return;
  }

#if ZLAY_COCOA_TARGET_IOS
  UIWindow* native_window = (UIWindow*)window->ns_window;
  native_window.hidden = NO;
#else
  NSWindow* native_window = (NSWindow*)window->ns_window;
  [native_window makeKeyAndOrderFront:nil];
#endif
}

void ZLay_Cocoa_WindowHide(ZLay_Window* window) {
  if (window == NULL || window->ns_window == NULL) {
    return;
  }

#if ZLAY_COCOA_TARGET_IOS
  UIWindow* native_window = (UIWindow*)window->ns_window;
  native_window.hidden = YES;
#else
  NSWindow* native_window = (NSWindow*)window->ns_window;
  [native_window orderOut:nil];
#endif
}

void ZLay_Cocoa_WindowSetTitle(ZLay_Window* window, const char* title) {
#if !ZLAY_COCOA_TARGET_IOS
  if (window == NULL || window->ns_window == NULL) {
    return;
  }

  NSString* text = title != NULL ? [NSString stringWithUTF8String:title] : @"";
  [(NSWindow*)window->ns_window setTitle:text != nil ? text : @""];
#else
  (void)window;
  (void)title;
#endif
}

bool ZLay_Cocoa_WindowShouldClose(ZLay_Window* window) {
  return window == NULL || window->should_close;
}

void ZLay_Cocoa_WindowRequestClose(ZLay_Window* window) {
  ZLay_Event event = {};

  if (window == NULL) {
    return;
  }

  event.type = ZLAY_EVENT_WINDOW_CLOSE;
  event.timestamp = ZLay_TimeNow();
  ZLay_Cocoa_WindowPushEvent(window, &event);
}

void ZLay_Cocoa_WindowGetSize(ZLay_Window* window, uint32_t* width, uint32_t* height) {
  if (window != NULL) {
    ZLay_Cocoa_ReadNativeSize(window);
  }

  if (width != NULL) {
    *width = window != NULL ? window->width : 0u;
  }
  if (height != NULL) {
    *height = window != NULL ? window->height : 0u;
  }
}

void ZLay_Cocoa_WindowGetFramebufferSize(ZLay_Window* window, uint32_t* width, uint32_t* height) {
  if (window != NULL) {
    ZLay_Cocoa_ReadNativeSize(window);
  }

  const float scale = window != NULL ? window->scale : 1.0f;
  if (width != NULL) {
    *width = window != NULL ? ZLay_DpiScalePixels(window->width, scale) : 0u;
  }
  if (height != NULL) {
    *height = window != NULL ? ZLay_DpiScalePixels(window->height, scale) : 0u;
  }
}

float ZLay_Cocoa_WindowGetScale(ZLay_Window* window) {
  if (window == NULL) {
    return 1.0f;
  }

  window->scale = ZLay_Cocoa_DpiScaleForNative(window->ns_window, window->ns_view, window->ca_layer);
  return window->scale > 0.0f ? window->scale : 1.0f;
}

ZLay_NativeWindowHandle ZLay_Cocoa_WindowGetNativeHandle(ZLay_Window* window) {
  ZLay_NativeWindowHandle handle = {};

  if (window == NULL) {
    return handle;
  }

  handle.platform = ZLAY_COCOA_TARGET_IOS ? ZLAY_OS_PLATFORM_IOS : ZLAY_OS_PLATFORM_COCOA;
  handle.display = window->ns_view;
  handle.window = window->ns_window;
  handle.surface = window->ca_layer;
  handle.raw0 = (uintptr_t)window->ns_window;
  handle.raw1 = (uintptr_t)window->ns_view;
  handle.raw2 = (uintptr_t)window->ca_layer;
  return handle;
}

void ZLay_Cocoa_WindowPushEvent(ZLay_Window* window, const ZLay_Event* event) {
  uint32_t index;

  if (window == NULL || event == NULL) {
    return;
  }

  if (window->event_count == ZLAY_COCOA_EVENT_QUEUE_CAPACITY) {
    window->event_head = ZLay_Cocoa_EventQueueIndex(window, 1u);
    window->event_count -= 1u;
  }

  index = ZLay_Cocoa_EventQueueIndex(window, window->event_count);
  window->events[index] = *event;
  window->event_count += 1u;

  if (event->type == ZLAY_EVENT_WINDOW_CLOSE) {
    window->should_close = true;
  }
}

bool ZLay_Cocoa_WindowNextEvent(ZLay_Window* window, ZLay_Event* out_event) {
  if (window == NULL || out_event == NULL || window->event_count == 0u) {
    return false;
  }

  *out_event = window->events[window->event_head];
  window->event_head = ZLay_Cocoa_EventQueueIndex(window, 1u);
  window->event_count -= 1u;
  return true;
}
