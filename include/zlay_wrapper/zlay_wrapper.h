#ifndef ZLAY_WRAPPER_H
#define ZLAY_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <zlay_ui.h>

typedef enum ZLayW_DeviceClass {
  ZLAYW_DEVICE_WEB = 0,
  ZLAYW_DEVICE_MOBILE = 1
} ZLayW_DeviceClass;

typedef enum ZLayW_SurfaceKind {
  ZLAYW_SURFACE_CANVAS = 0,
  ZLAYW_SURFACE_PANEL = 1,
  ZLAYW_SURFACE_CARD = 2,
  ZLAYW_SURFACE_MUTED = 3
} ZLayW_SurfaceKind;

typedef struct ZLayW_ScaffoldProps {
  const ZLay_Theme* theme;
  ZLayW_DeviceClass device;
  float width;
  float height;
  float padding;
  float gap;
} ZLayW_ScaffoldProps;

typedef struct ZLayW_BoxProps {
  const ZLay_Theme* theme;
  ZLay_Size width;
  ZLay_Size height;
  ZLay_EdgeInsets padding;
  ZLay_EdgeInsets margin;
  float gap;
  float radius;
  ZLay_Rounded rounded;
  ZLay_Color background;
  ZLay_Color text_color;
  ZLay_Color border_color;
  float border_width;
  ZLay_Shadow shadow;
  ZLay_LayoutMode layout_mode;
  ZLay_FlexDirection direction;
  uint32_t grid_columns;
  ZLay_Size grid_auto_row_height;
  float grid_column_gap;
  float grid_row_gap;
  bool use_background;
  bool use_text_color;
  bool use_radius;
  bool use_rounded;
  bool use_border;
  bool use_shadow;
} ZLayW_BoxProps;

static inline void ZLayW_BoxPropsSetShadowPreset(ZLayW_BoxProps* props, ZLay_ShadowPreset preset) {
  if (!props) return;
  props->shadow = ZLay_ShadowPresetValue(preset);
  props->use_shadow = true;
}

static inline void ZLayW_BoxPropsSetShadowPx(ZLayW_BoxProps* props, float offset_x, float offset_y, float blur, float spread, uint8_t alpha) {
  if (!props) return;
  props->shadow = ZLay_ShadowPx(offset_x, offset_y, blur, spread, alpha);
  props->use_shadow = true;
}

static inline void ZLayW_BoxPropsSetGrid(ZLayW_BoxProps* props, uint32_t columns, ZLay_Size auto_row_height, float column_gap, float row_gap) {
  if (!props) return;
  props->layout_mode = ZLAY_LAYOUT_GRID;
  props->grid_columns = columns == 0u ? 1u : columns;
  props->grid_auto_row_height = auto_row_height;
  props->grid_column_gap = column_gap;
  props->grid_row_gap = row_gap;
}

static inline void ZLayW_BoxPropsSetRounded(ZLayW_BoxProps* props, ZLay_Rounded rounded) {
  if (!props) return;
  props->rounded = rounded;
  props->use_rounded = true;
}

static inline void ZLayW_BoxPropsSetRoundedPreset(ZLayW_BoxProps* props, ZLay_RoundedPreset preset) {
  if (!props) return;
  props->rounded = ZLay_RoundedPresetValue(preset);
  props->use_rounded = true;
}

typedef struct ZLayW_AppBarProps {
  const ZLay_Theme* theme;
  float height;
} ZLayW_AppBarProps;

typedef struct ZLayW_TextProps {
  const ZLay_Theme* theme;
  ZLay_LabelRole role;
  ZLay_ComponentSize size;
  ZLay_Color color;
  bool use_color;
} ZLayW_TextProps;

typedef struct ZLayW_ButtonProps {
  const ZLay_Theme* theme;
  ZLay_ButtonVariant variant;
  ZLay_ComponentSize size;
  uint32_t state;
  ZLay_Size width;
  bool use_width;
} ZLayW_ButtonProps;

typedef struct ZLayW_InputProps {
  const ZLay_Theme* theme;
  ZLay_InputVariant variant;
  ZLay_ComponentSize size;
  uint32_t state;
  ZLay_Size width;
  bool use_width;
  bool cursor_visible;
  uint32_t cursor_index;
  uint32_t selection_start;
  uint32_t selection_end;
  bool selection_visible;
} ZLayW_InputProps;

static inline ZLay_String ZLayW_Str(const char* text) {
  return ZLay_StringFromCString(text);
}

static inline const ZLay_Theme* ZLayW_ThemeOrDefault(const ZLay_Theme* theme) {
  return theme ? theme : ZLay_ThemeDefault();
}

