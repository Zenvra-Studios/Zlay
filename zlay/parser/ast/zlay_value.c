#include <parser/ast/zlay_value.h>

const char* ZLay_ValueTypeName(ZLay_ValueType type) {
  switch (type) {
    case ZLAY_VALUE_NULL: return "null";
    case ZLAY_VALUE_STRING: return "string";
    case ZLAY_VALUE_NUMBER: return "number";
    case ZLAY_VALUE_BOOL: return "bool";
    case ZLAY_VALUE_COLOR: return "color";
    case ZLAY_VALUE_IDENT: return "ident";
    case ZLAY_VALUE_PERCENT: return "percent";
    default: return "unknown";
  }
}

ZLay_Value ZLay_ValueNull(void) {
  ZLay_Value value = {0};
  value.type = ZLAY_VALUE_NULL;
  return value;
}

ZLay_Value ZLay_ValueNumber(double number) {
  ZLay_Value value = ZLay_ValueNull();
  value.type = ZLAY_VALUE_NUMBER;
  value.number = number;
  return value;
}

ZLay_Value ZLay_ValuePercent(double number) {
  ZLay_Value value = ZLay_ValueNumber(number);
  value.type = ZLAY_VALUE_PERCENT;
  return value;
}

ZLay_Value ZLay_ValueBool(bool flag) {
  ZLay_Value value = ZLay_ValueNull();
  value.type = ZLAY_VALUE_BOOL;
  value.bool_value = flag;
  return value;
}

ZLay_Value ZLay_ValueString(ZLay_String string) {
  ZLay_Value value = ZLay_ValueNull();
  value.type = ZLAY_VALUE_STRING;
  value.string = string;
  return value;
}

ZLay_Value ZLay_ValueIdent(ZLay_String string) {
  ZLay_Value value = ZLay_ValueString(string);
  value.type = ZLAY_VALUE_IDENT;
  return value;
}

ZLay_Value ZLay_ValueColor(ZLay_Color color) {
  ZLay_Value value = ZLay_ValueNull();
  value.type = ZLAY_VALUE_COLOR;
  value.color = color;
  return value;
}
