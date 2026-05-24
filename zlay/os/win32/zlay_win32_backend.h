#ifndef ZLAY_OS_WIN32_ZLAY_WIN32_BACKEND_H
#define ZLAY_OS_WIN32_ZLAY_WIN32_BACKEND_H

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <os/zlay_os.h>
#if defined(ZLAY_ENABLE_WINRT_SUPPORT)
#  include <os/winrt/zlay_winrt_modern.h>
#endif

#ifndef ZLAY_WIN32_EVENT_QUEUE_CAPACITY
#  define ZLAY_WIN32_EVENT_QUEUE_CAPACITY 256u
#endif

struct ZLay_Window {
  HWND hwnd;
  HINSTANCE instance;
  WNDPROC previous_wndproc;
  uint32_t width;
  uint32_t height;
  float scale;
  bool should_close;
  bool subclassed;
  bool tracking_mouse;
  bool modern_enabled;
  void* user_data;
  ZLay_Event events[ZLAY_WIN32_EVENT_QUEUE_CAPACITY];
  uint32_t event_head;
  uint32_t event_count;
  int last_mouse_x;
  int last_mouse_y;
  bool has_last_mouse;
  struct ZLay_Window* next;
};

uint64_t ZLay_Win32_NowNs(void);
float ZLay_Win32_DpiScaleForHwnd(HWND hwnd);
uint32_t ZLay_Win32_Modifiers(void);
int ZLay_Win32_KeyFromVK(WPARAM vk);
ZLay_Window* ZLay_Win32_FindWindow(HWND hwnd);
ZLAY_API bool ZLay_Win32_ProcessMessage(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam, LRESULT* out_result);
#if defined(ZLAY_ENABLE_WINRT_SUPPORT)
ZLAY_API bool ZLay_Win32_WindowEnableModern(ZLay_Window* window, const ZLay_WinRTModernDesc* desc);
ZLAY_API void ZLay_Win32_WindowDisableModern(ZLay_Window* window);
#endif

#endif
