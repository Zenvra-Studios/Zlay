#include <parser/resolver/zlay_resolver.h>

#include <stdlib.h>

#include <parser/resolver/zlay_style_resolver.h>
#include <zlay_internal.h>
#include <zlay_ui.h>

void ZLay_ResolverInit(ZLay_Resolver* resolver) {
  if (resolver != NULL) {
    resolver->widget_count = 0u;
  }
}

bool ZLay_ResolverRegister(
  ZLay_Resolver* resolver,
  const char* type,
  ZLay_WidgetBuildFn build,
  void* user
) {
  ZLay_WidgetEntry* entry;

  if (resolver == NULL || type == NULL || build == NULL ||
      resolver->widget_count >= ZLAY_RESOLVER_MAX_WIDGETS) {
    return false;
  }

  entry = &resolver->widgets[resolver->widget_count++];
  entry->type = ZLay_ParserStringFromCString(type);
  entry->build = build;
  entry->user = user;
  return true;
}

static ZLay_WidgetEntry* ZLay__ResolverFindWidget(ZLay_Resolver* resolver, ZLay_String type) {
  if (resolver == NULL) {
    return NULL;
  }

  for (uint32_t i = 0u; i < resolver->widget_count; ++i) {
    if (ZLay_StringEquals(resolver->widgets[i].type, type)) {
      return &resolver->widgets[i];
    }
  }

  return NULL;
}

static ZLay_String ZLay__EmptyString(void) {
  return ZLay_ParserStringFromCString("");
}

static const ZLay_AstProp* ZLay__FindFirstTextProp(const ZLay_AstNode* node) {
  const ZLay_AstProp* prop = ZLay_AstFindProp(node, "value");
  if (prop != NULL) return prop;
  prop = ZLay_AstFindProp(node, "text");
  if (prop != NULL) return prop;
  return ZLay_AstFindProp(node, "label");
}

static ZLay_String ZLay__PropStringValue(const ZLay_AstProp* prop) {
  if (prop == NULL) {
    return ZLay__EmptyString();
  }
  if (prop->value.type == ZLAY_VALUE_STRING || prop->value.type == ZLAY_VALUE_IDENT) {
    return prop->value.string;
  }
  return ZLay__EmptyString();
}

ZLay_String ZLay_ResolverCopyString(ZLay_Context* ctx, ZLay_String source) {
  ZLay_String result = ZLay__EmptyString();
  char* chars;
  uint32_t write = 0u;

  if (ctx == NULL) {
    result.chars = NULL;
    result.length = 0u;
    result.is_static = false;
    return result;
  }

  if (source.length == 0u) {
    return result;
  }

  chars = (char*)ZLay_ArenaAlloc(&ctx->arena, (size_t)source.length + 1u, 1u);
  if (chars == NULL) {
    result.chars = NULL;
    result.length = 0u;
    result.is_static = false;
    return result;
  }

  for (uint32_t read = 0u; read < source.length; ++read) {
    char c = source.chars[read];
    if (c == '\\' && read + 1u < source.length) {
      char escaped = source.chars[++read];
      switch (escaped) {
        case 'n': chars[write++] = '\n'; break;
        case 'r': chars[write++] = '\r'; break;
        case 't': chars[write++] = '\t'; break;
        case '"': chars[write++] = '"'; break;
        case '\\': chars[write++] = '\\'; break;
        default:
          chars[write++] = escaped;
          break;
      }
    } else {
      chars[write++] = c;
    }
  }

  chars[write] = '\0';
  result.chars = chars;
  result.length = write;
  result.is_static = false;
  return result;
}

static ZLay_Id ZLay__NodeId(const ZLay_AstNode* node) {
  const ZLay_AstProp* id_prop = ZLay_AstFindProp(node, "id");
  const ZLay_AstProp* key_prop = ZLay_AstFindProp(node, "key");
  ZLay_Id id;

  if (id_prop != NULL) {
    return ZLay_IdFromString(ZLay__PropStringValue(id_prop));
  }
  if (key_prop != NULL) {
    return ZLay_IdFromString(ZLay__PropStringValue(key_prop));
  }

  id = ZLay_IdFromString(node->type);
  id.hash ^= ((uint64_t)node->line * 1099511628211ull);
  id.hash ^= ((uint64_t)node->column * 1469598103934665603ull);
  return id;
}

