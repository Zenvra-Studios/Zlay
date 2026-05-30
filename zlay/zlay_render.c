#include <zlay_internal.h>

static ZLay_Id ZLay__DerivedCommandId(ZLay_Id id, uint64_t salt) {
  id.hash ^= salt + 0x9E3779B97F4A7C15ull + (id.hash << 6u) + (id.hash >> 2u);
  return id;
}

static ZLay_Dimensions ZLay__MeasureTextSpan(const ZLay_Context* ctx, ZLay_String text, uint32_t start, uint32_t end) {
  ZLay_String span;

  if (start > text.length) start = text.length;
  if (end > text.length) end = text.length;
  if (end < start) end = start;

  span.chars = text.chars != NULL ? text.chars + start : NULL;
  span.length = end - start;
  span.is_static = text.is_static;

  if (ctx != NULL && ctx->measure_text != NULL) {
    return ctx->measure_text(span, ctx->measure_text_user);
  }

  return (ZLay_Dimensions){8.0f * (float)span.length, text.length > 0u ? 16.0f : 0.0f};
}

static void ZLay__PushTextRectCommand(
  ZLay_Context* ctx,
  ZLay_Id id,
  ZLay_Rect bounds,
  ZLay_Color color,
  int32_t z_index,
  ZLay_PointerCursor pointer_cursor
) {
  ZLay_RenderCommand* c;

  if (ctx->command_count >= ctx->config.max_commands || bounds.width <= 0.0f || bounds.height <= 0.0f) return;

  c = &ctx->commands[ctx->command_count++];
  *c = (ZLay_RenderCommand){0};
  c->type = ZLAY_CMD_RECT;
  c->id = id;
  c->bounds = bounds;
  c->color = color;
  c->radius = 0.0f;
  c->rounded = ZLay_RoundedCircular(0.0f);
  c->border_color = (ZLay_Color){0};
  c->border_width = 0.0f;
  c->shadow = ZLay_ShadowNone();
  c->z_index = z_index;
  c->text = (ZLay_String){0};
  c->font_family = (ZLay_String){0};
  c->font_size = 0.0f;
  c->pointer_cursor = pointer_cursor;
}

static void ZLay__PushRectCommand(ZLay_Context* ctx, const ZLay_Node* n) {
  if (ctx->command_count >= ctx->config.max_commands) return;
  ZLay_RenderCommand* c = &ctx->commands[ctx->command_count++];
  *c = (ZLay_RenderCommand){0};
  c->type = ZLAY_CMD_RECT;
  c->id = n->id;
  c->bounds = n->layout;
  c->color = n->style.background;
  c->rounded = ZLay_RoundedResolve(n->style.rounded.radius > 0.0f ? n->style.rounded : ZLay_RoundedCircular(n->style.radius));
  c->radius = ZLay_RoundedEffectiveRadius(c->rounded, n->layout.width, n->layout.height);
  c->border_color = n->style.border_color;
  c->border_width = n->style.border_width;
  c->shadow = n->style.shadow;
  c->z_index = n->style.z_index;
  c->text = (ZLay_String){0};
  c->font_family = n->style.font_family;
  c->font_size = n->style.font_size;
  c->pointer_cursor = n->style.pointer_cursor;
}

static void ZLay__PushTextCommand(ZLay_Context* ctx, const ZLay_Node* n) {
  if (ctx->command_count >= ctx->config.max_commands) return;
  ZLay_RenderCommand* c = &ctx->commands[ctx->command_count++];
  *c = (ZLay_RenderCommand){0};
  c->type = ZLAY_CMD_TEXT;
  c->id = n->id;
  c->bounds = n->layout;
  c->color = n->style.text_color;
  c->radius = 0;
  c->rounded = ZLay_RoundedCircular(0.0f);
  c->border_color = (ZLay_Color){0};
  c->border_width = 0.0f;
  c->shadow = ZLay_ShadowNone();
  c->z_index = n->style.z_index;
  c->text = n->text;
  c->font_family = n->style.font_family;
  c->font_size = n->style.font_size;
  c->pointer_cursor = n->style.pointer_cursor;
}

static void ZLay__PushBoxShadowCommands(ZLay_Context* ctx, const ZLay_Node* n) {
  const ZLay_Shadow shadow = n->style.shadow;
  const float blur = shadow.blur < 0.0f ? 0.0f : shadow.blur;
  const float spread = shadow.spread;
  const uint32_t layers = blur > 0.0f ? 8u : 1u;
  uint32_t i;

  if (shadow.color.a == 0u) return;

  for (i = 0u; i < layers; ++i) {
    const float t = layers > 1u ? ((float)i + 1.0f) / (float)layers : 1.0f;
    const float falloff = (1.0f - t) * (1.0f - t);
    const float expand = spread + blur * t;
    const uint8_t alpha = (uint8_t)((float)shadow.color.a * (layers > 1u ? falloff : 1.0f) * 0.32f);
    ZLay_Color color = shadow.color;
    ZLay_Rect bounds;

    if (alpha == 0u) continue;

    color.a = alpha;
    bounds.x = n->layout.x + shadow.offset_x - expand;
    bounds.y = n->layout.y + shadow.offset_y - expand;
    bounds.width = n->layout.width + expand * 2.0f;
    bounds.height = n->layout.height + expand * 2.0f;
    ZLay__PushTextRectCommand(
      ctx,
      ZLay__DerivedCommandId(n->id, 0x5AADC001ull + (uint64_t)i),
      bounds,
      color,
      n->style.z_index - 1,
      ZLAY_POINTER_CURSOR_DEFAULT
    );
    if (ctx->command_count > 0u) {
      ZLay_RenderCommand* c = &ctx->commands[ctx->command_count - 1u];
      c->radius = n->style.radius + expand;
      c->rounded = ZLay_RoundedCircular(c->radius);
    }
  }
}

