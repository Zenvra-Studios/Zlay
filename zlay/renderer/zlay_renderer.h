#ifndef ZLAY_RENDERER_H
#define ZLAY_RENDERER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "zlay.h"

typedef enum ZLay_RendererBackendType {
  ZLAY_RENDERER_BACKEND_NULL = 0,
  ZLAY_RENDERER_BACKEND_OPENGL,
  ZLAY_RENDERER_BACKEND_VULKAN,
  ZLAY_RENDERER_BACKEND_WEBGPU,
  ZLAY_RENDERER_BACKEND_METAL,
  ZLAY_RENDERER_BACKEND_D3D11,
  ZLAY_RENDERER_BACKEND_D3D12,
  ZLAY_RENDERER_BACKEND_SOFTWARE
} ZLay_RendererBackendType;

typedef struct ZLay_RendererFrameInfo {
  int32_t width;
  int32_t height;
  float dpi_scale;
  ZLay_Color clear_color;
} ZLay_RendererFrameInfo;

typedef struct ZLay_RendererStats {
  uint32_t command_count;
  uint32_t draw_call_count;
  uint32_t vertex_count;
  uint32_t index_count;
} ZLay_RendererStats;

typedef bool (*ZLay_RendererBeginFrameFn)(void* user, const ZLay_RendererFrameInfo* frame);
typedef bool (*ZLay_RendererRenderFn)(void* user, const ZLay_RenderCommandList* commands, ZLay_RendererStats* stats);
typedef void (*ZLay_RendererEndFrameFn)(void* user);
typedef void (*ZLay_RendererShutdownFn)(void* user);

typedef struct ZLay_Renderer {
  ZLay_RendererBackendType backend;
  void* user;
  ZLay_RendererBeginFrameFn begin_frame;
  ZLay_RendererRenderFn render;
  ZLay_RendererEndFrameFn end_frame;
  ZLay_RendererShutdownFn shutdown;
  ZLay_RendererStats last_stats;
} ZLay_Renderer;

ZLAY_API ZLay_Renderer ZLay_RendererNull(void);
ZLAY_API const char* ZLay_RendererBackendName(ZLay_RendererBackendType backend);
ZLAY_API bool ZLay_RendererBeginFrame(ZLay_Renderer* renderer, const ZLay_RendererFrameInfo* frame);
ZLAY_API bool ZLay_RendererRenderCommands(ZLay_Renderer* renderer, const ZLay_RenderCommandList* commands);
ZLAY_API void ZLay_RendererEndFrame(ZLay_Renderer* renderer);
ZLAY_API void ZLay_RendererShutdown(ZLay_Renderer* renderer);

#ifdef __cplusplus
}
#endif

#endif