static bool ZLay__BuildNode(ZLay_Resolver* resolver, ZLay_Context* ctx, const ZLay_AstNode* node);

static bool ZLay__BuildChildren(ZLay_Resolver* resolver, ZLay_Context* ctx, const ZLay_AstNode* node) {
  for (uint32_t i = 0u; i < node->child_count; ++i) {
    if (!ZLay__BuildNode(resolver, ctx, &node->children[i])) {
      return false;
    }
  }
  return true;
}

bool ZLay_ResolverBuildChildren(
  ZLay_Resolver* resolver,
  ZLay_Context* ctx,
  const ZLay_AstNode* node
) {
  if (ctx == NULL || node == NULL) {
    return false;
  }
  return ZLay__BuildChildren(resolver, ctx, node);
}

static bool ZLay__BuildText(ZLay_Context* ctx, const ZLay_AstNode* node) {
  ZLay_Style style;
  const ZLay_AstProp* text_prop = ZLay__FindFirstTextProp(node);
  ZLay_String text = ZLay__PropStringValue(text_prop);
  ZLay_String frame_text = ZLay_ResolverCopyString(ctx, text);
  if (text.length != 0u && frame_text.chars == NULL) {
    return false;
  }
  ZLay_ResolveStyle(node, &style);
  ZLay__OpenTextElement(ctx, ZLay__NodeId(node), frame_text, &style);
  return true;
}

static bool ZLay__BuildButton(ZLay_Context* ctx, const ZLay_AstNode* node) {
  ZLay_Style style;
  bool has_custom_style = ZLay_ResolveStyle(node, &style);
  const ZLay_AstProp* label_prop = ZLay__FindFirstTextProp(node);
  ZLay_String label = ZLay__PropStringValue(label_prop);
  ZLay_String frame_label = ZLay_ResolverCopyString(ctx, label);
  ZLay_ButtonProps props = ZLay_ButtonPropsDefault(ZLay_ThemeDefault());
  const ZLay_AstProp* disabled = ZLay_AstFindProp(node, "disabled");

  if (label.length != 0u && frame_label.chars == NULL) {
    return false;
  }

  if (has_custom_style) {
    props.style = style;
    props.use_custom_style = true;
  }
  if (disabled != NULL && disabled->value.type == ZLAY_VALUE_BOOL && disabled->value.bool_value) {
    props.state |= ZLAY_COMPONENT_STATE_DISABLED;
  }

  ZLay_Button(ctx, ZLay__NodeId(node), frame_label, &props);
  return true;
}

static bool ZLay__BuildBox(ZLay_Resolver* resolver, ZLay_Context* ctx, const ZLay_AstNode* node) {
  ZLay_Style style;
  ZLay_NodeDeclarationBuilder builder;

  ZLay_ResolveStyle(node, &style);
  builder = ZLay_BoxDecl(ZLay__NodeId(node), style);
  ZLay__OpenElement(ctx, builder.decl.id, ZLAY_NODE_BOX);
  ZLay__ConfigureOpenElement(ctx, &builder.decl);
  if (!ZLay__BuildChildren(resolver, ctx, node)) {
    ZLay__CloseElement(ctx);
    return false;
  }
  ZLay__CloseElement(ctx);
  return true;
}

static bool ZLay__BuildNode(ZLay_Resolver* resolver, ZLay_Context* ctx, const ZLay_AstNode* node) {
  ZLay_WidgetEntry* widget;

  if (ctx == NULL || node == NULL) {
    return false;
  }

  if (ZLay_StringEqualsCString(node->type, "Document")) {
    return ZLay__BuildChildren(resolver, ctx, node);
  }

  widget = ZLay__ResolverFindWidget(resolver, node->type);
  if (widget != NULL) {
    return widget->build(resolver, ctx, node, widget->user);
  }

  if (ZLay_StringEqualsCString(node->type, "Text")) {
    return ZLay__BuildText(ctx, node);
  }
  if (ZLay_StringEqualsCString(node->type, "Button")) {
    return ZLay__BuildButton(ctx, node);
  }

  return ZLay__BuildBox(resolver, ctx, node);
}