static inline ZLayW_ScaffoldProps ZLayW_ScaffoldPropsDefault(const ZLay_Theme* theme) {
  ZLayW_ScaffoldProps props;
  props.theme = ZLayW_ThemeOrDefault(theme);
  props.device = ZLAYW_DEVICE_WEB;
  props.width = 960.0f;
  props.height = 540.0f;
  props.padding = props.theme->space_6;
  props.gap = props.theme->space_5;
  return props;
}

static inline ZLayW_BoxProps ZLayW_BoxPropsDefault(const ZLay_Theme* theme) {
  ZLayW_BoxProps props;
  ZLay_Style style = ZLay_StyleDefault();
  props.theme = ZLayW_ThemeOrDefault(theme);
  props.width = style.width;
  props.height = style.height;
  props.padding = ZLay_Insets(0.0f);
  props.margin = ZLay_Insets(0.0f);
  props.gap = props.theme->space_4;
  props.radius = 0.0f;
  props.rounded = ZLay_RoundedCircular(0.0f);
  props.background = props.theme->surface;
  props.text_color = props.theme->text;
  props.border_color = props.theme->border;
  props.border_width = 0.0f;
  props.shadow = ZLay_ShadowNone();
  props.layout_mode = ZLAY_LAYOUT_FLEX;
  props.direction = ZLAY_FLEX_COLUMN;
  props.grid_columns = 1u;
  props.grid_auto_row_height = ZLay_Px(0.0f);
  props.grid_auto_row_height.type = ZLAY_SIZE_AUTO;
  props.grid_column_gap = props.gap;
  props.grid_row_gap = props.gap;
  props.use_background = false;
  props.use_text_color = false;
  props.use_radius = false;
  props.use_rounded = false;
  props.use_border = false;
  props.use_shadow = false;
  return props;
}

static inline ZLayW_AppBarProps ZLayW_AppBarPropsDefault(const ZLay_Theme* theme) {
  ZLayW_AppBarProps props;
  props.theme = ZLayW_ThemeOrDefault(theme);
  props.height = 56.0f;
  return props;
}

static inline ZLayW_TextProps ZLayW_TextPropsDefault(const ZLay_Theme* theme) {
  ZLayW_TextProps props;
  props.theme = ZLayW_ThemeOrDefault(theme);
  props.role = ZLAY_LABEL_BODY;
  props.size = ZLAY_COMPONENT_SIZE_MD;
  props.color = props.theme->text;
  props.use_color = false;
  return props;
}

static inline ZLayW_ButtonProps ZLayW_ButtonPropsDefault(const ZLay_Theme* theme) {
  ZLayW_ButtonProps props;
  props.theme = ZLayW_ThemeOrDefault(theme);
  props.variant = ZLAY_BUTTON_PRIMARY;
  props.size = ZLAY_COMPONENT_SIZE_MD;
  props.state = ZLAY_COMPONENT_STATE_NONE;
  props.width = ZLay_Px(0.0f);
  props.use_width = false;
  return props;
}

static inline ZLayW_InputProps ZLayW_InputPropsDefault(const ZLay_Theme* theme) {
  ZLayW_InputProps props;
  props.theme = ZLayW_ThemeOrDefault(theme);
  props.variant = ZLAY_INPUT_FILLED;
  props.size = ZLAY_COMPONENT_SIZE_MD;
  props.state = ZLAY_COMPONENT_STATE_NONE;
  props.width = ZLay_Grow(1.0f);
  props.use_width = true;
  props.cursor_visible = false;
  props.cursor_index = 0u;
  props.selection_start = 0u;
  props.selection_end = 0u;
  props.selection_visible = false;
  return props;
}

static inline ZLay_Style ZLayW_BoxStyle(const ZLayW_BoxProps* props) {
  ZLayW_BoxProps defaults = ZLayW_BoxPropsDefault(NULL);
  const ZLayW_BoxProps* p = props ? props : &defaults;
  ZLay_Style style = ZLay_StackStyle(p->direction, p->gap, p->padding);
  if (p->layout_mode == ZLAY_LAYOUT_GRID) {
    style = ZLay_StyleWithGrid(style, p->grid_columns, p->grid_auto_row_height, p->grid_column_gap, p->grid_row_gap);
  }
  style.width = p->width;
  style.height = p->height;
  style = ZLay_StyleWithMargin(style, p->margin);
  if (p->use_background) style.background = p->background;
  if (p->use_text_color) style.text_color = p->text_color;
  if (p->use_radius) style = ZLay_StyleWithRadius(style, p->radius);
  if (p->use_rounded) style = ZLay_StyleWithRounded(style, p->rounded);
  if (p->use_border) style = ZLay_StyleWithBorder(style, p->border_width, p->border_color);
  if (p->use_shadow) style = ZLay_StyleWithShadow(style, p->shadow);
  return style;
}

