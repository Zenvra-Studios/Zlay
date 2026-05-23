#ifndef ZLAY_PARSER_SYNTAX_PARSER_H
#define ZLAY_PARSER_SYNTAX_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <parser/ast/zlay_ast.h>
#include <parser/lexer/zlay_lexer.h>

typedef struct ZLay_ParseResult {
  bool success;
  ZLay_AstNode* root;
  ZLay_String error_message;
  uint32_t error_line;
  uint32_t error_column;
} ZLay_ParseResult;

ZLAY_API ZLay_ParseResult ZLay_Parse(ZLay_Arena* arena, ZLay_String source);

#ifdef __cplusplus
}
#endif

#endif
