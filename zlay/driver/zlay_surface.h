#ifndef ZLAY_DRIVER_ZLAY_SURFACE_H
#define ZLAY_DRIVER_ZLAY_SURFACE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <zlay.h>

typedef enum ZLay_SurfaceKind {
  ZLAY_SURFACE_KIND_UNKNOWN = 0,
  ZLAY_SURFACE_KIND_HEADLESS = 1,
  ZLAY_SURFACE_KIND_X11 = 2,
  ZLAY_SURFACE_KIND_WAYLAND = 3,
  ZLAY_SURFACE_KIND_WIN32 = 4,
  ZLAY_SURFACE_KIND_WINRT = 5,
  ZLAY_SURFACE_KIND_COCOA = 6
} ZLay_SurfaceKind;

typedef struct ZLay_SurfaceDesc {
  ZLay_SurfaceKind kind;
  void* native_display;
  void* native_window;
  uint32_t width;
  uint32_t height;
  float dpi_scale;
} ZLay_SurfaceDesc;

typedef struct ZLay_Surface {
  ZLay_SurfaceKind kind;
  void* native_display;
  void* native_window;
  uint32_t width;
  uint32_t height;
  float dpi_scale;
  bool valid;
} ZLay_Surface;

ZLAY_API ZLay_SurfaceDesc ZLay_SurfaceDescDefault(void);
ZLAY_API bool ZLay_SurfaceInit(ZLay_Surface* surface, const ZLay_SurfaceDesc* desc);
ZLAY_API void ZLay_SurfaceResize(ZLay_Surface* surface, uint32_t width, uint32_t height);
ZLAY_API ZLay_Dimensions ZLay_SurfaceDimensions(const ZLay_Surface* surface);
ZLAY_API float ZLay_SurfaceAspectRatio(const ZLay_Surface* surface);

#ifdef __cplusplus
}
#endif

#endif
