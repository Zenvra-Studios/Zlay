#ifndef ZLAY_DRIVERS_GPU_OPENGL_ZLAY_GL_BUFFER_H
#define ZLAY_DRIVERS_GPU_OPENGL_ZLAY_GL_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <zlay.h>

typedef enum ZLay_GLBufferKind {
  ZLAY_GL_BUFFER_VERTEX = 0,
  ZLAY_GL_BUFFER_INDEX = 1,
  ZLAY_GL_BUFFER_UNIFORM = 2
} ZLay_GLBufferKind;

typedef struct ZLay_GLBuffer {
  ZLay_GLBufferKind kind;
  const void* last_upload;
  size_t last_upload_size;
  uint32_t element_count;
  uint64_t generation;
  bool valid;
} ZLay_GLBuffer;

ZLAY_API void ZLay_GLBuffer_Init(ZLay_GLBuffer* buffer, ZLay_GLBufferKind kind);
ZLAY_API bool ZLay_GLBuffer_Upload(ZLay_GLBuffer* buffer, const void* data, size_t size, uint32_t element_count);
ZLAY_API void ZLay_GLBuffer_Shutdown(ZLay_GLBuffer* buffer);

#ifdef __cplusplus
}
#endif

#endif
