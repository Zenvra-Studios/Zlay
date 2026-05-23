#ifndef ZLAY_PARSER_SEMANTIC_H
#define ZLAY_PARSER_SEMANTIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <parser/ast/zlay_ast.h>

typedef struct ZLay_SemanticResult {
  bool success;
  ZLay_String error_message;
  uint32_t error_line;
  uint32_t error_column;
} ZLay_SemanticResult;

ZLAY_API ZLay_SemanticResult ZLay_SemanticValidate(const ZLay_AstNode* root);

#ifdef __cplusplus
}
#endif

#endif
