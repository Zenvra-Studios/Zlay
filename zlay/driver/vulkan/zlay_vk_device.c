#include <driver/vulkan/zlay_vk_device.h>

#include <string.h>

bool ZLay_VKDevice_Init(ZLay_VKDevice* device, const ZLay_VKInstance* instance) {
  if (!device || !instance || !instance->valid) return false;
  memset(device, 0, sizeof(*device));
  device->valid = true;
  device->generation = 1u;
  return true;
}

void ZLay_VKDevice_Shutdown(ZLay_VKDevice* device) {
  if (!device) return;
  device->valid = false;
  device->generation++;
}
