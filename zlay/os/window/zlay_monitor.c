#include <os/window/zlay_monitor.h>

#include <os/zlay_os_backend.h>

uint32_t ZLay_GetMonitorCount(void) {
#if defined(ZLAY_OS_BACKEND_SELECTED_WIN32)
  return ZLay_Win32_GetMonitorCount();
#elif defined(ZLAY_OS_BACKEND_SELECTED_COCOA)
  return ZLay_Cocoa_GetMonitorCount();
#elif defined(ZLAY_OS_BACKEND_SELECTED_GTK)
  return ZLay_GTK_GetMonitorCount();
#elif defined(ZLAY_OS_BACKEND_SELECTED_X11)
  return ZLay_X11_GetMonitorCount();
#else
  return 0u;
#endif
}

bool ZLay_GetMonitorInfo(uint32_t index, ZLay_MonitorInfo* out_info) {
#if defined(ZLAY_OS_BACKEND_SELECTED_WIN32)
  return ZLay_Win32_GetMonitorInfo(index, out_info);
#elif defined(ZLAY_OS_BACKEND_SELECTED_COCOA)
  return ZLay_Cocoa_GetMonitorInfo(index, out_info);
#elif defined(ZLAY_OS_BACKEND_SELECTED_GTK)
  return ZLay_GTK_GetMonitorInfo(index, out_info);
#elif defined(ZLAY_OS_BACKEND_SELECTED_X11)
  return ZLay_X11_GetMonitorInfo(index, out_info);
#else
  (void)index;
  (void)out_info;
  return false;
#endif
}
