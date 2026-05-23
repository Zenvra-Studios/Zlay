#include <driver/opengl/zlay_gl_buffer.h>

#include <string.h>

void ZLay_GLBuffer_Init(ZLay_GLBuffer* buffer, ZLay_GLBufferKind kind) {
  if (!buffer) return;
  memset(buffer, 0, sizeof(*buffer));
  buffer->kind = kind;
  buffer->valid = true;
  buffer->generation = 1u;
}

bool ZLay_GLBuffer_Upload(ZLay_GLBuffer* buffer, const void* data, size_t size, uint32_t element_count) {
  if (!buffer || !buffer->valid) return false;
  buffer->last_upload = data;
  buffer->last_upload_size = size;
  buffer->element_count = element_count;
  buffer->generation++;
  return true;
}

void ZLay_GLBuffer_Shutdown(ZLay_GLBuffer* buffer) {
  if (!buffer) return;
  buffer->last_upload = NULL;
  buffer->last_upload_size = 0u;
  buffer->element_count = 0u;
  buffer->valid = false;
  buffer->generation++;
}
