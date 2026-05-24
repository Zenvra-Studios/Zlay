#ifndef ZLAY_OS_WINDOW_ZLAY_WINDOW_H
#define ZLAY_OS_WINDOW_ZLAY_WINDOW_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <zlay.h>
#include <os/zlay_native.h>

typedef struct ZLay_Event ZLay_Event;
typedef struct ZLay_Window ZLay_Window;

typedef struct ZLay_WindowDesc {
  const char* title;
  uint32_t width;
  uint32_t height;
  int32_t x;
  int32_t y;
  bool resizable;
  bool decorated;
  bool visible;
  bool high_dpi;
  bool fullscreen;
  bool maximized;
  bool minimized;
  void* user_data;
} ZLay_WindowDesc;

typedef struct ZLay_WindowAttachDesc {
  ZLay_NativeWindowHandle native;
  uint32_t width;
  uint32_t height;
  float scale;
  bool select_events;
  void* user_data;
} ZLay_WindowAttachDesc;

ZLAY_API ZLay_Window* ZLay_WindowAttachNative(const ZLay_WindowAttachDesc* desc);
ZLAY_API void ZLay_WindowDetach(ZLay_Window* window);
ZLAY_API void ZLay_WindowDestroy(ZLay_Window* window);
ZLAY_API void ZLay_WindowShow(ZLay_Window* window);
ZLAY_API void ZLay_WindowHide(ZLay_Window* window);
ZLAY_API void ZLay_WindowSetTitle(ZLay_Window* window, const char* title);
ZLAY_API bool ZLay_WindowShouldClose(ZLay_Window* window);
ZLAY_API void ZLay_WindowRequestClose(ZLay_Window* window);
ZLAY_API void ZLay_WindowGetSize(ZLay_Window* window, uint32_t* width, uint32_t* height);
ZLAY_API void ZLay_WindowGetFramebufferSize(ZLay_Window* window, uint32_t* width, uint32_t* height);
ZLAY_API float ZLay_WindowGetScale(ZLay_Window* window);
ZLAY_API ZLay_NativeWindowHandle ZLay_WindowGetNativeHandle(ZLay_Window* window);

#ifdef __cplusplus
}
#endif

#endif
