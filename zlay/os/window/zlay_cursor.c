#include <os/window/zlay_cursor.h>

#include <os/zlay_os_backend.h>

void ZLay_WindowSetCursor(ZLay_Window* window, ZLay_Cursor cursor) {
#if defined(ZLAY_OS_BACKEND_SELECTED_WIN32)
  ZLay_Win32_WindowSetCursor(window, cursor);
#elif defined(ZLAY_OS_BACKEND_SELECTED_COCOA)
  ZLay_Cocoa_WindowSetCursor(window, cursor);
#elif defined(ZLAY_OS_BACKEND_SELECTED_GTK)
  ZLay_GTK_WindowSetCursor(window, cursor);
#elif defined(ZLAY_OS_BACKEND_SELECTED_X11)
  ZLay_X11_WindowSetCursor(window, cursor);
#else
  (void)window;
  (void)cursor;
#endif
}