static void ZLay__PushBoxBorderCommands(ZLay_Context* ctx, const ZLay_Node* n) {
  const float width = n->style.border_width;
  ZLay_Rect bounds;

  if (width <= 0.0f || n->style.border_color.a == 0u) return;

  bounds = n->layout;
  bounds.height = width;
  ZLay__PushTextRectCommand(ctx, ZLay__DerivedCommandId(n->id, 0xB0ADEF10ull), bounds, n->style.border_color, n->style.z_index, n->style.pointer_cursor);

  bounds = n->layout;
  bounds.y = n->layout.y + n->layout.height - width;
  bounds.height = width;
  ZLay__PushTextRectCommand(ctx, ZLay__DerivedCommandId(n->id, 0xB0ADEF11ull), bounds, n->style.border_color, n->style.z_index, n->style.pointer_cursor);

  bounds = n->layout;
  bounds.width = width;
  ZLay__PushTextRectCommand(ctx, ZLay__DerivedCommandId(n->id, 0xB0ADEF12ull), bounds, n->style.border_color, n->style.z_index, n->style.pointer_cursor);

  bounds = n->layout;
  bounds.x = n->layout.x + n->layout.width - width;
  bounds.width = width;
  ZLay__PushTextRectCommand(ctx, ZLay__DerivedCommandId(n->id, 0xB0ADEF13ull), bounds, n->style.border_color, n->style.z_index, n->style.pointer_cursor);
}

static void ZLay__PushTextSelectionCommand(ZLay_Context* ctx, const ZLay_Node* n) {
  uint32_t start;
  uint32_t end;
  ZLay_Dimensions prefix;
  ZLay_Dimensions selected;
  ZLay_Rect bounds;

  if (!n->style.text_selection_visible || n->text.chars == NULL || n->text.length == 0u) return;

  start = n->style.text_selection_start;
  end = n->style.text_selection_end;
  if (start > end) {
    uint32_t tmp = start;
    start = end;
    end = tmp;
  }
  if (start > n->text.length) start = n->text.length;
  if (end > n->text.length) end = n->text.length;
  if (start == end) return;

  prefix = ZLay__MeasureTextSpan(ctx, n->text, 0u, start);
  selected = ZLay__MeasureTextSpan(ctx, n->text, start, end);
  bounds = n->layout;
  bounds.x += prefix.width;
  bounds.width = selected.width;
  ZLay__PushTextRectCommand(
    ctx,
    ZLay__DerivedCommandId(n->id, 0x5E1EC710Full),
    bounds,
    n->style.text_selection_color,
    n->style.z_index,
    n->style.pointer_cursor
  );
}

static void ZLay__PushTextCursorCommand(ZLay_Context* ctx, const ZLay_Node* n) {
  ZLay_Dimensions prefix;
  ZLay_Rect bounds;
  float width;

  if (!n->style.text_cursor_visible) return;

  prefix = ZLay__MeasureTextSpan(ctx, n->text, 0u, n->style.text_cursor_index);
  width = n->style.text_cursor_width > 0.0f ? n->style.text_cursor_width : 1.0f;
  bounds = n->layout;
  bounds.x += prefix.width;
  bounds.width = width;
  ZLay__PushTextRectCommand(
    ctx,
    ZLay__DerivedCommandId(n->id, 0xC012501Full),
    bounds,
    n->style.text_cursor_color,
    n->style.z_index,
    n->style.pointer_cursor
  );
}

static void ZLay__Walk(ZLay_Context* ctx, int32_t idx) {
  const ZLay_Node* n = &ctx->nodes[idx];

  if (n->type == ZLAY_NODE_BOX) {
    ZLay__PushBoxShadowCommands(ctx, n);
    if (n->style.background.a != 0) ZLay__PushRectCommand(ctx, n);
    ZLay__PushBoxBorderCommands(ctx, n);
  } else if (n->type == ZLAY_NODE_TEXT) {
    ZLay__PushTextSelectionCommand(ctx, n);
    ZLay__PushTextCommand(ctx, n);
    ZLay__PushTextCursorCommand(ctx, n);
  }

  for (int32_t child = n->first_child; child >= 0; child = ctx->nodes[child].next_sibling) {
    ZLay__Walk(ctx, child);
  }
}

void ZLay__BuildRenderCommands(ZLay_Context* ctx) {
  if (!ctx) return;
  ctx->command_count = 0;
  if (ctx->root_index < 0) return;
  ZLay__Walk(ctx, ctx->root_index);
}
