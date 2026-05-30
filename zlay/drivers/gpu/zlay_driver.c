#include <drivers/gpu/zlay_driver.h>

#if defined(ZLAY_ENABLE_DRIVER_OPENGL)
#  include <drivers/gpu/opengl/zlay_gl_driver.h>
#endif
#if defined(ZLAY_ENABLE_DRIVER_VULKAN)
#  include <drivers/gpu/vulkan/zlay_vk_driver.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <time.h>

static double ZLay__DriverNowMs(void) {
  return ((double)clock() * 1000.0) / (double)CLOCKS_PER_SEC;
}

static bool ZLay__DriverNoopCreate(ZLay_Driver* driver, const ZLay_DriverDesc* desc) {
  (void)desc;
  if (driver) {
    driver->backend_state.backend = driver->backend;
    driver->backend_state.name = ZLay_BackendName(driver->backend);
    driver->backend_state.context_created = true;
  }
  return true;
}

static void ZLay__DriverNoopDestroy(ZLay_Driver* driver) {
  if (driver) {
    driver->backend_state.context_created = false;
    driver->backend_state.frame_open = false;
  }
}

static void ZLay__DriverNoopResize(ZLay_Driver* driver, uint32_t width, uint32_t height) {
  (void)driver;
  (void)width;
  (void)height;
}

static bool ZLay__DriverNoopBeginFrame(ZLay_Driver* driver) {
  if (driver) {
    driver->backend_state.frame_index = driver->stats.frame_index;
    driver->backend_state.swapchain_image_index = driver->stats.swapchain_image_index;
    driver->backend_state.frame_open = true;
    driver->backend_state.submitted = false;
    driver->backend_state.presented = false;
  }
  return true;
}

static bool ZLay__DriverNoopSubmit(ZLay_Driver* driver, const ZLay_RenderCommandList* commands) {
  (void)commands;
  if (driver) {
    driver->backend_state.submitted_frame_index = driver->stats.frame_index;
    driver->backend_state.swapchain_generation = (uint32_t)driver->swapchain.generation;
    driver->backend_state.draw_calls = driver->stats.draw_calls;
    driver->backend_state.vertex_count = driver->stats.vertex_count;
    driver->backend_state.index_count = driver->stats.index_count;
    driver->backend_state.uploaded_bytes = driver->stats.upload_bytes;
    driver->backend_state.submitted = true;
  }
  return true;
}

static bool ZLay__DriverNoopEndFrame(ZLay_Driver* driver) {
  (void)driver;
  return true;
}

static bool ZLay__DriverNoopPresent(ZLay_Driver* driver) {
  if (driver) {
    driver->backend_state.presented_frame_index = driver->stats.frame_index;
    driver->backend_state.presented = true;
    driver->backend_state.frame_open = false;
  }
  return true;
}

static ZLay_DriverAPI ZLay__DriverNoopAPI(void) {
  ZLay_DriverAPI api;
  api.create = ZLay__DriverNoopCreate;
  api.destroy = ZLay__DriverNoopDestroy;
  api.resize = ZLay__DriverNoopResize;
  api.begin_frame = ZLay__DriverNoopBeginFrame;
  api.submit = ZLay__DriverNoopSubmit;
  api.end_frame = ZLay__DriverNoopEndFrame;
  api.present = ZLay__DriverNoopPresent;
  return api;
}

static ZLay_DriverAPI ZLay__DriverAPIForBackend(ZLay_Backend backend) {
  switch (backend) {
#if defined(ZLAY_ENABLE_DRIVER_OPENGL)
    case ZLAY_BACKEND_OPENGL:
      return ZLay_OpenGLDriverAPI();
#endif
#if defined(ZLAY_ENABLE_DRIVER_VULKAN)
    case ZLAY_BACKEND_VULKAN:
      return ZLay_VulkanDriverAPI();
#endif
    case ZLAY_BACKEND_SOFTWARE:
    case ZLAY_BACKEND_NONE:
    default:
      return ZLay__DriverNoopAPI();
  }
}

