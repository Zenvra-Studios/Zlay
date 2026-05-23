#ifndef ZLAY_UI_BUTTON_H
#define ZLAY_UI_BUTTON_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zlay_ui.h>

typedef enum ZLay_ButtonVariant {
  ZLAY_BUTTON_PRIMARY = 0,
  ZLAY_BUTTON_SECONDARY = 1,
  ZLAY_BUTTON_GHOST = 2,
  ZLAY_BUTTON_DANGER = 3
} ZLay_ButtonVariant;

typedef struct ZLay_ButtonProps {
  const ZLay_Theme* theme;
  ZLay_ButtonVariant variant;
  ZLay_ComponentSize size;
  uint32_t state;
  ZLay_Style style;
  bool use_custom_style;
  ZLay_Style label_style;
  bool use_custom_label_style;
} ZLay_ButtonProps;

ZLAY_API ZLay_ButtonProps ZLay_ButtonPropsDefault(const ZLay_Theme* theme);
ZLAY_API ZLay_Style ZLay_ButtonStyle(const ZLay_ButtonProps* props);
ZLAY_API ZLay_Style ZLay_ButtonLabelStyle(const ZLay_ButtonProps* props);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_ButtonDecl(ZLay_Id id, const ZLay_ButtonProps* props);
ZLAY_API void ZLay_Button(ZLay_Context* ctx, ZLay_Id id, ZLay_String label, const ZLay_ButtonProps* props);

#ifdef __cplusplus
}
#endif

#endif
