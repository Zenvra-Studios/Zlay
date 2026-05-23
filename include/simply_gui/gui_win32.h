#pragma once

#include <stdbool.h>

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include <zlay.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ZLay_Win32ButtonDemo {
  HINSTANCE instance;
  HWND window;
  HFONT font;
  ZLay_Arena arena;
  ZLay_Context* ctx;
  ZLay_Rect button_bounds;
  void* memory;
  int width;
  int height;
  bool running;
  bool hovered;
  bool pressed;
} ZLay_Win32ButtonDemo;

bool ZLay_Win32ButtonDemo_Init(ZLay_Win32ButtonDemo* demo, HINSTANCE instance, int show_command);
void ZLay_Win32ButtonDemo_Run(ZLay_Win32ButtonDemo* demo);
void ZLay_Win32ButtonDemo_Shutdown(ZLay_Win32ButtonDemo* demo);

#ifdef __cplusplus
}
#endif
