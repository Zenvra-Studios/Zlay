#include <drivers/gpu/opengl/zlay_gl_context.h>

#include <string.h>

bool ZLay_GLContext_Init(ZLay_GLContext* context, const ZLay_DriverDesc* desc) {
  if (!context || !desc) return false;

  memset(context, 0, sizeof(*context));
  context->native_display = desc->native_display;
  context->native_window = desc->native_window;
  context->width = desc->width != 0u ? desc->width : 1u;
  context->height = desc->height != 0u ? desc->height : 1u;
  context->dpi_scale = desc->dpi_scale > 0.0f ? desc->dpi_scale : 1.0f;
  context->vsync = desc->vsync;
  context->valid = true;
  context->generation = 1u;
  return true;
}

void ZLay_GLContext_Shutdown(ZLay_GLContext* context) {
  if (!context) return;
  context->current = false;
  context->valid = false;
  context->generation++;
}

void ZLay_GLContext_Resize(ZLay_GLContext* context, uint32_t width, uint32_t height, float dpi_scale) {
  if (!context) return;
  context->width = width != 0u ? width : 1u;
  context->height = height != 0u ? height : 1u;
  context->dpi_scale = dpi_scale > 0.0f ? dpi_scale : 1.0f;
  context->generation++;
}

bool ZLay_GLContext_BeginFrame(ZLay_GLContext* context) {
  if (!context || !context->valid) return false;
  context->current = true;
  return true;
}

void ZLay_GLContext_EndFrame(ZLay_GLContext* context) {
  if (!context) return;
  context->current = false;
}
