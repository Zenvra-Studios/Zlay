#ifndef ZLAY_DRIVERS_GPU_VULKAN_ZLAY_VK_SWAPCHAIN_H
#define ZLAY_DRIVERS_GPU_VULKAN_ZLAY_VK_SWAPCHAIN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <drivers/gpu/zlay_swapchain.h>
#include <drivers/gpu/vulkan/zlay_vk_device.h>

typedef struct ZLay_VKSwapchain {
  ZLay_SwapchainDesc desc;
  uint32_t current_image;
  bool valid;
  uint64_t generation;
} ZLay_VKSwapchain;

ZLAY_API bool ZLay_VKSwapchain_Init(ZLay_VKSwapchain* swapchain, const ZLay_VKDevice* device, const ZLay_SwapchainDesc* desc);
ZLAY_API void ZLay_VKSwapchain_Resize(ZLay_VKSwapchain* swapchain, uint32_t width, uint32_t height, float dpi_scale);
ZLAY_API uint32_t ZLay_VKSwapchain_Acquire(ZLay_VKSwapchain* swapchain);
ZLAY_API void ZLay_VKSwapchain_Present(ZLay_VKSwapchain* swapchain);
ZLAY_API void ZLay_VKSwapchain_Shutdown(ZLay_VKSwapchain* swapchain);

#ifdef __cplusplus
}
#endif

#endif
