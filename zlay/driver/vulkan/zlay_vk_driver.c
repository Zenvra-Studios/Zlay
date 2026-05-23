#include <driver/vulkan/zlay_vk_driver.h>

#include <stdlib.h>

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
    stats->draw_call_count = renderer->last_batch.rect_count > 0u ? 1u : 0u;
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

static void ZLay__VulkanDriverStateShutdown(ZLay_VKDriverState* vk) {
  if (!vk) return;
  ZLay_VKSync_Shutdown(&vk->sync);
  ZLay_VKCommand_Reset(&vk->command);
  ZLay_VKBuffer_Shutdown(&vk->uniform_buffer);
  ZLay_VKBuffer_Shutdown(&vk->index_buffer);
  ZLay_VKBuffer_Shutdown(&vk->vertex_buffer);
  ZLay_VKPipeline_Shutdown(&vk->rect_pipeline);
  ZLay_VKSwapchain_Shutdown(&vk->swapchain);
  ZLay_VKDevice_Shutdown(&vk->device);
  ZLay_VKInstance_Shutdown(&vk->instance);
}

static bool ZLay_VulkanDriver_Create(ZLay_Driver* driver, const ZLay_DriverDesc* desc) {
  ZLay_VKDriverState* vk;
  ZLay_DriverBackendState* state = ZLay_DriverGetBackendState(driver);
  const ZLay_Swapchain* shared_swapchain = ZLay_DriverGetSwapchainConst(driver);
  ZLay_SwapchainDesc swapchain_desc;

  if (!driver || !desc || !state) return false;

  vk = (ZLay_VKDriverState*)calloc(1u, sizeof(ZLay_VKDriverState));
  if (!vk) return false;

  swapchain_desc = shared_swapchain
    ? shared_swapchain->desc
    : ZLay_SwapchainDescFromSurface(ZLay_DriverGetSurfaceConst(driver), &driver->present);

  if (
    !ZLay_VKInstance_Init(&vk->instance, desc) ||
    !ZLay_VKDevice_Init(&vk->device, &vk->instance) ||
    !ZLay_VKSwapchain_Init(&vk->swapchain, &vk->device, &swapchain_desc) ||
    !ZLay_VKPipeline_InitRect(&vk->rect_pipeline)
  ) {
    ZLay__VulkanDriverStateShutdown(vk);
    free(vk);
    return false;
  }

  ZLay_VKBuffer_Init(&vk->vertex_buffer, ZLAY_VK_BUFFER_VERTEX);
  ZLay_VKBuffer_Init(&vk->index_buffer, ZLAY_VK_BUFFER_INDEX);
  ZLay_VKBuffer_Init(&vk->uniform_buffer, ZLAY_VK_BUFFER_UNIFORM);
  ZLay_VKCommand_Init(&vk->command);
  ZLay_VKSync_Init(&vk->sync);

  driver->backend_data = vk;
  driver->owns_backend_data = true;

  state->backend = ZLAY_BACKEND_VULKAN;
  state->name = "Vulkan";
  state->context_created = true;
  state->frame_open = false;
  state->submitted = false;
  state->presented = false;
  state->swapchain_generation = (uint32_t)vk->swapchain.generation;
  return true;
}

static void ZLay_VulkanDriver_Destroy(ZLay_Driver* driver) {
  ZLay_VKDriverState* vk = driver ? (ZLay_VKDriverState*)driver->backend_data : NULL;
  ZLay_DriverBackendState* state = ZLay_DriverGetBackendState(driver);

  ZLay__VulkanDriverStateShutdown(vk);

  if (state) {
    state->context_created = false;
    state->frame_open = false;
  }
}

static void ZLay_VulkanDriver_Resize(ZLay_Driver* driver, uint32_t width, uint32_t height) {
  ZLay_VKDriverState* vk = driver ? (ZLay_VKDriverState*)driver->backend_data : NULL;
  const ZLay_Surface* surface = ZLay_DriverGetSurfaceConst(driver);
  ZLay_DriverBackendState* state = ZLay_DriverGetBackendState(driver);
  if (!vk) return;

  ZLay_VKSwapchain_Resize(&vk->swapchain, width, height, surface ? surface->dpi_scale : 1.0f);
  ZLay_VKPipeline_Invalidate(&vk->rect_pipeline);
  ZLay_VKPipeline_InitRect(&vk->rect_pipeline);

  if (state) {
    state->swapchain_generation = (uint32_t)vk->swapchain.generation;
  }
}

