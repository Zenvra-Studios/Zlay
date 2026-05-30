#include <drivers/gpu/opengl/zlay_gl_pipeline.h>

#include <string.h>

bool ZLay_GLPipeline_InitRect(ZLay_GLPipeline* pipeline) {
  if (!pipeline) return false;
  memset(pipeline, 0, sizeof(*pipeline));
  pipeline->kind = ZLAY_PIPELINE_RECT;
  pipeline->vertex_shader = ZLay_Shader_RectSource(ZLAY_SHADER_LANGUAGE_GLSL_330, ZLAY_SHADER_STAGE_VERTEX);
  pipeline->fragment_shader = ZLay_Shader_RectSource(ZLAY_SHADER_LANGUAGE_GLSL_330, ZLAY_SHADER_STAGE_FRAGMENT);
  pipeline->generation = 1u;
  pipeline->valid = true;
  return true;
}

void ZLay_GLPipeline_Shutdown(ZLay_GLPipeline* pipeline) {
  if (!pipeline) return;
  pipeline->valid = false;
  pipeline->generation++;
}

void ZLay_GLPipeline_Invalidate(ZLay_GLPipeline* pipeline) {
  if (!pipeline) return;
  pipeline->valid = false;
  pipeline->generation++;
}
