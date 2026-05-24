#include <zlay_ui.h>

static const ZLay_Theme* ZLay__ThemeOrDefault(const ZLay_Theme* theme) {
  return theme ? theme : ZLay_ThemeDefault();
}

static void ZLay__EmitBox(ZLay_Context* ctx, ZLay_Id id, ZLay_Style style) {
  ZLay_NodeDeclarationBuilder builder = ZLay_BoxDecl(id, style);
  ZLay__OpenElement(ctx, id, ZLAY_NODE_BOX);
  ZLay__ConfigureOpenElement(ctx, &builder.decl);
  ZLay__CloseElement(ctx);
}

bool ZLay_StringIsEmpty(ZLay_String string) {
  return !string.chars || string.length == 0;
}

ZLay_String ZLay_StringFromCString(const char* cstr) {
  ZLay_String result = {0};
  result.chars = cstr;
  result.is_static = true;
  if (!cstr) return result;
  while (cstr[result.length]) result.length++;
  return result;
}

ZLay_EdgeInsets ZLay_Insets(float all) {
  return ZLay_InsetsLTRB(all, all, all, all);
}

ZLay_EdgeInsets ZLay_InsetsXY(float x, float y) {
  return ZLay_InsetsLTRB(x, y, x, y);
}

ZLay_EdgeInsets ZLay_InsetsLTRB(float left, float top, float right, float bottom) {
  return (ZLay_EdgeInsets){left, top, right, bottom};
}

ZLay_Theme ZLay_ThemeLight(void) {
  ZLay_Theme theme;
  theme.canvas = ZLay_ColorRGB(246, 248, 251);
  theme.surface = ZLay_ColorRGB(255, 255, 255);
  theme.surface_raised = ZLay_ColorRGB(255, 255, 255);
  theme.surface_muted = ZLay_ColorRGB(239, 243, 248);
  theme.border = ZLay_ColorRGB(220, 228, 238);
  theme.border_strong = ZLay_ColorRGB(188, 199, 214);
  theme.text = ZLay_ColorRGB(18, 24, 38);
  theme.text_muted = ZLay_ColorRGB(91, 103, 125);
  theme.text_inverse = ZLay_ColorRGB(255, 255, 255);
  theme.primary = ZLay_ColorRGB(54, 105, 255);
  theme.primary_hover = ZLay_ColorRGB(39, 89, 235);
  theme.primary_active = ZLay_ColorRGB(26, 70, 205);
  theme.primary_soft = ZLay_ColorRGB(225, 234, 255);
  theme.danger = ZLay_ColorRGB(220, 53, 69);
  theme.danger_hover = ZLay_ColorRGB(196, 38, 54);
  theme.danger_soft = ZLay_ColorRGB(255, 229, 233);
  theme.success = ZLay_ColorRGB(20, 160, 92);
  theme.warning = ZLay_ColorRGB(235, 164, 31);
  theme.focus_ring = ZLay_ColorRGBA(54, 105, 255, 96);
  theme.space_1 = 4.0f;
  theme.space_2 = 8.0f;
  theme.space_3 = 12.0f;
  theme.space_4 = 16.0f;
  theme.space_5 = 20.0f;
  theme.space_6 = 24.0f;
  theme.space_8 = 32.0f;
  theme.radius_sm = 6.0f;
  theme.radius_md = 10.0f;
  theme.radius_lg = 16.0f;
  theme.radius_pill = 999.0f;
  theme.control_sm = 32.0f;
  theme.control_md = 40.0f;
  theme.control_lg = 48.0f;
  return theme;
}

ZLay_Theme ZLay_ThemeDark(void) {
  ZLay_Theme theme;
  theme.canvas = ZLay_ColorRGB(11, 15, 24);
  theme.surface = ZLay_ColorRGB(17, 24, 39);
  theme.surface_raised = ZLay_ColorRGB(25, 34, 51);
  theme.surface_muted = ZLay_ColorRGB(31, 41, 59);
  theme.border = ZLay_ColorRGB(51, 65, 85);
  theme.border_strong = ZLay_ColorRGB(71, 85, 105);
  theme.text = ZLay_ColorRGB(241, 245, 249);
  theme.text_muted = ZLay_ColorRGB(148, 163, 184);
  theme.text_inverse = ZLay_ColorRGB(15, 23, 42);
  theme.primary = ZLay_ColorRGB(96, 165, 250);
  theme.primary_hover = ZLay_ColorRGB(59, 130, 246);
  theme.primary_active = ZLay_ColorRGB(37, 99, 235);
  theme.primary_soft = ZLay_ColorRGB(30, 58, 138);
  theme.danger = ZLay_ColorRGB(248, 113, 113);
  theme.danger_hover = ZLay_ColorRGB(239, 68, 68);
  theme.danger_soft = ZLay_ColorRGB(127, 29, 29);
  theme.success = ZLay_ColorRGB(52, 211, 153);
  theme.warning = ZLay_ColorRGB(251, 191, 36);
  theme.focus_ring = ZLay_ColorRGBA(96, 165, 250, 112);
  theme.space_1 = 4.0f;
  theme.space_2 = 8.0f;
  theme.space_3 = 12.0f;
  theme.space_4 = 16.0f;
  theme.space_5 = 20.0f;
  theme.space_6 = 24.0f;
  theme.space_8 = 32.0f;
  theme.radius_sm = 6.0f;
  theme.radius_md = 10.0f;
  theme.radius_lg = 16.0f;
  theme.radius_pill = 999.0f;
  theme.control_sm = 32.0f;
  theme.control_md = 40.0f;
  theme.control_lg = 48.0f;
  return theme;
}

