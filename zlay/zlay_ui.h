#ifndef ZLAY_UI_H
#define ZLAY_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <zlay.h>
#include <zlay_color.h>

typedef enum ZLay_ComponentSize {
  ZLAY_COMPONENT_SIZE_SM = 0,
  ZLAY_COMPONENT_SIZE_MD = 1,
  ZLAY_COMPONENT_SIZE_LG = 2
} ZLay_ComponentSize;

typedef enum ZLay_ComponentStateFlags {
  ZLAY_COMPONENT_STATE_NONE = 0,
  ZLAY_COMPONENT_STATE_HOVERED = 1 << 0,
  ZLAY_COMPONENT_STATE_ACTIVE = 1 << 1,
  ZLAY_COMPONENT_STATE_FOCUSED = 1 << 2,
  ZLAY_COMPONENT_STATE_DISABLED = 1 << 3,
  ZLAY_COMPONENT_STATE_INVALID = 1 << 4,
  ZLAY_COMPONENT_STATE_SELECTED = 1 << 5
} ZLay_ComponentStateFlags;

typedef enum ZLay_SurfaceVariant {
  ZLAY_SURFACE_CANVAS = 0,
  ZLAY_SURFACE_PANEL = 1,
  ZLAY_SURFACE_CARD = 2,
  ZLAY_SURFACE_MUTED = 3
} ZLay_SurfaceVariant;

typedef enum ZLay_Axis {
  ZLAY_AXIS_HORIZONTAL = 0,
  ZLAY_AXIS_VERTICAL = 1
} ZLay_Axis;

typedef struct ZLay_EdgeInsets {
  float left;
  float top;
  float right;
  float bottom;
} ZLay_EdgeInsets;

typedef struct ZLay_Theme {
  ZLay_Color canvas;
  ZLay_Color surface;
  ZLay_Color surface_raised;
  ZLay_Color surface_muted;
  ZLay_Color border;
  ZLay_Color border_strong;
  ZLay_Color text;
  ZLay_Color text_muted;
  ZLay_Color text_inverse;
  ZLay_Color primary;
  ZLay_Color primary_hover;
  ZLay_Color primary_active;
  ZLay_Color primary_soft;
  ZLay_Color danger;
  ZLay_Color danger_hover;
  ZLay_Color danger_soft;
  ZLay_Color success;
  ZLay_Color warning;
  ZLay_Color focus_ring;
  float space_1;
  float space_2;
  float space_3;
  float space_4;
  float space_5;
  float space_6;
  float space_8;
  float radius_sm;
  float radius_md;
  float radius_lg;
  float radius_pill;
  float control_sm;
  float control_md;
  float control_lg;
} ZLay_Theme;

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
ZLAY_API ZLay_Style ZLay_StyleWithColors(ZLay_Style style, ZLay_Color background, ZLay_Color text);
ZLAY_API ZLay_Style ZLay_StackStyle(ZLay_FlexDirection direction, float gap, ZLay_EdgeInsets padding);
ZLAY_API ZLay_Style ZLay_SurfaceStyle(ZLay_SurfaceVariant variant, const ZLay_Theme* theme);
ZLAY_API ZLay_Style ZLay_CardStyle(const ZLay_Theme* theme);
ZLAY_API ZLay_Style ZLay_SpacerStyle(float size, ZLay_Axis axis);
ZLAY_API ZLay_Style ZLay_DividerStyle(ZLay_Axis axis, const ZLay_Theme* theme);

ZLAY_API ZLay_NodeDeclarationBuilder ZLay_RowDecl(ZLay_Id id, float gap, ZLay_EdgeInsets padding);
ZLAY_API ZLay_NodeDeclarationBuilder ZLay_ColumnDecl(ZLay_Id id, float gap, ZLay_EdgeInsets padding);
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
