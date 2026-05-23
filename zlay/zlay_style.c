#include <zlay_internal.h>

ZLay_Config ZLay_ConfigDefault(void) {
  ZLay_Config c;
  c.max_nodes = 4096;
  c.max_commands = 4096;
  c.max_stack = 256;
  return c;
}

size_t ZLay_MinMemorySize(ZLay_Config config) {
  // Layout:
  // - context
  // - nodes
  // - commands
  // - open stack
  size_t bytes = 0;
  bytes += sizeof(ZLay_Context);
  bytes += sizeof(ZLay_Node) * (size_t)config.max_nodes;
  bytes += sizeof(ZLay_RenderCommand) * (size_t)config.max_commands;
  bytes += sizeof(int32_t) * (size_t)config.max_stack;
  // Small safety margin for alignment
  bytes += 256;
  return bytes;
}

ZLay_Size ZLay_Px(float v) { return (ZLay_Size){ZLAY_SIZE_PX, v}; }
ZLay_Size ZLay_Percent(float v) { return (ZLay_Size){ZLAY_SIZE_PERCENT, v}; }
ZLay_Size ZLay_Grow(float weight) { return (ZLay_Size){ZLAY_SIZE_GROW, weight}; }

const char* ZLay_DefaultFontFamily(void) {
  return ZLAY_DEFAULT_FONT_FAMILY;
}

ZLay_String ZLay_DefaultFontFamilyString(void) {
  return ZLAY_STRING_LITERAL(ZLAY_DEFAULT_FONT_FAMILY);
}

ZLay_Style ZLay_StyleDefault(void) {
  ZLay_Style s;
  s.width = (ZLay_Size){ZLAY_SIZE_AUTO, 0};
  s.height = (ZLay_Size){ZLAY_SIZE_AUTO, 0};

  s.padding_left = s.padding_top = s.padding_right = s.padding_bottom = 0;
  s.margin_left = s.margin_top = s.margin_right = s.margin_bottom = 0;
  s.gap = 0;

  s.layout_mode = ZLAY_LAYOUT_FLEX;
  s.flex_direction = ZLAY_FLEX_COLUMN;
  s.grid_columns = 1u;
  s.grid_rows = 0u;
  s.grid_auto_row_height = (ZLay_Size){ZLAY_SIZE_AUTO, 0.0f};
  s.grid_column_gap = 0.0f;
  s.grid_row_gap = 0.0f;
  s.grid_column_span = 1u;
  s.grid_row_span = 1u;
  s.position = ZLAY_POSITION_RELATIVE;
  s.left = s.top = s.right = s.bottom = 0;
  s.z_index = 0;

  s.background = (ZLay_Color){0, 0, 0, 0};
  s.text_color = (ZLay_Color){255, 255, 255, 255};
  s.border_color = (ZLay_Color){0, 0, 0, 0};
  s.border_width = 0.0f;
  s.radius = 0;
  s.rounded = ZLay_RoundedCircular(0.0f);
  s.shadow = ZLay_ShadowNone();
  s.font_family = ZLay_DefaultFontFamilyString();
  s.font_size = 0.0f;
  s.pointer_cursor = ZLAY_POINTER_CURSOR_DEFAULT;
  s.text_cursor_visible = false;
  s.text_cursor_index = 0u;
  s.text_cursor_width = 1.0f;
  s.text_cursor_color = (ZLay_Color){255, 255, 255, 255};
  s.text_selection_visible = false;
  s.text_selection_start = 0u;
  s.text_selection_end = 0u;
  s.text_selection_color = (ZLay_Color){80, 140, 255, 96};
  return s;
}

ZLay_EdgeInsets ZLay_EdgeInsetsAll(float value) {
  return (ZLay_EdgeInsets){value, value, value, value};
}

ZLay_EdgeInsets ZLay_EdgeInsetsSymmetric(float horizontal, float vertical) {
  return (ZLay_EdgeInsets){horizontal, vertical, horizontal, vertical};
}

ZLay_EdgeInsets ZLay_EdgeInsetsOnly(float left, float top, float right, float bottom) {
  return (ZLay_EdgeInsets){left, top, right, bottom};
}

ZLay_Style ZLay_StyleWithBackground(ZLay_Style style, ZLay_Color color) {
  style.background = color;
  return style;
}

ZLay_Style ZLay_StyleWithRadius(ZLay_Style style, float radius) {
  return ZLay_StyleWithRounded(style, ZLay_RoundedCircular(radius));
}

ZLay_Style ZLay_StyleWithRounded(ZLay_Style style, ZLay_Rounded rounded) {
  style.rounded = ZLay_RoundedResolve(rounded);
  style.radius = style.rounded.radius;
  return style;
}

ZLay_Style ZLay_StyleWithRoundedPreset(ZLay_Style style, ZLay_RoundedPreset preset) {
  return ZLay_StyleWithRounded(style, ZLay_RoundedPresetValue(preset));
}

ZLay_Shadow ZLay_ShadowNone(void) {
  return (ZLay_Shadow){(ZLay_Color){0, 0, 0, 0}, 0.0f, 0.0f, 0.0f, 0.0f};
}

ZLay_Shadow ZLay_ShadowMake(ZLay_Color color, float offset_x, float offset_y, float blur, float spread) {
  ZLay_Shadow shadow;
  shadow.color = color;
  shadow.offset_x = offset_x;
  shadow.offset_y = offset_y;
  shadow.blur = blur < 0.0f ? 0.0f : blur;
  shadow.spread = spread;
  return shadow;
}

