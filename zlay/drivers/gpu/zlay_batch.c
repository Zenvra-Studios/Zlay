#include <drivers/gpu/zlay_batch.h>

#include <math.h>

static bool ZLay__BatchHasArea(ZLay_Rect bounds) {
  return bounds.width > 0.0f && bounds.height > 0.0f;
}

static ZLay_Vertex ZLay__BatchVertex(
  float x,
  float y,
  float u,
  float v,
  ZLay_ColorU32 color,
  float radius,
  int32_t z_index
) {
  ZLay_Vertex vertex;
  vertex.x = x;
  vertex.y = y;
  vertex.u = u;
  vertex.v = v;
  vertex.color = color;
  vertex.radius = radius;
  vertex.z_index = (float)z_index;
  vertex.extra0 = 0.0f;
  vertex.extra1 = 0.0f;
  return vertex;
}

void ZLay_BatchInit(
  ZLay_Batch* batch,
  ZLay_Vertex* vertices,
  uint32_t vertex_capacity,
  uint32_t* indices,
  uint32_t index_capacity
) {
  if (!batch) return;
  batch->vertices = vertices;
  batch->vertex_capacity = vertex_capacity;
  batch->indices = indices;
  batch->index_capacity = index_capacity;
  ZLay_BatchReset(batch);
}

void ZLay_BatchReset(ZLay_Batch* batch) {
  if (!batch) return;
  batch->vertex_count = 0u;
  batch->index_count = 0u;
  batch->draw_calls = 0u;
  batch->rect_count = 0u;
  batch->line_count = 0u;
  batch->text_count = 0u;
  batch->image_count = 0u;
  batch->clip_count = 0u;
  batch->overflowed = false;
}

bool ZLay_BatchCanPush(const ZLay_Batch* batch, uint32_t vertex_count, uint32_t index_count) {
  if (!batch) return false;
  if (vertex_count > batch->vertex_capacity || index_count > batch->index_capacity) return false;
  if (batch->vertex_count > batch->vertex_capacity - vertex_count) return false;
  if (batch->index_count > batch->index_capacity - index_count) return false;
  return batch->vertices != NULL && batch->indices != NULL;
}

bool ZLay_BatchPushRect(
  ZLay_Batch* batch,
  ZLay_Rect bounds,
  ZLay_Color color,
  float radius,
  int32_t z_index
) {
  if (!batch || !ZLay__BatchHasArea(bounds)) return false;
  if (!ZLay_BatchCanPush(batch, 4u, 6u)) {
    if (batch) batch->overflowed = true;
    return false;
  }

  const uint32_t base_v = batch->vertex_count;
  const uint32_t base_i = batch->index_count;
  const ZLay_ColorU32 packed = ZLay_ColorToPackedU32(color);
  const float x0 = bounds.x;
  const float y0 = bounds.y;
  const float x1 = bounds.x + bounds.width;
  const float y1 = bounds.y + bounds.height;

  batch->vertices[base_v + 0u] = ZLay__BatchVertex(x0, y0, 0.0f, 0.0f, packed, radius, z_index);
  batch->vertices[base_v + 1u] = ZLay__BatchVertex(x1, y0, 1.0f, 0.0f, packed, radius, z_index);
  batch->vertices[base_v + 2u] = ZLay__BatchVertex(x1, y1, 1.0f, 1.0f, packed, radius, z_index);
  batch->vertices[base_v + 3u] = ZLay__BatchVertex(x0, y1, 0.0f, 1.0f, packed, radius, z_index);

  batch->indices[base_i + 0u] = base_v + 0u;
  batch->indices[base_i + 1u] = base_v + 1u;
  batch->indices[base_i + 2u] = base_v + 2u;
  batch->indices[base_i + 3u] = base_v + 0u;
  batch->indices[base_i + 4u] = base_v + 2u;
  batch->indices[base_i + 5u] = base_v + 3u;

  batch->vertex_count += 4u;
  batch->index_count += 6u;
  batch->rect_count++;
  return true;
}