static inline ZLay_NodeDeclarationBuilder ZLayW_ScaffoldDecl(ZLay_Id id, const ZLayW_ScaffoldProps* props) {
  ZLayW_ScaffoldProps defaults = ZLayW_ScaffoldPropsDefault(NULL);
  const ZLayW_ScaffoldProps* p = props ? props : &defaults;
  ZLay_Style style = ZLay_StackStyle(ZLAY_FLEX_COLUMN, p->gap, ZLay_Insets(p->padding));
  style.width = ZLay_Px(p->width);
  style.height = ZLay_Px(p->height);
  style.background = p->theme->canvas;
  style.text_color = p->theme->text;
  if (p->device == ZLAYW_DEVICE_MOBILE) {
    style.radius = p->theme->radius_lg;
  }
  return ZLay_BoxDecl(id, style);
}

static inline ZLay_NodeDeclarationBuilder ZLayW_BoxDecl(ZLay_Id id, const ZLayW_BoxProps* props) {
  return ZLay_BoxDecl(id, ZLayW_BoxStyle(props));
}

static inline ZLay_NodeDeclarationBuilder ZLayW_RowDecl(ZLay_Id id, const ZLayW_BoxProps* props) {
  ZLayW_BoxProps row = props ? *props : ZLayW_BoxPropsDefault(NULL);
  row.layout_mode = ZLAY_LAYOUT_FLEX;
  row.direction = ZLAY_FLEX_ROW;
  return ZLayW_BoxDecl(id, &row);
}

static inline ZLay_NodeDeclarationBuilder ZLayW_ColumnDecl(ZLay_Id id, const ZLayW_BoxProps* props) {
  ZLayW_BoxProps column = props ? *props : ZLayW_BoxPropsDefault(NULL);
  column.layout_mode = ZLAY_LAYOUT_FLEX;
  column.direction = ZLAY_FLEX_COLUMN;
  return ZLayW_BoxDecl(id, &column);
}

static inline ZLay_NodeDeclarationBuilder ZLayW_GridDecl(ZLay_Id id, const ZLayW_BoxProps* props) {
  ZLayW_BoxProps grid = props ? *props : ZLayW_BoxPropsDefault(NULL);
  grid.layout_mode = ZLAY_LAYOUT_GRID;
  if (grid.grid_columns == 0u) grid.grid_columns = 1u;
  return ZLayW_BoxDecl(id, &grid);
}

static inline ZLay_NodeDeclarationBuilder ZLayW_SurfaceDecl(ZLay_Id id, ZLayW_SurfaceKind kind, const ZLay_Theme* theme) {
  ZLay_SurfaceVariant variant = ZLAY_SURFACE_PANEL;
  switch (kind) {
    case ZLAYW_SURFACE_CANVAS: variant = ZLAY_SURFACE_CANVAS; break;
    case ZLAYW_SURFACE_CARD: variant = ZLAY_SURFACE_CARD; break;
    case ZLAYW_SURFACE_MUTED: variant = ZLAY_SURFACE_MUTED; break;
    case ZLAYW_SURFACE_PANEL:
    default: variant = ZLAY_SURFACE_PANEL; break;
  }
  return ZLay_BoxDecl(id, ZLay_SurfaceStyle(variant, ZLayW_ThemeOrDefault(theme)));
}

static inline ZLay_NodeDeclarationBuilder ZLayW_AppBarDecl(ZLay_Id id, const ZLayW_AppBarProps* props) {
  ZLayW_AppBarProps defaults = ZLayW_AppBarPropsDefault(NULL);
  const ZLayW_AppBarProps* p = props ? props : &defaults;
  ZLay_Style style = ZLay_StackStyle(ZLAY_FLEX_ROW, p->theme->space_3, ZLay_InsetsXY(0.0f, p->theme->space_2));
  style.width = ZLay_Grow(1.0f);
  style.height = ZLay_Px(p->height);
  style.text_color = p->theme->text;
  return ZLay_BoxDecl(id, style);
}

static inline void ZLayW_Text(ZLay_Context* ctx, ZLay_Id id, const char* text, const ZLayW_TextProps* props) {
  ZLayW_TextProps defaults = ZLayW_TextPropsDefault(NULL);
  const ZLayW_TextProps* p = props ? props : &defaults;
  ZLay_LabelProps label = ZLay_LabelPropsDefault(p->theme);
  label.role = p->role;
  label.size = p->size;
  if (p->use_color) {
    label.color = p->color;
    label.use_custom_color = true;
  }
  ZLay_Label(ctx, id, ZLayW_Str(text), &label);
}

