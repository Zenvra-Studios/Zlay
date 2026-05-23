#ifndef ZLAY_DRIVER_ZLAY_DRIVER_H
#define ZLAY_DRIVER_ZLAY_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <zlay.h>
#include <driver/zlay_backend.h>
#include <driver/zlay_batch.h>
#include <driver/zlay_context.h>
#include <driver/zlay_frame.h>
#include <driver/zlay_gpu.h>
#include <driver/zlay_pipeline.h>
#include <driver/zlay_present.h>
#include <driver/zlay_surface.h>
#include <driver/zlay_swapchain.h>

#ifndef ZLAY_DRIVER_DEFAULT_VERTEX_CAPACITY
#  define ZLAY_DRIVER_DEFAULT_VERTEX_CAPACITY 8192u
#endif

#ifndef ZLAY_DRIVER_DEFAULT_INDEX_CAPACITY
#  define ZLAY_DRIVER_DEFAULT_INDEX_CAPACITY 12288u
#endif

#ifndef ZLAY_DRIVER_DEFAULT_UPLOAD_CAPACITY
#  define ZLAY_DRIVER_DEFAULT_UPLOAD_CAPACITY (1024u * 1024u)
#endif

typedef struct ZLay_Driver ZLay_Driver;

typedef struct ZLay_DriverDesc {
  ZLay_Backend backend;
  ZLay_SurfaceKind surface_kind;
  void* native_display;
  void* native_window;
  uint32_t width;
  uint32_t height;
  float dpi_scale;
  bool vsync;
  bool low_latency;
  uint32_t frames_in_flight;
  ZLay_Vertex* vertices;
  uint32_t vertex_capacity;
  uint32_t* indices;
  uint32_t index_capacity;
  void* upload_memory;
  size_t upload_capacity;
  void* backend_user;
} ZLay_DriverDesc;

typedef struct ZLay_DriverFramePacket {
  const ZLay_Batch* batch;
  ZLay_FrameResources* resources;
  void* vertex_upload;
  void* index_upload;
  size_t vertex_upload_size;
  size_t index_upload_size;
  uint32_t frame_resource_index;
  uint32_t swapchain_image_index;
  bool has_geometry;
} ZLay_DriverFramePacket;

typedef struct ZLay_DriverBackendState {
  ZLay_Backend backend;
  const char* name;
  uint64_t frame_index;
  uint64_t submitted_frame_index;
  uint64_t presented_frame_index;
  uint32_t swapchain_generation;
  uint32_t swapchain_image_index;
  uint32_t draw_calls;
  uint32_t vertex_count;
  uint32_t index_count;
  size_t uploaded_bytes;
  bool context_created;
  bool frame_open;
  bool submitted;
  bool presented;
} ZLay_DriverBackendState;

typedef struct ZLay_DriverStats {
  uint64_t frame_index;
  uint32_t command_count;
  uint32_t draw_calls;
  uint32_t rect_count;
  uint32_t text_count;
  uint32_t clip_count;
  uint32_t vertex_count;
  uint32_t index_count;
  uint32_t dirty_flags;
  uint32_t frame_resource_index;
  uint32_t swapchain_image_index;
  size_t upload_bytes;
  bool batch_overflowed;
  float cpu_frame_ms;
  float gpu_frame_ms;
  float present_ms;
} ZLay_DriverStats;

typedef bool (*ZLay_DriverCreateFn)(ZLay_Driver* driver, const ZLay_DriverDesc* desc);
typedef void (*ZLay_DriverDestroyFn)(ZLay_Driver* driver);
typedef void (*ZLay_DriverResizeFn)(ZLay_Driver* driver, uint32_t width, uint32_t height);
typedef bool (*ZLay_DriverBeginFrameFn)(ZLay_Driver* driver);
typedef bool (*ZLay_DriverSubmitFn)(ZLay_Driver* driver, const ZLay_RenderCommandList* commands);
typedef bool (*ZLay_DriverEndFrameFn)(ZLay_Driver* driver);
typedef bool (*ZLay_DriverPresentFn)(ZLay_Driver* driver);

typedef struct ZLay_DriverAPI {
  ZLay_DriverCreateFn create;
  ZLay_DriverDestroyFn destroy;
  ZLay_DriverResizeFn resize;
  ZLay_DriverBeginFrameFn begin_frame;
  ZLay_DriverSubmitFn submit;
  ZLay_DriverEndFrameFn end_frame;
  ZLay_DriverPresentFn present;
} ZLay_DriverAPI;

