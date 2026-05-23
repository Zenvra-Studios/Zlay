#include <parser/syntax/zlay_parser.h>

typedef struct ZLay_SyntaxParser {
  ZLay_Arena* arena;
  ZLay_Lexer lexer;
  ZLay_Token current;
  ZLay_Token next;
  ZLay_ParseResult result;
} ZLay_SyntaxParser;

static ZLay_String ZLay__ParserStaticString(const char* message) {
  return ZLay_ParserStringFromCString(message);
}

static void ZLay__ParserErrorAt(ZLay_SyntaxParser* parser, ZLay_Token token, const char* message) {
  if (parser == NULL || parser->result.success == false) {
    return;
  }
  parser->result.success = false;
  parser->result.error_message = ZLay__ParserStaticString(message);
  parser->result.error_line = token.line;
  parser->result.error_column = token.column;
}

static void ZLay__ParserAdvance(ZLay_SyntaxParser* parser) {
  if (parser == NULL) {
    return;
  }
  parser->current = parser->next;
  parser->next = ZLay_LexerNext(&parser->lexer);
  if (parser->current.type == ZLAY_TOKEN_ERROR) {
    ZLay__ParserErrorAt(parser, parser->current, parser->current.lexeme.chars);
  }
}

static bool ZLay__ParserCheck(const ZLay_SyntaxParser* parser, ZLay_TokenType type) {
  return parser != NULL && parser->current.type == type;
}

static bool ZLay__ParserMatch(ZLay_SyntaxParser* parser, ZLay_TokenType type) {
  if (!ZLay__ParserCheck(parser, type)) {
    return false;
  }
  ZLay__ParserAdvance(parser);
  return true;
}

static bool ZLay__ParserExpect(ZLay_SyntaxParser* parser, ZLay_TokenType type, const char* message) {
  if (ZLay__ParserMatch(parser, type)) {
    return true;
  }
  ZLay__ParserErrorAt(parser, parser->current, message);
  return false;
}

static ZLay_Value ZLay__ParserParseValue(ZLay_SyntaxParser* parser) {
  ZLay_Token token = parser->current;

  switch (token.type) {
    case ZLAY_TOKEN_STRING:
      ZLay__ParserAdvance(parser);
      return ZLay_ValueString(token.lexeme);
    case ZLAY_TOKEN_NUMBER:
      ZLay__ParserAdvance(parser);
      if (ZLay__ParserMatch(parser, ZLAY_TOKEN_PERCENT)) {
        return ZLay_ValuePercent(token.number);
      }
      return ZLay_ValueNumber(token.number);
    case ZLAY_TOKEN_COLOR:
      ZLay__ParserAdvance(parser);
      return ZLay_ValueColor(token.color);
    case ZLAY_TOKEN_TRUE:
      ZLay__ParserAdvance(parser);
      return ZLay_ValueBool(true);
    case ZLAY_TOKEN_FALSE:
      ZLay__ParserAdvance(parser);
      return ZLay_ValueBool(false);
    case ZLAY_TOKEN_NULL:
      ZLay__ParserAdvance(parser);
      return ZLay_ValueNull();
    case ZLAY_TOKEN_IDENT:
      ZLay__ParserAdvance(parser);
      return ZLay_ValueIdent(token.lexeme);
    default:
      ZLay__ParserErrorAt(parser, token, "expected property value");
      return ZLay_ValueNull();
  }
}

static bool ZLay__ParserParseNodeInto(ZLay_SyntaxParser* parser, ZLay_AstNode* node);

static bool ZLay__ParserParseItem(ZLay_SyntaxParser* parser, ZLay_AstNode* parent) {
  ZLay_Token name;
  ZLay_AstNode* child;
  ZLay_Value value;

  if (!ZLay__ParserCheck(parser, ZLAY_TOKEN_IDENT)) {
    ZLay__ParserErrorAt(parser, parser->current, "expected property or child node");
    return false;
  }

  name = parser->current;

  if (parser->next.type == ZLAY_TOKEN_COLON) {
    ZLay__ParserAdvance(parser);
    ZLay__ParserAdvance(parser);
    value = ZLay__ParserParseValue(parser);
    if (!parser->result.success) {
      return false;
    }
    if (!ZLay_AstAddProp(parser->arena, parent, name.lexeme, value, name.line, name.column)) {
      ZLay__ParserErrorAt(parser, name, "out of parser memory");
      return false;
    }
    return true;
  }

  child = ZLay_AstAddChild(parser->arena, parent, name.lexeme, name.line, name.column);
  if (child == NULL) {
    ZLay__ParserErrorAt(parser, name, "out of parser memory");
    return false;
  }
  return ZLay__ParserParseNodeInto(parser, child);
}

static bool ZLay__ParserParseNodeInto(ZLay_SyntaxParser* parser, ZLay_AstNode* node) {
  if (!ZLay__ParserExpect(parser, ZLAY_TOKEN_IDENT, "expected node type")) {
    return false;
  }
  if (!ZLay__ParserExpect(parser, ZLAY_TOKEN_LBRACE, "expected '{' after node type")) {
    return false;
  }

  while (parser->result.success &&
         !ZLay__ParserCheck(parser, ZLAY_TOKEN_RBRACE) &&
         !ZLay__ParserCheck(parser, ZLAY_TOKEN_EOF)) {
    if (!ZLay__ParserParseItem(parser, node)) {
      return false;
    }
    ZLay__ParserMatch(parser, ZLAY_TOKEN_COMMA);
  }

  return ZLay__ParserExpect(parser, ZLAY_TOKEN_RBRACE, "expected '}' after node body");
}

ZLay_ParseResult ZLay_Parse(ZLay_Arena* arena, ZLay_String source) {
  ZLay_SyntaxParser parser;
  ZLay_String document_type = ZLay_ParserStringFromCString("Document");

  parser.arena = arena;
  ZLay_LexerInit(&parser.lexer, source);
  parser.current = ZLay_LexerNext(&parser.lexer);
  parser.next = ZLay_LexerNext(&parser.lexer);
  parser.result.success = true;
  parser.result.root = NULL;
  parser.result.error_message = ZLay_ParserStringFromCString(NULL);
  parser.result.error_line = 0u;
  parser.result.error_column = 0u;

  if (arena == NULL) {
    parser.result.success = false;
    parser.result.error_message = ZLay__ParserStaticString("parser arena is null");
    return parser.result;
  }

  parser.result.root = ZLay_AstCreateNode(arena, document_type, 1u, 1u);
  if (parser.result.root == NULL) {
    parser.result.success = false;
    parser.result.error_message = ZLay__ParserStaticString("out of parser memory");
    return parser.result;
  }

  while (parser.result.success && !ZLay__ParserCheck(&parser, ZLAY_TOKEN_EOF)) {
    if (parser.current.type == ZLAY_TOKEN_ERROR) {
      ZLay__ParserErrorAt(&parser, parser.current, parser.current.lexeme.chars);
      break;
    }
    if (!ZLay__ParserParseItem(&parser, parser.result.root)) {
      break;
    }
    ZLay__ParserMatch(&parser, ZLAY_TOKEN_COMMA);
  }

  if (!parser.result.success) {
    parser.result.root = NULL;
  }

  return parser.result;
}
