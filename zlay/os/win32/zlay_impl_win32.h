#ifndef ZLAY_IMPL_WIN32_H
#define ZLAY_IMPL_WIN32_H

#ifdef __cplusplus
extern "C" {
#endif

#include <os/zlay_os.h>

typedef struct ZLay_ImplWin32_InitInfo {
  void* hwnd;
  int32_t width;
  int32_t height;
} ZLay_ImplWin32_InitInfo;

ZLAY_API bool ZLay_ImplWin32_Init(const ZLay_ImplWin32_InitInfo* info);
ZLAY_API void ZLay_ImplWin32_Shutdown(void);
ZLAY_API void ZLay_ImplWin32_NewFrame(ZLay_Context* ctx, int32_t width, int32_t height);
ZLAY_API void ZLay_ImplWin32_EnableDpiAwareness(void);
ZLAY_API float ZLay_ImplWin32_GetDpiScaleForHwnd(void* hwnd);
ZLAY_API ZLay_OSStyleInfo ZLay_ImplWin32_GetStyleInfo(void);

#ifdef __cplusplus
}
#endif

#endif
