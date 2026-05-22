#ifndef ZLAY_IMPL_GTK_H
#define ZLAY_IMPL_GTK_H

#ifdef __cplusplus
extern "C" {
#endif

#include "os/zlay_os.h"

typedef struct ZLay_ImplGTK_InitInfo {
  void* widget;
  int32_t width;
  int32_t height;
} ZLay_ImplGTK_InitInfo;

ZLAY_API bool ZLay_ImplGTK_Init(const ZLay_ImplGTK_InitInfo* info);
ZLAY_API void ZLay_ImplGTK_Shutdown(void);
ZLAY_API void ZLay_ImplGTK_NewFrame(ZLay_Context* ctx, int32_t width, int32_t height);
ZLAY_API float ZLay_ImplGTK_GetDpiScale(void* widget);
ZLAY_API ZLay_OSStyleInfo ZLay_ImplGTK_GetStyleInfo(void);

#ifdef __cplusplus
}
#endif

#endif

