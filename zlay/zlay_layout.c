#include <zlay_internal.h>

static float ZLay__ResolveSize(ZLay_Size size, float parent, float content_default) {
  switch (size.type) {
    case ZLAY_SIZE_PX: return size.value;
    case ZLAY_SIZE_PERCENT: return parent * (size.value / 100.0f);
    case ZLAY_SIZE_GROW: return content_default; // resolved in grow pass
    case ZLAY_SIZE_AUTO:
    default: return content_default;
  }
}

static ZLay_Dimensions ZLay__MeasureNode(const ZLay_Context* ctx, const ZLay_Node* node) {
  if (!ctx || !node) return (ZLay_Dimensions){0};
  if (node->type != ZLAY_NODE_TEXT) return (ZLay_Dimensions){0};
  if (!ctx->measure_text) return (ZLay_Dimensions){0};
  return ctx->measure_text(node->text, ctx->measure_text_user);
}

static void ZLay__LayoutNode(ZLay_Context* ctx, int32_t idx);

static void ZLay__LayoutChildren_Flex(ZLay_Context* ctx, ZLay_Node* parent) {
  const float inner_x = parent->layout.x + parent->style.padding_left;
  const float inner_y = parent->layout.y + parent->style.padding_top;
  const float inner_w = parent->layout.width - parent->style.padding_left - parent->style.padding_right;
  const float inner_h = parent->layout.height - parent->style.padding_top - parent->style.padding_bottom;

  // First pass: measure fixed sizes + sum grow weights.
  float used = 0.0f;
  float total_grow = 0.0f;

  for (int32_t child_idx = parent->first_child; child_idx >= 0; child_idx = ctx->nodes[child_idx].next_sibling) {
    ZLay_Node* child = &ctx->nodes[child_idx];
    if (child->style.position != ZLAY_POSITION_RELATIVE) continue;
    const ZLay_Dimensions measured = ZLay__MeasureNode(ctx, child);

    if (parent->style.flex_direction == ZLAY_FLEX_COLUMN) {
      if (child->style.height.type == ZLAY_SIZE_GROW) total_grow += (child->style.height.value <= 0 ? 1.0f : child->style.height.value);
      else if (child->style.height.type == ZLAY_SIZE_AUTO) used += measured.height;
      else used += ZLay__ResolveSize(child->style.height, inner_h, 0.0f);
    } else {
      if (child->style.width.type == ZLAY_SIZE_GROW) total_grow += (child->style.width.value <= 0 ? 1.0f : child->style.width.value);
      else if (child->style.width.type == ZLAY_SIZE_AUTO) used += measured.width;
      else used += ZLay__ResolveSize(child->style.width, inner_w, 0.0f);
    }
    used += parent->style.gap;
  }
  if (used > 0.0f) used -= parent->style.gap;

  const float available = (parent->style.flex_direction == ZLAY_FLEX_COLUMN) ? inner_h : inner_w;
  const float remaining = available - used;

  float cursor = (parent->style.flex_direction == ZLAY_FLEX_COLUMN) ? inner_y : inner_x;

  // Second pass: assign rects.
  for (int32_t child_idx = parent->first_child; child_idx >= 0; child_idx = ctx->nodes[child_idx].next_sibling) {
    ZLay_Node* child = &ctx->nodes[child_idx];
    if (child->style.position != ZLAY_POSITION_RELATIVE) continue;
    const ZLay_Dimensions measured = ZLay__MeasureNode(ctx, child);

    if (parent->style.flex_direction == ZLAY_FLEX_COLUMN) {
      float h = 0.0f;
      if (child->style.height.type == ZLAY_SIZE_GROW) {
        const float w = (child->style.height.value <= 0 ? 1.0f : child->style.height.value);
        h = (total_grow > 0.0f) ? (remaining * (w / total_grow)) : 0.0f;
      } else if (child->style.height.type == ZLAY_SIZE_AUTO) {
        h = measured.height;
      } else {
        h = ZLay__ResolveSize(child->style.height, inner_h, 0.0f);
      }

      child->layout.x = inner_x + child->style.margin_left;
      child->layout.y = cursor + child->style.margin_top;
      float wpx = 0.0f;
      if (child->style.width.type == ZLAY_SIZE_PX) wpx = child->style.width.value;
      else if (child->style.width.type == ZLAY_SIZE_PERCENT) wpx = inner_w * (child->style.width.value / 100.0f);
      else if (child->style.width.type == ZLAY_SIZE_AUTO) wpx = (child->type == ZLAY_NODE_TEXT) ? measured.width : inner_w;
      else wpx = inner_w;
      child->layout.width = wpx - child->style.margin_left - child->style.margin_right;
      child->layout.height = h - child->style.margin_top - child->style.margin_bottom;

      cursor += h + parent->style.gap;
    } else {
      float wpx = 0.0f;
      if (child->style.width.type == ZLAY_SIZE_GROW) {
        const float w = (child->style.width.value <= 0 ? 1.0f : child->style.width.value);
        wpx = (total_grow > 0.0f) ? (remaining * (w / total_grow)) : 0.0f;
      } else if (child->style.width.type == ZLAY_SIZE_AUTO) {
        wpx = measured.width;
      } else {
        wpx = ZLay__ResolveSize(child->style.width, inner_w, 0.0f);
      }

      child->layout.x = cursor + child->style.margin_left;
      child->layout.y = inner_y + child->style.margin_top;
      child->layout.width = wpx - child->style.margin_left - child->style.margin_right;
      float hpx = 0.0f;
      if (child->style.height.type == ZLAY_SIZE_PX) hpx = child->style.height.value;
      else if (child->style.height.type == ZLAY_SIZE_PERCENT) hpx = inner_h * (child->style.height.value / 100.0f);
      else if (child->style.height.type == ZLAY_SIZE_AUTO) hpx = (child->type == ZLAY_NODE_TEXT) ? measured.height : inner_h;
      else hpx = inner_h;
      child->layout.height = hpx - child->style.margin_top - child->style.margin_bottom;

      cursor += wpx + parent->style.gap;
    }

    ZLay__LayoutNode(ctx, child_idx);
  }
}

