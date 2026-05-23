#ifndef ZLAY_IMPL_X11_H
#define ZLAY_IMPL_X11_H

#ifdef __cplusplus
extern "C" {
#endif

#include <os/zlay_os.h>

typedef struct ZLay_ImplX11_InitInfo {
  void* display;
  unsigned long window;
  int32_t width;
  int32_t height;
} ZLay_ImplX11_InitInfo;

typedef ZLay_ImplX11_InitInfo ZLay_X11InitInfo;

ZLAY_API bool ZLay_ImplX11_Init(const ZLay_ImplX11_InitInfo* info);
ZLAY_API void ZLay_ImplX11_Shutdown(void);
ZLAY_API void ZLay_ImplX11_NewFrame(ZLay_Context* ctx, int32_t width, int32_t height);
ZLAY_API float ZLay_ImplX11_GetDpiScale(void* display);
ZLAY_API ZLay_OSStyleInfo ZLay_ImplX11_GetStyleInfo(void);
ZLAY_API bool ZLay_X11_Init(const ZLay_X11InitInfo* info);
ZLAY_API void ZLay_X11_Shutdown(void);
ZLAY_API void ZLay_X11_NewFrame(ZLay_Context* ctx, int32_t width, int32_t height);
ZLAY_API float ZLay_X11_GetDpiScale(void* display);
ZLAY_API ZLay_OSStyleInfo ZLay_X11_GetStyleInfo(void);

#ifdef __cplusplus
}
#endif

#endif