bool ZLay_BatchPushLine(
  ZLay_Batch* batch,
  ZLay_Vec2 start,
  ZLay_Vec2 end,
  float width,
  ZLay_Color color,
  int32_t z_index
) {
  const float dx = end.x - start.x;
  const float dy = end.y - start.y;
  const float len = sqrtf(dx * dx + dy * dy);
  const float half_width = width > 0.0f ? width * 0.5f : 0.5f;
  float nx;
  float ny;
  float xs[4];
  float ys[4];
  uint32_t base_v;
  uint32_t base_i;
  ZLay_ColorU32 packed;

  if (!batch || width <= 0.0f) return false;
  if (!ZLay_BatchCanPush(batch, 4u, 6u)) {
    if (batch) batch->overflowed = true;
    return false;
  }

  if (len <= ZLAY_MATH_EPSILON) {
    xs[0] = start.x - half_width;
    ys[0] = start.y - half_width;
    xs[1] = start.x + half_width;
    ys[1] = start.y - half_width;
    xs[2] = start.x + half_width;
    ys[2] = start.y + half_width;
    xs[3] = start.x - half_width;
    ys[3] = start.y + half_width;
  } else {
    nx = (-dy / len) * half_width;
    ny = (dx / len) * half_width;
    xs[0] = start.x + nx;
    ys[0] = start.y + ny;
    xs[1] = end.x + nx;
    ys[1] = end.y + ny;
    xs[2] = end.x - nx;
    ys[2] = end.y - ny;
    xs[3] = start.x - nx;
    ys[3] = start.y - ny;
  }

  base_v = batch->vertex_count;
  base_i = batch->index_count;
  packed = ZLay_ColorToPackedU32(color);
  batch->vertices[base_v + 0u] = ZLay__BatchVertex(xs[0], ys[0], 0.0f, 0.0f, packed, 0.0f, z_index);
  batch->vertices[base_v + 1u] = ZLay__BatchVertex(xs[1], ys[1], 1.0f, 0.0f, packed, 0.0f, z_index);
  batch->vertices[base_v + 2u] = ZLay__BatchVertex(xs[2], ys[2], 1.0f, 1.0f, packed, 0.0f, z_index);
  batch->vertices[base_v + 3u] = ZLay__BatchVertex(xs[3], ys[3], 0.0f, 1.0f, packed, 0.0f, z_index);

  batch->indices[base_i + 0u] = base_v + 0u;
  batch->indices[base_i + 1u] = base_v + 1u;
  batch->indices[base_i + 2u] = base_v + 2u;
  batch->indices[base_i + 3u] = base_v + 0u;
  batch->indices[base_i + 4u] = base_v + 2u;
  batch->indices[base_i + 5u] = base_v + 3u;

  batch->vertex_count += 4u;
  batch->index_count += 6u;
  batch->line_count++;
  return true;
}

bool ZLay_BatchPushText(
  ZLay_Batch* batch,
  ZLay_Rect bounds,
  ZLay_String text,
  ZLay_Color color,
  int32_t z_index
) {
  if (!batch) return false;
  batch->text_count++;
  if (!text.chars || text.length == 0u || !ZLay__BatchHasArea(bounds)) return true;

  if (!ZLay_BatchCanPush(batch, 4u, 6u)) {
    batch->overflowed = true;
    return false;
  }

  const uint32_t before_rects = batch->rect_count;
  const bool pushed = ZLay_BatchPushRect(batch, bounds, color, 0.0f, z_index);
  batch->rect_count = before_rects;
  return pushed;
}

bool ZLay_BatchPushImage(
  ZLay_Batch* batch,
  ZLay_Rect bounds,
  ZLay_ImageHandle image,
  ZLay_Color tint,
  int32_t z_index
) {
  uint32_t before_rects;
  bool pushed;
  if (!batch) return false;
  batch->image_count++;
  if (image.id == 0u || tint.a == 0u || !ZLay__BatchHasArea(bounds)) return true;

  before_rects = batch->rect_count;
  pushed = ZLay_BatchPushRect(batch, bounds, tint, 0.0f, z_index);
  batch->rect_count = before_rects;
  return pushed;
}

