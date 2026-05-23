#ifndef ZLAY_PARSER_STYLE_RESOLVER_H
#define ZLAY_PARSER_STYLE_RESOLVER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <parser/ast/zlay_ast.h>

ZLAY_API bool ZLay_ResolveStyle(const ZLay_AstNode* node, ZLay_Style* out_style);
ZLAY_API ZLay_Size ZLay_ResolveSizeValue(const ZLay_Value* value);

#ifdef __cplusplus
}
#endif

#endif
