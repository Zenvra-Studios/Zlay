#include <parser/semantic/zlay_semantic.h>

static ZLay_SemanticResult ZLay__SemanticOk(void) {
  ZLay_SemanticResult result;
  result.success = true;
  result.error_message = ZLay_ParserStringFromCString(NULL);
  result.error_line = 0u;
  result.error_column = 0u;
  return result;
}

static ZLay_SemanticResult ZLay__SemanticError(
  const ZLay_AstProp* prop,
  const ZLay_AstNode* node,
  const char* message
) {
  ZLay_SemanticResult result;
  result.success = false;
  result.error_message = ZLay_ParserStringFromCString(message);
  result.error_line = prop != NULL ? prop->line : node != NULL ? node->line : 0u;
  result.error_column = prop != NULL ? prop->column : node != NULL ? node->column : 0u;
  return result;
}

static bool ZLay__ValueIsIdent(const ZLay_Value* value, const char* ident) {
  return value != NULL &&
         value->type == ZLAY_VALUE_IDENT &&
         ZLay_StringEqualsCString(value->string, ident);
}

static bool ZLay__ValueIsDimension(const ZLay_Value* value) {
  if (value == NULL) {
    return false;
  }
  return value->type == ZLAY_VALUE_NUMBER ||
         value->type == ZLAY_VALUE_PERCENT ||
         ZLay__ValueIsIdent(value, "auto") ||
         ZLay__ValueIsIdent(value, "grow");
}

static bool ZLay__ValueIsNumber(const ZLay_Value* value) {
  return value != NULL && value->type == ZLAY_VALUE_NUMBER;
}

static bool ZLay__ValueIsColor(const ZLay_Value* value) {
  return value != NULL && value->type == ZLAY_VALUE_COLOR;
}

static bool ZLay__ValueIsText(const ZLay_Value* value) {
  return value != NULL && (value->type == ZLAY_VALUE_STRING || value->type == ZLAY_VALUE_IDENT);
}

static bool ZLay__ValueIsDirection(const ZLay_Value* value) {
  return ZLay__ValueIsIdent(value, "row") || ZLay__ValueIsIdent(value, "column");
}

static bool ZLay__ValueIsPosition(const ZLay_Value* value) {
  return ZLay__ValueIsIdent(value, "relative") ||
         ZLay__ValueIsIdent(value, "absolute") ||
         ZLay__ValueIsIdent(value, "floating");
}

static bool ZLay__ValueIsBoolish(const ZLay_Value* value) {
  return value != NULL &&
         (value->type == ZLAY_VALUE_BOOL ||
          value->type == ZLAY_VALUE_NULL ||
          value->type == ZLAY_VALUE_STRING ||
          value->type == ZLAY_VALUE_IDENT);
}