const ZLay_Theme* ZLay_ThemeDefault(void) {
  static bool initialized = false;
  static ZLay_Theme theme;
  if (!initialized) {
    theme = ZLay_ThemeDark();
    initialized = true;
  }
  return &theme;
}

float ZLay_ComponentControlHeight(ZLay_ComponentSize size, const ZLay_Theme* theme) {
  const ZLay_Theme* resolved = ZLay__ThemeOrDefault(theme);
  switch (size) {
    case ZLAY_COMPONENT_SIZE_SM: return resolved->control_sm;
    case ZLAY_COMPONENT_SIZE_LG: return resolved->control_lg;
    case ZLAY_COMPONENT_SIZE_MD:
    default: return resolved->control_md;
  }
}

ZLay_EdgeInsets ZLay_ComponentControlPadding(ZLay_ComponentSize size, const ZLay_Theme* theme) {
  const ZLay_Theme* resolved = ZLay__ThemeOrDefault(theme);
  switch (size) {
    case ZLAY_COMPONENT_SIZE_SM: return ZLay_InsetsXY(resolved->space_3, resolved->space_2);
    case ZLAY_COMPONENT_SIZE_LG: return ZLay_InsetsXY(resolved->space_5, resolved->space_3);
    case ZLAY_COMPONENT_SIZE_MD:
    default: return ZLay_InsetsXY(resolved->space_4, resolved->space_2);
  }
}

ZLay_Style ZLay_StyleWithSize(ZLay_Style style, ZLay_Size width, ZLay_Size height) {
  style.width = width;
  style.height = height;
  return style;
}

ZLay_Style ZLay_StyleWithPadding(ZLay_Style style, ZLay_EdgeInsets padding) {
  style.padding_left = padding.left;
  style.padding_top = padding.top;
  style.padding_right = padding.right;
  style.padding_bottom = padding.bottom;
  return style;
}

ZLay_Style ZLay_StyleWithMargin(ZLay_Style style, ZLay_EdgeInsets margin) {
  style.margin_left = margin.left;
  style.margin_top = margin.top;
  style.margin_right = margin.right;
  style.margin_bottom = margin.bottom;
  return style;
}

ZLay_Style ZLay_StyleWithGap(ZLay_Style style, float gap) {
  style.gap = gap;
  return style;
}

ZLay_Style ZLay_StyleWithDirection(ZLay_Style style, ZLay_FlexDirection direction) {
  style.layout_mode = ZLAY_LAYOUT_FLEX;
  style.flex_direction = direction;
  return style;
}

ZLay_Style ZLay_StyleWithGrid(ZLay_Style style, uint32_t columns, ZLay_Size auto_row_height, float column_gap, float row_gap) {
  style.layout_mode = ZLAY_LAYOUT_GRID;
  style.grid_columns = columns == 0u ? 1u : columns;
  style.grid_auto_row_height = auto_row_height;
  style.grid_column_gap = column_gap;
  style.grid_row_gap = row_gap;
  style.gap = column_gap > row_gap ? column_gap : row_gap;
  return style;
}

ZLay_Style ZLay_StyleWithGridSpan(ZLay_Style style, uint32_t column_span, uint32_t row_span) {
  style.grid_column_span = column_span == 0u ? 1u : column_span;
  style.grid_row_span = row_span == 0u ? 1u : row_span;
  return style;
}

ZLay_Style ZLay_StyleWithColors(ZLay_Style style, ZLay_Color background, ZLay_Color text) {
  style.background = background;
  style.text_color = text;
  return style;
}

ZLay_Style ZLay_StackStyle(ZLay_FlexDirection direction, float gap, ZLay_EdgeInsets padding) {
  ZLay_Style style = ZLay_StyleDefault();
  style.layout_mode = ZLAY_LAYOUT_FLEX;
  style.flex_direction = direction;
  style.gap = gap;
  return ZLay_StyleWithPadding(style, padding);
}

