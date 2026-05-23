#ifndef ZLAY_PARSER_VALUE_H
#define ZLAY_PARSER_VALUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <zlay.h>

typedef enum ZLay_ValueType {
  ZLAY_VALUE_NULL = 0,
  ZLAY_VALUE_STRING,
  ZLAY_VALUE_NUMBER,
  ZLAY_VALUE_BOOL,
  ZLAY_VALUE_COLOR,
  ZLAY_VALUE_IDENT,
  ZLAY_VALUE_PERCENT
} ZLay_ValueType;

typedef struct ZLay_Value {
  ZLay_ValueType type;
  ZLay_String string;
  double number;
  bool bool_value;
  ZLay_Color color;
} ZLay_Value;

ZLAY_API const char* ZLay_ValueTypeName(ZLay_ValueType type);
ZLAY_API ZLay_Value ZLay_ValueNull(void);
ZLAY_API ZLay_Value ZLay_ValueNumber(double number);
ZLAY_API ZLay_Value ZLay_ValuePercent(double number);
ZLAY_API ZLay_Value ZLay_ValueBool(bool value);
ZLAY_API ZLay_Value ZLay_ValueString(ZLay_String value);
ZLAY_API ZLay_Value ZLay_ValueIdent(ZLay_String value);
ZLAY_API ZLay_Value ZLay_ValueColor(ZLay_Color color);

#ifdef __cplusplus
}
#endif

#endif
