#include <parser/resolver/zlay_style_resolver.h>

static bool ZLay__StylePropName(ZLay_String name) {
  return ZLay_StringEqualsCString(name, "width") ||
         ZLay_StringEqualsCString(name, "height") ||
         ZLay_StringEqualsCString(name, "padding") ||
         ZLay_StringEqualsCString(name, "paddingLeft") ||
         ZLay_StringEqualsCString(name, "padding_left") ||
         ZLay_StringEqualsCString(name, "paddingTop") ||
         ZLay_StringEqualsCString(name, "padding_top") ||
         ZLay_StringEqualsCString(name, "paddingRight") ||
         ZLay_StringEqualsCString(name, "padding_right") ||
         ZLay_StringEqualsCString(name, "paddingBottom") ||
         ZLay_StringEqualsCString(name, "padding_bottom") ||
         ZLay_StringEqualsCString(name, "margin") ||
         ZLay_StringEqualsCString(name, "marginLeft") ||
         ZLay_StringEqualsCString(name, "margin_left") ||
         ZLay_StringEqualsCString(name, "marginTop") ||
         ZLay_StringEqualsCString(name, "margin_top") ||
         ZLay_StringEqualsCString(name, "marginRight") ||
         ZLay_StringEqualsCString(name, "margin_right") ||
         ZLay_StringEqualsCString(name, "marginBottom") ||
         ZLay_StringEqualsCString(name, "margin_bottom") ||
         ZLay_StringEqualsCString(name, "gap") ||
         ZLay_StringEqualsCString(name, "direction") ||
         ZLay_StringEqualsCString(name, "flexDirection") ||
         ZLay_StringEqualsCString(name, "flex_direction") ||
         ZLay_StringEqualsCString(name, "position") ||
         ZLay_StringEqualsCString(name, "left") ||
         ZLay_StringEqualsCString(name, "top") ||
         ZLay_StringEqualsCString(name, "right") ||
         ZLay_StringEqualsCString(name, "bottom") ||
         ZLay_StringEqualsCString(name, "zIndex") ||
         ZLay_StringEqualsCString(name, "z_index") ||
         ZLay_StringEqualsCString(name, "background") ||
         ZLay_StringEqualsCString(name, "backgroundColor") ||
         ZLay_StringEqualsCString(name, "background_color") ||
         ZLay_StringEqualsCString(name, "color") ||
         ZLay_StringEqualsCString(name, "radius");
}

ZLay_Size ZLay_ResolveSizeValue(const ZLay_Value* value) {
  if (value == NULL) {
    return (ZLay_Size){ZLAY_SIZE_AUTO, 0.0f};
  }

  if (value->type == ZLAY_VALUE_NUMBER) {
    return ZLay_Px((float)value->number);
  }
  if (value->type == ZLAY_VALUE_PERCENT) {
    return ZLay_Percent((float)value->number);
  }
  if (value->type == ZLAY_VALUE_IDENT && ZLay_StringEqualsCString(value->string, "grow")) {
    return ZLay_Grow(1.0f);
  }
  return (ZLay_Size){ZLAY_SIZE_AUTO, 0.0f};
}

static float ZLay__NumberValue(const ZLay_Value* value) {
  return value != NULL && value->type == ZLAY_VALUE_NUMBER ? (float)value->number : 0.0f;
}

