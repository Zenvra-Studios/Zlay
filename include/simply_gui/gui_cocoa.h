#pragma once

#include <stdbool.h>

#include <zlay.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ZLay_CocoaButtonDemo {
  void* app;
  void* window;
  void* view;
  void* window_delegate;
  void* font;
  ZLay_Arena arena;
  ZLay_Context* ctx;
  ZLay_Rect button_bounds;
  void* memory;
  int width;
  int height;
  bool running;
  bool hovered;
  bool pressed;
} ZLay_CocoaButtonDemo;

bool ZLay_CocoaButtonDemo_Init(ZLay_CocoaButtonDemo* demo);
void ZLay_CocoaButtonDemo_Run(ZLay_CocoaButtonDemo* demo);
void ZLay_CocoaButtonDemo_Shutdown(ZLay_CocoaButtonDemo* demo);

#ifdef __cplusplus
}
#endif
