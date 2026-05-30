#ifndef ZLAY_DRIVERS_GPU_VULKAN_ZLAY_VK_DRIVER_H
#define ZLAY_DRIVERS_GPU_VULKAN_ZLAY_VK_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <drivers/gpu/vulkan/zlay_vk_buffer.h>
#include <drivers/gpu/vulkan/zlay_vk_command.h>
#include <drivers/gpu/vulkan/zlay_vk_device.h>
#include <drivers/gpu/vulkan/zlay_vk_instance.h>
#include <drivers/gpu/vulkan/zlay_vk_pipeline.h>
#include <drivers/gpu/vulkan/zlay_vk_swapchain.h>
#include <drivers/gpu/vulkan/zlay_vk_sync.h>
#include <drivers/gpu/zlay_driver.h>
#include <zlay.h>

typedef struct ZLay_VKDriverState {
  ZLay_VKInstance instance;
  ZLay_VKDevice device;
  ZLay_VKSwapchain swapchain;
  ZLay_VKPipeline rect_pipeline;
  ZLay_VKBuffer vertex_buffer;
  ZLay_VKBuffer index_buffer;
  ZLay_VKBuffer uniform_buffer;
  ZLay_VKCommand command;
  ZLay_VKSync sync;
} ZLay_VKDriverState;

ZLAY_API ZLay_DriverAPI ZLay_VulkanDriverAPI(void);

#ifdef __cplusplus
}
#endif

#endif