static void ZLay__DriverFreeOwnedBuffers(ZLay_Driver* driver) {
  if (!driver) return;
  if (driver->owns_vertices) {
    free(driver->batch.vertices);
    driver->batch.vertices = NULL;
  }
  if (driver->owns_indices) {
    free(driver->batch.indices);
    driver->batch.indices = NULL;
  }
  if (driver->owns_upload_memory) {
    free(driver->upload_memory);
    driver->upload_memory = NULL;
  }
  driver->owns_vertices = false;
  driver->owns_indices = false;
  driver->owns_upload_memory = false;
}

static bool ZLay__DriverEnsureBatchMemory(ZLay_Driver* driver, const ZLay_DriverDesc* desc) {
  uint32_t vertex_capacity;
  uint32_t index_capacity;
  ZLay_Vertex* vertices;
  uint32_t* indices;

  if (!driver || !desc) return false;

  vertex_capacity = desc->vertex_capacity != 0u ? desc->vertex_capacity : ZLAY_DRIVER_DEFAULT_VERTEX_CAPACITY;
  index_capacity = desc->index_capacity != 0u ? desc->index_capacity : ZLAY_DRIVER_DEFAULT_INDEX_CAPACITY;
  vertices = desc->vertices;
  indices = desc->indices;

  if (!vertices) {
    vertices = (ZLay_Vertex*)calloc((size_t)vertex_capacity, sizeof(ZLay_Vertex));
    if (!vertices) return false;
    driver->owns_vertices = true;
  }

  if (!indices) {
    indices = (uint32_t*)calloc((size_t)index_capacity, sizeof(uint32_t));
    if (!indices) {
      if (driver->owns_vertices) {
        free(vertices);
        driver->owns_vertices = false;
      }
      return false;
    }
    driver->owns_indices = true;
  }

  ZLay_BatchInit(&driver->batch, vertices, vertex_capacity, indices, index_capacity);
  return true;
}

static bool ZLay__DriverEnsureUploadMemory(ZLay_Driver* driver, const ZLay_DriverDesc* desc) {
  size_t upload_capacity;
  void* upload_memory;

  if (!driver || !desc) return false;

  upload_capacity = desc->upload_capacity != 0u ? desc->upload_capacity : (size_t)ZLAY_DRIVER_DEFAULT_UPLOAD_CAPACITY;
  upload_memory = desc->upload_memory;

  if (!upload_memory) {
    upload_memory = calloc(1u, upload_capacity);
    if (!upload_memory) return false;
    driver->owns_upload_memory = true;
  }

  driver->upload_memory = upload_memory;
  ZLay_GpuRingBufferInit(&driver->upload_ring, upload_memory, upload_capacity);
  return true;
}

static void ZLay__DriverResetFramePacket(ZLay_Driver* driver) {
  if (!driver) return;
  memset(&driver->frame_packet, 0, sizeof(driver->frame_packet));
  driver->frame_packet.batch = &driver->batch;
  driver->frame_packet.resources = driver->current_frame;
}

static bool ZLay__DriverUploadBatch(ZLay_Driver* driver) {
  const size_t vertex_bytes = (size_t)driver->batch.vertex_count * sizeof(ZLay_Vertex);
  const size_t index_bytes = (size_t)driver->batch.index_count * sizeof(uint32_t);
  void* vertex_upload = NULL;
  void* index_upload = NULL;

  if (!driver) return false;

  ZLay__DriverResetFramePacket(driver);

  if (vertex_bytes != 0u) {
    vertex_upload = ZLay_GpuRingBufferAlloc(&driver->upload_ring, vertex_bytes, 16u);
    if (!vertex_upload) return false;
    memcpy(vertex_upload, driver->batch.vertices, vertex_bytes);
  }

  if (index_bytes != 0u) {
    index_upload = ZLay_GpuRingBufferAlloc(&driver->upload_ring, index_bytes, 4u);
    if (!index_upload) return false;
    memcpy(index_upload, driver->batch.indices, index_bytes);
  }

  driver->frame_packet.vertex_upload = vertex_upload;
  driver->frame_packet.index_upload = index_upload;
  driver->frame_packet.vertex_upload_size = vertex_bytes;
  driver->frame_packet.index_upload_size = index_bytes;
  driver->frame_packet.frame_resource_index = driver->stats.frame_resource_index;
  driver->frame_packet.swapchain_image_index = driver->stats.swapchain_image_index;
  driver->frame_packet.has_geometry = vertex_bytes != 0u && index_bytes != 0u;
  driver->stats.upload_bytes = vertex_bytes + index_bytes;
  return true;
}

