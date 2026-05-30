#include <drivers/gpu/opengl/zlay_gl_driver.h>

#include <stdlib.h>

static bool ZLay_OpenGLDriver_Create(ZLay_Driver* driver, const ZLay_DriverDesc* desc) {
  ZLay_GLDriverState* gl;
  ZLay_DriverBackendState* state = ZLay_DriverGetBackendState(driver);

  if (!driver || !desc || !state) return false;

  gl = (ZLay_GLDriverState*)calloc(1u, sizeof(ZLay_GLDriverState));
  if (!gl) return false;

  if (!ZLay_GLContext_Init(&gl->context, desc) || !ZLay_GLPipeline_InitRect(&gl->rect_pipeline)) {
    free(gl);
    return false;
  }

  ZLay_GLBuffer_Init(&gl->vertex_buffer, ZLAY_GL_BUFFER_VERTEX);
  ZLay_GLBuffer_Init(&gl->index_buffer, ZLAY_GL_BUFFER_INDEX);
  ZLay_GLBuffer_Init(&gl->uniform_buffer, ZLAY_GL_BUFFER_UNIFORM);
  ZLay_GLTexture_Init(&gl->font_texture);

  driver->backend_data = gl;
  driver->owns_backend_data = true;

  state->backend = ZLAY_BACKEND_OPENGL;
  state->name = "OpenGL";
  state->context_created = true;
  state->frame_open = false;
  state->submitted = false;
  state->presented = false;
  return true;
}

static void ZLay_OpenGLDriver_Destroy(ZLay_Driver* driver) {
  ZLay_GLDriverState* gl = driver ? (ZLay_GLDriverState*)driver->backend_data : NULL;
  ZLay_DriverBackendState* state = ZLay_DriverGetBackendState(driver);

  if (gl) {
    ZLay_GLTexture_Shutdown(&gl->font_texture);
    ZLay_GLBuffer_Shutdown(&gl->uniform_buffer);
    ZLay_GLBuffer_Shutdown(&gl->index_buffer);
    ZLay_GLBuffer_Shutdown(&gl->vertex_buffer);
    ZLay_GLPipeline_Shutdown(&gl->rect_pipeline);
    ZLay_GLContext_Shutdown(&gl->context);
  }

  if (state) {
    state->context_created = false;
    state->frame_open = false;
  }
}

static void ZLay_OpenGLDriver_Resize(ZLay_Driver* driver, uint32_t width, uint32_t height) {
  ZLay_GLDriverState* gl = driver ? (ZLay_GLDriverState*)driver->backend_data : NULL;
  const ZLay_Surface* surface = ZLay_DriverGetSurfaceConst(driver);
  if (!gl) return;
  ZLay_GLContext_Resize(&gl->context, width, height, surface ? surface->dpi_scale : 1.0f);
  ZLay_GLPipeline_Invalidate(&gl->rect_pipeline);
  ZLay_GLPipeline_InitRect(&gl->rect_pipeline);
}

static bool ZLay_OpenGLDriver_BeginFrame(ZLay_Driver* driver) {
  ZLay_GLDriverState* gl = driver ? (ZLay_GLDriverState*)driver->backend_data : NULL;
  ZLay_DriverBackendState* state = ZLay_DriverGetBackendState(driver);
  const ZLay_DriverStats stats = ZLay_DriverGetStats(driver);
  if (!gl || !state || !state->context_created) return false;
  if (!ZLay_GLContext_BeginFrame(&gl->context)) return false;
  state->frame_index = stats.frame_index;
  state->swapchain_image_index = stats.swapchain_image_index;
  state->frame_open = true;
  state->submitted = false;
  state->presented = false;
  return true;
}

static bool ZLay_OpenGLDriver_Submit(ZLay_Driver* driver, const ZLay_RenderCommandList* commands) {
  (void)commands;
  ZLay_GLDriverState* gl = driver ? (ZLay_GLDriverState*)driver->backend_data : NULL;
  ZLay_DriverBackendState* state = ZLay_DriverGetBackendState(driver);
  const ZLay_DriverFramePacket* packet = ZLay_DriverGetFramePacket(driver);
  const ZLay_DriverStats stats = ZLay_DriverGetStats(driver);
  const ZLay_Swapchain* swapchain = ZLay_DriverGetSwapchainConst(driver);
  if (!gl || !state || !packet || !state->frame_open || !gl->rect_pipeline.valid) return false;

  if (!ZLay_GLBuffer_Upload(&gl->vertex_buffer, packet->vertex_upload, packet->vertex_upload_size, stats.vertex_count)) return false;
  if (!ZLay_GLBuffer_Upload(&gl->index_buffer, packet->index_upload, packet->index_upload_size, stats.index_count)) return false;

  state->submitted_frame_index = stats.frame_index;
  state->swapchain_generation = swapchain ? (uint32_t)swapchain->generation : 0u;
  state->swapchain_image_index = packet->swapchain_image_index;
  state->draw_calls = stats.draw_calls;
  state->vertex_count = stats.vertex_count;
  state->index_count = stats.index_count;
  state->uploaded_bytes = packet->vertex_upload_size + packet->index_upload_size;
  state->submitted = true;
  return true;
}

static bool ZLay_OpenGLDriver_EndFrame(ZLay_Driver* driver) {
  ZLay_GLDriverState* gl = driver ? (ZLay_GLDriverState*)driver->backend_data : NULL;
  if (!gl) return false;
  ZLay_GLContext_EndFrame(&gl->context);
  return true;
}

static bool ZLay_OpenGLDriver_Present(ZLay_Driver* driver) {
  ZLay_DriverBackendState* state = ZLay_DriverGetBackendState(driver);
  if (!state || !state->frame_open) return false;
  state->presented_frame_index = ZLay_DriverGetStats(driver).frame_index;
  state->presented = true;
  state->frame_open = false;
  return true;
}

ZLay_DriverAPI ZLay_OpenGLDriverAPI(void) {
  ZLay_DriverAPI api;
  api.create = ZLay_OpenGLDriver_Create;
  api.destroy = ZLay_OpenGLDriver_Destroy;
  api.resize = ZLay_OpenGLDriver_Resize;
  api.begin_frame = ZLay_OpenGLDriver_BeginFrame;
  api.submit = ZLay_OpenGLDriver_Submit;
  api.end_frame = ZLay_OpenGLDriver_EndFrame;
  api.present = ZLay_OpenGLDriver_Present;
  return api;
}
