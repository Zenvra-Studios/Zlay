#ifndef ZLAY_PARSER_LEXER_H
#define ZLAY_PARSER_LEXER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <parser/lexer/zlay_token.h>

typedef struct ZLay_Lexer {
  ZLay_String source;
  uint32_t offset;
  uint32_t line;
  uint32_t column;
} ZLay_Lexer;

ZLAY_API void ZLay_LexerInit(ZLay_Lexer* lexer, ZLay_String source);
ZLAY_API ZLay_Token ZLay_LexerNext(ZLay_Lexer* lexer);

#ifdef __cplusplus
}
#endif

#endif
