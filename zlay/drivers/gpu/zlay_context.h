#ifndef ZLAY_DRIVERS_GPU_ZLAY_CONTEXT_H
#define ZLAY_DRIVERS_GPU_ZLAY_CONTEXT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <zlay.h>
#include <drivers/gpu/zlay_backend.h>
#include <drivers/gpu/zlay_surface.h>

typedef enum ZLay_DriverFeatureFlags {
  ZLAY_DRIVER_FEATURE_NONE = 0,
  ZLAY_DRIVER_FEATURE_CONTEXT = 1u << 0,
  ZLAY_DRIVER_FEATURE_SWAPCHAIN = 1u << 1,
  ZLAY_DRIVER_FEATURE_PERSISTENT_BUFFERS = 1u << 2,
  ZLAY_DRIVER_FEATURE_INDEXED_DRAW = 1u << 3,
  ZLAY_DRIVER_FEATURE_SCISSOR_CLIP = 1u << 4,
  ZLAY_DRIVER_FEATURE_TEXT_ATLAS = 1u << 5,
  ZLAY_DRIVER_FEATURE_LOW_LATENCY = 1u << 6,
  ZLAY_DRIVER_FEATURE_GPU_TIMESTAMPS = 1u << 7
} ZLay_DriverFeatureFlags;

typedef struct ZLay_DriverCaps {
  ZLay_Backend backend;
  uint32_t features;
  uint32_t max_frames_in_flight;
  uint32_t max_texture_size;
  uint32_t uniform_alignment;
  const char* backend_name;
} ZLay_DriverCaps;

typedef struct ZLay_DriverContextDesc {
  ZLay_Backend backend;
  ZLay_SurfaceKind surface_kind;
  void* native_display;
  void* native_window;
  bool vsync;
  bool low_latency;
} ZLay_DriverContextDesc;

typedef struct ZLay_DriverContext {
  ZLay_DriverContextDesc desc;
  ZLay_DriverCaps caps;
  uint64_t generation;
  bool valid;
} ZLay_DriverContext;

ZLAY_API ZLay_DriverContextDesc ZLay_DriverContextDescDefault(ZLay_Backend backend);
ZLAY_API ZLay_DriverCaps ZLay_DriverCapsDefault(ZLay_Backend backend, bool low_latency);
ZLAY_API bool ZLay_DriverContextInit(ZLay_DriverContext* context, const ZLay_DriverContextDesc* desc);
ZLAY_API void ZLay_DriverContextShutdown(ZLay_DriverContext* context);
ZLAY_API void ZLay_DriverContextInvalidate(ZLay_DriverContext* context);
ZLAY_API bool ZLay_DriverContextSupports(const ZLay_DriverContext* context, uint32_t features);

#ifdef __cplusplus
}
#endif

#endif
