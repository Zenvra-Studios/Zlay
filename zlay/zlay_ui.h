#ifndef ZLAY_UI_H
#define ZLAY_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <zlay.h>
#include <zlay_color.h>

ZLAY_API bool ZLay_StringIsEmpty(ZLay_String string);
ZLAY_API ZLay_String ZLay_StringFromCString(const char* cstr);

ZLAY_API ZLay_EdgeInsets ZLay_Insets(float all);
ZLAY_API ZLay_EdgeInsets ZLay_InsetsXY(float x, float y);
ZLAY_API ZLay_EdgeInsets ZLay_InsetsLTRB(float left, float top, float right, float bottom);

ZLAY_API ZLay_Theme ZLay_ThemeLight(void);
ZLAY_API ZLay_Theme ZLay_ThemeDark(void);
ZLAY_API const ZLay_Theme* ZLay_ThemeDefault(void);

ZLAY_API float ZLay_ComponentControlHeight(ZLay_ComponentSize size, const ZLay_Theme* theme);
ZLAY_API ZLay_EdgeInsets ZLay_ComponentControlPadding(ZLay_ComponentSize size, const ZLay_Theme* theme);

ZLAY_API ZLay_Style ZLay_StyleWithSize(ZLay_Style style, ZLay_Size width, ZLay_Size height);
ZLAY_API ZLay_Style ZLay_StyleWithPadding(ZLay_Style style, ZLay_EdgeInsets padding);
ZLAY_API ZLay_Style ZLay_StyleWithMargin(ZLay_Style style, ZLay_EdgeInsets margin);
ZLAY_API ZLay_Style ZLay_StyleWithGap(ZLay_Style style, float gap);
ZLAY_API ZLay_Style ZLay_StyleWithDirection(ZLay_Style style, ZLay_FlexDirection direction);
ZLAY_API ZLay_Style ZLay_StyleWithGrid(ZLay_Style style, uint32_t columns, ZLay_Size auto_row_height, float column_gap, float row_gap);
ZLAY_API ZLay_Style ZLay_StyleWithGridSpan(ZLay_Style style, uint32_t column_span, uint32_t row_span);
ZLAY_API ZLay_Style ZLay_StyleWithColors(ZLay_Style style, ZLay_Color background, ZLay_Color text);
ZLAY_API ZLay_Style ZLay_StackStyle(ZLay_FlexDirection direction, float gap, ZLay_EdgeInsets padding);
ZLAY_API ZLay_Style ZLay_SurfaceStyle(ZLay_SurfaceVariant variant, const ZLay_Theme* theme);
ZLAY_API ZLay_Style ZLay_CardStyle(const ZLay_Theme* theme);
ZLAY_API ZLay_Style ZLay_SpacerStyle(float size, ZLay_Axis axis);
ZLAY_API ZLay_Style ZLay_DividerStyle(ZLay_Axis axis, const ZLay_Theme* theme);

ZLAY_API ZLay_NodeDeclarationBuilder ZLay_RowDecl(ZLay_Id id, float gap, ZLay_EdgeInsets padding);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_ColumnDecl(ZLay_Id id, float gap, ZLay_EdgeInsets padding);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_GridDecl(ZLay_Id id, uint32_t columns, ZLay_Size auto_row_height, float gap, ZLay_EdgeInsets padding);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_PanelDecl(ZLay_Id id, const ZLay_Theme* theme);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_CardDecl(ZLay_Id id, const ZLay_Theme* theme);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_SpacerDecl(ZLay_Id id, float size, ZLay_Axis axis);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_DividerDecl(ZLay_Id id, ZLay_Axis axis, const ZLay_Theme* theme);

ZLAY_API void ZLay_Spacer(ZLay_Context* ctx, ZLay_Id id, float size, ZLay_Axis axis);
ZLAY_API void ZLay_Divider(ZLay_Context* ctx, ZLay_Id id, ZLay_Axis axis, const ZLay_Theme* theme);

#ifdef __cplusplus
}
#endif

#include <ui/label.h>
#include <ui/button.h>
#include <ui/input.h>

#endif
