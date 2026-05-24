#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <utility>

#include <zlay_ui.h>

namespace zlay {

class Context {
public:
  explicit Context(ZLay_Context* raw) : raw_(raw) {}

  ZLay_Context* raw() const { return raw_; }

private:
  ZLay_Context* raw_;
};

inline ZLay_Size px(float value) {
  return ZLay_Px(value);
}

inline ZLay_Size percent(float value) {
  return ZLay_Percent(value);
}

inline ZLay_Size grow(float weight = 1.0f) {
  return ZLay_Grow(weight);
}

inline ZLay_Size autoSize() {
  ZLay_Size size{};
  size.type = ZLAY_SIZE_AUTO;
  size.value = 0.0f;
  return size;
}

inline ZLay_Color rgb(int r, int g, int b) {
  return ZLay_ColorRGB(r, g, b);
}

inline ZLay_Color rgba(int r, int g, int b, int a) {
  return ZLay_ColorRGBA(r, g, b, a);
}

inline ZLay_String str(std::string_view text) {
  ZLay_String value{};
  value.chars = text.data();
  value.length = static_cast<uint32_t>(text.size());
  value.is_static = true;
  return value;
}

inline ZLay_Id id(const char* text) {
  return ZLay_IdFromCString(text);
}

inline ZLay_Id id(std::string_view text) {
  return ZLay_IdFromString(str(text));
}

inline bool contains(ZLay_Rect rect, float x, float y) {
  return ZLay_RectContains(rect, x, y);
}

inline bool findBounds(ZLay_RenderCommandList commands, ZLay_Id widgetId, ZLay_Rect* out) {
  return ZLay_RenderCommandListFindBounds(commands, widgetId, out);
}

inline bool findBounds(ZLay_RenderCommandList commands, std::string_view widgetId, ZLay_Rect* out) {
  return findBounds(commands, id(widgetId), out);
}

inline ZLay_PointerCursor cursorAt(ZLay_RenderCommandList commands, float x, float y) {
  return ZLay_RenderCommandListCursorAt(commands, x, y);
}

inline bool findBounds(const Context& ctx, ZLay_Id widgetId, ZLay_Rect* out) {
  return ZLay_ContextFindBounds(ctx.raw(), widgetId, out);
}

inline bool findBounds(const Context& ctx, std::string_view widgetId, ZLay_Rect* out) {
  return findBounds(ctx, id(widgetId), out);
}

inline ZLay_EdgeInsets insets(float value) {
  return ZLay_Insets(value);
}

inline ZLay_EdgeInsets insets(float horizontal, float vertical) {
  return ZLay_InsetsXY(horizontal, vertical);
}

inline ZLay_EdgeInsets insets(float left, float top, float right, float bottom) {
  return ZLay_InsetsLTRB(left, top, right, bottom);
}

inline ZLay_EdgeInsets insetsAll(float value) {
  return ZLay_EdgeInsetsAll(value);
}

inline ZLay_EdgeInsets insetsSymmetric(float horizontal, float vertical) {
  return ZLay_EdgeInsetsSymmetric(horizontal, vertical);
}

inline ZLay_EdgeInsets insetsOnly(float left = 0.0f, float top = 0.0f, float right = 0.0f, float bottom = 0.0f) {
  return ZLay_EdgeInsetsOnly(left, top, right, bottom);
}

inline const char* defaultFontFamily() {
  return ZLay_DefaultFontFamily();
}

inline ZLay_String defaultFontFamilyString() {
  return ZLay_DefaultFontFamilyString();
}

inline uint32_t componentState(const ZLay_PointerState& pointer, ZLay_Rect bounds, bool tracked) {
  return ZLay_ComponentStateFromPointer(&pointer, bounds, tracked);
}

inline uint32_t componentState(const ZLay_PointerState* pointer, ZLay_Rect bounds, bool tracked) {
  return ZLay_ComponentStateFromPointer(pointer, bounds, tracked);
}

class Style {
public:
  Style() : style_(ZLay_StyleDefault()) {}
  explicit Style(ZLay_Style style) : style_(style) {}

  Style& width(ZLay_Size value) {
    style_.width = value;
    return *this;
  }

  Style& height(ZLay_Size value) {
    style_.height = value;
    return *this;
  }

  Style& size(ZLay_Size widthValue, ZLay_Size heightValue) {
    style_.width = widthValue;
    style_.height = heightValue;
    return *this;
  }

  Style& padding(ZLay_EdgeInsets value) {
    style_ = ZLay_StyleWithPadding(style_, value);
    return *this;
  }

  Style& padding(float value) {
    style_ = ZLay_StyleWithPadding(style_, ZLay_Insets(value));
    return *this;
  }

