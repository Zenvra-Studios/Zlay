#include <os/window/zlay_window.h>

#include <os/zlay_os_backend.h>

ZLay_Window* ZLay_WindowAttachNative(const ZLay_WindowAttachDesc* desc) {
#if defined(ZLAY_OS_BACKEND_SELECTED_WIN32)
  return ZLay_Win32_WindowAttachNative(desc);
#elif defined(ZLAY_OS_BACKEND_SELECTED_COCOA)
  return ZLay_Cocoa_WindowAttachNative(desc);
#elif defined(ZLAY_OS_BACKEND_SELECTED_X11)
  return ZLay_X11_WindowAttachNative(desc);
#else
  (void)desc;
  return NULL;
#endif
}

void ZLay_WindowDetach(ZLay_Window* window) {
#if defined(ZLAY_OS_BACKEND_SELECTED_WIN32)
  ZLay_Win32_WindowDetach(window);
#elif defined(ZLAY_OS_BACKEND_SELECTED_COCOA)
  ZLay_Cocoa_WindowDetach(window);
#elif defined(ZLAY_OS_BACKEND_SELECTED_X11)
  ZLay_X11_WindowDetach(window);
#else
  (void)window;
#endif
}

void ZLay_WindowDestroy(ZLay_Window* window) {
  ZLay_WindowDetach(window);
}

ZLay_Window* ZLay_WindowCreate(const ZLay_WindowDesc* desc) {
  (void)desc;
  return NULL;
}

void ZLay_WindowShow(ZLay_Window* window) {
#if defined(ZLAY_OS_BACKEND_SELECTED_WIN32)
  ZLay_Win32_WindowShow(window);
#elif defined(ZLAY_OS_BACKEND_SELECTED_COCOA)
  ZLay_Cocoa_WindowShow(window);
#elif defined(ZLAY_OS_BACKEND_SELECTED_X11)
  ZLay_X11_WindowShow(window);
#else
  (void)window;
#endif
}

void ZLay_WindowHide(ZLay_Window* window) {
#if defined(ZLAY_OS_BACKEND_SELECTED_WIN32)
  ZLay_Win32_WindowHide(window);
#elif defined(ZLAY_OS_BACKEND_SELECTED_COCOA)
  ZLay_Cocoa_WindowHide(window);
#elif defined(ZLAY_OS_BACKEND_SELECTED_X11)
  ZLay_X11_WindowHide(window);
#else
  (void)window;
#endif
}

void ZLay_WindowSetTitle(ZLay_Window* window, const char* title) {
#if defined(ZLAY_OS_BACKEND_SELECTED_WIN32)
  ZLay_Win32_WindowSetTitle(window, title);
#elif defined(ZLAY_OS_BACKEND_SELECTED_COCOA)
  ZLay_Cocoa_WindowSetTitle(window, title);
#elif defined(ZLAY_OS_BACKEND_SELECTED_X11)
  ZLay_X11_WindowSetTitle(window, title);
#else
  (void)window;
  (void)title;
#endif
}

bool ZLay_WindowShouldClose(ZLay_Window* window) {
#if defined(ZLAY_OS_BACKEND_SELECTED_WIN32)
  return ZLay_Win32_WindowShouldClose(window);
#elif defined(ZLAY_OS_BACKEND_SELECTED_COCOA)
  return ZLay_Cocoa_WindowShouldClose(window);
#elif defined(ZLAY_OS_BACKEND_SELECTED_X11)
  return ZLay_X11_WindowShouldClose(window);
#else
  (void)window;
  return true;
#endif
}

void ZLay_WindowRequestClose(ZLay_Window* window) {
#if defined(ZLAY_OS_BACKEND_SELECTED_WIN32)
  ZLay_Win32_WindowRequestClose(window);
#elif defined(ZLAY_OS_BACKEND_SELECTED_COCOA)
  ZLay_Cocoa_WindowRequestClose(window);
#elif defined(ZLAY_OS_BACKEND_SELECTED_X11)
  ZLay_X11_WindowRequestClose(window);
#else
  (void)window;
#endif
}

void ZLay_WindowGetSize(ZLay_Window* window, uint32_t* width, uint32_t* height) {
#if defined(ZLAY_OS_BACKEND_SELECTED_WIN32)
  ZLay_Win32_WindowGetSize(window, width, height);
#elif defined(ZLAY_OS_BACKEND_SELECTED_COCOA)
  ZLay_Cocoa_WindowGetSize(window, width, height);
#elif defined(ZLAY_OS_BACKEND_SELECTED_X11)
  ZLay_X11_WindowGetSize(window, width, height);
#else
  (void)window;
  if (width != NULL) *width = 0u;
  if (height != NULL) *height = 0u;
#endif
}

void ZLay_WindowGetFramebufferSize(ZLay_Window* window, uint32_t* width, uint32_t* height) {
#if defined(ZLAY_OS_BACKEND_SELECTED_WIN32)
  ZLay_Win32_WindowGetFramebufferSize(window, width, height);
#elif defined(ZLAY_OS_BACKEND_SELECTED_COCOA)
  ZLay_Cocoa_WindowGetFramebufferSize(window, width, height);
#elif defined(ZLAY_OS_BACKEND_SELECTED_X11)
  ZLay_X11_WindowGetFramebufferSize(window, width, height);
#else
  (void)window;
  if (width != NULL) *width = 0u;
  if (height != NULL) *height = 0u;
#endif
}

float ZLay_WindowGetScale(ZLay_Window* window) {
#if defined(ZLAY_OS_BACKEND_SELECTED_WIN32)
  return ZLay_Win32_WindowGetScale(window);
#elif defined(ZLAY_OS_BACKEND_SELECTED_COCOA)
  return ZLay_Cocoa_WindowGetScale(window);
#elif defined(ZLAY_OS_BACKEND_SELECTED_X11)
  return ZLay_X11_WindowGetScale(window);
#else
  (void)window;
  return 1.0f;
#endif
}

ZLay_NativeWindowHandle ZLay_WindowGetNativeHandle(ZLay_Window* window) {
#if defined(ZLAY_OS_BACKEND_SELECTED_WIN32)
  return ZLay_Win32_WindowGetNativeHandle(window);
#elif defined(ZLAY_OS_BACKEND_SELECTED_COCOA)
  return ZLay_Cocoa_WindowGetNativeHandle(window);
#elif defined(ZLAY_OS_BACKEND_SELECTED_X11)
  return ZLay_X11_WindowGetNativeHandle(window);
#else
  (void)window;
  return (ZLay_NativeWindowHandle){0};
#endif
}
