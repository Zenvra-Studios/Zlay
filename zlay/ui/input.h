#ifndef ZLAY_UI_INPUT_H
#define ZLAY_UI_INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zlay_ui.h>

typedef enum ZLay_InputVariant {
  ZLAY_INPUT_OUTLINED = 0,
  ZLAY_INPUT_FILLED = 1,
  ZLAY_INPUT_UNDERLINE = 2
} ZLay_InputVariant;

typedef struct ZLay_InputProps {
  const ZLay_Theme* theme;
  ZLay_InputVariant variant;
  ZLay_ComponentSize size;
  uint32_t state;
  ZLay_Style style;
  bool use_custom_style;
  ZLay_Style text_style;
  bool use_custom_text_style;
  bool cursor_visible;
  uint32_t cursor_index;
  uint32_t selection_start;
  uint32_t selection_end;
  bool selection_visible;
} ZLay_InputProps;

ZLAY_API ZLay_InputProps ZLay_InputPropsDefault(const ZLay_Theme* theme);
ZLAY_API ZLay_Style ZLay_InputStyle(const ZLay_InputProps* props);
ZLAY_API ZLay_Style ZLay_InputTextStyle(const ZLay_InputProps* props, bool placeholder);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_InputDecl(ZLay_Id id, const ZLay_InputProps* props);
ZLAY_API void ZLay_Input(ZLay_Context* ctx, ZLay_Id id, ZLay_String value, ZLay_String placeholder, const ZLay_InputProps* props);

#ifdef __cplusplus
}
#endif

#endif
