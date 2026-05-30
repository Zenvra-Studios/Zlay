#include <os/event/zlay_event.h>

#include <os/zlay_os_backend.h>

void ZLay_PollEvents(void) {
#if defined(ZLAY_OS_BACKEND_SELECTED_WIN32)
  ZLay_Win32_PollEvents();
#elif defined(ZLAY_OS_BACKEND_SELECTED_COCOA)
  ZLay_Cocoa_PollEvents();
#elif defined(ZLAY_OS_BACKEND_SELECTED_GTK)
  ZLay_GTK_PollEvents();
#elif defined(ZLAY_OS_BACKEND_SELECTED_X11)
  ZLay_X11_PollEvents();
#endif
}

bool ZLay_WindowNextEvent(ZLay_Window* window, ZLay_Event* out_event) {
#if defined(ZLAY_OS_BACKEND_SELECTED_WIN32)
  return ZLay_Win32_WindowNextEvent(window, out_event);
#elif defined(ZLAY_OS_BACKEND_SELECTED_COCOA)
  return ZLay_Cocoa_WindowNextEvent(window, out_event);
#elif defined(ZLAY_OS_BACKEND_SELECTED_GTK)
  return ZLay_GTK_WindowNextEvent(window, out_event);
#elif defined(ZLAY_OS_BACKEND_SELECTED_X11)
  return ZLay_X11_WindowNextEvent(window, out_event);
#else
  (void)window;
  (void)out_event;
  return false;
#endif
}

void ZLay_WindowPushEvent(ZLay_Window* window, const ZLay_Event* event) {
#if defined(ZLAY_OS_BACKEND_SELECTED_WIN32)
  ZLay_Win32_WindowPushEvent(window, event);
#elif defined(ZLAY_OS_BACKEND_SELECTED_COCOA)
  ZLay_Cocoa_WindowPushEvent(window, event);
#elif defined(ZLAY_OS_BACKEND_SELECTED_GTK)
  ZLay_GTK_WindowPushEvent(window, event);
#elif defined(ZLAY_OS_BACKEND_SELECTED_X11)
  ZLay_X11_WindowPushEvent(window, event);
#else
  (void)window;
  (void)event;
#endif
}