  Style& padding(float horizontal, float vertical) {
    style_ = ZLay_StyleWithPadding(style_, ZLay_InsetsXY(horizontal, vertical));
    return *this;
  }

  Style& padding(float left, float top, float right, float bottom) {
    style_ = ZLay_StyleWithPadding(style_, ZLay_InsetsLTRB(left, top, right, bottom));
    return *this;
  }

  Style& margin(ZLay_EdgeInsets value) {
    style_ = ZLay_StyleWithMargin(style_, value);
    return *this;
  }

  Style& margin(float value) {
    style_ = ZLay_StyleWithMargin(style_, ZLay_Insets(value));
    return *this;
  }

  Style& margin(float horizontal, float vertical) {
    style_ = ZLay_StyleWithMargin(style_, ZLay_InsetsXY(horizontal, vertical));
    return *this;
  }

  Style& margin(float left, float top, float right, float bottom) {
    style_ = ZLay_StyleWithMargin(style_, ZLay_InsetsLTRB(left, top, right, bottom));
    return *this;
  }

  Style& gap(float value) {
    style_.gap = value;
    return *this;
  }

  Style& radius(float value) {
    style_ = ZLay_StyleWithRadius(style_, value);
    return *this;
  }

  Style& rounded(ZLay_Rounded value) {
    style_ = ZLay_StyleWithRounded(style_, value);
    return *this;
  }

  Style& roundedPreset(ZLay_RoundedPreset preset) {
    style_ = ZLay_StyleWithRoundedPreset(style_, preset);
    return *this;
  }

  Style& roundedSquircle(float radiusValue) {
    style_ = ZLay_StyleWithRounded(style_, ZLay_RoundedSquircle(radiusValue));
    return *this;
  }

  Style& roundedContinuous(float radiusValue) {
    style_ = ZLay_StyleWithRounded(style_, ZLay_RoundedContinuous(radiusValue));
    return *this;
  }

  Style& roundedCustom(float radiusValue, float smoothness, float exponent) {
    style_ = ZLay_StyleWithRounded(style_, ZLay_RoundedCustom(radiusValue, smoothness, exponent));
    return *this;
  }

  Style& background(ZLay_Color color) {
    style_.background = color;
    return *this;
  }

  Style& colors(ZLay_Color background, ZLay_Color text) {
    style_ = ZLay_StyleWithColors(style_, background, text);
    return *this;
  }

  Style& border(float width, ZLay_Color color) {
    style_ = ZLay_StyleWithBorder(style_, width, color);
    return *this;
  }

  Style& shadow(ZLay_Shadow value) {
    style_ = ZLay_StyleWithShadow(style_, value);
    return *this;
  }

  Style& shadow(ZLay_ShadowPreset preset) {
    style_ = ZLay_StyleWithShadowPreset(style_, preset);
    return *this;
  }

  Style& shadowPx(float offsetX, float offsetY, float blur, float spread, uint8_t alpha = 46u) {
    style_ = ZLay_StyleWithShadow(style_, ZLay_ShadowPx(offsetX, offsetY, blur, spread, alpha));
    return *this;
  }

  Style& floatingShadow() {
    style_ = ZLay_StyleWithShadow(style_, ZLay_ShadowFloating());
    return *this;
  }

  Style& textColor(ZLay_Color color) {
    style_.text_color = color;
    return *this;
  }

  Style& font(std::string_view family) {
    style_.font_family = str(family);
    return *this;
  }

  Style& fontSize(float value) {
    style_.font_size = value;
    if (style_.height.type == ZLAY_SIZE_AUTO) {
      style_.height = px(value + 6.0f);
    }
    return *this;
  }

  Style& cursor(ZLay_PointerCursor value) {
    style_.pointer_cursor = value;
    return *this;
  }

  Style& textCursor(uint32_t index, ZLay_Color color, bool visible = true) {
    style_ = ZLay_StyleWithTextCursor(style_, visible, index, color);
    return *this;
  }

  Style& textSelection(uint32_t start, uint32_t end, ZLay_Color color) {
    style_ = ZLay_StyleWithTextSelection(style_, start, end, color);
    return *this;
  }

  Style& row() {
    style_.flex_direction = ZLAY_FLEX_ROW;
    style_.layout_mode = ZLAY_LAYOUT_FLEX;
    return *this;
  }

  Style& column() {
    style_.flex_direction = ZLAY_FLEX_COLUMN;
    style_.layout_mode = ZLAY_LAYOUT_FLEX;
    return *this;
  }

  Style& grid(uint32_t columns, ZLay_Size autoRowHeight, float gapValue) {
    style_ = ZLay_StyleWithGrid(style_, columns, autoRowHeight, gapValue, gapValue);
    return *this;
  }

