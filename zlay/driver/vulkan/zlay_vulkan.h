#ifndef ZLAY_ZLAY_VULKAN_H
#define ZLAY_ZLAY_VULKAN_H

#ifdef __cplusplus
extern "C" {
#endif

#include "zlay.h"
#include "renderer/zlay_renderer.h"
#include "zlay_antialiasing.h"

// Vulkan driver is intentionally kept as a thin translation layer.
// Real VkDevice/VkPipeline integration will live in app/tool code.

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

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ZLAY_ZLAY_VULKAN_H
