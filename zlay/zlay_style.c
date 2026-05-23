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

ZLay_Style ZLay_StyleDefault(void) {
  ZLay_Style s;
  s.width = (ZLay_Size){ZLAY_SIZE_AUTO, 0};
  s.height = (ZLay_Size){ZLAY_SIZE_AUTO, 0};

  s.padding_left = s.padding_top = s.padding_right = s.padding_bottom = 0;
  s.margin_left = s.margin_top = s.margin_right = s.margin_bottom = 0;
  s.gap = 0;

  s.flex_direction = ZLAY_FLEX_COLUMN;
  s.position = ZLAY_POSITION_RELATIVE;
  s.left = s.top = s.right = s.bottom = 0;
  s.z_index = 0;

  s.background = (ZLay_Color){0, 0, 0, 0};
  s.text_color = (ZLay_Color){255, 255, 255, 255};
  s.radius = 0;
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
  style.radius = radius;
  return style;
}

ZLay_Style ZLay_RowStyle(ZLay_Size width, ZLay_Size height, float gap) {
  ZLay_Style style = ZLay_StyleDefault();
  style.width = width;
  style.height = height;
  style.flex_direction = ZLAY_FLEX_ROW;
  style.gap = gap;
  return style;
}

ZLay_Style ZLay_ColumnStyle(ZLay_Size width, ZLay_Size height, float gap) {
  ZLay_Style style = ZLay_StyleDefault();
  style.width = width;
  style.height = height;
  style.flex_direction = ZLAY_FLEX_COLUMN;
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