  Style& grid(uint32_t columns, ZLay_Size autoRowHeight, float columnGap, float rowGap) {
    style_ = ZLay_StyleWithGrid(style_, columns, autoRowHeight, columnGap, rowGap);
    return *this;
  }

  Style& gridSpan(uint32_t columnSpan, uint32_t rowSpan = 1u) {
    style_ = ZLay_StyleWithGridSpan(style_, columnSpan, rowSpan);
    return *this;
  }

  Style& absolute(float left, float top) {
    style_.position = ZLAY_POSITION_ABSOLUTE;
    style_.left = left;
    style_.top = top;
    return *this;
  }

  Style& zIndex(int32_t value) {
    style_.z_index = value;
    return *this;
  }

  const ZLay_Style& raw() const {
    return style_;
  }

private:
  ZLay_Style style_;
};

inline Style rowStyle(float gap = 0.0f) {
  return Style().row().gap(gap);
}

inline Style columnStyle(float gap = 0.0f) {
  return Style().column().gap(gap);
}

inline Style surfaceStyle(const ZLay_Theme& theme, ZLay_SurfaceVariant variant = ZLAY_SURFACE_CARD) {
  return Style(ZLay_SurfaceStyle(variant, &theme));
}

class TextStyle {
public:
  explicit TextStyle(const ZLay_Theme* theme = nullptr)
      : props_(ZLay_LabelPropsDefault(theme ? theme : ZLay_ThemeDefault())) {}

  TextStyle& role(ZLay_LabelRole value) {
    props_.role = value;
    return *this;
  }

  TextStyle& size(ZLay_ComponentSize value) {
    props_.size = value;
    return *this;
  }

  TextStyle& state(uint32_t value) {
    props_.state = value;
    return *this;
  }

  TextStyle& color(ZLay_Color value) {
    props_.color = value;
    props_.use_custom_color = true;
    return *this;
  }

  TextStyle& style(const Style& value) {
    props_.style = value.raw();
    props_.use_custom_style = true;
    return *this;
  }

  TextStyle& font(std::string_view family) {
    props_.style.font_family = str(family);
    props_.use_custom_style = true;
    return *this;
  }

  TextStyle& fontSize(float value) {
    props_.style.font_size = value;
    if (props_.style.height.type == ZLAY_SIZE_AUTO) {
      props_.style.height = px(value + 6.0f);
    }
    props_.use_custom_style = true;
    return *this;
  }

  TextStyle& cursor(ZLay_PointerCursor value) {
    props_.style.pointer_cursor = value;
    props_.use_custom_style = true;
    return *this;
  }

  TextStyle& textCursor(uint32_t index, ZLay_Color color, bool visible = true) {
    props_.style = ZLay_StyleWithTextCursor(props_.style, visible, index, color);
    props_.use_custom_style = true;
    return *this;
  }

  TextStyle& textSelection(uint32_t start, uint32_t end, ZLay_Color color) {
    props_.style = ZLay_StyleWithTextSelection(props_.style, start, end, color);
    props_.use_custom_style = true;
    return *this;
  }

  const ZLay_LabelProps& raw() const {
    return props_;
  }

private:
  ZLay_LabelProps props_;
};

class ButtonStyle {
public:
  explicit ButtonStyle(const ZLay_Theme* theme = nullptr)
      : props_(ZLay_ButtonPropsDefault(theme ? theme : ZLay_ThemeDefault())) {}

  ButtonStyle& variant(ZLay_ButtonVariant value) {
    props_.variant = value;
    return *this;
  }

  ButtonStyle& size(ZLay_ComponentSize value) {
    props_.size = value;
    return *this;
  }

  ButtonStyle& state(uint32_t value) {
    props_.state = value;
    return *this;
  }

  ButtonStyle& style(const Style& value) {
    props_.style = value.raw();
    props_.use_custom_style = true;
    return *this;
  }

  ButtonStyle& labelStyle(const Style& value) {
    props_.label_style = value.raw();
    props_.use_custom_label_style = true;
    return *this;
  }

  const ZLay_ButtonProps& raw() const {
    return props_;
  }

private:
  ZLay_ButtonProps props_;
};

class InputStyle {
public:
  explicit InputStyle(const ZLay_Theme* theme = nullptr)
      : props_(ZLay_InputPropsDefault(theme ? theme : ZLay_ThemeDefault())) {}

  InputStyle& variant(ZLay_InputVariant value) {
    props_.variant = value;
    return *this;
  }

  InputStyle& size(ZLay_ComponentSize value) {
    props_.size = value;
    return *this;
  }

  InputStyle& state(uint32_t value) {
    props_.state = value;
    return *this;
  }

