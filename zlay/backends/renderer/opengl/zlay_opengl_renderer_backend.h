#ifndef ZLAY_BACKENDS_RENDERER_OPENGL_ZLAY_OPENGL_RENDERER_BACKEND_H
#define ZLAY_BACKENDS_RENDERER_OPENGL_ZLAY_OPENGL_RENDERER_BACKEND_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <renderer/zlay_renderer.h>
#include <zlay_antialiasing.h>
#include <zlay.h>

typedef struct ZLay_GLVertex {
  float x;
  float y;
  uint32_t color_rgba8;
  float rect_x;
  float rect_y;
  float rect_width;
  float rect_height;
  float radius;
  float rounded_exponent;
  uint32_t border_rgba8;
  float border_width;
} ZLay_GLVertex;

typedef struct ZLay_GLBatchedGeometry {
  ZLay_GLVertex* vertices;
  uint32_t vertex_count;
  uint32_t* indices;
  uint32_t index_count;
} ZLay_GLBatchedGeometry;

typedef struct ZLay_OpenGLRenderer {
  ZLay_Arena frame_arena;
  ZLay_GLBatchedGeometry last_geometry;
  ZLay_RendererFrameInfo last_frame;
  ZLay_ShaderSource rect_vertex_shader;
  ZLay_ShaderSource rect_fragment_shader;
} ZLay_OpenGLRenderer;

typedef struct ZLay_OpenGLRendererInitInfo {
  ZLay_Arena frame_arena;
} ZLay_OpenGLRendererInitInfo;

ZLAY_API ZLay_GLBatchedGeometry ZLay_OpenGL_BuildGeometry(ZLay_Arena* frame_arena, const ZLay_RenderCommandList* list);
ZLAY_API bool ZLay_OpenGLRenderer_Init(ZLay_OpenGLRenderer* renderer, const ZLay_OpenGLRendererInitInfo* info);
ZLAY_API ZLay_Renderer ZLay_OpenGLRenderer_AsRenderer(ZLay_OpenGLRenderer* renderer);

#ifdef __cplusplus
}
#endif

#endif
