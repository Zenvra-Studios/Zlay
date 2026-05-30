#ifndef ZLAY_DRIVERS_GPU_VULKAN_ZLAY_VK_PIPELINE_H
#define ZLAY_DRIVERS_GPU_VULKAN_ZLAY_VK_PIPELINE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <drivers/gpu/zlay_pipeline.h>
#include <zlay_antialiasing.h>

typedef struct ZLay_VKPipeline {
  ZLay_PipelineKind kind;
  ZLay_ShaderSource vertex_shader;
  ZLay_ShaderSource fragment_shader;
  bool valid;
  uint64_t generation;
} ZLay_VKPipeline;

ZLAY_API bool ZLay_VKPipeline_InitRect(ZLay_VKPipeline* pipeline);
ZLAY_API void ZLay_VKPipeline_Invalidate(ZLay_VKPipeline* pipeline);
ZLAY_API void ZLay_VKPipeline_Shutdown(ZLay_VKPipeline* pipeline);

#ifdef __cplusplus
}
#endif

#endif
