#include <parser/lexer/zlay_lexer.h>

static bool ZLay__LexerAtEnd(const ZLay_Lexer* lexer) {
  return lexer == NULL || lexer->offset >= lexer->source.length;
}

static char ZLay__LexerPeek(const ZLay_Lexer* lexer) {
  if (ZLay__LexerAtEnd(lexer)) {
    return '\0';
  }
  return lexer->source.chars[lexer->offset];
}

static char ZLay__LexerPeekNext(const ZLay_Lexer* lexer) {
  if (lexer == NULL || lexer->offset + 1u >= lexer->source.length) {
    return '\0';
  }
  return lexer->source.chars[lexer->offset + 1u];
}

static char ZLay__LexerAdvance(ZLay_Lexer* lexer) {
  char c = ZLay__LexerPeek(lexer);
  if (c == '\0') {
    return c;
  }

  ++lexer->offset;
  if (c == '\n') {
    ++lexer->line;
    lexer->column = 1u;
  } else {
    ++lexer->column;
  }
  return c;
}

static bool ZLay__IsAlpha(char c) {
  return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || c == '_';
}

static bool ZLay__IsDigit(char c) {
  return c >= '0' && c <= '9';
}

static bool ZLay__IsIdentChar(char c) {
  return ZLay__IsAlpha(c) || ZLay__IsDigit(c) || c == '-';
}

static int ZLay__HexValue(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return 10 + (c - 'a');
  if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
  return -1;
}

static bool ZLay__StringEqualsCString(ZLay_String text, const char* cstr) {
  uint32_t i = 0u;
  if (cstr == NULL) {
    return false;
  }
  while (cstr[i] != '\0') {
    if (i >= text.length || text.chars[i] != cstr[i]) {
      return false;
    }
    ++i;
  }
  return i == text.length;
}

static ZLay_Token ZLay__Token(
  ZLay_TokenType type,
  const char* start,
  uint32_t length,
  uint32_t line,
  uint32_t column
) {
  ZLay_Token token;
  token.type = type;
  token.lexeme.chars = start;
  token.lexeme.length = length;
  token.lexeme.is_static = false;
  token.number = 0.0;
  token.color = (ZLay_Color){0, 0, 0, 0};
  token.line = line;
  token.column = column;
  return token;
}

static ZLay_Token ZLay__ErrorToken(const char* message, uint32_t line, uint32_t column) {
  uint32_t length = 0u;
  if (message != NULL) {
    while (message[length] != '\0') {
      ++length;
    }
  }
  return ZLay__Token(ZLAY_TOKEN_ERROR, message, length, line, column);
}

static void ZLay__SkipWhitespaceAndComments(ZLay_Lexer* lexer) {
  bool scanning = true;
  while (scanning && !ZLay__LexerAtEnd(lexer)) {
    char c = ZLay__LexerPeek(lexer);
    switch (c) {
      case ' ':
      case '\r':
      case '\t':
      case '\n':
        ZLay__LexerAdvance(lexer);
        break;
      case '/':
        if (ZLay__LexerPeekNext(lexer) == '/') {
          while (!ZLay__LexerAtEnd(lexer) && ZLay__LexerPeek(lexer) != '\n') {
            ZLay__LexerAdvance(lexer);
          }
        } else if (ZLay__LexerPeekNext(lexer) == '*') {
          ZLay__LexerAdvance(lexer);
          ZLay__LexerAdvance(lexer);
          while (!ZLay__LexerAtEnd(lexer)) {
            if (ZLay__LexerPeek(lexer) == '*' && ZLay__LexerPeekNext(lexer) == '/') {
              ZLay__LexerAdvance(lexer);
              ZLay__LexerAdvance(lexer);
              break;
            }
            ZLay__LexerAdvance(lexer);
          }
        } else {
          scanning = false;
        }
        break;
      default:
        scanning = false;
        break;
    }
  }
}

static ZLay_Token ZLay__LexIdentifier(ZLay_Lexer* lexer, uint32_t start, uint32_t line, uint32_t column) {
  ZLay_Token token;
  while (ZLay__IsIdentChar(ZLay__LexerPeek(lexer))) {
    ZLay__LexerAdvance(lexer);
  }

  token = ZLay__Token(
    ZLAY_TOKEN_IDENT,
    lexer->source.chars + start,
    lexer->offset - start,
    line,
    column
  );

  if (ZLay__StringEqualsCString(token.lexeme, "true")) {
    token.type = ZLAY_TOKEN_TRUE;
  } else if (ZLay__StringEqualsCString(token.lexeme, "false")) {
    token.type = ZLAY_TOKEN_FALSE;
  } else if (ZLay__StringEqualsCString(token.lexeme, "null")) {
    token.type = ZLAY_TOKEN_NULL;
  }

  return token;
}

static ZLay_Token ZLay__LexNumber(ZLay_Lexer* lexer, uint32_t start, uint32_t line, uint32_t column) {
  double value = 0.0;
  double fraction = 0.1;
  bool negative = false;
  ZLay_Token token;

  if (ZLay__LexerPeek(lexer) == '-') {
    negative = true;
    ZLay__LexerAdvance(lexer);
  }

  while (ZLay__IsDigit(ZLay__LexerPeek(lexer))) {
    value = (value * 10.0) + (double)(ZLay__LexerAdvance(lexer) - '0');
  }

  if (ZLay__LexerPeek(lexer) == '.' && ZLay__IsDigit(ZLay__LexerPeekNext(lexer))) {
    ZLay__LexerAdvance(lexer);
    while (ZLay__IsDigit(ZLay__LexerPeek(lexer))) {
      value += (double)(ZLay__LexerAdvance(lexer) - '0') * fraction;
      fraction *= 0.1;
    }
  }

  token = ZLay__Token(
    ZLAY_TOKEN_NUMBER,
    lexer->source.chars + start,
    lexer->offset - start,
    line,
    column
  );
  token.number = negative ? -value : value;
  return token;
}

