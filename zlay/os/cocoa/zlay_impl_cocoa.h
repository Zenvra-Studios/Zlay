#ifndef ZLAY_IMPL_COCOA_H
#define ZLAY_IMPL_COCOA_H

#ifdef __cplusplus
extern "C" {
#endif

#include <os/zlay_os.h>

typedef struct ZLay_ImplCocoa_InitInfo {
  void* ns_window;
  void* ns_view;
  int32_t width;
  int32_t height;
} ZLay_ImplCocoa_InitInfo;

typedef ZLay_ImplCocoa_InitInfo ZLay_CocoaInitInfo;

ZLAY_API bool ZLay_ImplCocoa_Init(const ZLay_ImplCocoa_InitInfo* info);
ZLAY_API void ZLay_ImplCocoa_Shutdown(void);
ZLAY_API void ZLay_ImplCocoa_NewFrame(ZLay_Context* ctx, int32_t width, int32_t height);
ZLAY_API float ZLay_ImplCocoa_GetDpiScale(void* ns_view);
ZLAY_API ZLay_OSStyleInfo ZLay_ImplCocoa_GetStyleInfo(void);
ZLAY_API bool ZLay_Cocoa_Init(const ZLay_CocoaInitInfo* info);
ZLAY_API void ZLay_Cocoa_Shutdown(void);
ZLAY_API void ZLay_Cocoa_NewFrame(ZLay_Context* ctx, int32_t width, int32_t height);
ZLAY_API float ZLay_Cocoa_GetDpiScale(void* ns_view);
ZLAY_API ZLay_OSStyleInfo ZLay_Cocoa_GetStyleInfo(void);

#ifdef __cplusplus
}
#endif

#endif
