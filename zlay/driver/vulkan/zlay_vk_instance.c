#include <driver/vulkan/zlay_vk_instance.h>

#include <string.h>

bool ZLay_VKInstance_Init(ZLay_VKInstance* instance, const ZLay_DriverDesc* desc) {
  if (!instance) return false;
  memset(instance, 0, sizeof(*instance));
  instance->native_instance = desc ? desc->backend_user : NULL;
  instance->owns_instance = false;
  instance->valid = true;
  instance->generation = 1u;
  return true;
}

void ZLay_VKInstance_Shutdown(ZLay_VKInstance* instance) {
  if (!instance) return;
  instance->valid = false;
  instance->generation++;
}