static float ZLay__GridRowHeight(const ZLay_Context* ctx, const ZLay_Node* parent, const ZLay_Node* child, float inner_h) {
  const ZLay_Dimensions measured = ZLay__MeasureNode(ctx, child);
  const uint32_t rows = parent->style.grid_rows;

  if (parent->style.grid_auto_row_height.type != ZLAY_SIZE_AUTO) {
    return ZLay__ResolveSize(parent->style.grid_auto_row_height, inner_h, measured.height);
  }

  if (child->style.height.type != ZLAY_SIZE_AUTO && child->style.height.type != ZLAY_SIZE_GROW) {
    return ZLay__ResolveSize(child->style.height, inner_h, measured.height);
  }

  if (rows > 0u) {
    const float gap = parent->style.grid_row_gap;
    const float total_gap = rows > 1u ? gap * (float)(rows - 1u) : 0.0f;
    return (inner_h - total_gap) / (float)rows;
  }

  return child->type == ZLAY_NODE_TEXT ? measured.height : inner_h;
}

static void ZLay__LayoutChildren_Grid(ZLay_Context* ctx, ZLay_Node* parent) {
  const float inner_x = parent->layout.x + parent->style.padding_left;
  const float inner_y = parent->layout.y + parent->style.padding_top;
  const float inner_w = parent->layout.width - parent->style.padding_left - parent->style.padding_right;
  const float inner_h = parent->layout.height - parent->style.padding_top - parent->style.padding_bottom;
  const uint32_t columns = parent->style.grid_columns == 0u ? 1u : parent->style.grid_columns;
  const float column_gap = parent->style.grid_column_gap;
  const float row_gap = parent->style.grid_row_gap;
  const float total_column_gap = columns > 1u ? column_gap * (float)(columns - 1u) : 0.0f;
  const float column_width = columns > 0u ? (inner_w - total_column_gap) / (float)columns : inner_w;
  uint32_t cursor = 0u;

  for (int32_t child_idx = parent->first_child; child_idx >= 0; child_idx = ctx->nodes[child_idx].next_sibling) {
    ZLay_Node* child = &ctx->nodes[child_idx];
    uint32_t column_span;
    uint32_t row_span;
    uint32_t column;
    uint32_t row;
    float row_height;
    float cell_width;
    float cell_height;

    if (child->style.position != ZLAY_POSITION_RELATIVE) continue;

    column_span = child->style.grid_column_span == 0u ? 1u : child->style.grid_column_span;
    row_span = child->style.grid_row_span == 0u ? 1u : child->style.grid_row_span;
    if (column_span > columns) column_span = columns;

    column = cursor % columns;
    if (column + column_span > columns) {
      cursor += columns - column;
      column = 0u;
    }
    row = cursor / columns;
    row_height = ZLay__GridRowHeight(ctx, parent, child, inner_h);
    cell_width = (column_width * (float)column_span) + (column_gap * (float)(column_span - 1u));
    cell_height = (row_height * (float)row_span) + (row_gap * (float)(row_span - 1u));

    child->layout.x = inner_x + ((float)column * (column_width + column_gap)) + child->style.margin_left;
    child->layout.y = inner_y + ((float)row * (row_height + row_gap)) + child->style.margin_top;
    child->layout.width = cell_width - child->style.margin_left - child->style.margin_right;
    child->layout.height = cell_height - child->style.margin_top - child->style.margin_bottom;

    cursor += column_span * row_span;
    ZLay__LayoutNode(ctx, child_idx);
  }
}

