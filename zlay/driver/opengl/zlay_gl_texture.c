#include <driver/opengl/zlay_gl_texture.h>

#include <string.h>

void ZLay_GLTexture_Init(ZLay_GLTexture* texture) {
  if (!texture) return;
  memset(texture, 0, sizeof(*texture));
  texture->generation = 1u;
}

bool ZLay_GLTexture_Upload(ZLay_GLTexture* texture, uint32_t width, uint32_t height, const void* pixels) {
  if (!texture || width == 0u || height == 0u) return false;
  texture->width = width;
  texture->height = height;
  texture->pixels = pixels;
  texture->valid = true;
  texture->generation++;
  return true;
}

void ZLay_GLTexture_Shutdown(ZLay_GLTexture* texture) {
  if (!texture) return;
  texture->width = 0u;
  texture->height = 0u;
  texture->pixels = NULL;
  texture->valid = false;
  texture->generation++;
}
