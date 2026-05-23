#ifndef ZLAY_DRIVER_ZLAY_BACKEND_H
#define ZLAY_DRIVER_ZLAY_BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <zlay.h>

typedef enum ZLay_Backend {
  ZLAY_BACKEND_NONE = 0,
  ZLAY_BACKEND_OPENGL = 1,
  ZLAY_BACKEND_VULKAN = 2,
  ZLAY_BACKEND_METAL = 3,
  ZLAY_BACKEND_D3D11 = 4,
  ZLAY_BACKEND_D3D12 = 5,
  ZLAY_BACKEND_WEBGPU = 6,
  ZLAY_BACKEND_SOFTWARE = 7
} ZLay_Backend;

ZLAY_API const char* ZLay_BackendName(ZLay_Backend backend);
ZLAY_API bool ZLay_BackendIsGpu(ZLay_Backend backend);
ZLAY_API bool ZLay_BackendIsSupportedByDriver(ZLay_Backend backend);

#ifdef __cplusplus
}
#endif

#endif
