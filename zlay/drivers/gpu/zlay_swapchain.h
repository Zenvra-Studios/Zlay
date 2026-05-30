#ifndef ZLAY_DRIVERS_GPU_ZLAY_SWAPCHAIN_H
#define ZLAY_DRIVERS_GPU_ZLAY_SWAPCHAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <zlay.h>
#include <drivers/gpu/zlay_frame.h>
#include <drivers/gpu/zlay_present.h>
#include <drivers/gpu/zlay_surface.h>

typedef enum ZLay_SurfaceFormat {
  ZLAY_SURFACE_FORMAT_UNKNOWN = 0,
  ZLAY_SURFACE_FORMAT_RGBA8_UNORM = 1,
  ZLAY_SURFACE_FORMAT_BGRA8_UNORM = 2
} ZLay_SurfaceFormat;

typedef struct ZLay_SwapchainDesc {
  uint32_t width;
  uint32_t height;
  uint32_t image_count;
  float dpi_scale;
  ZLay_SurfaceFormat format;
  ZLay_PresentMode present_mode;
} ZLay_SwapchainDesc;

typedef struct ZLay_SwapchainImage {
  uint32_t index;
  void* native_image;
  void* framebuffer;
  bool acquired;
} ZLay_SwapchainImage;

typedef struct ZLay_Swapchain {
  ZLay_SwapchainDesc desc;
  ZLay_SwapchainImage images[ZLAY_DRIVER_MAX_FRAMES_IN_FLIGHT];
  uint32_t current_image;
  uint64_t generation;
  bool valid;
  bool needs_rebuild;
} ZLay_Swapchain;

ZLAY_API ZLay_SwapchainDesc ZLay_SwapchainDescFromSurface(
  const ZLay_Surface* surface,
  const ZLay_PresentConfig* present
);
ZLAY_API bool ZLay_SwapchainInit(ZLay_Swapchain* swapchain, const ZLay_SwapchainDesc* desc);
ZLAY_API void ZLay_SwapchainShutdown(ZLay_Swapchain* swapchain);
ZLAY_API void ZLay_SwapchainResize(ZLay_Swapchain* swapchain, uint32_t width, uint32_t height, float dpi_scale);
ZLAY_API ZLay_SwapchainImage* ZLay_SwapchainAcquireNextImage(ZLay_Swapchain* swapchain);
ZLAY_API void ZLay_SwapchainReleaseCurrentImage(ZLay_Swapchain* swapchain);
ZLAY_API void ZLay_SwapchainPresent(ZLay_Swapchain* swapchain);
ZLAY_API void ZLay_SwapchainInvalidate(ZLay_Swapchain* swapchain);
ZLAY_API const char* ZLay_SurfaceFormatName(ZLay_SurfaceFormat format);

#ifdef __cplusplus
}
#endif

#endif