bool ZLay_BuildFromAst(ZLay_Context* ctx, const ZLay_AstNode* root) {
  return ZLay_BuildFromAstWithResolver(ctx, root, NULL);
}

bool ZLay_BuildFromAstWithResolver(
  ZLay_Context* ctx,
  const ZLay_AstNode* root,
  ZLay_Resolver* resolver
) {
  if (ctx == NULL || root == NULL) {
    return false;
  }
  return ZLay__BuildNode(resolver, ctx, root);
}

size_t ZLay_ParseScratchSize(ZLay_String source) {
  size_t base = 16u * 1024u;
  size_t per_char = (size_t)source.length * 48u;
  return base + per_char;
}

bool ZLay_BuildFromStringEx(
  ZLay_Context* ctx,
  ZLay_Arena* scratch,
  ZLay_String source,
  ZLay_ParseResult* out_result
) {
  return ZLay_BuildFromStringExWithResolver(ctx, scratch, source, NULL, out_result);
}

bool ZLay_BuildFromStringExWithResolver(
  ZLay_Context* ctx,
  ZLay_Arena* scratch,
  ZLay_String source,
  ZLay_Resolver* resolver,
  ZLay_ParseResult* out_result
) {
  ZLay_ParseResult parse;
  ZLay_SemanticResult semantic;

  if (out_result != NULL) {
    out_result->success = false;
    out_result->root = NULL;
    out_result->error_message = ZLay_ParserStringFromCString(NULL);
    out_result->error_line = 0u;
    out_result->error_column = 0u;
  }

  if (ctx == NULL || scratch == NULL) {
    if (out_result != NULL) {
      out_result->error_message = ZLay_ParserStringFromCString("context or parser scratch arena is null");
    }
    return false;
  }

  parse = ZLay_Parse(scratch, source);
  if (!parse.success) {
    if (out_result != NULL) {
      *out_result = parse;
    }
    return false;
  }

  semantic = ZLay_SemanticValidate(parse.root);
  if (!semantic.success) {
    parse.success = false;
    parse.error_message = semantic.error_message;
    parse.error_line = semantic.error_line;
    parse.error_column = semantic.error_column;
    if (out_result != NULL) {
      *out_result = parse;
    }
    return false;
  }

  parse.success = ZLay_BuildFromAstWithResolver(ctx, parse.root, resolver);
  if (!parse.success) {
    parse.error_message = ZLay_ParserStringFromCString("failed to build ZLay tree from AST");
  }
  if (out_result != NULL) {
    *out_result = parse;
  }
  return parse.success;
}

bool ZLay_BuildFromString(
  ZLay_Context* ctx,
  ZLay_String source,
  ZLay_ParseResult* out_result
) {
  return ZLay_BuildFromStringWithResolver(ctx, source, NULL, out_result);
}

bool ZLay_BuildFromStringWithResolver(
  ZLay_Context* ctx,
  ZLay_String source,
  ZLay_Resolver* resolver,
  ZLay_ParseResult* out_result
) {
  size_t size = ZLay_ParseScratchSize(source);
  void* memory = malloc(size);
  ZLay_Arena arena;
  bool ok;

  if (memory == NULL) {
    if (out_result != NULL) {
      out_result->success = false;
      out_result->root = NULL;
      out_result->error_message = ZLay_ParserStringFromCString("out of parser memory");
      out_result->error_line = 0u;
      out_result->error_column = 0u;
    }
    return false;
  }

  arena = ZLay_CreateArenaWithCapacityAndMemory(size, memory);
  ok = ZLay_BuildFromStringExWithResolver(ctx, &arena, source, resolver, out_result);
  if (out_result != NULL) {
    out_result->root = NULL;
  }
  free(memory);
  return ok;
}
