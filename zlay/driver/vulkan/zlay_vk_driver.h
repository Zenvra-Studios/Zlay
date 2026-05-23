#ifndef ZLAY_DRIVER_VULKAN_ZLAY_VK_DRIVER_H
#define ZLAY_DRIVER_VULKAN_ZLAY_VK_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <driver/vulkan/zlay_vk_buffer.h>
#include <driver/vulkan/zlay_vk_command.h>
#include <driver/vulkan/zlay_vk_device.h>
#include <driver/vulkan/zlay_vk_instance.h>
#include <driver/vulkan/zlay_vk_pipeline.h>
#include <driver/vulkan/zlay_vk_swapchain.h>
#include <driver/vulkan/zlay_vk_sync.h>
#include <driver/zlay_driver.h>
#include <renderer/zlay_renderer.h>
#include <zlay_antialiasing.h>
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

typedef struct ZLay_Vulkan_DrawBatch {
  uint32_t rect_count;
  uint32_t text_count;
  uint32_t clip_count;
  uint32_t vertex_count;
  uint32_t index_count;
} ZLay_Vulkan_DrawBatch;

typedef struct ZLay_VulkanRenderer {
  ZLay_Vulkan_DrawBatch last_batch;
  ZLay_RendererFrameInfo last_frame;
  ZLay_ShaderSource rect_vertex_shader;
  ZLay_ShaderSource rect_fragment_shader;
} ZLay_VulkanRenderer;

typedef struct ZLay_VulkanRendererInitInfo {
  void* instance;
  void* physical_device;
  void* device;
  void* queue;
  uint32_t queue_family_index;
} ZLay_VulkanRendererInitInfo;

ZLAY_API ZLay_Vulkan_DrawBatch ZLay_Vulkan_BuildBatches(const ZLay_RenderCommandList* list);
ZLAY_API bool ZLay_VulkanRenderer_Init(ZLay_VulkanRenderer* renderer, const ZLay_VulkanRendererInitInfo* info);
ZLAY_API ZLay_Renderer ZLay_VulkanRenderer_AsRenderer(ZLay_VulkanRenderer* renderer);
ZLAY_API ZLay_DriverAPI ZLay_VulkanDriverAPI(void);

#ifdef __cplusplus
}
#endif

#endif
