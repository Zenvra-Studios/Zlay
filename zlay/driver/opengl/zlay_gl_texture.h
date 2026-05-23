#ifndef ZLAY_DRIVER_OPENGL_ZLAY_GL_TEXTURE_H
#define ZLAY_DRIVER_OPENGL_ZLAY_GL_TEXTURE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <zlay.h>

typedef struct ZLay_GLTexture {
  uint32_t width;
  uint32_t height;
  const void* pixels;
  uint64_t generation;
  bool valid;
} ZLay_GLTexture;

ZLAY_API void ZLay_GLTexture_Init(ZLay_GLTexture* texture);
ZLAY_API bool ZLay_GLTexture_Upload(ZLay_GLTexture* texture, uint32_t width, uint32_t height, const void* pixels);
ZLAY_API void ZLay_GLTexture_Shutdown(ZLay_GLTexture* texture);

#ifdef __cplusplus
}
#endif

#endif
