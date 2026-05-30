#ifndef ZLAY_DRIVERS_GPU_OPENGL_ZLAY_GL_DRIVER_H
#define ZLAY_DRIVERS_GPU_OPENGL_ZLAY_GL_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <drivers/gpu/opengl/zlay_gl_buffer.h>
#include <drivers/gpu/opengl/zlay_gl_context.h>
#include <drivers/gpu/opengl/zlay_gl_pipeline.h>
#include <drivers/gpu/opengl/zlay_gl_texture.h>
#include <drivers/gpu/zlay_driver.h>
#include <zlay.h>

typedef struct ZLay_GLDriverState {
  ZLay_GLContext context;
  ZLay_GLPipeline rect_pipeline;
  ZLay_GLBuffer vertex_buffer;
  ZLay_GLBuffer index_buffer;
  ZLay_GLBuffer uniform_buffer;
  ZLay_GLTexture font_texture;
} ZLay_GLDriverState;

ZLAY_API ZLay_DriverAPI ZLay_OpenGLDriverAPI(void);

#ifdef __cplusplus
}
#endif

#endif