ZLay_DriverDesc ZLay_DriverDescDefault(ZLay_Backend backend, uint32_t width, uint32_t height) {
  ZLay_DriverDesc desc;
  memset(&desc, 0, sizeof(desc));
  desc.backend = backend;
  desc.surface_kind = ZLAY_SURFACE_KIND_HEADLESS;
  desc.width = width;
  desc.height = height;
  desc.dpi_scale = 1.0f;
  desc.vsync = true;
  desc.low_latency = false;
  desc.frames_in_flight = 0u;
  desc.vertex_capacity = ZLAY_DRIVER_DEFAULT_VERTEX_CAPACITY;
  desc.index_capacity = ZLAY_DRIVER_DEFAULT_INDEX_CAPACITY;
  desc.upload_capacity = (size_t)ZLAY_DRIVER_DEFAULT_UPLOAD_CAPACITY;
  return desc;
}

bool ZLay_DriverInit(ZLay_Driver* driver, const ZLay_DriverDesc* desc) {
  ZLay_DriverDesc resolved;
  ZLay_DriverContextDesc context_desc;
  ZLay_SurfaceDesc surface_desc;
  ZLay_SwapchainDesc swapchain_desc;

  if (!driver) return false;

  resolved = desc ? *desc : ZLay_DriverDescDefault(ZLAY_BACKEND_SOFTWARE, 1u, 1u);
  if (!ZLay_BackendIsSupportedByDriver(resolved.backend)) {
    return false;
  }

  memset(driver, 0, sizeof(*driver));
  driver->backend = resolved.backend;
  driver->backend_user = resolved.backend_user;
  driver->api = ZLay__DriverAPIForBackend(resolved.backend);

  context_desc = ZLay_DriverContextDescDefault(resolved.backend);
  context_desc.surface_kind = resolved.surface_kind;
  context_desc.native_display = resolved.native_display;
  context_desc.native_window = resolved.native_window;
  context_desc.vsync = resolved.vsync;
  context_desc.low_latency = resolved.low_latency;
  if (!ZLay_DriverContextInit(&driver->context, &context_desc)) {
    return false;
  }

  surface_desc = ZLay_SurfaceDescDefault();
  surface_desc.kind = resolved.surface_kind;
  surface_desc.native_display = resolved.native_display;
  surface_desc.native_window = resolved.native_window;
  surface_desc.width = resolved.width;
  surface_desc.height = resolved.height;
  surface_desc.dpi_scale = resolved.dpi_scale;
  if (!ZLay_SurfaceInit(&driver->surface, &surface_desc)) {
    ZLay_DriverContextShutdown(&driver->context);
    return false;
  }

  driver->present = ZLay_PresentConfigDefault();
  driver->present.vsync = resolved.vsync;
  driver->present.low_latency = resolved.low_latency;
  driver->present.frames_in_flight = ZLay_PresentChooseFramesInFlight(resolved.low_latency, resolved.frames_in_flight);
  driver->present.mode = ZLay_PresentChooseMode(resolved.vsync, resolved.low_latency);

  swapchain_desc = ZLay_SwapchainDescFromSurface(&driver->surface, &driver->present);
  if (!ZLay_SwapchainInit(&driver->swapchain, &swapchain_desc)) {
    ZLay_DriverContextShutdown(&driver->context);
    return false;
  }

  ZLay_FrameSchedulerInit(&driver->scheduler, driver->present.frames_in_flight, resolved.low_latency);
  for (uint32_t i = 0u; i < ZLAY_DRIVER_MAX_FRAMES_IN_FLIGHT; ++i) {
    ZLay_FrameResourcesReset(&driver->frame_resources[i]);
  }

  ZLay_DirtyStateInit(&driver->dirty);
  if (!ZLay__DriverEnsureBatchMemory(driver, &resolved)) {
    ZLay__DriverFreeOwnedBuffers(driver);
    ZLay_SwapchainShutdown(&driver->swapchain);
    ZLay_DriverContextShutdown(&driver->context);
    return false;
  }

  if (!ZLay__DriverEnsureUploadMemory(driver, &resolved)) {
    ZLay__DriverFreeOwnedBuffers(driver);
    ZLay_SwapchainShutdown(&driver->swapchain);
    ZLay_DriverContextShutdown(&driver->context);
    return false;
  }

  ZLay_PipelineCacheInit(&driver->pipelines, &driver->context.caps);
  ZLay_PipelineCacheEnsure(&driver->pipelines, ZLAY_PIPELINE_RECT);

  driver->backend_state.backend = resolved.backend;
  driver->backend_state.name = ZLay_BackendName(resolved.backend);
  driver->backend_state.context_created = true;

  driver->stats.frame_index = 0u;
  driver->stats.dirty_flags = driver->dirty.flags;

  if (driver->api.create && !driver->api.create(driver, &resolved)) {
    ZLay__DriverFreeOwnedBuffers(driver);
    ZLay_SwapchainShutdown(&driver->swapchain);
    ZLay_DriverContextShutdown(&driver->context);
    return false;
  }

  driver->initialized = true;
  return true;
}

