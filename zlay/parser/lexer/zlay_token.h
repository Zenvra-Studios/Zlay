#ifndef ZLAY_PARSER_TOKEN_H
#define ZLAY_PARSER_TOKEN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <zlay.h>

typedef enum ZLay_TokenType {
  ZLAY_TOKEN_EOF = 0,
  ZLAY_TOKEN_ERROR,
  ZLAY_TOKEN_IDENT,
  ZLAY_TOKEN_STRING,
  ZLAY_TOKEN_NUMBER,
  ZLAY_TOKEN_COLOR,
  ZLAY_TOKEN_LBRACE,
  ZLAY_TOKEN_RBRACE,
  ZLAY_TOKEN_LPAREN,
  ZLAY_TOKEN_RPAREN,
  ZLAY_TOKEN_COLON,
  ZLAY_TOKEN_COMMA,
  ZLAY_TOKEN_DOT,
  ZLAY_TOKEN_HASH,
  ZLAY_TOKEN_PERCENT,
  ZLAY_TOKEN_TRUE,
  ZLAY_TOKEN_FALSE,
  ZLAY_TOKEN_NULL
} ZLay_TokenType;

typedef struct ZLay_Token {
  ZLay_TokenType type;
  ZLay_String lexeme;
  double number;
  ZLay_Color color;
  uint32_t line;
  uint32_t column;
} ZLay_Token;

ZLAY_API const char* ZLay_TokenTypeName(ZLay_TokenType type);
ZLAY_API bool ZLay_TokenIsValue(ZLay_TokenType type);

#ifdef __cplusplus
}
#endif

#endif
