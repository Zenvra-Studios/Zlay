#ifndef ZLAY_PARSER_ERROR_HANDLING_H
#define ZLAY_PARSER_ERROR_HANDLING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <parser/ast/zlay_ast.h>

typedef struct ZLay_ParseError {
  ZLay_String message;
  uint32_t line;
  uint32_t column;
} ZLay_ParseError;

static inline ZLay_ParseError ZLay_ParseErrorNone(void) {
  ZLay_ParseError error;
  error.message = ZLay_ParserStringFromCString(NULL);
  error.line = 0u;
  error.column = 0u;
  return error;
}

#ifdef __cplusplus
}
#endif

#endif
