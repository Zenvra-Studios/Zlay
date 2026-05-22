#include "zlay_vulkan.h"

ZLay_Vulkan_DrawBatch ZLay_Vulkan_BuildBatches(const ZLay_RenderCommandList* list) {
  ZLay_Vulkan_DrawBatch batch = {0};
  if (!list || !list->commands) return batch;

  for (uint32_t i = 0; i < list->count; ++i) {
    const ZLay_RenderCommand* cmd = &list->commands[i];
    switch (cmd->type) {
      case ZLAY_CMD_RECT:
        batch.rect_count++;
        batch.vertex_count += 4u;
        batch.index_count += 6u;
        break;
      case ZLAY_CMD_TEXT:
        batch.text_count++;
        break;
      case ZLAY_CMD_CLIP_BEGIN:
      case ZLAY_CMD_CLIP_END:
        batch.clip_count++;
        break;
      default:
        break;
    }
  }

  return batch;
}

static bool ZLay_VulkanRenderer_BeginFrame(void* user, const ZLay_RendererFrameInfo* frame) {
  ZLay_VulkanRenderer* renderer = (ZLay_VulkanRenderer*)user;
  if (!renderer) return false;
  renderer->last_batch = (ZLay_Vulkan_DrawBatch){0};
  renderer->last_frame = frame ? *frame : (ZLay_RendererFrameInfo){0};
  return true;
}

static bool ZLay_VulkanRenderer_Render(void* user, const ZLay_RenderCommandList* commands, ZLay_RendererStats* stats) {
  ZLay_VulkanRenderer* renderer = (ZLay_VulkanRenderer*)user;
  if (!renderer || !commands) return false;

  renderer->last_batch = ZLay_Vulkan_BuildBatches(commands);
  if (stats) {
    stats->command_count = commands->count;
    stats->draw_call_count = renderer->last_batch.rect_count > 0 ? 1u : 0u;
    stats->vertex_count = renderer->last_batch.vertex_count;
    stats->index_count = renderer->last_batch.index_count;
  }
  return true;
}

static void ZLay_VulkanRenderer_EndFrame(void* user) {
  (void)user;
}

static void ZLay_VulkanRenderer_Shutdown(void* user) {
  ZLay_VulkanRenderer* renderer = (ZLay_VulkanRenderer*)user;
  if (!renderer) return;
  renderer->last_batch = (ZLay_Vulkan_DrawBatch){0};
  renderer->last_frame = (ZLay_RendererFrameInfo){0};
  renderer->rect_vertex_shader = (ZLay_ShaderSource){0};
  renderer->rect_fragment_shader = (ZLay_ShaderSource){0};
}

bool ZLay_VulkanRenderer_Init(ZLay_VulkanRenderer* renderer, const ZLay_VulkanRendererInitInfo* info) {
  (void)info;
  if (!renderer) return false;
  renderer->last_batch = (ZLay_Vulkan_DrawBatch){0};
  renderer->last_frame = (ZLay_RendererFrameInfo){0};
  renderer->rect_vertex_shader = ZLay_Shader_RectSource(ZLAY_SHADER_LANGUAGE_GLSL_450, ZLAY_SHADER_STAGE_VERTEX);
  renderer->rect_fragment_shader = ZLay_Shader_RectSource(ZLAY_SHADER_LANGUAGE_GLSL_450, ZLAY_SHADER_STAGE_FRAGMENT);
  return true;
}

ZLay_Renderer ZLay_VulkanRenderer_AsRenderer(ZLay_VulkanRenderer* renderer) {
  ZLay_Renderer result = ZLay_RendererNull();
  if (!renderer) return result;
  result.backend = ZLAY_RENDERER_BACKEND_VULKAN;
  result.user = renderer;
  result.begin_frame = ZLay_VulkanRenderer_BeginFrame;
  result.render = ZLay_VulkanRenderer_Render;
  result.end_frame = ZLay_VulkanRenderer_EndFrame;
  result.shutdown = ZLay_VulkanRenderer_Shutdown;
  return result;
}