bool ZLay_DriverCreate(ZLay_Driver** out_driver, const ZLay_DriverDesc* desc) {
  ZLay_Driver* driver;

  if (!out_driver) return false;
  *out_driver = NULL;

  driver = (ZLay_Driver*)calloc(1u, sizeof(ZLay_Driver));
  if (!driver) return false;

  if (!ZLay_DriverInit(driver, desc)) {
    free(driver);
    return false;
  }

  driver->owns_self = true;
  *out_driver = driver;
  return true;
}

void ZLay_DriverShutdown(ZLay_Driver* driver) {
  if (!driver || !driver->initialized) return;

  if (driver->api.destroy) {
    driver->api.destroy(driver);
  }

  if (driver->owns_backend_data) {
    free(driver->backend_data);
    driver->backend_data = NULL;
    driver->owns_backend_data = false;
  }

  ZLay__DriverFreeOwnedBuffers(driver);
  ZLay_PipelineCacheInvalidate(&driver->pipelines);
  ZLay_SwapchainShutdown(&driver->swapchain);
  ZLay_DriverContextShutdown(&driver->context);
  driver->current_frame = NULL;
  driver->initialized = false;
  driver->in_frame = false;
}

void ZLay_DriverDestroy(ZLay_Driver* driver) {
  bool owns_self;

  if (!driver) return;
  owns_self = driver->owns_self;
  ZLay_DriverShutdown(driver);
  if (owns_self) {
    free(driver);
  }
}

void ZLay_DriverResize(ZLay_Driver* driver, uint32_t width, uint32_t height) {
  if (!driver || !driver->initialized) return;

  ZLay_SurfaceResize(&driver->surface, width, height);
  ZLay_SwapchainResize(&driver->swapchain, width, height, driver->surface.dpi_scale);
  ZLay_DriverContextInvalidate(&driver->context);
  ZLay_PipelineCacheInvalidate(&driver->pipelines);
  ZLay_PipelineCacheEnsure(&driver->pipelines, ZLAY_PIPELINE_RECT);
  ZLay_DriverMarkDirty(driver, ZLAY_DIRTY_SURFACE | ZLAY_DIRTY_LAYOUT | ZLAY_DIRTY_PAINT | ZLAY_DIRTY_GPU_RESOURCES);
  if (driver->api.resize) {
    driver->api.resize(driver, width, height);
  }
}