bool ZLay_BatchPushClip(ZLay_Batch* batch) {
  if (!batch) return false;
  batch->clip_count++;
  return true;
}

bool ZLay_BatchBuildFromCommands(ZLay_Batch* batch, const ZLay_RenderCommandList* commands) {
  if (!batch) return false;
  ZLay_BatchReset(batch);
  if (!commands || !commands->commands) return true;

  bool ok = true;
  bool has_rect_geometry = false;
  bool has_line_geometry = false;
  bool has_text_geometry = false;
  bool has_image_geometry = false;

  for (uint32_t i = 0u; i < commands->count; ++i) {
    const ZLay_RenderCommand* command = &commands->commands[i];
    switch (command->type) {
      case ZLAY_CMD_RECT:
        if (ZLay_BatchPushRect(batch, command->bounds, command->color, command->radius, command->z_index)) {
          has_rect_geometry = true;
        } else if (batch->overflowed) {
          ok = false;
        }
        break;
      case ZLAY_CMD_LINE:
        if (ZLay_BatchPushLine(batch, command->line_start, command->line_end, command->stroke_width, command->color, command->z_index)) {
          has_line_geometry = true;
        } else if (batch->overflowed) {
          ok = false;
        }
        break;
      case ZLAY_CMD_TEXT:
        if (ZLay_BatchPushText(batch, command->bounds, command->text, command->color, command->z_index)) {
          if (command->text.chars && command->text.length > 0u && ZLay__BatchHasArea(command->bounds)) {
            has_text_geometry = true;
          }
        } else if (batch->overflowed) {
          ok = false;
        }
        break;
      case ZLAY_CMD_IMAGE:
        if (ZLay_BatchPushImage(batch, command->bounds, command->image, command->color, command->z_index)) {
          if (command->image.id != 0u && ZLay__BatchHasArea(command->bounds)) {
            has_image_geometry = true;
          }
        } else if (batch->overflowed) {
          ok = false;
        }
        break;
      case ZLAY_CMD_CLIP_BEGIN:
      case ZLAY_CMD_CLIP_END:
        ZLay_BatchPushClip(batch);
        break;
      default:
        break;
    }
  }

  batch->draw_calls = (has_rect_geometry || has_line_geometry || has_image_geometry ? 1u : 0u) +
                      (has_text_geometry ? 1u : 0u);
  return ok;
}

ZLay_BatchStats ZLay_BatchGetStats(const ZLay_Batch* batch) {
  ZLay_BatchStats stats;
  stats.draw_calls = batch ? batch->draw_calls : 0u;
  stats.rect_count = batch ? batch->rect_count : 0u;
  stats.line_count = batch ? batch->line_count : 0u;
  stats.text_count = batch ? batch->text_count : 0u;
  stats.image_count = batch ? batch->image_count : 0u;
  stats.clip_count = batch ? batch->clip_count : 0u;
  stats.vertex_count = batch ? batch->vertex_count : 0u;
  stats.index_count = batch ? batch->index_count : 0u;
  stats.overflowed = batch ? batch->overflowed : false;
  return stats;
}

ZLay_GpuStats ZLay_BatchGetGpuStats(const ZLay_Batch* batch) {
  ZLay_BatchStats batch_stats = ZLay_BatchGetStats(batch);
  ZLay_GpuStats stats;
  stats.draw_calls = batch_stats.draw_calls;
  stats.rect_count = batch_stats.rect_count;
  stats.line_count = batch_stats.line_count;
  stats.text_count = batch_stats.text_count;
  stats.image_count = batch_stats.image_count;
  stats.clip_count = batch_stats.clip_count;
  stats.vertex_count = batch_stats.vertex_count;
  stats.index_count = batch_stats.index_count;
  stats.overflowed = batch_stats.overflowed;
  return stats;
}