static ZLay_Token ZLay__LexString(ZLay_Lexer* lexer, uint32_t line, uint32_t column) {
  uint32_t start = lexer->offset;
  while (!ZLay__LexerAtEnd(lexer) && ZLay__LexerPeek(lexer) != '"') {
    if (ZLay__LexerPeek(lexer) == '\\' && ZLay__LexerPeekNext(lexer) != '\0') {
      ZLay__LexerAdvance(lexer);
    }
    ZLay__LexerAdvance(lexer);
  }

  if (ZLay__LexerAtEnd(lexer)) {
    return ZLay__ErrorToken("unterminated string", line, column);
  }

  ZLay_Token token = ZLay__Token(
    ZLAY_TOKEN_STRING,
    lexer->source.chars + start,
    lexer->offset - start,
    line,
    column
  );
  ZLay__LexerAdvance(lexer);
  return token;
}

static uint8_t ZLay__HexByte(int high, int low) {
  return (uint8_t)((high << 4) | low);
}

static ZLay_Token ZLay__LexColor(ZLay_Lexer* lexer, uint32_t hash_offset, uint32_t line, uint32_t column) {
  int values[8];
  uint32_t count = 0u;
  ZLay_Token token;

  while (count < 8u) {
    int value = ZLay__HexValue(ZLay__LexerPeek(lexer));
    if (value < 0) {
      break;
    }
    values[count++] = value;
    ZLay__LexerAdvance(lexer);
  }

  if (count != 3u && count != 4u && count != 6u && count != 8u) {
    return ZLay__ErrorToken("invalid color literal", line, column);
  }

  token = ZLay__Token(
    ZLAY_TOKEN_COLOR,
    lexer->source.chars + hash_offset,
    lexer->offset - hash_offset,
    line,
    column
  );

  if (count == 3u || count == 4u) {
    token.color.r = (uint8_t)((values[0] << 4) | values[0]);
    token.color.g = (uint8_t)((values[1] << 4) | values[1]);
    token.color.b = (uint8_t)((values[2] << 4) | values[2]);
    token.color.a = count == 4u ? (uint8_t)((values[3] << 4) | values[3]) : 255u;
  } else {
    token.color.r = ZLay__HexByte(values[0], values[1]);
    token.color.g = ZLay__HexByte(values[2], values[3]);
    token.color.b = ZLay__HexByte(values[4], values[5]);
    token.color.a = count == 8u ? ZLay__HexByte(values[6], values[7]) : 255u;
  }

  return token;
}

void ZLay_LexerInit(ZLay_Lexer* lexer, ZLay_String source) {
  if (lexer == NULL) {
    return;
  }
  lexer->source = source;
  lexer->offset = 0u;
  lexer->line = 1u;
  lexer->column = 1u;
}

ZLay_Token ZLay_LexerNext(ZLay_Lexer* lexer) {
  uint32_t start;
  uint32_t line;
  uint32_t column;
  char c;

  if (lexer == NULL) {
    return ZLay__ErrorToken("lexer is null", 0u, 0u);
  }

  ZLay__SkipWhitespaceAndComments(lexer);
  start = lexer->offset;
  line = lexer->line;
  column = lexer->column;

  if (ZLay__LexerAtEnd(lexer)) {
    return ZLay__Token(ZLAY_TOKEN_EOF, lexer->source.chars + lexer->source.length, 0u, line, column);
  }

  c = ZLay__LexerAdvance(lexer);

  if (ZLay__IsAlpha(c)) {
    return ZLay__LexIdentifier(lexer, start, line, column);
  }
  if (ZLay__IsDigit(c) || (c == '-' && ZLay__IsDigit(ZLay__LexerPeek(lexer)))) {
    lexer->offset = start;
    lexer->column = column;
    return ZLay__LexNumber(lexer, start, line, column);
  }

  switch (c) {
    case '{': return ZLay__Token(ZLAY_TOKEN_LBRACE, lexer->source.chars + start, 1u, line, column);
    case '}': return ZLay__Token(ZLAY_TOKEN_RBRACE, lexer->source.chars + start, 1u, line, column);
    case '(': return ZLay__Token(ZLAY_TOKEN_LPAREN, lexer->source.chars + start, 1u, line, column);
    case ')': return ZLay__Token(ZLAY_TOKEN_RPAREN, lexer->source.chars + start, 1u, line, column);
    case ':': return ZLay__Token(ZLAY_TOKEN_COLON, lexer->source.chars + start, 1u, line, column);
    case ',': return ZLay__Token(ZLAY_TOKEN_COMMA, lexer->source.chars + start, 1u, line, column);
    case '.': return ZLay__Token(ZLAY_TOKEN_DOT, lexer->source.chars + start, 1u, line, column);
    case '%': return ZLay__Token(ZLAY_TOKEN_PERCENT, lexer->source.chars + start, 1u, line, column);
    case '"': return ZLay__LexString(lexer, line, column);
    case '#': return ZLay__LexColor(lexer, start, line, column);
    default: break;
  }

  return ZLay__ErrorToken("unexpected character", line, column);
}