bool ZLay_DriverBeginFrame(ZLay_Driver* driver) {
  ZLay_SwapchainImage* image;
  uint32_t frame_resource_index;

  if (!driver || !driver->initialized || driver->in_frame) return false;

  frame_resource_index = driver->scheduler.current_frame;
  driver->current_frame = ZLay_FrameSchedulerAcquire(
    &driver->scheduler,
    driver->frame_resources,
    driver->present.frames_in_flight
  );
  if (!driver->current_frame) return false;

  image = ZLay_SwapchainAcquireNextImage(&driver->swapchain);
  if (!image) {
    driver->current_frame->in_use = false;
    driver->current_frame = NULL;
    return false;
  }

  driver->frame_start_ms = ZLay__DriverNowMs();
  ZLay_GpuRingBufferReset(&driver->upload_ring);
  ZLay_BatchReset(&driver->batch);
  ZLay__DriverResetFramePacket(driver);
  memset(&driver->stats, 0, sizeof(driver->stats));
  driver->stats.frame_index = driver->scheduler.frame_index;
  driver->stats.dirty_flags = driver->dirty.flags;
  driver->stats.frame_resource_index = frame_resource_index;
  driver->stats.swapchain_image_index = image->index;
  driver->frame_packet.frame_resource_index = frame_resource_index;
  driver->frame_packet.swapchain_image_index = image->index;
  driver->in_frame = true;

  if (driver->api.begin_frame && !driver->api.begin_frame(driver)) {
    image->acquired = false;
    if (driver->current_frame) driver->current_frame->in_use = false;
    driver->current_frame = NULL;
    driver->in_frame = false;
    return false;
  }

  return true;
}

bool ZLay_DriverSubmit(ZLay_Driver* driver, const ZLay_RenderCommandList* commands) {
  ZLay_BatchStats batch_stats;
  bool ok;

  if (!driver || !driver->initialized || !driver->in_frame) return false;

  ok = ZLay_BatchBuildFromCommands(&driver->batch, commands);
  batch_stats = ZLay_BatchGetStats(&driver->batch);

  driver->stats.command_count = commands ? commands->count : 0u;
  driver->stats.draw_calls = batch_stats.draw_calls;
  driver->stats.rect_count = batch_stats.rect_count;
  driver->stats.line_count = batch_stats.line_count;
  driver->stats.text_count = batch_stats.text_count;
  driver->stats.image_count = batch_stats.image_count;
  driver->stats.clip_count = batch_stats.clip_count;
  driver->stats.vertex_count = batch_stats.vertex_count;
  driver->stats.index_count = batch_stats.index_count;
  driver->stats.batch_overflowed = batch_stats.overflowed;
  driver->stats.dirty_flags = driver->dirty.flags;

  if (!ZLay__DriverUploadBatch(driver)) {
    ok = false;
    driver->stats.batch_overflowed = true;
    ZLay_DriverMarkDirty(driver, ZLAY_DIRTY_GPU_RESOURCES);
  } else if (driver->api.submit && !driver->api.submit(driver, commands)) {
    ok = false;
  }

  if (batch_stats.overflowed) {
    ZLay_DriverMarkDirty(driver, ZLAY_DIRTY_GPU_RESOURCES);
  }

  return ok;
}

bool ZLay_DriverEndFrame(ZLay_Driver* driver) {
  if (!driver || !driver->initialized || !driver->in_frame) return false;

  if (driver->api.end_frame && !driver->api.end_frame(driver)) {
    return false;
  }

  driver->stats.cpu_frame_ms = (float)(ZLay__DriverNowMs() - driver->frame_start_ms);
  return true;
}

