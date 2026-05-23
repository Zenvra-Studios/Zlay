#include <driver/zlay_context.h>

#include <string.h>

ZLay_DriverContextDesc ZLay_DriverContextDescDefault(ZLay_Backend backend) {
  ZLay_DriverContextDesc desc;
  memset(&desc, 0, sizeof(desc));
  desc.backend = backend;
  desc.surface_kind = ZLAY_SURFACE_KIND_HEADLESS;
  desc.vsync = true;
  return desc;
}

ZLay_DriverCaps ZLay_DriverCapsDefault(ZLay_Backend backend, bool low_latency) {
  ZLay_DriverCaps caps;
  memset(&caps, 0, sizeof(caps));
  caps.backend = backend;
  caps.backend_name = ZLay_BackendName(backend);
  caps.max_frames_in_flight = low_latency ? 1u : 2u;
  caps.max_texture_size = 4096u;
  caps.uniform_alignment = 256u;
  caps.features = ZLAY_DRIVER_FEATURE_CONTEXT |
                  ZLAY_DRIVER_FEATURE_PERSISTENT_BUFFERS |
                  ZLAY_DRIVER_FEATURE_INDEXED_DRAW |
                  ZLAY_DRIVER_FEATURE_SCISSOR_CLIP;

  if (ZLay_BackendIsGpu(backend)) {
    caps.features |= ZLAY_DRIVER_FEATURE_SWAPCHAIN;
  }

  if (low_latency) {
    caps.features |= ZLAY_DRIVER_FEATURE_LOW_LATENCY;
  }

  if (backend == ZLAY_BACKEND_SOFTWARE) {
    caps.max_texture_size = 2048u;
  }

  return caps;
}

bool ZLay_DriverContextInit(ZLay_DriverContext* context, const ZLay_DriverContextDesc* desc) {
  ZLay_DriverContextDesc resolved;

  if (!context) return false;

  resolved = desc ? *desc : ZLay_DriverContextDescDefault(ZLAY_BACKEND_SOFTWARE);
  if (!ZLay_BackendIsSupportedByDriver(resolved.backend)) return false;

  memset(context, 0, sizeof(*context));
  context->desc = resolved;
  context->caps = ZLay_DriverCapsDefault(resolved.backend, resolved.low_latency);
  context->generation = 1u;
  context->valid = true;
  return true;
}

void ZLay_DriverContextShutdown(ZLay_DriverContext* context) {
  if (!context) return;
  context->valid = false;
  context->generation++;
}

void ZLay_DriverContextInvalidate(ZLay_DriverContext* context) {
  if (!context) return;
  context->generation++;
}

bool ZLay_DriverContextSupports(const ZLay_DriverContext* context, uint32_t features) {
  return context != NULL && context->valid && (context->caps.features & features) == features;
}
