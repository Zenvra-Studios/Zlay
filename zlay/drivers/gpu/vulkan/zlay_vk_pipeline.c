#include <drivers/gpu/vulkan/zlay_vk_pipeline.h>

#include <string.h>

bool ZLay_VKPipeline_InitRect(ZLay_VKPipeline* pipeline) {
  if (!pipeline) return false;
  memset(pipeline, 0, sizeof(*pipeline));
  pipeline->kind = ZLAY_PIPELINE_RECT;
  pipeline->vertex_shader = ZLay_Shader_RectSource(ZLAY_SHADER_LANGUAGE_GLSL_450, ZLAY_SHADER_STAGE_VERTEX);
  pipeline->fragment_shader = ZLay_Shader_RectSource(ZLAY_SHADER_LANGUAGE_GLSL_450, ZLAY_SHADER_STAGE_FRAGMENT);
  pipeline->valid = true;
  pipeline->generation = 1u;
  return true;
}

void ZLay_VKPipeline_Invalidate(ZLay_VKPipeline* pipeline) {
  if (!pipeline) return;
  pipeline->valid = false;
  pipeline->generation++;
}

void ZLay_VKPipeline_Shutdown(ZLay_VKPipeline* pipeline) {
  if (!pipeline) return;
  pipeline->valid = false;
  pipeline->generation++;
}
