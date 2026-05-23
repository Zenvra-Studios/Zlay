#include <ui/label.h>

static const ZLay_Theme* ZLay__LabelTheme(const ZLay_LabelProps* props) {
  return props && props->theme ? props->theme : ZLay_ThemeDefault();
}

ZLay_LabelProps ZLay_LabelPropsDefault(const ZLay_Theme* theme) {
  ZLay_LabelProps props;
  props.theme = theme ? theme : ZLay_ThemeDefault();
  props.role = ZLAY_LABEL_BODY;
  props.size = ZLAY_COMPONENT_SIZE_MD;
  props.state = ZLAY_COMPONENT_STATE_NONE;
  props.color = ZLay_ColorTransparent();
  props.use_custom_color = false;
  props.style = ZLay_StyleDefault();
  props.use_custom_style = false;
  return props;
}

ZLay_Style ZLay_LabelStyle(const ZLay_LabelProps* props) {
  const ZLay_Theme* theme = ZLay__LabelTheme(props);
  ZLay_LabelProps fallback = ZLay_LabelPropsDefault(theme);
  const ZLay_LabelProps* resolved = props ? props : &fallback;
  ZLay_Style style = resolved->use_custom_style ? resolved->style : ZLay_StyleDefault();

  style.background = ZLay_ColorTransparent();
  style.text_color = resolved->use_custom_color ? resolved->color : theme->text;

  if (resolved->state & ZLAY_COMPONENT_STATE_DISABLED) {
    style.text_color = ZLay_ColorWithAlpha(style.text_color, 112);
  }

  switch (resolved->role) {
    case ZLAY_LABEL_TITLE:
      style.text_color = resolved->use_custom_color ? resolved->color : theme->text;
      style.height = ZLay_Px(resolved->size == ZLAY_COMPONENT_SIZE_LG ? 28.0f : 24.0f);
      break;
    case ZLAY_LABEL_SUBTITLE:
      style.text_color = resolved->use_custom_color ? resolved->color : theme->text_muted;
      style.height = ZLay_Px(20.0f);
      break;
    case ZLAY_LABEL_CAPTION:
      style.text_color = resolved->use_custom_color ? resolved->color : theme->text_muted;
      style.height = ZLay_Px(16.0f);
      break;
    case ZLAY_LABEL_BADGE:
      style.background = theme->primary_soft;
      style.text_color = resolved->use_custom_color ? resolved->color : theme->primary;
      style.radius = theme->radius_pill;
      style.height = ZLay_Px(24.0f);
      style.padding_left = style.padding_right = theme->space_2;
      style.padding_top = style.padding_bottom = theme->space_1;
      break;
    case ZLAY_LABEL_CODE:
      style.background = theme->surface_muted;
      style.text_color = resolved->use_custom_color ? resolved->color : theme->text;
      style.radius = theme->radius_sm;
      style.padding_left = style.padding_right = theme->space_2;
      style.padding_top = style.padding_bottom = theme->space_1;
      break;
    case ZLAY_LABEL_BODY:
    default:
      style.height = ZLay_Px(resolved->size == ZLAY_COMPONENT_SIZE_SM ? 16.0f : 20.0f);
      break;
  }

  return style;
}

ZLay_NodeDeclarationBuilder ZLay_LabelDecl(ZLay_Id id, ZLay_String text, const ZLay_LabelProps* props) {
  return ZLay_TextDecl(id, text, ZLay_LabelStyle(props));
}

void ZLay_Label(ZLay_Context* ctx, ZLay_Id id, ZLay_String text, const ZLay_LabelProps* props) {
  ZLay_Style style = ZLay_LabelStyle(props);
  ZLay__OpenTextElement(ctx, id, text, &style);
}
