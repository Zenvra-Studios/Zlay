#include <zlay_internal.h>

static void* ZLay__AllocOrNull(ZLay_Arena* arena, size_t bytes, size_t alignment) {
  return ZLay_ArenaAlloc(arena, bytes, alignment);
}

static ZLay_Dimensions ZLay__DefaultMeasureText(ZLay_String text, void* user) {
  (void)user;
  return (ZLay_Dimensions){8.0f * (float)text.length, 16.0f};
}

uint32_t ZLay_Version(void) {
  return 0x00010000u;
}

const char* ZLay_VersionString(void) {
  return "0.1.0";
}

const char* ZLay_LibraryName(void) {
  return "ZLay";
}

const char* ZLay_VendorName(void) {
  return "Zenvra Studio";
}

ZLay_Context* ZLay_Initialize(ZLay_Arena arena, ZLay_Config config) {
  ZLay_Arena cursor = arena;
  ZLay_Context* ctx = (ZLay_Context*)ZLay__AllocOrNull(&cursor, sizeof(ZLay_Context), 16u);
  if (ctx == NULL) {
    return NULL;
  }

  ctx->config = config;
  ctx->viewport = (ZLay_Dimensions){0};
  ctx->arena = cursor;
  ctx->measure_text = ZLay__DefaultMeasureText;
  ctx->measure_text_user = NULL;

  ctx->nodes = (ZLay_Node*)ZLay__AllocOrNull(&ctx->arena, sizeof(ZLay_Node) * (size_t)config.max_nodes, 16u);
  ctx->commands = (ZLay_RenderCommand*)ZLay__AllocOrNull(
    &ctx->arena,
    sizeof(ZLay_RenderCommand) * (size_t)config.max_commands,
    16u
  );
  ctx->open_stack = (int32_t*)ZLay__AllocOrNull(&ctx->arena, sizeof(int32_t) * (size_t)config.max_stack, 16u);
  if (ctx->nodes == NULL || ctx->commands == NULL || ctx->open_stack == NULL) {
    return NULL;
  }

  ctx->frame_arena_offset = ctx->arena.offset;
  ctx->node_count = 0u;
  ctx->command_count = 0u;
  ctx->open_stack_count = 0u;
  ctx->root_index = -1;
  return ctx;
}

void ZLay_SetLayoutDimensions(ZLay_Context* ctx, ZLay_Dimensions dims) {
  if (ctx != NULL) {
    ctx->viewport = dims;
  }
}

void ZLay_SetMeasureTextFunction(ZLay_Context* ctx, ZLay_MeasureTextFn fn, void* user) {
  if (ctx == NULL) {
    return;
  }
  ctx->measure_text = fn != NULL ? fn : ZLay__DefaultMeasureText;
  ctx->measure_text_user = user;
}

void ZLay_BeginLayout(ZLay_Context* ctx) {
  if (ctx == NULL) {
    return;
  }
  ctx->node_count = 0u;
  ctx->command_count = 0u;
  ctx->open_stack_count = 0u;
  ctx->root_index = -1;
  ctx->arena.offset = ctx->frame_arena_offset;
}

static int32_t ZLay__AllocNode(ZLay_Context* ctx) {
  uint32_t index;
  ZLay_Node* node;

  if (ctx == NULL || ctx->node_count >= ctx->config.max_nodes) {
    return -1;
  }

  index = ctx->node_count++;
  node = &ctx->nodes[index];
  *node = (ZLay_Node){0};
  node->parent = -1;
  node->first_child = -1;
  node->next_sibling = -1;
  return (int32_t)index;
}

static void ZLay__AttachNode(ZLay_Context* ctx, int32_t index) {
  if (ctx->open_stack_count == 0u) {
    ctx->root_index = index;
  } else {
    int32_t parent_index = ctx->open_stack[ctx->open_stack_count - 1u];
    ZLay_Node* parent = &ctx->nodes[parent_index];
    ctx->nodes[index].parent = parent_index;
    if (parent->first_child < 0) {
      parent->first_child = index;
    } else {
      int32_t current = parent->first_child;
      while (ctx->nodes[current].next_sibling >= 0) {
        current = ctx->nodes[current].next_sibling;
      }
      ctx->nodes[current].next_sibling = index;
    }
  }
}

void ZLay__OpenElement(ZLay_Context* ctx, ZLay_Id id, ZLay_NodeType type) {
  int32_t index;
  ZLay_Node* node;

  if (ctx == NULL || ctx->open_stack_count >= ctx->config.max_stack) {
    return;
  }

  index = ZLay__AllocNode(ctx);
  if (index < 0) {
    return;
  }

  node = &ctx->nodes[index];
  node->id = id;
  node->type = type;
  node->style = ZLay_StyleDefault();
  ZLay__AttachNode(ctx, index);
  ctx->open_stack[ctx->open_stack_count++] = index;
}

void ZLay__ConfigureOpenElement(ZLay_Context* ctx, const ZLay_NodeDeclaration* decl) {
  int32_t index;
  ZLay_Node* node;

  if (ctx == NULL || decl == NULL || ctx->open_stack_count == 0u) {
    return;
  }

  index = ctx->open_stack[ctx->open_stack_count - 1u];
  node = &ctx->nodes[index];
  node->style = decl->style;
  node->id = decl->id.hash != 0u ? decl->id : node->id;
  node->type = decl->type;
  if (decl->type == ZLAY_NODE_TEXT) {
    node->text = decl->text;
  }
}

void ZLay__CloseElement(ZLay_Context* ctx) {
  if (ctx != NULL && ctx->open_stack_count != 0u) {
    --ctx->open_stack_count;
  }
}

void ZLay__OpenTextElement(ZLay_Context* ctx, ZLay_Id id, ZLay_String text, const ZLay_Style* style) {
  int32_t index;
  ZLay_Node* node;

  if (ctx == NULL) {
    return;
  }

  index = ZLay__AllocNode(ctx);
  if (index < 0) {
    return;
  }

  node = &ctx->nodes[index];
  node->id = id;
  node->type = ZLAY_NODE_TEXT;
  node->style = style != NULL ? *style : ZLay_StyleDefault();
  node->text = text;
  ZLay__AttachNode(ctx, index);
}

ZLay_RenderCommandList ZLay_EndLayout(ZLay_Context* ctx) {
  ZLay_RenderCommandList list;

  if (ctx == NULL) {
    list.commands = NULL;
    list.count = 0u;
    return list;
  }

  ZLay__ComputeLayout(ctx);
  ZLay__BuildRenderCommands(ctx);

  list.commands = ctx->commands;
  list.count = ctx->command_count;
  return list;
}