static inline void ZLayW_Button(ZLay_Context* ctx, ZLay_Id id, const char* label_text, const ZLayW_ButtonProps* props) {
  ZLayW_ButtonProps defaults = ZLayW_ButtonPropsDefault(NULL);
  const ZLayW_ButtonProps* p = props ? props : &defaults;
  ZLay_ButtonProps button = ZLay_ButtonPropsDefault(p->theme);
  button.variant = p->variant;
  button.size = p->size;
  button.state = p->state;
  if (p->use_width) {
    button.style = ZLay_ButtonStyle(&button);
    button.style.width = p->width;
    button.use_custom_style = true;
  }
  ZLay_Button(ctx, id, ZLayW_Str(label_text), &button);
}

static inline void ZLayW_Input(ZLay_Context* ctx, ZLay_Id id, const char* value, const char* placeholder, const ZLayW_InputProps* props) {
  ZLayW_InputProps defaults = ZLayW_InputPropsDefault(NULL);
  const ZLayW_InputProps* p = props ? props : &defaults;
  ZLay_InputProps input = ZLay_InputPropsDefault(p->theme);
  input.variant = p->variant;
  input.size = p->size;
  input.state = p->state;
  input.cursor_visible = p->cursor_visible;
  input.cursor_index = p->cursor_index;
  input.selection_start = p->selection_start;
  input.selection_end = p->selection_end;
  input.selection_visible = p->selection_visible;
  if (p->use_width) {
    input.style = ZLay_InputStyle(&input);
    input.style.width = p->width;
    input.use_custom_style = true;
  }
  ZLay_Input(ctx, id, ZLayW_Str(value), ZLayW_Str(placeholder), &input);
}

static inline void ZLayW_Fill(ZLay_Context* ctx, ZLay_Id id) {
  ZLay_Style style = ZLay_StyleDefault();
  style.width = ZLay_Grow(1.0f);
  style.height = ZLay_Px(1.0f);
  ZLAY(ctx, id, ZLAY_BOX(.id = id, .style = style)) {
  }
}

#define ZLAYW_SCAFFOLD(ctx, id_lit, props_ptr) \
  ZLAY((ctx), ZLAY_ID(id_lit), ZLayW_ScaffoldDecl(ZLAY_ID(id_lit), (props_ptr)))

#define ZLAYW_APP_BAR(ctx, id_lit, props_ptr) \
  ZLAY((ctx), ZLAY_ID(id_lit), ZLayW_AppBarDecl(ZLAY_ID(id_lit), (props_ptr)))

#define ZLAYW_ROW(ctx, id_lit, props_ptr) \
  ZLAY((ctx), ZLAY_ID(id_lit), ZLayW_RowDecl(ZLAY_ID(id_lit), (props_ptr)))

#define ZLAYW_COLUMN(ctx, id_lit, props_ptr) \
  ZLAY((ctx), ZLAY_ID(id_lit), ZLayW_ColumnDecl(ZLAY_ID(id_lit), (props_ptr)))

#define ZLAYW_GRID(ctx, id_lit, props_ptr) \
  ZLAY((ctx), ZLAY_ID(id_lit), ZLayW_GridDecl(ZLAY_ID(id_lit), (props_ptr)))

#define ZLAYW_BOX(ctx, id_lit, props_ptr) \
  ZLAY((ctx), ZLAY_ID(id_lit), ZLayW_BoxDecl(ZLAY_ID(id_lit), (props_ptr)))

#define ZLAYW_SURFACE(ctx, id_lit, kind, theme_ptr) \
  ZLAY((ctx), ZLAY_ID(id_lit), ZLayW_SurfaceDecl(ZLAY_ID(id_lit), (kind), (theme_ptr)))

#define ZLAYW_TEXT(ctx, id_lit, text, props_ptr) \
  ZLayW_Text((ctx), ZLAY_ID(id_lit), (text), (props_ptr))

#define ZLAYW_BUTTON(ctx, id_lit, label, props_ptr) \
  ZLayW_Button((ctx), ZLAY_ID(id_lit), (label), (props_ptr))

#define ZLAYW_INPUT(ctx, id_lit, value, placeholder, props_ptr) \
  ZLayW_Input((ctx), ZLAY_ID(id_lit), (value), (placeholder), (props_ptr))

#define ZLAYW_FILL(ctx, id_lit) \
  ZLayW_Fill((ctx), ZLAY_ID(id_lit))

#ifdef __cplusplus
}
#endif

#endif