  InputStyle& cursor(uint32_t index, bool visible = true) {
    props_.cursor_index = index;
    props_.cursor_visible = visible;
    return *this;
  }

  InputStyle& selection(uint32_t start, uint32_t end) {
    props_.selection_start = start;
    props_.selection_end = end;
    props_.selection_visible = start != end;
    return *this;
  }

  InputStyle& style(const Style& value) {
    props_.style = value.raw();
    props_.use_custom_style = true;
    return *this;
  }

  InputStyle& textStyle(const Style& value) {
    props_.text_style = value.raw();
    props_.use_custom_text_style = true;
    return *this;
  }

  const ZLay_InputProps& raw() const {
    return props_;
  }

private:
  ZLay_InputProps props_;
};

template <typename Children>
void Box(Context& ctx, std::string_view widgetId, const Style& style, Children&& children) {
  ZLAY(ctx.raw(), id(widgetId), ZLay_BoxDecl(id(widgetId), style.raw())) {
    std::forward<Children>(children)();
  }
}

inline void Box(Context& ctx, std::string_view widgetId, const Style& style) {
  ZLAY(ctx.raw(), id(widgetId), ZLay_BoxDecl(id(widgetId), style.raw())) {
  }
}

template <typename Children>
void Row(Context& ctx, std::string_view widgetId, const Style& style, Children&& children) {
  Box(ctx, widgetId, Style(style).row(), std::forward<Children>(children));
}

template <typename Children>
void Column(Context& ctx, std::string_view widgetId, const Style& style, Children&& children) {
  Box(ctx, widgetId, Style(style).column(), std::forward<Children>(children));
}

template <typename Children>
void Grid(Context& ctx, std::string_view widgetId, const Style& style, Children&& children) {
  Box(ctx, widgetId, style, std::forward<Children>(children));
}

template <typename Children>
void Grid(Context& ctx,
          std::string_view widgetId,
          uint32_t columns,
          ZLay_Size autoRowHeight,
          float gapValue,
          Children&& children) {
  Box(ctx, widgetId, Style().grid(columns, autoRowHeight, gapValue), std::forward<Children>(children));
}

template <typename Children>
void Card(Context& ctx, std::string_view widgetId, const ZLay_Theme& theme, Children&& children) {
  Box(ctx, widgetId, surfaceStyle(theme, ZLAY_SURFACE_CARD), std::forward<Children>(children));
}

inline void Text(Context& ctx, std::string_view widgetId, std::string_view value, const TextStyle& style = TextStyle()) {
  ZLay_Label(ctx.raw(), id(widgetId), str(value), &style.raw());
}

inline void RawText(Context& ctx, std::string_view widgetId, std::string_view value, const Style& style = Style()) {
  ZLay_Style rawStyle = style.raw();
  ZLAY_TEXT(ctx.raw(), id(widgetId), str(value), &rawStyle);
}

inline void Button(Context& ctx, std::string_view widgetId, std::string_view label, const ButtonStyle& style = ButtonStyle()) {
  ZLay_Button(ctx.raw(), id(widgetId), str(label), &style.raw());
}

inline void Input(Context& ctx,
                  std::string_view widgetId,
                  std::string_view value,
                  std::string_view placeholder,
                  const InputStyle& style = InputStyle()) {
  ZLay_Input(ctx.raw(), id(widgetId), str(value), str(placeholder), &style.raw());
}

inline void Spacer(Context& ctx, std::string_view widgetId, float size, ZLay_Axis axis = ZLAY_AXIS_VERTICAL) {
  ZLay_Spacer(ctx.raw(), id(widgetId), size, axis);
}

inline void Divider(Context& ctx, std::string_view widgetId, const ZLay_Theme& theme, ZLay_Axis axis = ZLAY_AXIS_HORIZONTAL) {
  ZLay_Divider(ctx.raw(), id(widgetId), axis, &theme);
}

inline void Fill(Context& ctx, std::string_view widgetId) {
  Box(ctx, widgetId, Style().width(grow()).height(px(1.0f)));
}

class App {
public:
  using FrameCommands = ZLay_RenderCommandList;

  explicit App(ZLay_Context* raw) : ctx_(raw) {}

  ZLay_Context* raw() const {
    return ctx_;
  }

  void setSize(float width, float height) {
    ZLay_SetLayoutDimensions(ctx_, ZLay_DimensionsMake(width, height));
  }

  template <typename FrameFn>
  FrameCommands frame(FrameFn&& fn) {
    ZLay_BeginLayout(ctx_);
    Context ctx(ctx_);
    std::forward<FrameFn>(fn)(ctx);
    return ZLay_EndLayout(ctx_);
  }

private:
  ZLay_Context* ctx_;
};

} // namespace zlay
