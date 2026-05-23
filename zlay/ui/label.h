#ifndef ZLAY_UI_LABEL_H
#define ZLAY_UI_LABEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <zlay_ui.h>

typedef enum ZLay_LabelRole {
  ZLAY_LABEL_BODY = 0,
  ZLAY_LABEL_TITLE = 1,
  ZLAY_LABEL_SUBTITLE = 2,
  ZLAY_LABEL_CAPTION = 3,
  ZLAY_LABEL_BADGE = 4,
  ZLAY_LABEL_CODE = 5
} ZLay_LabelRole;

typedef struct ZLay_LabelProps {
  const ZLay_Theme* theme;
  ZLay_LabelRole role;
  ZLay_ComponentSize size;
  uint32_t state;
  ZLay_Color color;
  bool use_custom_color;
  ZLay_Style style;
  bool use_custom_style;
} ZLay_LabelProps;

ZLAY_API ZLay_LabelProps ZLay_LabelPropsDefault(const ZLay_Theme* theme);
ZLAY_API ZLay_Style ZLay_LabelStyle(const ZLay_LabelProps* props);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_LabelDecl(ZLay_Id id, ZLay_String text, const ZLay_LabelProps* props);
ZLAY_API void ZLay_Label(ZLay_Context* ctx, ZLay_Id id, ZLay_String text, const ZLay_LabelProps* props);

#ifdef __cplusplus
}
#endif

#endif
