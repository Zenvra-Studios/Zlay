#ifndef ZLAY_OS_WINDOW_ZLAY_CURSOR_H
#define ZLAY_OS_WINDOW_ZLAY_CURSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zlay.h>
#include <os/window/zlay_window.h>

typedef enum ZLay_Cursor {
  ZLAY_CURSOR_ARROW = 0,
  ZLAY_CURSOR_HAND,
  ZLAY_CURSOR_TEXT,
  ZLAY_CURSOR_RESIZE_HORIZONTAL,
  ZLAY_CURSOR_RESIZE_VERTICAL,
  ZLAY_CURSOR_CROSSHAIR,
  ZLAY_CURSOR_HIDDEN
} ZLay_Cursor;

ZLAY_API void ZLay_WindowSetCursor(ZLay_Window* window, ZLay_Cursor cursor);

#ifdef __cplusplus
}
#endif

#endif