static ZLay_SemanticResult ZLay__ValidateProperty(const ZLay_AstNode* node, const ZLay_AstProp* prop) {
  const ZLay_Value* value = &prop->value;

  if (ZLay_StringEqualsCString(prop->name, "width") ||
      ZLay_StringEqualsCString(prop->name, "height")) {
    return ZLay__ValueIsDimension(value) ? ZLay__SemanticOk() : ZLay__SemanticError(prop, node, "size expects number, percent, auto, or grow");
  }

  if (ZLay_StringEqualsCString(prop->name, "padding") ||
      ZLay_StringEqualsCString(prop->name, "paddingLeft") ||
      ZLay_StringEqualsCString(prop->name, "padding_left") ||
      ZLay_StringEqualsCString(prop->name, "paddingTop") ||
      ZLay_StringEqualsCString(prop->name, "padding_top") ||
      ZLay_StringEqualsCString(prop->name, "paddingRight") ||
      ZLay_StringEqualsCString(prop->name, "padding_right") ||
      ZLay_StringEqualsCString(prop->name, "paddingBottom") ||
      ZLay_StringEqualsCString(prop->name, "padding_bottom") ||
      ZLay_StringEqualsCString(prop->name, "margin") ||
      ZLay_StringEqualsCString(prop->name, "marginLeft") ||
      ZLay_StringEqualsCString(prop->name, "margin_left") ||
      ZLay_StringEqualsCString(prop->name, "marginTop") ||
      ZLay_StringEqualsCString(prop->name, "margin_top") ||
      ZLay_StringEqualsCString(prop->name, "marginRight") ||
      ZLay_StringEqualsCString(prop->name, "margin_right") ||
      ZLay_StringEqualsCString(prop->name, "marginBottom") ||
      ZLay_StringEqualsCString(prop->name, "margin_bottom") ||
      ZLay_StringEqualsCString(prop->name, "gap") ||
      ZLay_StringEqualsCString(prop->name, "radius") ||
      ZLay_StringEqualsCString(prop->name, "left") ||
      ZLay_StringEqualsCString(prop->name, "top") ||
      ZLay_StringEqualsCString(prop->name, "right") ||
      ZLay_StringEqualsCString(prop->name, "bottom") ||
      ZLay_StringEqualsCString(prop->name, "zIndex") ||
      ZLay_StringEqualsCString(prop->name, "z_index")) {
    return ZLay__ValueIsNumber(value) ? ZLay__SemanticOk() : ZLay__SemanticError(prop, node, "layout property expects number");
  }

  if (ZLay_StringEqualsCString(prop->name, "background") ||
      ZLay_StringEqualsCString(prop->name, "backgroundColor") ||
      ZLay_StringEqualsCString(prop->name, "background_color") ||
      ZLay_StringEqualsCString(prop->name, "color")) {
    return ZLay__ValueIsColor(value) ? ZLay__SemanticOk() : ZLay__SemanticError(prop, node, "color property expects hex color");
  }

  if (ZLay_StringEqualsCString(prop->name, "direction") ||
      ZLay_StringEqualsCString(prop->name, "flexDirection") ||
      ZLay_StringEqualsCString(prop->name, "flex_direction")) {
    return ZLay__ValueIsDirection(value) ? ZLay__SemanticOk() : ZLay__SemanticError(prop, node, "direction expects row or column");
  }

  if (ZLay_StringEqualsCString(prop->name, "position")) {
    return ZLay__ValueIsPosition(value) ? ZLay__SemanticOk() : ZLay__SemanticError(prop, node, "position expects relative, absolute, or floating");
  }

  if (ZLay_StringEqualsCString(prop->name, "id") ||
      ZLay_StringEqualsCString(prop->name, "key") ||
      ZLay_StringEqualsCString(prop->name, "value") ||
      ZLay_StringEqualsCString(prop->name, "text") ||
      ZLay_StringEqualsCString(prop->name, "label") ||
      ZLay_StringEqualsCString(prop->name, "title")) {
    return ZLay__ValueIsText(value) ? ZLay__SemanticOk() : ZLay__SemanticError(prop, node, "text property expects string or identifier");
  }

  if (ZLay_StringEqualsCString(prop->name, "disabled") ||
      ZLay_StringEqualsCString(prop->name, "selected") ||
      ZLay_StringEqualsCString(prop->name, "checked") ||
      ZLay_StringEqualsCString(prop->name, "onClick")) {
    return ZLay__ValueIsBoolish(value) ? ZLay__SemanticOk() : ZLay__SemanticError(prop, node, "state or callback property has unsupported value");
  }

  return ZLay__SemanticOk();
}

static ZLay_SemanticResult ZLay__ValidateNode(const ZLay_AstNode* node) {
  ZLay_SemanticResult result;

  if (node == NULL) {
    return ZLay__SemanticError(NULL, NULL, "semantic node is null");
  }

  if (ZLay_StringEqualsCString(node->type, "Text") &&
      ZLay_AstFindProp(node, "value") == NULL &&
      ZLay_AstFindProp(node, "text") == NULL) {
    return ZLay__SemanticError(NULL, node, "Text node requires value or text");
  }

  for (uint32_t i = 0u; i < node->prop_count; ++i) {
    result = ZLay__ValidateProperty(node, &node->props[i]);
    if (!result.success) {
      return result;
    }
  }

  for (uint32_t i = 0u; i < node->child_count; ++i) {
    result = ZLay__ValidateNode(&node->children[i]);
    if (!result.success) {
      return result;
    }
  }

  return ZLay__SemanticOk();
}

ZLay_SemanticResult ZLay_SemanticValidate(const ZLay_AstNode* root) {
  if (root == NULL) {
    return ZLay__SemanticError(NULL, NULL, "parse result has no AST root");
  }
  return ZLay__ValidateNode(root);
}
