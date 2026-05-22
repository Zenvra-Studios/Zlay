#include "zlay_opengl.h"

#include "zlay_simd.h"

static uint32_t ZLay__PackRGBA8(ZLay_Color c) {
  return (uint32_t)c.r | ((uint32_t)c.g << 8u) | ((uint32_t)c.b << 16u) | ((uint32_t)c.a << 24u);
}

static ZLay_GLVertex ZLay__MakeGLVertex(float x, float y, const ZLay_RenderCommand* cmd, uint32_t color) {
  return (ZLay_GLVertex){
    x,
    y,
    color,
    cmd->bounds.x,
    cmd->bounds.y,
    cmd->bounds.width,
    cmd->bounds.height,
    cmd->radius
  };
}

ZLay_GLBatchedGeometry ZLay_OpenGL_BuildGeometry(ZLay_Arena* frame_arena, const ZLay_RenderCommandList* list) {
  if (!frame_arena || !list || !list->commands) return (ZLay_GLBatchedGeometry){0};

  uint32_t rect_count = 0;
  for (uint32_t i = 0; i < list->count; ++i) {
    if (list->commands[i].type == ZLAY_CMD_RECT) rect_count++;
  }

  const uint32_t vertex_count = rect_count * 4u;
  const uint32_t index_count = rect_count * 6u;

  ZLay_GLVertex* vertices = (ZLay_GLVertex*)ZLay_ArenaAlloc(frame_arena, (size_t)vertex_count * sizeof(ZLay_GLVertex), 16);
  uint32_t* indices = (uint32_t*)ZLay_ArenaAlloc(frame_arena, (size_t)index_count * sizeof(uint32_t), 16);
  if (!vertices || !indices) return (ZLay_GLBatchedGeometry){0};

  uint32_t rect_i = 0;
  for (uint32_t i = 0; i < list->count; ++i) {
    const ZLay_RenderCommand* cmd = &list->commands[i];
    if (cmd->type != ZLAY_CMD_RECT) continue;

    const uint32_t base_v = rect_i * 4u;
    const uint32_t base_i = rect_i * 6u;
    rect_i++;

    const uint32_t packed = ZLay__PackRGBA8(cmd->color);

    float xs[4];
    float ys[4];
    ZLay__BuildQuadXY(cmd->bounds.x, cmd->bounds.y, cmd->bounds.width, cmd->bounds.height, xs, ys);

    vertices[base_v + 0u] = ZLay__MakeGLVertex(xs[0], ys[0], cmd, packed);
    vertices[base_v + 1u] = ZLay__MakeGLVertex(xs[1], ys[1], cmd, packed);
    vertices[base_v + 2u] = ZLay__MakeGLVertex(xs[2], ys[2], cmd, packed);
    vertices[base_v + 3u] = ZLay__MakeGLVertex(xs[3], ys[3], cmd, packed);

    indices[base_i + 0u] = base_v + 0u;
    indices[base_i + 1u] = base_v + 1u;
    indices[base_i + 2u] = base_v + 2u;
    indices[base_i + 3u] = base_v + 0u;
    indices[base_i + 4u] = base_v + 2u;
    indices[base_i + 5u] = base_v + 3u;
  }

  ZLay_GLBatchedGeometry geom;
  geom.vertices = vertices;
  geom.vertex_count = vertex_count;
  geom.indices = indices;
  geom.index_count = index_count;
  return geom;
}

static bool ZLay_OpenGLRenderer_BeginFrame(void* user, const ZLay_RendererFrameInfo* frame) {
  ZLay_OpenGLRenderer* renderer = (ZLay_OpenGLRenderer*)user;
  if (!renderer) return false;
  ZLay_ArenaReset(&renderer->frame_arena);
  renderer->last_geometry = (ZLay_GLBatchedGeometry){0};
  renderer->last_frame = frame ? *frame : (ZLay_RendererFrameInfo){0};
  return true;
}

static bool ZLay_OpenGLRenderer_Render(void* user, const ZLay_RenderCommandList* commands, ZLay_RendererStats* stats) {
  ZLay_OpenGLRenderer* renderer = (ZLay_OpenGLRenderer*)user;
  if (!renderer || !commands) return false;

  renderer->last_geometry = ZLay_OpenGL_BuildGeometry(&renderer->frame_arena, commands);
  if (stats) {
    stats->command_count = commands->count;
    stats->draw_call_count = renderer->last_geometry.index_count > 0 ? 1u : 0u;
    stats->vertex_count = renderer->last_geometry.vertex_count;
    stats->index_count = renderer->last_geometry.index_count;
  }
  return true;
}

static void ZLay_OpenGLRenderer_EndFrame(void* user) {
  (void)user;
}

static void ZLay_OpenGLRenderer_Shutdown(void* user) {
  ZLay_OpenGLRenderer* renderer = (ZLay_OpenGLRenderer*)user;
  if (!renderer) return;
  renderer->last_geometry = (ZLay_GLBatchedGeometry){0};
  renderer->last_frame = (ZLay_RendererFrameInfo){0};
}

bool ZLay_OpenGLRenderer_Init(ZLay_OpenGLRenderer* renderer, const ZLay_OpenGLRendererInitInfo* info) {
  if (!renderer || !info || !info->frame_arena.memory) return false;
  renderer->frame_arena = info->frame_arena;
  renderer->last_geometry = (ZLay_GLBatchedGeometry){0};
  renderer->last_frame = (ZLay_RendererFrameInfo){0};
  renderer->rect_vertex_shader = ZLay_Shader_RectSource(ZLAY_SHADER_LANGUAGE_GLSL_330, ZLAY_SHADER_STAGE_VERTEX);
  renderer->rect_fragment_shader = ZLay_Shader_RectSource(ZLAY_SHADER_LANGUAGE_GLSL_330, ZLAY_SHADER_STAGE_FRAGMENT);
  return true;
}

ZLay_Renderer ZLay_OpenGLRenderer_AsRenderer(ZLay_OpenGLRenderer* renderer) {
  ZLay_Renderer result = ZLay_RendererNull();
  if (!renderer) return result;
  result.backend = ZLAY_RENDERER_BACKEND_OPENGL;
  result.user = renderer;
  result.begin_frame = ZLay_OpenGLRenderer_BeginFrame;
  result.render = ZLay_OpenGLRenderer_Render;
  result.end_frame = ZLay_OpenGLRenderer_EndFrame;
  result.shutdown = ZLay_OpenGLRenderer_Shutdown;
  return result;
}