bool ZLay_ResolveStyle(const ZLay_AstNode* node, ZLay_Style* out_style) {
  bool changed = false;
  ZLay_Style style;

  if (out_style == NULL) {
    return false;
  }

  style = ZLay_StyleDefault();
  if (node == NULL) {
    *out_style = style;
    return false;
  }

  for (uint32_t i = 0u; i < node->prop_count; ++i) {
    const ZLay_AstProp* prop = &node->props[i];
    const ZLay_Value* value = &prop->value;

    if (!ZLay__StylePropName(prop->name)) {
      continue;
    }
    changed = true;

    if (ZLay_StringEqualsCString(prop->name, "width")) {
      style.width = ZLay_ResolveSizeValue(value);
    } else if (ZLay_StringEqualsCString(prop->name, "height")) {
      style.height = ZLay_ResolveSizeValue(value);
    } else if (ZLay_StringEqualsCString(prop->name, "padding")) {
      style.padding_left = style.padding_top = style.padding_right = style.padding_bottom = ZLay__NumberValue(value);
    } else if (ZLay_StringEqualsCString(prop->name, "paddingLeft") ||
               ZLay_StringEqualsCString(prop->name, "padding_left")) {
      style.padding_left = ZLay__NumberValue(value);
    } else if (ZLay_StringEqualsCString(prop->name, "paddingTop") ||
               ZLay_StringEqualsCString(prop->name, "padding_top")) {
      style.padding_top = ZLay__NumberValue(value);
    } else if (ZLay_StringEqualsCString(prop->name, "paddingRight") ||
               ZLay_StringEqualsCString(prop->name, "padding_right")) {
      style.padding_right = ZLay__NumberValue(value);
    } else if (ZLay_StringEqualsCString(prop->name, "paddingBottom") ||
               ZLay_StringEqualsCString(prop->name, "padding_bottom")) {
      style.padding_bottom = ZLay__NumberValue(value);
    } else if (ZLay_StringEqualsCString(prop->name, "margin")) {
      style.margin_left = style.margin_top = style.margin_right = style.margin_bottom = ZLay__NumberValue(value);
    } else if (ZLay_StringEqualsCString(prop->name, "marginLeft") ||
               ZLay_StringEqualsCString(prop->name, "margin_left")) {
      style.margin_left = ZLay__NumberValue(value);
    } else if (ZLay_StringEqualsCString(prop->name, "marginTop") ||
               ZLay_StringEqualsCString(prop->name, "margin_top")) {
      style.margin_top = ZLay__NumberValue(value);
    } else if (ZLay_StringEqualsCString(prop->name, "marginRight") ||
               ZLay_StringEqualsCString(prop->name, "margin_right")) {
      style.margin_right = ZLay__NumberValue(value);
    } else if (ZLay_StringEqualsCString(prop->name, "marginBottom") ||
               ZLay_StringEqualsCString(prop->name, "margin_bottom")) {
      style.margin_bottom = ZLay__NumberValue(value);
    } else if (ZLay_StringEqualsCString(prop->name, "gap")) {
      style.gap = ZLay__NumberValue(value);
    } else if (ZLay_StringEqualsCString(prop->name, "direction") ||
               ZLay_StringEqualsCString(prop->name, "flexDirection") ||
               ZLay_StringEqualsCString(prop->name, "flex_direction")) {
      style.flex_direction = ZLay_StringEqualsCString(value->string, "row") ? ZLAY_FLEX_ROW : ZLAY_FLEX_COLUMN;
    } else if (ZLay_StringEqualsCString(prop->name, "position")) {
      if (ZLay_StringEqualsCString(value->string, "absolute")) {
        style.position = ZLAY_POSITION_ABSOLUTE;
      } else if (ZLay_StringEqualsCString(value->string, "floating")) {
        style.position = ZLAY_POSITION_FLOATING;
      } else {
        style.position = ZLAY_POSITION_RELATIVE;
      }
    } else if (ZLay_StringEqualsCString(prop->name, "left")) {
      style.left = ZLay__NumberValue(value);
    } else if (ZLay_StringEqualsCString(prop->name, "top")) {
      style.top = ZLay__NumberValue(value);
    } else if (ZLay_StringEqualsCString(prop->name, "right")) {
      style.right = ZLay__NumberValue(value);
    } else if (ZLay_StringEqualsCString(prop->name, "bottom")) {
      style.bottom = ZLay__NumberValue(value);
    } else if (ZLay_StringEqualsCString(prop->name, "zIndex") ||
               ZLay_StringEqualsCString(prop->name, "z_index")) {
      style.z_index = (int32_t)ZLay__NumberValue(value);
    } else if (ZLay_StringEqualsCString(prop->name, "background") ||
               ZLay_StringEqualsCString(prop->name, "backgroundColor") ||
               ZLay_StringEqualsCString(prop->name, "background_color")) {
      style.background = value->color;
    } else if (ZLay_StringEqualsCString(prop->name, "color")) {
      style.text_color = value->color;
    } else if (ZLay_StringEqualsCString(prop->name, "radius")) {
      style.radius = ZLay__NumberValue(value);
    }
  }

  if (ZLay_StringEqualsCString(node->type, "Row")) {
    style.flex_direction = ZLAY_FLEX_ROW;
    changed = true;
  } else if (ZLay_StringEqualsCString(node->type, "Column")) {
    style.flex_direction = ZLAY_FLEX_COLUMN;
    changed = true;
  }

  *out_style = style;
  return changed;
}
