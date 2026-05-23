#ifndef ZLAY_DRIVER_VULKAN_ZLAY_VK_BUFFER_H
#define ZLAY_DRIVER_VULKAN_ZLAY_VK_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <zlay.h>

typedef enum ZLay_VKBufferKind {
  ZLAY_VK_BUFFER_VERTEX = 0,
  ZLAY_VK_BUFFER_INDEX = 1,
  ZLAY_VK_BUFFER_UNIFORM = 2
} ZLay_VKBufferKind;

typedef struct ZLay_VKBuffer {
  ZLay_VKBufferKind kind;
  const void* last_upload;
  size_t last_upload_size;
  uint32_t element_count;
  bool valid;
  uint64_t generation;
} ZLay_VKBuffer;

ZLAY_API void ZLay_VKBuffer_Init(ZLay_VKBuffer* buffer, ZLay_VKBufferKind kind);
ZLAY_API bool ZLay_VKBuffer_Upload(ZLay_VKBuffer* buffer, const void* data, size_t size, uint32_t element_count);
ZLAY_API void ZLay_VKBuffer_Shutdown(ZLay_VKBuffer* buffer);

#ifdef __cplusplus
}
#endif

#endif
