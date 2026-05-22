#include "renderer/zlay_renderer.h"

static bool ZLay_RendererNullBeginFrame(void* user, const ZLay_RendererFrameInfo* frame) {
  (void)user;
  (void)frame;
  return true;
}

static bool ZLay_RendererNullRender(void* user, const ZLay_RenderCommandList* commands, ZLay_RendererStats* stats) {
  (void)user;
  if (stats) {
    stats->command_count = commands ? commands->count : 0;
    stats->draw_call_count = 0;
    stats->vertex_count = 0;
    stats->index_count = 0;
  }
  return true;
}

static void ZLay_RendererNullEndFrame(void* user) {
  (void)user;
}

static void ZLay_RendererNullShutdown(void* user) {
  (void)user;
}

ZLay_Renderer ZLay_RendererNull(void) {
  ZLay_Renderer renderer;
  renderer.backend = ZLAY_RENDERER_BACKEND_NULL;
  renderer.user = 0;
  renderer.begin_frame = ZLay_RendererNullBeginFrame;
  renderer.render = ZLay_RendererNullRender;
  renderer.end_frame = ZLay_RendererNullEndFrame;
  renderer.shutdown = ZLay_RendererNullShutdown;
  renderer.last_stats = (ZLay_RendererStats){0};
  return renderer;
}

const char* ZLay_RendererBackendName(ZLay_RendererBackendType backend) {
  switch (backend) {
    case ZLAY_RENDERER_BACKEND_OPENGL: return "OpenGL";
    case ZLAY_RENDERER_BACKEND_VULKAN: return "Vulkan";
    case ZLAY_RENDERER_BACKEND_WEBGPU: return "WebGPU";
    case ZLAY_RENDERER_BACKEND_METAL: return "Metal";
    case ZLAY_RENDERER_BACKEND_D3D11: return "D3D11";
    case ZLAY_RENDERER_BACKEND_D3D12: return "D3D12";
    case ZLAY_RENDERER_BACKEND_SOFTWARE: return "Software";
    case ZLAY_RENDERER_BACKEND_NULL:
    default: return "Null";
  }
}

bool ZLay_RendererBeginFrame(ZLay_Renderer* renderer, const ZLay_RendererFrameInfo* frame) {
  if (!renderer || !renderer->begin_frame) return false;
  renderer->last_stats = (ZLay_RendererStats){0};
  return renderer->begin_frame(renderer->user, frame);
}

bool ZLay_RendererRenderCommands(ZLay_Renderer* renderer, const ZLay_RenderCommandList* commands) {
  if (!renderer || !renderer->render) return false;
  ZLay_RendererStats stats = {0};
  const bool ok = renderer->render(renderer->user, commands, &stats);
  renderer->last_stats = stats;
  return ok;
}

void ZLay_RendererEndFrame(ZLay_Renderer* renderer) {
  if (!renderer || !renderer->end_frame) return;
  renderer->end_frame(renderer->user);
}

void ZLay_RendererShutdown(ZLay_Renderer* renderer) {
  if (!renderer || !renderer->shutdown) return;
  renderer->shutdown(renderer->user);
  *renderer = ZLay_RendererNull();
}
