#ifndef ZLAY_DRIVERS_GPU_VULKAN_ZLAY_VK_DEVICE_H
#define ZLAY_DRIVERS_GPU_VULKAN_ZLAY_VK_DEVICE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <drivers/gpu/vulkan/zlay_vk_instance.h>

typedef struct ZLay_VKDevice {
  void* physical_device;
  void* device;
  void* queue;
  uint32_t queue_family_index;
  bool valid;
  uint64_t generation;
} ZLay_VKDevice;

ZLAY_API bool ZLay_VKDevice_Init(ZLay_VKDevice* device, const ZLay_VKInstance* instance);
ZLAY_API void ZLay_VKDevice_Shutdown(ZLay_VKDevice* device);

#ifdef __cplusplus
}
#endif

#endif