struct ZLay_Driver {
  ZLay_Backend backend;
  ZLay_DriverContext context;
  ZLay_Surface surface;
  ZLay_Swapchain swapchain;
  ZLay_PresentConfig present;
  ZLay_FrameScheduler scheduler;
  ZLay_FrameResources frame_resources[ZLAY_DRIVER_MAX_FRAMES_IN_FLIGHT];
  ZLay_FrameResources* current_frame;
  ZLay_DirtyState dirty;
  ZLay_Batch batch;
  ZLay_PipelineCache pipelines;
  ZLay_GpuRingBuffer upload_ring;
  ZLay_DriverFramePacket frame_packet;
  ZLay_DriverBackendState backend_state;
  ZLay_DriverStats stats;
  ZLay_DriverAPI api;
  void* backend_data;
  void* upload_memory;
  void* backend_user;
  bool owns_self;
  bool owns_backend_data;
  bool owns_vertices;
  bool owns_indices;
  bool owns_upload_memory;
  bool initialized;
  bool in_frame;
  double frame_start_ms;
  double present_start_ms;
};

ZLAY_API ZLay_DriverDesc ZLay_DriverDescDefault(ZLay_Backend backend, uint32_t width, uint32_t height);
ZLAY_API bool ZLay_DriverInit(ZLay_Driver* driver, const ZLay_DriverDesc* desc);
ZLAY_API bool ZLay_DriverCreate(ZLay_Driver** out_driver, const ZLay_DriverDesc* desc);
ZLAY_API void ZLay_DriverShutdown(ZLay_Driver* driver);
ZLAY_API void ZLay_DriverDestroy(ZLay_Driver* driver);
ZLAY_API void ZLay_DriverResize(ZLay_Driver* driver, uint32_t width, uint32_t height);
ZLAY_API bool ZLay_DriverBeginFrame(ZLay_Driver* driver);
ZLAY_API bool ZLay_DriverSubmit(ZLay_Driver* driver, const ZLay_RenderCommandList* commands);
ZLAY_API bool ZLay_DriverEndFrame(ZLay_Driver* driver);
ZLAY_API bool ZLay_DriverPresent(ZLay_Driver* driver);
ZLAY_API bool ZLay_DriverRunFrame(ZLay_Driver* driver, const ZLay_RenderCommandList* commands);
ZLAY_API ZLay_DriverStats ZLay_DriverGetStats(const ZLay_Driver* driver);
ZLAY_API ZLay_Backend ZLay_DriverGetBackend(const ZLay_Driver* driver);
ZLAY_API const ZLay_DriverCaps* ZLay_DriverGetCaps(const ZLay_Driver* driver);
ZLAY_API ZLay_Surface* ZLay_DriverGetSurface(ZLay_Driver* driver);
ZLAY_API const ZLay_Surface* ZLay_DriverGetSurfaceConst(const ZLay_Driver* driver);
ZLAY_API ZLay_Swapchain* ZLay_DriverGetSwapchain(ZLay_Driver* driver);
ZLAY_API const ZLay_Swapchain* ZLay_DriverGetSwapchainConst(const ZLay_Driver* driver);
ZLAY_API ZLay_Batch* ZLay_DriverGetBatch(ZLay_Driver* driver);
ZLAY_API const ZLay_Batch* ZLay_DriverGetBatchConst(const ZLay_Driver* driver);
ZLAY_API const ZLay_DriverFramePacket* ZLay_DriverGetFramePacket(const ZLay_Driver* driver);
ZLAY_API ZLay_DriverBackendState* ZLay_DriverGetBackendState(ZLay_Driver* driver);
ZLAY_API const ZLay_DriverBackendState* ZLay_DriverGetBackendStateConst(const ZLay_Driver* driver);
ZLAY_API void ZLay_DriverMarkDirty(ZLay_Driver* driver, uint32_t flags);
ZLAY_API void ZLay_DriverClearDirty(ZLay_Driver* driver, uint32_t flags);
ZLAY_API void ZLay_DriverClearAllDirty(ZLay_Driver* driver);

#ifdef __cplusplus
}
#endif

#endif
