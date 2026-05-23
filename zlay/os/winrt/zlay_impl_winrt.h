#ifndef ZLAY_IMPL_WINRT_H
#define ZLAY_IMPL_WINRT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <os/zlay_os.h>

typedef struct ZLay_ImplWinRT_InitInfo {
  void* core_window;
  int32_t width;
  int32_t height;
} ZLay_ImplWinRT_InitInfo;

ZLAY_API bool ZLay_ImplWinRT_Init(const ZLay_ImplWinRT_InitInfo* info);
ZLAY_API void ZLay_ImplWinRT_Shutdown(void);
ZLAY_API void ZLay_ImplWinRT_NewFrame(ZLay_Context* ctx, int32_t width, int32_t height);
ZLAY_API float ZLay_ImplWinRT_GetDpiScale(void* core_window);
ZLAY_API ZLay_OSStyleInfo ZLay_ImplWinRT_GetStyleInfo(void);

#ifdef __cplusplus
}
#endif

#endif
