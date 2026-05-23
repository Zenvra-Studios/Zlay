#ifndef ZLAY_PARSER_AST_H
#define ZLAY_PARSER_AST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include <parser/ast/zlay_value.h>

typedef struct ZLay_AstProp {
  ZLay_String name;
  ZLay_Value value;
  uint32_t line;
  uint32_t column;
} ZLay_AstProp;

typedef struct ZLay_AstNode {
  ZLay_String type;
  ZLay_AstProp* props;
  uint32_t prop_count;
  uint32_t prop_capacity;
  struct ZLay_AstNode* children;
  uint32_t child_count;
  uint32_t child_capacity;
  uint32_t line;
  uint32_t column;
} ZLay_AstNode;

ZLAY_API bool ZLay_StringEquals(ZLay_String a, ZLay_String b);
ZLAY_API bool ZLay_StringEqualsCString(ZLay_String a, const char* b);
ZLAY_API ZLay_String ZLay_ParserStringFromCString(const char* cstr);
ZLAY_API ZLay_String ZLay_ArenaCopyString(ZLay_Arena* arena, ZLay_String source);

ZLAY_API ZLay_AstNode* ZLay_AstCreateNode(
  ZLay_Arena* arena,
  ZLay_String type,
  uint32_t line,
  uint32_t column
);
ZLAY_API bool ZLay_AstAddProp(
  ZLay_Arena* arena,
  ZLay_AstNode* node,
  ZLay_String name,
  ZLay_Value value,
  uint32_t line,
  uint32_t column
);
ZLAY_API ZLay_AstNode* ZLay_AstAddChild(
  ZLay_Arena* arena,
  ZLay_AstNode* parent,
  ZLay_String type,
  uint32_t line,
  uint32_t column
);
ZLAY_API const ZLay_AstProp* ZLay_AstFindProp(const ZLay_AstNode* node, const char* name);

#ifdef __cplusplus
}
#endif

#endif
