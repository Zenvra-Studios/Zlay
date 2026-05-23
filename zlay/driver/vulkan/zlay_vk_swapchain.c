#include <driver/vulkan/zlay_vk_swapchain.h>

#include <string.h>

bool ZLay_VKSwapchain_Init(ZLay_VKSwapchain* swapchain, const ZLay_VKDevice* device, const ZLay_SwapchainDesc* desc) {
  if (!swapchain || !device || !device->valid || !desc) return false;
  memset(swapchain, 0, sizeof(*swapchain));
  swapchain->desc = *desc;
  if (swapchain->desc.image_count == 0u) swapchain->desc.image_count = 1u;
  swapchain->valid = true;
  swapchain->generation = 1u;
  return true;
}

void ZLay_VKSwapchain_Resize(ZLay_VKSwapchain* swapchain, uint32_t width, uint32_t height, float dpi_scale) {
  if (!swapchain) return;
  swapchain->desc.width = width != 0u ? width : 1u;
  swapchain->desc.height = height != 0u ? height : 1u;
  swapchain->desc.dpi_scale = dpi_scale > 0.0f ? dpi_scale : 1.0f;
  swapchain->generation++;
}

uint32_t ZLay_VKSwapchain_Acquire(ZLay_VKSwapchain* swapchain) {
  if (!swapchain || !swapchain->valid || swapchain->desc.image_count == 0u) return 0u;
  return swapchain->current_image;
}

void ZLay_VKSwapchain_Present(ZLay_VKSwapchain* swapchain) {
  if (!swapchain || !swapchain->valid || swapchain->desc.image_count == 0u) return;
  swapchain->current_image = (swapchain->current_image + 1u) % swapchain->desc.image_count;
}

void ZLay_VKSwapchain_Shutdown(ZLay_VKSwapchain* swapchain) {
  if (!swapchain) return;
  swapchain->valid = false;
  swapchain->generation++;
}
