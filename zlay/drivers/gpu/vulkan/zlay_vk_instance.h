#ifndef ZLAY_DRIVERS_GPU_VULKAN_ZLAY_VK_INSTANCE_H
#define ZLAY_DRIVERS_GPU_VULKAN_ZLAY_VK_INSTANCE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <drivers/gpu/zlay_driver.h>

typedef struct ZLay_VKInstance {
  void* native_instance;
  bool owns_instance;
  bool valid;
  uint64_t generation;
} ZLay_VKInstance;

ZLAY_API bool ZLay_VKInstance_Init(ZLay_VKInstance* instance, const ZLay_DriverDesc* desc);
ZLAY_API void ZLay_VKInstance_Shutdown(ZLay_VKInstance* instance);

#ifdef __cplusplus
}
#endif

#endif