bool ZLay_DriverPresent(ZLay_Driver* driver) {
  if (!driver || !driver->initialized || !driver->in_frame) return false;

  driver->present_start_ms = ZLay__DriverNowMs();
  if (driver->api.present && !driver->api.present(driver)) {
    return false;
  }
  driver->stats.present_ms = (float)(ZLay__DriverNowMs() - driver->present_start_ms);

  if (driver->current_frame) {
    driver->current_frame->in_use = false;
  }

  ZLay_SwapchainPresent(&driver->swapchain);
  ZLay_FrameSchedulerPresent(&driver->scheduler);
  driver->in_frame = false;
  ZLay_DirtyStateClearAll(&driver->dirty);
  driver->stats.dirty_flags = driver->dirty.flags;
  return true;
}

bool ZLay_DriverRunFrame(ZLay_Driver* driver, const ZLay_RenderCommandList* commands) {
  if (!ZLay_DriverBeginFrame(driver)) return false;
  if (!ZLay_DriverSubmit(driver, commands)) {
    if (driver->current_frame) driver->current_frame->in_use = false;
    ZLay_SwapchainReleaseCurrentImage(&driver->swapchain);
    driver->in_frame = false;
    return false;
  }
  if (!ZLay_DriverEndFrame(driver)) {
    if (driver->current_frame) driver->current_frame->in_use = false;
    ZLay_SwapchainReleaseCurrentImage(&driver->swapchain);
    driver->in_frame = false;
    return false;
  }
  return ZLay_DriverPresent(driver);
}

ZLay_DriverStats ZLay_DriverGetStats(const ZLay_Driver* driver) {
  if (!driver) return (ZLay_DriverStats){0};
  return driver->stats;
}

ZLay_Backend ZLay_DriverGetBackend(const ZLay_Driver* driver) {
  return driver ? driver->backend : ZLAY_BACKEND_NONE;
}

const ZLay_DriverCaps* ZLay_DriverGetCaps(const ZLay_Driver* driver) {
  return driver ? &driver->context.caps : NULL;
}

ZLay_Surface* ZLay_DriverGetSurface(ZLay_Driver* driver) {
  return driver ? &driver->surface : NULL;
}

const ZLay_Surface* ZLay_DriverGetSurfaceConst(const ZLay_Driver* driver) {
  return driver ? &driver->surface : NULL;
}

ZLay_Swapchain* ZLay_DriverGetSwapchain(ZLay_Driver* driver) {
  return driver ? &driver->swapchain : NULL;
}

const ZLay_Swapchain* ZLay_DriverGetSwapchainConst(const ZLay_Driver* driver) {
  return driver ? &driver->swapchain : NULL;
}

ZLay_Batch* ZLay_DriverGetBatch(ZLay_Driver* driver) {
  return driver ? &driver->batch : NULL;
}

const ZLay_Batch* ZLay_DriverGetBatchConst(const ZLay_Driver* driver) {
  return driver ? &driver->batch : NULL;
}

const ZLay_DriverFramePacket* ZLay_DriverGetFramePacket(const ZLay_Driver* driver) {
  return driver ? &driver->frame_packet : NULL;
}

ZLay_DriverBackendState* ZLay_DriverGetBackendState(ZLay_Driver* driver) {
  return driver ? &driver->backend_state : NULL;
}

const ZLay_DriverBackendState* ZLay_DriverGetBackendStateConst(const ZLay_Driver* driver) {
  return driver ? &driver->backend_state : NULL;
}

void ZLay_DriverMarkDirty(ZLay_Driver* driver, uint32_t flags) {
  if (!driver) return;
  ZLay_DirtyStateMark(&driver->dirty, flags);
  driver->stats.dirty_flags = driver->dirty.flags;
}

void ZLay_DriverClearDirty(ZLay_Driver* driver, uint32_t flags) {
  if (!driver) return;
  ZLay_DirtyStateClear(&driver->dirty, flags);
  driver->stats.dirty_flags = driver->dirty.flags;
}

void ZLay_DriverClearAllDirty(ZLay_Driver* driver) {
  if (!driver) return;
  ZLay_DirtyStateClearAll(&driver->dirty);
  driver->stats.dirty_flags = driver->dirty.flags;
}
