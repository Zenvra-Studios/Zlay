#include <parser/lexer/zlay_token.h>

const char* ZLay_TokenTypeName(ZLay_TokenType type) {
  switch (type) {
    case ZLAY_TOKEN_EOF: return "EOF";
    case ZLAY_TOKEN_ERROR: return "ERROR";
    case ZLAY_TOKEN_IDENT: return "IDENT";
    case ZLAY_TOKEN_STRING: return "STRING";
    case ZLAY_TOKEN_NUMBER: return "NUMBER";
    case ZLAY_TOKEN_COLOR: return "COLOR";
    case ZLAY_TOKEN_LBRACE: return "LBRACE";
    case ZLAY_TOKEN_RBRACE: return "RBRACE";
    case ZLAY_TOKEN_LPAREN: return "LPAREN";
    case ZLAY_TOKEN_RPAREN: return "RPAREN";
    case ZLAY_TOKEN_COLON: return "COLON";
    case ZLAY_TOKEN_COMMA: return "COMMA";
    case ZLAY_TOKEN_DOT: return "DOT";
    case ZLAY_TOKEN_HASH: return "HASH";
    case ZLAY_TOKEN_PERCENT: return "PERCENT";
    case ZLAY_TOKEN_TRUE: return "TRUE";
    case ZLAY_TOKEN_FALSE: return "FALSE";
    case ZLAY_TOKEN_NULL: return "NULL";
    default: return "UNKNOWN";
  }
}

bool ZLay_TokenIsValue(ZLay_TokenType type) {
  return type == ZLAY_TOKEN_STRING ||
         type == ZLAY_TOKEN_NUMBER ||
         type == ZLAY_TOKEN_COLOR ||
         type == ZLAY_TOKEN_IDENT ||
         type == ZLAY_TOKEN_TRUE ||
         type == ZLAY_TOKEN_FALSE ||
         type == ZLAY_TOKEN_NULL;
}
