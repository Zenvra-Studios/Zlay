#ifndef ZLAY_DRIVERS_GPU_OPENGL_ZLAY_GL_CONTEXT_H
#define ZLAY_DRIVERS_GPU_OPENGL_ZLAY_GL_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <drivers/gpu/zlay_driver.h>

typedef struct ZLay_GLContext {
  void* native_display;
  void* native_window;
  uint32_t width;
  uint32_t height;
  float dpi_scale;
  bool vsync;
  bool current;
  bool valid;
  uint64_t generation;
} ZLay_GLContext;

ZLAY_API bool ZLay_GLContext_Init(ZLay_GLContext* context, const ZLay_DriverDesc* desc);
ZLAY_API void ZLay_GLContext_Shutdown(ZLay_GLContext* context);
ZLAY_API void ZLay_GLContext_Resize(ZLay_GLContext* context, uint32_t width, uint32_t height, float dpi_scale);
ZLAY_API bool ZLay_GLContext_BeginFrame(ZLay_GLContext* context);
ZLAY_API void ZLay_GLContext_EndFrame(ZLay_GLContext* context);

#ifdef __cplusplus
}
#endif

#endif
