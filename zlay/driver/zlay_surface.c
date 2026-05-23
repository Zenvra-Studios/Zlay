#include <driver/zlay_surface.h>

#include <string.h>

ZLay_SurfaceDesc ZLay_SurfaceDescDefault(void) {
  ZLay_SurfaceDesc desc;
  memset(&desc, 0, sizeof(desc));
  desc.kind = ZLAY_SURFACE_KIND_UNKNOWN;
  desc.dpi_scale = 1.0f;
  return desc;
}

bool ZLay_SurfaceInit(ZLay_Surface* surface, const ZLay_SurfaceDesc* desc) {
  ZLay_SurfaceDesc resolved;

  if (surface == NULL) {
    return false;
  }

  resolved = desc != NULL ? *desc : ZLay_SurfaceDescDefault();
  memset(surface, 0, sizeof(*surface));
  surface->kind = resolved.kind;
  surface->native_display = resolved.native_display;
  surface->native_window = resolved.native_window;
  surface->width = resolved.width;
  surface->height = resolved.height;
  surface->dpi_scale = resolved.dpi_scale > 0.0f ? resolved.dpi_scale : 1.0f;
  surface->valid = surface->width > 0u && surface->height > 0u;
  return surface->valid || surface->kind == ZLAY_SURFACE_KIND_HEADLESS;
}

void ZLay_SurfaceResize(ZLay_Surface* surface, uint32_t width, uint32_t height) {
  if (surface == NULL) {
    return;
  }

  surface->width = width;
  surface->height = height;
  surface->valid = width > 0u && height > 0u;
}

ZLay_Dimensions ZLay_SurfaceDimensions(const ZLay_Surface* surface) {
  if (surface == NULL) {
    return (ZLay_Dimensions){0.0f, 0.0f};
  }

  return (ZLay_Dimensions){(float)surface->width, (float)surface->height};
}

float ZLay_SurfaceAspectRatio(const ZLay_Surface* surface) {
  if (surface == NULL || surface->height == 0u) {
    return 0.0f;
  }

  return (float)surface->width / (float)surface->height;
}