static void ZLay__LayoutNode(ZLay_Context* ctx, int32_t idx) {
  ZLay_Node* n = &ctx->nodes[idx];

  // Absolute / floating: minimal pass (relative to parent / screen).
  if (n->style.position == ZLAY_POSITION_ABSOLUTE) {
    int32_t parent_idx = n->parent;
    const ZLay_Rect pr = (parent_idx >= 0) ? ctx->nodes[parent_idx].layout : (ZLay_Rect){0, 0, ctx->viewport.width, ctx->viewport.height};
    n->layout.x = pr.x + n->style.left;
    n->layout.y = pr.y + n->style.top;
    if (n->layout.width == 0) n->layout.width = ZLay__ResolveSize(n->style.width, pr.width, 0.0f);
    if (n->layout.height == 0) n->layout.height = ZLay__ResolveSize(n->style.height, pr.height, 0.0f);
  } else if (n->style.position == ZLAY_POSITION_FLOATING) {
    n->layout.x = n->style.left;
    n->layout.y = n->style.top;
    if (n->layout.width == 0) n->layout.width = ZLay__ResolveSize(n->style.width, ctx->viewport.width, 0.0f);
    if (n->layout.height == 0) n->layout.height = ZLay__ResolveSize(n->style.height, ctx->viewport.height, 0.0f);
  }

  if (n->type == ZLAY_NODE_BOX && n->style.layout_mode == ZLAY_LAYOUT_GRID) {
    ZLay__LayoutChildren_Grid(ctx, n);
  } else if (n->type == ZLAY_NODE_BOX) {
    ZLay__LayoutChildren_Flex(ctx, n);
  }
}

void ZLay__ComputeLayout(ZLay_Context* ctx) {
  if (!ctx) return;
  if (ctx->root_index < 0) return;

  ZLay_Node* root = &ctx->nodes[ctx->root_index];
  root->layout.x = 0;
  root->layout.y = 0;
  root->layout.width = (root->style.width.type == ZLAY_SIZE_AUTO) ? ctx->viewport.width : ZLay__ResolveSize(root->style.width, ctx->viewport.width, ctx->viewport.width);
  root->layout.height = (root->style.height.type == ZLAY_SIZE_AUTO) ? ctx->viewport.height : ZLay__ResolveSize(root->style.height, ctx->viewport.height, ctx->viewport.height);

  ZLay__LayoutNode(ctx, ctx->root_index);
}
