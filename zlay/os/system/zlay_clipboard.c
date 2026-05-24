#include <os/system/zlay_clipboard.h>

#include <os/zlay_os_backend.h>

bool ZLay_ClipboardSetText(const char* text) {
#if defined(ZLAY_OS_BACKEND_SELECTED_WIN32)
  return ZLay_Win32_ClipboardSetText(text);
#elif defined(ZLAY_OS_BACKEND_SELECTED_COCOA)
  return ZLay_Cocoa_ClipboardSetText(text);
#elif defined(ZLAY_OS_BACKEND_SELECTED_X11)
  return ZLay_X11_ClipboardSetText(text);
#else
  (void)text;
  return false;
#endif
}

const char* ZLay_ClipboardGetText(void) {
#if defined(ZLAY_OS_BACKEND_SELECTED_WIN32)
  return ZLay_Win32_ClipboardGetText();
#elif defined(ZLAY_OS_BACKEND_SELECTED_COCOA)
  return ZLay_Cocoa_ClipboardGetText();
#elif defined(ZLAY_OS_BACKEND_SELECTED_X11)
  return ZLay_X11_ClipboardGetText();
#else
  return "";
#endif
}
