#include <ui/button.h>

static const ZLay_Theme* ZLay__ButtonTheme(const ZLay_ButtonProps* props) {
  return props && props->theme ? props->theme : ZLay_ThemeDefault();
}

static ZLay_Id ZLay__ButtonChildId(ZLay_Id parent, uint64_t salt) {
  parent.hash ^= salt + 0x9E3779B97F4A7C15ull + (parent.hash << 6u) + (parent.hash >> 2u);
  return parent;
}

static float ZLay__ButtonAutoWidth(ZLay_String label, const ZLay_Style* style) {
  const float label_width = (float)label.length * 8.0f;
  return label_width + style->padding_left + style->padding_right;
}

ZLay_ButtonProps ZLay_ButtonPropsDefault(const ZLay_Theme* theme) {
  ZLay_ButtonProps props;
  props.theme = theme ? theme : ZLay_ThemeDefault();
  props.variant = ZLAY_BUTTON_PRIMARY;
  props.size = ZLAY_COMPONENT_SIZE_MD;
  props.state = ZLAY_COMPONENT_STATE_NONE;
  props.style = ZLay_StyleDefault();
  props.use_custom_style = false;
  props.label_style = ZLay_StyleDefault();
  props.use_custom_label_style = false;
  return props;
}

ZLay_Style ZLay_ButtonStyle(const ZLay_ButtonProps* props) {
  const ZLay_Theme* theme = ZLay__ButtonTheme(props);
  ZLay_ButtonProps fallback = ZLay_ButtonPropsDefault(theme);
  const ZLay_ButtonProps* resolved = props ? props : &fallback;
  ZLay_Style style = resolved->use_custom_style ? resolved->style : ZLay_StyleDefault();
  const ZLay_EdgeInsets padding = ZLay_ComponentControlPadding(resolved->size, theme);

  style.flex_direction = ZLAY_FLEX_ROW;
  style.gap = theme->space_2;
  style.height = ZLay_Px(ZLay_ComponentControlHeight(resolved->size, theme));
  style.padding_left = padding.left;
  style.padding_top = padding.top;
  style.padding_right = padding.right;
  style.padding_bottom = padding.bottom;
  style.radius = resolved->variant == ZLAY_BUTTON_GHOST ? theme->radius_md : theme->radius_pill;
  style.text_color = theme->text_inverse;
  style.pointer_cursor = ZLAY_POINTER_CURSOR_POINTER;

  switch (resolved->variant) {
    case ZLAY_BUTTON_SECONDARY:
      style.background = theme->surface_muted;
      style.text_color = theme->text;
      break;
    case ZLAY_BUTTON_GHOST:
      style.background = ZLay_ColorTransparent();
      style.text_color = theme->primary;
      break;
    case ZLAY_BUTTON_DANGER:
      style.background = theme->danger;
      style.text_color = theme->text_inverse;
      break;
    case ZLAY_BUTTON_PRIMARY:
    default:
      style.background = theme->primary;
      style.text_color = theme->text_inverse;
      break;
  }

  if (resolved->state & ZLAY_COMPONENT_STATE_HOVERED) {
    if (resolved->variant == ZLAY_BUTTON_PRIMARY) style.background = theme->primary_hover;
    else if (resolved->variant == ZLAY_BUTTON_DANGER) style.background = theme->danger_hover;
    else if (resolved->variant == ZLAY_BUTTON_GHOST) style.background = theme->primary_soft;
    else style.background = theme->surface_raised;
  }

  if (resolved->state & ZLAY_COMPONENT_STATE_ACTIVE) {
    if (resolved->variant == ZLAY_BUTTON_PRIMARY) style.background = theme->primary_active;
    else if (resolved->variant == ZLAY_BUTTON_DANGER) style.background = theme->danger_hover;
    else style.background = theme->border;
  }

  if (resolved->state & ZLAY_COMPONENT_STATE_DISABLED) {
    style.background = ZLay_ColorWithAlpha(style.background, style.background.a == 0 ? 0 : 96);
    style.text_color = ZLay_ColorWithAlpha(style.text_color, 112);
    style.pointer_cursor = ZLAY_POINTER_CURSOR_DEFAULT;
  }

  return style;
}

ZLay_Style ZLay_ButtonLabelStyle(const ZLay_ButtonProps* props) {
  const ZLay_Theme* theme = ZLay__ButtonTheme(props);
  ZLay_ButtonProps fallback = ZLay_ButtonPropsDefault(theme);
  const ZLay_ButtonProps* resolved = props ? props : &fallback;
  ZLay_Style button_style = ZLay_ButtonStyle(resolved);
  ZLay_Style style = resolved->use_custom_label_style ? resolved->label_style : ZLay_StyleDefault();

  style.background = ZLay_ColorTransparent();
  style.text_color = button_style.text_color;
  style.height = ZLay_Px(resolved->size == ZLAY_COMPONENT_SIZE_SM ? 16.0f : 20.0f);
  return style;
}

ZLay_NodeDeclarationBuilder ZLay_ButtonDecl(ZLay_Id id, const ZLay_ButtonProps* props) {
  return ZLay_BoxDecl(id, ZLay_ButtonStyle(props));
}

void ZLay_Button(ZLay_Context* ctx, ZLay_Id id, ZLay_String label, const ZLay_ButtonProps* props) {
  if (!ctx) return;

  ZLay_NodeDeclarationBuilder builder = ZLay_ButtonDecl(id, props);
  if (builder.decl.style.width.type == ZLAY_SIZE_AUTO) {
    builder.decl.style.width = ZLay_Px(ZLay__ButtonAutoWidth(label, &builder.decl.style));
  }

  ZLay__OpenElement(ctx, id, ZLAY_NODE_BOX);
  ZLay__ConfigureOpenElement(ctx, &builder.decl);

  ZLay_Style label_style = ZLay_ButtonLabelStyle(props);
  ZLay__OpenTextElement(ctx, ZLay__ButtonChildId(id, 0xB0770A11ull), label, &label_style);

  ZLay__CloseElement(ctx);
}
