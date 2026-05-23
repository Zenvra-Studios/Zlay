#include <driver/vulkan/zlay_vk_buffer.h>

#include <string.h>

void ZLay_VKBuffer_Init(ZLay_VKBuffer* buffer, ZLay_VKBufferKind kind) {
  if (!buffer) return;
  memset(buffer, 0, sizeof(*buffer));
  buffer->kind = kind;
  buffer->valid = true;
  buffer->generation = 1u;
}

bool ZLay_VKBuffer_Upload(ZLay_VKBuffer* buffer, const void* data, size_t size, uint32_t element_count) {
  if (!buffer || !buffer->valid) return false;
  buffer->last_upload = data;
  buffer->last_upload_size = size;
  buffer->element_count = element_count;
  buffer->generation++;
  return true;
}

void ZLay_VKBuffer_Shutdown(ZLay_VKBuffer* buffer) {
  if (!buffer) return;
  buffer->last_upload = NULL;
  buffer->last_upload_size = 0u;
  buffer->element_count = 0u;
  buffer->valid = false;
  buffer->generation++;
}
