#include <backends/renderer/opengl/zlay_opengl_renderer_backend.h>

#include <zlay_simd.h>

#include <math.h>

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
    cmd->radius,
    ZLay_RoundedEffectiveExponent(cmd->rounded),
    ZLay__PackRGBA8(cmd->border_color),
    cmd->border_width
  };
}

static bool ZLay__CommandEmitsQuad(const ZLay_RenderCommand* cmd) {
  return cmd &&
         (cmd->type == ZLAY_CMD_RECT ||
          cmd->type == ZLAY_CMD_LINE ||
          cmd->type == ZLAY_CMD_IMAGE);
}

static bool ZLay__BuildLineQuadXY(const ZLay_RenderCommand* cmd, float xs[4], float ys[4]) {
  const float dx = cmd->line_end.x - cmd->line_start.x;
  const float dy = cmd->line_end.y - cmd->line_start.y;
  const float len = sqrtf(dx * dx + dy * dy);
  const float half_width = cmd->stroke_width > 0.0f ? cmd->stroke_width * 0.5f : 0.5f;
  float nx;
  float ny;

  if (len <= ZLAY_MATH_EPSILON) {
    ZLay__BuildQuadXY(cmd->bounds.x, cmd->bounds.y, cmd->bounds.width, cmd->bounds.height, xs, ys);
    return cmd->bounds.width > 0.0f && cmd->bounds.height > 0.0f;
  }

  nx = (-dy / len) * half_width;
  ny = (dx / len) * half_width;
  xs[0] = cmd->line_start.x + nx;
  ys[0] = cmd->line_start.y + ny;
  xs[1] = cmd->line_end.x + nx;
  ys[1] = cmd->line_end.y + ny;
  xs[2] = cmd->line_end.x - nx;
  ys[2] = cmd->line_end.y - ny;
  xs[3] = cmd->line_start.x - nx;
  ys[3] = cmd->line_start.y - ny;
  return true;
}

ZLay_GLBatchedGeometry ZLay_OpenGL_BuildGeometry(ZLay_Arena* frame_arena, const ZLay_RenderCommandList* list) {
  if (!frame_arena || !list || !list->commands) return (ZLay_GLBatchedGeometry){0};

  uint32_t quad_count = 0;
  for (uint32_t i = 0; i < list->count; ++i) {
    if (ZLay__CommandEmitsQuad(&list->commands[i])) quad_count++;
  }

  const uint32_t vertex_count = quad_count * 4u;
  const uint32_t index_count = quad_count * 6u;

  ZLay_GLVertex* vertices = (ZLay_GLVertex*)ZLay_ArenaAlloc(frame_arena, (size_t)vertex_count * sizeof(ZLay_GLVertex), 16);
  uint32_t* indices = (uint32_t*)ZLay_ArenaAlloc(frame_arena, (size_t)index_count * sizeof(uint32_t), 16);
  if (!vertices || !indices) return (ZLay_GLBatchedGeometry){0};

  uint32_t quad_i = 0;
  for (uint32_t i = 0; i < list->count; ++i) {
    const ZLay_RenderCommand* cmd = &list->commands[i];
    if (!ZLay__CommandEmitsQuad(cmd)) continue;

    const uint32_t base_v = quad_i * 4u;
    const uint32_t base_i = quad_i * 6u;
    quad_i++;

    const uint32_t packed = ZLay__PackRGBA8(cmd->color);

    float xs[4];
    float ys[4];
    if (cmd->type == ZLAY_CMD_LINE) {
      if (!ZLay__BuildLineQuadXY(cmd, xs, ys)) continue;
    } else {
      ZLay__BuildQuadXY(cmd->bounds.x, cmd->bounds.y, cmd->bounds.width, cmd->bounds.height, xs, ys);
    }

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
