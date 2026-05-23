#pragma once

#include <stdbool.h>

#include <X11/Xlib.h>

#include <zlay.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ZLay_X11ButtonDemo {
  Display* display;
  int screen;
  Window window;
  GC gc;
  Atom wm_delete_window;
  XFontStruct* font;
  ZLay_Arena arena;
  ZLay_Context* ctx;
  ZLay_Rect button_bounds;
  void* memory;
  int width;
  int height;
  bool running;
  bool hovered;
  bool pressed;
} ZLay_X11ButtonDemo;

bool ZLay_X11ButtonDemo_Init(ZLay_X11ButtonDemo* demo);
void ZLay_X11ButtonDemo_Run(ZLay_X11ButtonDemo* demo);
void ZLay_X11ButtonDemo_Shutdown(ZLay_X11ButtonDemo* demo);

#ifdef __cplusplus
}
#endif
