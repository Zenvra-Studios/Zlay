#include <drivers/gpu/zlay_swapchain.h>

#include <string.h>

static uint32_t ZLay__SwapchainClampImageCount(uint32_t image_count) {
  if (image_count == 0u) return 1u;
  if (image_count > ZLAY_DRIVER_MAX_FRAMES_IN_FLIGHT) return ZLAY_DRIVER_MAX_FRAMES_IN_FLIGHT;
  return image_count;
}

ZLay_SwapchainDesc ZLay_SwapchainDescFromSurface(
  const ZLay_Surface* surface,
  const ZLay_PresentConfig* present
) {
  ZLay_SwapchainDesc desc;
  memset(&desc, 0, sizeof(desc));
  desc.width = surface ? surface->width : 1u;
  desc.height = surface ? surface->height : 1u;
  desc.dpi_scale = surface && surface->dpi_scale > 0.0f ? surface->dpi_scale : 1.0f;
  desc.image_count = present ? ZLay__SwapchainClampImageCount(present->frames_in_flight) : 2u;
  desc.format = ZLAY_SURFACE_FORMAT_RGBA8_UNORM;
  desc.present_mode = present ? present->mode : ZLAY_PRESENT_MODE_FIFO;
  return desc;
}

bool ZLay_SwapchainInit(ZLay_Swapchain* swapchain, const ZLay_SwapchainDesc* desc) {
  ZLay_SwapchainDesc resolved;

  if (!swapchain) return false;

  resolved = desc ? *desc : (ZLay_SwapchainDesc){0};
  resolved.width = resolved.width != 0u ? resolved.width : 1u;
  resolved.height = resolved.height != 0u ? resolved.height : 1u;
  resolved.dpi_scale = resolved.dpi_scale > 0.0f ? resolved.dpi_scale : 1.0f;
  resolved.image_count = ZLay__SwapchainClampImageCount(resolved.image_count);
  if (resolved.format == ZLAY_SURFACE_FORMAT_UNKNOWN) {
    resolved.format = ZLAY_SURFACE_FORMAT_RGBA8_UNORM;
  }

  memset(swapchain, 0, sizeof(*swapchain));
  swapchain->desc = resolved;
  swapchain->generation = 1u;
  swapchain->valid = true;
  swapchain->needs_rebuild = false;

  for (uint32_t i = 0u; i < resolved.image_count; ++i) {
    swapchain->images[i].index = i;
  }

  return true;
}

void ZLay_SwapchainShutdown(ZLay_Swapchain* swapchain) {
  if (!swapchain) return;
  swapchain->valid = false;
  swapchain->needs_rebuild = false;
  swapchain->generation++;
}

void ZLay_SwapchainResize(ZLay_Swapchain* swapchain, uint32_t width, uint32_t height, float dpi_scale) {
  if (!swapchain) return;
  swapchain->desc.width = width != 0u ? width : 1u;
  swapchain->desc.height = height != 0u ? height : 1u;
  swapchain->desc.dpi_scale = dpi_scale > 0.0f ? dpi_scale : 1.0f;
  swapchain->generation++;
  swapchain->valid = true;
  swapchain->needs_rebuild = false;
}

ZLay_SwapchainImage* ZLay_SwapchainAcquireNextImage(ZLay_Swapchain* swapchain) {
  ZLay_SwapchainImage* image;

  if (!swapchain || !swapchain->valid || swapchain->desc.image_count == 0u) return NULL;

  if (swapchain->current_image >= swapchain->desc.image_count) {
    swapchain->current_image = 0u;
  }

  image = &swapchain->images[swapchain->current_image];
  image->acquired = true;
  return image;
}

void ZLay_SwapchainReleaseCurrentImage(ZLay_Swapchain* swapchain) {
  if (!swapchain || !swapchain->valid || swapchain->desc.image_count == 0u) return;
  if (swapchain->current_image >= swapchain->desc.image_count) return;
  swapchain->images[swapchain->current_image].acquired = false;
}

void ZLay_SwapchainPresent(ZLay_Swapchain* swapchain) {
  ZLay_SwapchainImage* image;

  if (!swapchain || !swapchain->valid || swapchain->desc.image_count == 0u) return;

  image = &swapchain->images[swapchain->current_image];
  image->acquired = false;
  swapchain->current_image = (swapchain->current_image + 1u) % swapchain->desc.image_count;
}

void ZLay_SwapchainInvalidate(ZLay_Swapchain* swapchain) {
  if (!swapchain) return;
  swapchain->valid = false;
  swapchain->needs_rebuild = true;
  swapchain->generation++;
}

const char* ZLay_SurfaceFormatName(ZLay_SurfaceFormat format) {
  switch (format) {
    case ZLAY_SURFACE_FORMAT_RGBA8_UNORM: return "RGBA8_UNORM";
    case ZLAY_SURFACE_FORMAT_BGRA8_UNORM: return "BGRA8_UNORM";
    case ZLAY_SURFACE_FORMAT_UNKNOWN:
    default: return "Unknown";
  }
}
