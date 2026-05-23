#ifndef ZLAY_DRIVER_OPENGL_ZLAY_GL_PIPELINE_H
#define ZLAY_DRIVER_OPENGL_ZLAY_GL_PIPELINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <driver/zlay_pipeline.h>
#include <zlay_antialiasing.h>

typedef struct ZLay_GLPipeline {
  ZLay_PipelineKind kind;
  ZLay_ShaderSource vertex_shader;
  ZLay_ShaderSource fragment_shader;
  uint64_t generation;
  bool valid;
} ZLay_GLPipeline;

ZLAY_API bool ZLay_GLPipeline_InitRect(ZLay_GLPipeline* pipeline);
ZLAY_API void ZLay_GLPipeline_Shutdown(ZLay_GLPipeline* pipeline);
ZLAY_API void ZLay_GLPipeline_Invalidate(ZLay_GLPipeline* pipeline);

#ifdef __cplusplus
}
#endif

#endif
