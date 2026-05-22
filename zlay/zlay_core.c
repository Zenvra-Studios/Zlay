#include "zlay_internal.h"

static void* ZLay__AllocOrNull(ZLay_Arena* a, size_t bytes, size_t alignment) {
  void* p = ZLay_ArenaAlloc(a, bytes, alignment);
  return p;
}

static ZLay_Dimensions ZLay__DefaultMeasureText(ZLay_String text, void* user) {
  (void)user;
  // Extremely simple fallback: fixed line height + monospace advance.
  const float advance = 8.0f;
  const float line_h = 16.0f;
  return (ZLay_Dimensions){advance * (float)text.length, line_h};
}

uint32_t ZLay_Version(void) {
  return ZLAY_VERSION;
}

const char* ZLay_VersionString(void) {
  return ZLAY_VERSION_STRING;
}

const char* ZLay_LibraryName(void) {
  return "ZLay";
}

const char* ZLay_VendorName(void) {
  return "Zenvra Studio";
}

ZLay_Context* ZLay_Initialize(ZLay_Arena arena, ZLay_Config config) {
  ZLay_Arena a = arena;

  ZLay_Context* ctx = (ZLay_Context*)ZLay__AllocOrNull(&a, sizeof(ZLay_Context), 16);
  if (!ctx) return NULL;

  ctx->config = config;
  ctx->viewport = (ZLay_Dimensions){0};
  ctx->arena = a; // store remaining arena for frame allocations
  ctx->measure_text = ZLay__DefaultMeasureText;
  ctx->measure_text_user = NULL;

  ctx->nodes = (ZLay_Node*)ZLay__AllocOrNull(&ctx->arena, sizeof(ZLay_Node) * (size_t)config.max_nodes, 16);
  ctx->commands = (ZLay_RenderCommand*)ZLay__AllocOrNull(&ctx->arena, sizeof(ZLay_RenderCommand) * (size_t)config.max_commands, 16);
  ctx->open_stack = (int32_t*)ZLay__AllocOrNull(&ctx->arena, sizeof(int32_t) * (size_t)config.max_stack, 16);
  if (!ctx->nodes || !ctx->commands || !ctx->open_stack) return NULL;

  ctx->node_count = 0;
  ctx->command_count = 0;
  ctx->open_stack_count = 0;
  ctx->root_index = -1;

  return ctx;
}

void ZLay_SetLayoutDimensions(ZLay_Context* ctx, ZLay_Dimensions dims) {
  if (!ctx) return;
  ctx->viewport = dims;
}

void ZLay_SetMeasureTextFunction(ZLay_Context* ctx, ZLay_MeasureTextFn fn, void* user) {
  if (!ctx) return;
  ctx->measure_text = fn ? fn : ZLay__DefaultMeasureText;
  ctx->measure_text_user = user;
}

void ZLay_BeginLayout(ZLay_Context* ctx) {
  if (!ctx) return;
  ctx->node_count = 0;
  ctx->command_count = 0;
  ctx->open_stack_count = 0;
  ctx->root_index = -1;
}

static int32_t ZLay__AllocNode(ZLay_Context* ctx) {
  if (ctx->node_count >= ctx->config.max_nodes) return -1;
  const uint32_t idx = ctx->node_count++;
  ZLay_Node* n = &ctx->nodes[idx];
  *n = (ZLay_Node){0};
  n->parent = -1;
  n->first_child = -1;
  n->next_sibling = -1;
  return (int32_t)idx;
}

void ZLay__OpenElement(ZLay_Context* ctx, ZLay_Id id, ZLay_NodeType type) {
  if (!ctx) return;
  if (ctx->open_stack_count >= ctx->config.max_stack) return;

  const int32_t idx = ZLay__AllocNode(ctx);
  if (idx < 0) return;

  ZLay_Node* n = &ctx->nodes[idx];
  n->id = id;
  n->type = type;
  n->style = ZLay_StyleDefault();

  if (ctx->open_stack_count == 0) {
    ctx->root_index = idx;
  } else {
    const int32_t parent_idx = ctx->open_stack[ctx->open_stack_count - 1u];
    ZLay_Node* parent = &ctx->nodes[parent_idx];
    n->parent = parent_idx;
    if (parent->first_child < 0) {
      parent->first_child = idx;
    } else {
      int32_t cur = parent->first_child;
      while (ctx->nodes[cur].next_sibling >= 0) cur = ctx->nodes[cur].next_sibling;
      ctx->nodes[cur].next_sibling = idx;
    }
  }

  ctx->open_stack[ctx->open_stack_count++] = idx;
}

void ZLay__ConfigureOpenElement(ZLay_Context* ctx, const ZLay_NodeDeclaration* decl) {
  if (!ctx || !decl) return;
  if (ctx->open_stack_count == 0) return;
  const int32_t idx = ctx->open_stack[ctx->open_stack_count - 1u];
  ZLay_Node* n = &ctx->nodes[idx];
  n->style = decl->style;
  n->id = decl->id.hash ? decl->id : n->id;
  n->type = decl->type;
  if (decl->type == ZLAY_NODE_TEXT) {
    n->text = decl->text;
  }
}

void ZLay__CloseElement(ZLay_Context* ctx) {
  if (!ctx) return;
  if (ctx->open_stack_count == 0) return;
  ctx->open_stack_count--;
}

void ZLay__OpenTextElement(ZLay_Context* ctx, ZLay_Id id, ZLay_String text, const ZLay_Style* style) {
  if (!ctx) return;
  const int32_t idx = ZLay__AllocNode(ctx);
  if (idx < 0) return;

  ZLay_Node* n = &ctx->nodes[idx];
  n->id = id;
  n->type = ZLAY_NODE_TEXT;
  n->style = style ? *style : ZLay_StyleDefault();
  n->text = text;

  if (ctx->open_stack_count == 0) {
    ctx->root_index = idx;
  } else {
    const int32_t parent_idx = ctx->open_stack[ctx->open_stack_count - 1u];
    ZLay_Node* parent = &ctx->nodes[parent_idx];
    n->parent = parent_idx;
    if (parent->first_child < 0) {
      parent->first_child = idx;
    } else {
      int32_t cur = parent->first_child;
      while (ctx->nodes[cur].next_sibling >= 0) cur = ctx->nodes[cur].next_sibling;
      ctx->nodes[cur].next_sibling = idx;
    }
  }
}

ZLay_RenderCommandList ZLay_EndLayout(ZLay_Context* ctx) {
  if (!ctx) return (ZLay_RenderCommandList){0};

  ZLay__ComputeLayout(ctx);
  ZLay__BuildRenderCommands(ctx);

  ZLay_RenderCommandList list;
  list.commands = ctx->commands;
  list.count = ctx->command_count;
  return list;
}