static bool ZLay_VulkanDriver_BeginFrame(ZLay_Driver* driver) {
  ZLay_VKDriverState* vk = driver ? (ZLay_VKDriverState*)driver->backend_data : NULL;
  ZLay_DriverBackendState* state = ZLay_DriverGetBackendState(driver);
  const ZLay_DriverStats stats = ZLay_DriverGetStats(driver);
  uint32_t image_index;

  if (!vk || !state || !state->context_created || !vk->swapchain.valid) return false;

  if (vk->swapchain.desc.image_count != 0u) {
    vk->swapchain.current_image = stats.swapchain_image_index % vk->swapchain.desc.image_count;
  }
  image_index = ZLay_VKSwapchain_Acquire(&vk->swapchain);

  if (!ZLay_VKSync_BeginFrame(&vk->sync, stats.frame_index)) return false;
  if (!ZLay_VKCommand_Begin(&vk->command, stats.frame_index, image_index)) {
    ZLay_VKSync_EndFrame(&vk->sync);
    return false;
  }

  state->frame_index = stats.frame_index;
  state->swapchain_image_index = image_index;
  state->frame_open = true;
  state->submitted = false;
  state->presented = false;
  return true;
}

static bool ZLay_VulkanDriver_Submit(ZLay_Driver* driver, const ZLay_RenderCommandList* commands) {
  (void)commands;
  ZLay_VKDriverState* vk = driver ? (ZLay_VKDriverState*)driver->backend_data : NULL;
  ZLay_DriverBackendState* state = ZLay_DriverGetBackendState(driver);
  const ZLay_DriverFramePacket* packet = ZLay_DriverGetFramePacket(driver);
  const ZLay_DriverStats stats = ZLay_DriverGetStats(driver);
  const ZLay_Swapchain* swapchain = ZLay_DriverGetSwapchainConst(driver);

  if (!vk || !state || !packet || !state->frame_open || !vk->rect_pipeline.valid) return false;
  if (!ZLay_VKBuffer_Upload(&vk->vertex_buffer, packet->vertex_upload, packet->vertex_upload_size, stats.vertex_count)) return false;
  if (!ZLay_VKBuffer_Upload(&vk->index_buffer, packet->index_upload, packet->index_upload_size, stats.index_count)) return false;
  if (!ZLay_VKCommand_RecordPacket(&vk->command, packet, stats.draw_calls)) return false;

  state->submitted_frame_index = stats.frame_index;
  state->swapchain_generation = swapchain ? (uint32_t)swapchain->generation : (uint32_t)vk->swapchain.generation;
  state->swapchain_image_index = packet->swapchain_image_index;
  state->draw_calls = stats.draw_calls;
  state->vertex_count = stats.vertex_count;
  state->index_count = stats.index_count;
  state->uploaded_bytes = packet->vertex_upload_size + packet->index_upload_size;
  state->submitted = true;
  return true;
}

static bool ZLay_VulkanDriver_EndFrame(ZLay_Driver* driver) {
  ZLay_VKDriverState* vk = driver ? (ZLay_VKDriverState*)driver->backend_data : NULL;
  if (!vk) return false;
  return ZLay_VKCommand_End(&vk->command);
}

static bool ZLay_VulkanDriver_Present(ZLay_Driver* driver) {
  ZLay_VKDriverState* vk = driver ? (ZLay_VKDriverState*)driver->backend_data : NULL;
  ZLay_DriverBackendState* state = ZLay_DriverGetBackendState(driver);
  if (!vk || !state || !state->frame_open) return false;

  if (!ZLay_VKSync_EndFrame(&vk->sync)) return false;
  ZLay_VKSwapchain_Present(&vk->swapchain);

  state->presented_frame_index = ZLay_DriverGetStats(driver).frame_index;
  state->presented = true;
  state->frame_open = false;
  return true;
}

ZLay_DriverAPI ZLay_VulkanDriverAPI(void) {
  ZLay_DriverAPI api;
  api.create = ZLay_VulkanDriver_Create;
  api.destroy = ZLay_VulkanDriver_Destroy;
  api.resize = ZLay_VulkanDriver_Resize;
  api.begin_frame = ZLay_VulkanDriver_BeginFrame;
  api.submit = ZLay_VulkanDriver_Submit;
  api.end_frame = ZLay_VulkanDriver_EndFrame;
  api.present = ZLay_VulkanDriver_Present;
  return api;
}
