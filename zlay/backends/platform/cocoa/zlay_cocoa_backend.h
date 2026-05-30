#ifndef ZLAY_BACKENDS_PLATFORM_COCOA_ZLAY_COCOA_BACKEND_H
#define ZLAY_BACKENDS_PLATFORM_COCOA_ZLAY_COCOA_BACKEND_H

#include <stdbool.h>
#include <stdint.h>

#include <os/zlay_os.h>

#ifndef ZLAY_COCOA_EVENT_QUEUE_CAPACITY
#  define ZLAY_COCOA_EVENT_QUEUE_CAPACITY 256u
#endif

struct ZLay_Window {
  void* ns_window;
  void* ns_view;
  void* ca_layer;
  uint32_t width;
  uint32_t height;
  float scale;
  bool should_close;
  bool events_enabled;
  void* user_data;
  ZLay_Event events[ZLAY_COCOA_EVENT_QUEUE_CAPACITY];
  uint32_t event_head;
  uint32_t event_count;
  float last_mouse_x;
  float last_mouse_y;
  bool has_last_mouse;
  struct ZLay_Window* next;
};

uint64_t ZLay_Cocoa_NowNs(void);
float ZLay_Cocoa_DpiScaleForNative(void* native_window, void* native_view, void* native_layer);
ZLay_OSStyleInfo ZLay_Cocoa_GetStyleInfo(void);
ZLay_Window* ZLay_Cocoa_FindWindow(void* ns_window, void* ns_view);

#endif
