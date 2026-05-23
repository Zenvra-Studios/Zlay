#ifndef ZLAY_PARSER_RESOLVER_H
#define ZLAY_PARSER_RESOLVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

#include <parser/semantic/zlay_semantic.h>
#include <parser/syntax/zlay_parser.h>

typedef struct ZLay_Resolver ZLay_Resolver;
typedef bool (*ZLay_WidgetBuildFn)(
  ZLay_Resolver* resolver,
  ZLay_Context* ctx,
  const ZLay_AstNode* node,
  void* user
);

#ifndef ZLAY_RESOLVER_MAX_WIDGETS
#  define ZLAY_RESOLVER_MAX_WIDGETS 64u
#endif

typedef struct ZLay_WidgetEntry {
  ZLay_String type;
  ZLay_WidgetBuildFn build;
  void* user;
} ZLay_WidgetEntry;

struct ZLay_Resolver {
  ZLay_WidgetEntry widgets[ZLAY_RESOLVER_MAX_WIDGETS];
  uint32_t widget_count;
};

ZLAY_API void ZLay_ResolverInit(ZLay_Resolver* resolver);
ZLAY_API bool ZLay_ResolverRegister(
  ZLay_Resolver* resolver,
  const char* type,
  ZLay_WidgetBuildFn build,
  void* user
);
ZLAY_API bool ZLay_ResolverBuildChildren(
  ZLay_Resolver* resolver,
  ZLay_Context* ctx,
  const ZLay_AstNode* node
);
ZLAY_API ZLay_String ZLay_ResolverCopyString(ZLay_Context* ctx, ZLay_String source);

ZLAY_API bool ZLay_BuildFromAst(ZLay_Context* ctx, const ZLay_AstNode* root);
ZLAY_API bool ZLay_BuildFromAstWithResolver(
  ZLay_Context* ctx,
  const ZLay_AstNode* root,
  ZLay_Resolver* resolver
);
/* Keeps out_result->root valid for as long as scratch remains alive. */
ZLAY_API bool ZLay_BuildFromStringEx(
  ZLay_Context* ctx,
  ZLay_Arena* scratch,
  ZLay_String source,
  ZLay_ParseResult* out_result
);
ZLAY_API bool ZLay_BuildFromStringExWithResolver(
  ZLay_Context* ctx,
  ZLay_Arena* scratch,
  ZLay_String source,
  ZLay_Resolver* resolver,
  ZLay_ParseResult* out_result
);
ZLAY_API bool ZLay_BuildFromStringWithResolver(
  ZLay_Context* ctx,
  ZLay_String source,
  ZLay_Resolver* resolver,
  ZLay_ParseResult* out_result
);
/* Convenience API for one-shot builds; out_result->root is cleared after build. */
ZLAY_API bool ZLay_BuildFromString(
  ZLay_Context* ctx,
  ZLay_String source,
  ZLay_ParseResult* out_result
);
ZLAY_API size_t ZLay_ParseScratchSize(ZLay_String source);

#ifdef __cplusplus
}
#endif

#endif
