#include <ui/input.h>

static const ZLay_Theme* ZLay__InputTheme(const ZLay_InputProps* props) {
  return props && props->theme ? props->theme : ZLay_ThemeDefault();
}

static ZLay_Id ZLay__InputChildId(ZLay_Id parent, uint64_t salt) {
  parent.hash ^= salt + 0x9E3779B97F4A7C15ull + (parent.hash << 6u) + (parent.hash >> 2u);
  return parent;
}

ZLay_InputProps ZLay_InputPropsDefault(const ZLay_Theme* theme) {
  ZLay_InputProps props;
  props.theme = theme ? theme : ZLay_ThemeDefault();
  props.variant = ZLAY_INPUT_OUTLINED;
  props.size = ZLAY_COMPONENT_SIZE_MD;
  props.state = ZLAY_COMPONENT_STATE_NONE;
  props.style = ZLay_StyleDefault();
  props.use_custom_style = false;
  props.text_style = ZLay_StyleDefault();
  props.use_custom_text_style = false;
  props.cursor_visible = false;
  props.cursor_index = 0u;
  props.selection_start = 0u;
  props.selection_end = 0u;
  props.selection_visible = false;
  return props;
}

ZLay_Style ZLay_InputStyle(const ZLay_InputProps* props) {
  const ZLay_Theme* theme = ZLay__InputTheme(props);
  ZLay_InputProps fallback = ZLay_InputPropsDefault(theme);
  const ZLay_InputProps* resolved = props ? props : &fallback;
  ZLay_Style style = resolved->use_custom_style ? resolved->style : ZLay_StyleDefault();
  ZLay_EdgeInsets padding = ZLay_ComponentControlPadding(resolved->size, theme);

  style.flex_direction = ZLAY_FLEX_ROW;
  style.gap = theme->space_2;
  style.height = ZLay_Px(ZLay_ComponentControlHeight(resolved->size, theme));
  style.padding_left = padding.left;
  style.padding_top = padding.top;
  style.padding_right = padding.right;
  style.padding_bottom = padding.bottom;
  style.radius = theme->radius_md;
  style.text_color = theme->text;
  style.pointer_cursor = ZLAY_POINTER_CURSOR_TEXT;

  switch (resolved->variant) {
    case ZLAY_INPUT_FILLED:
      style.background = theme->surface_muted;
      break;
    case ZLAY_INPUT_UNDERLINE:
      style.background = ZLay_ColorTransparent();
      style.radius = 0.0f;
      style.padding_left = 0.0f;
      style.padding_right = 0.0f;
      break;
    case ZLAY_INPUT_OUTLINED:
    default:
      style.background = theme->surface;
      break;
  }

  if (resolved->state & ZLAY_COMPONENT_STATE_FOCUSED) {
    style.background = ZLay_ColorMix(style.background, theme->primary_soft, 0.35f);
  }

  if (resolved->state & ZLAY_COMPONENT_STATE_INVALID) {
    style.background = ZLay_ColorMix(style.background, theme->danger_soft, 0.45f);
  }

  if (resolved->state & ZLAY_COMPONENT_STATE_DISABLED) {
    style.background = ZLay_ColorWithAlpha(style.background, style.background.a == 0 ? 0 : 96);
    style.text_color = ZLay_ColorWithAlpha(style.text_color, 112);
  }

  return style;
}

ZLay_Style ZLay_InputTextStyle(const ZLay_InputProps* props, bool placeholder) {
  const ZLay_Theme* theme = ZLay__InputTheme(props);
  ZLay_InputProps fallback = ZLay_InputPropsDefault(theme);
  const ZLay_InputProps* resolved = props ? props : &fallback;
  ZLay_Style field_style = ZLay_InputStyle(resolved);
  ZLay_Style style = resolved->use_custom_text_style ? resolved->text_style : ZLay_StyleDefault();
  const bool focused = (resolved->state & ZLAY_COMPONENT_STATE_FOCUSED) != 0u;

  style.background = ZLay_ColorTransparent();
  style.text_color = placeholder ? theme->text_muted : field_style.text_color;
  style.height = ZLay_Px(resolved->size == ZLAY_COMPONENT_SIZE_SM ? 16.0f : 20.0f);
  style.pointer_cursor = ZLAY_POINTER_CURSOR_TEXT;
  style.text_cursor_visible = resolved->cursor_visible || focused;
  style.text_cursor_index = placeholder ? 0u : resolved->cursor_index;
  style.text_cursor_color = field_style.text_color;
  style.text_selection_visible = !placeholder && resolved->selection_visible && resolved->selection_start != resolved->selection_end;
  style.text_selection_start = resolved->selection_start;
  style.text_selection_end = resolved->selection_end;
  style.text_selection_color = ZLay_ColorWithAlpha(theme->primary, 96);

  if (resolved->state & ZLAY_COMPONENT_STATE_DISABLED) {
    style.text_color = ZLay_ColorWithAlpha(style.text_color, 112);
    style.text_cursor_visible = false;
    style.text_selection_visible = false;
  }

  return style;
}

ZLay_NodeDeclarationBuilder ZLay_InputDecl(ZLay_Id id, const ZLay_InputProps* props) {
  return ZLay_BoxDecl(id, ZLay_InputStyle(props));
}

void ZLay_Input(ZLay_Context* ctx, ZLay_Id id, ZLay_String value, ZLay_String placeholder, const ZLay_InputProps* props) {
  if (!ctx) return;

  const bool use_placeholder = ZLay_StringIsEmpty(value);
  ZLay_NodeDeclarationBuilder builder = ZLay_InputDecl(id, props);
  ZLay__OpenElement(ctx, id, ZLAY_NODE_BOX);
  ZLay__ConfigureOpenElement(ctx, &builder.decl);

  ZLay_Style text_style = ZLay_InputTextStyle(props, use_placeholder);
  ZLay__OpenTextElement(ctx, ZLay__InputChildId(id, 0x1A9075ull), use_placeholder ? placeholder : value, &text_style);

  ZLay__CloseElement(ctx);
}
