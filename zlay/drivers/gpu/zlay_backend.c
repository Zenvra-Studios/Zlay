#include <drivers/gpu/zlay_backend.h>

const char* ZLay_BackendName(ZLay_Backend backend) {
  switch (backend) {
    case ZLAY_BACKEND_OPENGL: return "OpenGL";
    case ZLAY_BACKEND_VULKAN: return "Vulkan";
    case ZLAY_BACKEND_METAL: return "Metal";
    case ZLAY_BACKEND_D3D11: return "D3D11";
    case ZLAY_BACKEND_D3D12: return "D3D12";
    case ZLAY_BACKEND_WEBGPU: return "WebGPU";
    case ZLAY_BACKEND_SOFTWARE: return "Software";
    case ZLAY_BACKEND_NONE:
    default: return "None";
  }
}

bool ZLay_BackendIsGpu(ZLay_Backend backend) {
  return backend == ZLAY_BACKEND_OPENGL ||
         backend == ZLAY_BACKEND_VULKAN ||
         backend == ZLAY_BACKEND_METAL ||
         backend == ZLAY_BACKEND_D3D11 ||
         backend == ZLAY_BACKEND_D3D12 ||
         backend == ZLAY_BACKEND_WEBGPU;
}

bool ZLay_BackendIsSupportedByDriver(ZLay_Backend backend) {
  if (backend == ZLAY_BACKEND_SOFTWARE) {
    return true;
  }
#if defined(ZLAY_ENABLE_DRIVER_OPENGL)
  if (backend == ZLAY_BACKEND_OPENGL) {
    return true;
  }
#endif
#if defined(ZLAY_ENABLE_DRIVER_VULKAN)
  if (backend == ZLAY_BACKEND_VULKAN) {
    return true;
  }
#endif
  return false;
}