ZLay_Shadow ZLay_ShadowPresetValue(ZLay_ShadowPreset preset) {
  switch (preset) {
    case ZLAY_SHADOW_SM:
      return ZLay_ShadowMake((ZLay_Color){0, 0, 0, 30}, 0.0f, 1.0f, 3.0f, 0.0f);
    case ZLAY_SHADOW_MD:
      return ZLay_ShadowMake((ZLay_Color){0, 0, 0, 38}, 0.0f, 4.0f, 10.0f, -1.0f);
    case ZLAY_SHADOW_LG:
      return ZLay_ShadowMake((ZLay_Color){0, 0, 0, 42}, 0.0f, 10.0f, 18.0f, -3.0f);
    case ZLAY_SHADOW_XL:
      return ZLay_ShadowMake((ZLay_Color){0, 0, 0, 46}, 0.0f, 20.0f, 30.0f, -5.0f);
    case ZLAY_SHADOW_2XL:
      return ZLay_ShadowMake((ZLay_Color){0, 0, 0, 58}, 0.0f, 25.0f, 50.0f, -12.0f);
    case ZLAY_SHADOW_NONE:
    default:
      return ZLay_ShadowNone();
  }
}

ZLay_Shadow ZLay_ShadowPx(float offset_x, float offset_y, float blur, float spread, uint8_t alpha) {
  return ZLay_ShadowMake((ZLay_Color){0, 0, 0, alpha}, offset_x, offset_y, blur, spread);
}

ZLay_Shadow ZLay_ShadowMacOS(void) {
  return ZLay_ShadowMake((ZLay_Color){0, 0, 0, 46}, 0.0f, 12.0f, 28.0f, -2.0f);
}

ZLay_Style ZLay_StyleWithBorder(ZLay_Style style, float width, ZLay_Color color) {
  style.border_width = width < 0.0f ? 0.0f : width;
  style.border_color = color;
  return style;
}

ZLay_Style ZLay_StyleWithShadow(ZLay_Style style, ZLay_Shadow shadow) {
  style.shadow = shadow;
  return style;
}

ZLay_Style ZLay_StyleWithShadowPreset(ZLay_Style style, ZLay_ShadowPreset preset) {
  style.shadow = ZLay_ShadowPresetValue(preset);
  return style;
}

ZLay_Style ZLay_StyleWithFont(ZLay_Style style, ZLay_String family, float size) {
  style.font_family = family;
  style.font_size = size;
  return style;
}

ZLay_Style ZLay_StyleWithPointerCursor(ZLay_Style style, ZLay_PointerCursor cursor) {
  style.pointer_cursor = cursor;
  return style;
}

ZLay_Style ZLay_StyleWithTextCursor(ZLay_Style style, bool visible, uint32_t index, ZLay_Color color) {
  style.text_cursor_visible = visible;
  style.text_cursor_index = index;
  style.text_cursor_color = color;
  if (style.text_cursor_width <= 0.0f) {
    style.text_cursor_width = 1.0f;
  }
  return style;
}

ZLay_Style ZLay_StyleWithTextSelection(ZLay_Style style, uint32_t start, uint32_t end, ZLay_Color color) {
  style.text_selection_visible = start != end;
  style.text_selection_start = start;
  style.text_selection_end = end;
  style.text_selection_color = color;
  return style;
}

ZLay_Style ZLay_RowStyle(ZLay_Size width, ZLay_Size height, float gap) {
  ZLay_Style style = ZLay_StyleDefault();
  style.width = width;
  style.height = height;
  style.layout_mode = ZLAY_LAYOUT_FLEX;
  style.flex_direction = ZLAY_FLEX_ROW;
  style.gap = gap;
  return style;
}

ZLay_Style ZLay_ColumnStyle(ZLay_Size width, ZLay_Size height, float gap) {
  ZLay_Style style = ZLay_StyleDefault();
  style.width = width;
  style.height = height;
  style.layout_mode = ZLAY_LAYOUT_FLEX;
  style.flex_direction = ZLAY_FLEX_COLUMN;
  style.gap = gap;
  return style;
}

ZLay_Style ZLay_GridStyle(ZLay_Size width, ZLay_Size height, uint32_t columns, ZLay_Size auto_row_height, float gap) {
  ZLay_Style style = ZLay_StyleDefault();
  style.width = width;
  style.height = height;
  style.layout_mode = ZLAY_LAYOUT_GRID;
  style.grid_columns = columns == 0u ? 1u : columns;
  style.grid_auto_row_height = auto_row_height;
  style.grid_column_gap = gap;
  style.grid_row_gap = gap;
  style.gap = gap;
  return style;
}

ZLay_NodeDeclarationBuilder ZLay_BoxDecl(ZLay_Id id, ZLay_Style style) {
  ZLay_NodeDeclarationBuilder b;
  b.decl.type = ZLAY_NODE_BOX;
  b.decl.id = id;
  b.decl.style = style;
  b.decl.text = (ZLay_String){0};
  return b;
}

ZLay_NodeDeclarationBuilder ZLay_TextDecl(ZLay_Id id, ZLay_String text, ZLay_Style style) {
  ZLay_NodeDeclarationBuilder b;
  b.decl.type = ZLAY_NODE_TEXT;
  b.decl.id = id;
  b.decl.style = style;
  b.decl.text = text;
  return b;
}
