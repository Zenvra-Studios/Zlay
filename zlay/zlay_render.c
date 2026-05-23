#include <zlay_internal.h>

static void ZLay__PushRectCommand(ZLay_Context* ctx, const ZLay_Node* n) {
  if (ctx->command_count >= ctx->config.max_commands) return;
  ZLay_RenderCommand* c = &ctx->commands[ctx->command_count++];
  c->type = ZLAY_CMD_RECT;
  c->bounds = n->layout;
  c->color = n->style.background;
  c->radius = n->style.radius;
  c->z_index = n->style.z_index;
  c->text = (ZLay_String){0};
}

static void ZLay__PushTextCommand(ZLay_Context* ctx, const ZLay_Node* n) {
  if (ctx->command_count >= ctx->config.max_commands) return;
  ZLay_RenderCommand* c = &ctx->commands[ctx->command_count++];
  c->type = ZLAY_CMD_TEXT;
  c->bounds = n->layout;
  c->color = n->style.text_color;
  c->radius = 0;
  c->z_index = n->style.z_index;
  c->text = n->text;
}

static void ZLay__Walk(ZLay_Context* ctx, int32_t idx) {
  const ZLay_Node* n = &ctx->nodes[idx];

  if (n->type == ZLAY_NODE_BOX) {
    if (n->style.background.a != 0) ZLay__PushRectCommand(ctx, n);
  } else if (n->type == ZLAY_NODE_TEXT) {
    ZLay__PushTextCommand(ctx, n);
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
