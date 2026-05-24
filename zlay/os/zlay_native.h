#ifndef ZLAY_OS_ZLAY_NATIVE_H
#define ZLAY_OS_ZLAY_NATIVE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <os/zlay_platform.h>

typedef struct ZLay_NativeWindowHandle {
  ZLay_Platform platform;
  void* display;
  void* window;
  void* surface;
  void* connection;
  uintptr_t raw0;
  uintptr_t raw1;
  uintptr_t raw2;
} ZLay_NativeWindowHandle;

#ifdef __cplusplus
}
#endif

#endif