ZLay_Style ZLay_SurfaceStyle(ZLay_SurfaceVariant variant, const ZLay_Theme* theme) {
  const ZLay_Theme* resolved = ZLay__ThemeOrDefault(theme);
  ZLay_Style style = ZLay_StyleDefault();
  style.flex_direction = ZLAY_FLEX_COLUMN;
  style.gap = resolved->space_4;
  style.radius = resolved->radius_lg;
  style.text_color = resolved->text;
  style.padding_left = style.padding_top = style.padding_right = style.padding_bottom = resolved->space_4;

  switch (variant) {
    case ZLAY_SURFACE_CANVAS: style.background = resolved->canvas; break;
    case ZLAY_SURFACE_CARD:
      style.background = resolved->surface_raised;
      style.border_width = 1.0f;
      style.border_color = ZLay_ColorWithAlpha(resolved->border, 128);
      style.shadow = ZLay_ShadowFloating();
      break;
    case ZLAY_SURFACE_MUTED: style.background = resolved->surface_muted; break;
    case ZLAY_SURFACE_PANEL:
    default:
      style.background = resolved->surface;
      style.border_width = 1.0f;
      style.border_color = ZLay_ColorWithAlpha(resolved->border, 96);
      break;
  }
  return style;
}

ZLay_Style ZLay_CardStyle(const ZLay_Theme* theme) {
  return ZLay_SurfaceStyle(ZLAY_SURFACE_CARD, theme);
}

ZLay_Style ZLay_SpacerStyle(float size, ZLay_Axis axis) {
  ZLay_Style style = ZLay_StyleDefault();
  if (axis == ZLAY_AXIS_HORIZONTAL) {
    style.width = ZLay_Px(size);
    style.height = ZLay_Px(1.0f);
  } else {
    style.width = ZLay_Px(1.0f);
    style.height = ZLay_Px(size);
  }
  return style;
}

ZLay_Style ZLay_DividerStyle(ZLay_Axis axis, const ZLay_Theme* theme) {
  const ZLay_Theme* resolved = ZLay__ThemeOrDefault(theme);
  ZLay_Style style = ZLay_SpacerStyle(1.0f, axis == ZLAY_AXIS_HORIZONTAL ? ZLAY_AXIS_VERTICAL : ZLAY_AXIS_HORIZONTAL);
  style.background = resolved->border;
  if (axis == ZLAY_AXIS_HORIZONTAL) {
    style.width = ZLay_Grow(1.0f);
    style.height = ZLay_Px(1.0f);
  } else {
    style.width = ZLay_Px(1.0f);
    style.height = ZLay_Grow(1.0f);
  }
  return style;
}

ZLay_NodeDeclarationBuilder ZLay_RowDecl(ZLay_Id id, float gap, ZLay_EdgeInsets padding) {
  return ZLay_BoxDecl(id, ZLay_StackStyle(ZLAY_FLEX_ROW, gap, padding));
}

ZLay_NodeDeclarationBuilder ZLay_ColumnDecl(ZLay_Id id, float gap, ZLay_EdgeInsets padding) {
  return ZLay_BoxDecl(id, ZLay_StackStyle(ZLAY_FLEX_COLUMN, gap, padding));
}

ZLay_NodeDeclarationBuilder ZLay_GridDecl(ZLay_Id id, uint32_t columns, ZLay_Size auto_row_height, float gap, ZLay_EdgeInsets padding) {
  ZLay_Style style = ZLay_GridStyle(ZLay_Grow(1.0f), ZLay_Grow(1.0f), columns, auto_row_height, gap);
  style = ZLay_StyleWithPadding(style, padding);
  return ZLay_BoxDecl(id, style);
}

ZLay_NodeDeclarationBuilder ZLay_PanelDecl(ZLay_Id id, const ZLay_Theme* theme) {
  return ZLay_BoxDecl(id, ZLay_SurfaceStyle(ZLAY_SURFACE_PANEL, theme));
}

ZLay_NodeDeclarationBuilder ZLay_CardDecl(ZLay_Id id, const ZLay_Theme* theme) {
  return ZLay_BoxDecl(id, ZLay_CardStyle(theme));
}

ZLay_NodeDeclarationBuilder ZLay_SpacerDecl(ZLay_Id id, float size, ZLay_Axis axis) {
  return ZLay_BoxDecl(id, ZLay_SpacerStyle(size, axis));
}

ZLay_NodeDeclarationBuilder ZLay_DividerDecl(ZLay_Id id, ZLay_Axis axis, const ZLay_Theme* theme) {
  return ZLay_BoxDecl(id, ZLay_DividerStyle(axis, theme));
}

void ZLay_Spacer(ZLay_Context* ctx, ZLay_Id id, float size, ZLay_Axis axis) {
  ZLay__EmitBox(ctx, id, ZLay_SpacerStyle(size, axis));
}

void ZLay_Divider(ZLay_Context* ctx, ZLay_Id id, ZLay_Axis axis, const ZLay_Theme* theme) {
  ZLay__EmitBox(ctx, id, ZLay_DividerStyle(axis, theme));
}
