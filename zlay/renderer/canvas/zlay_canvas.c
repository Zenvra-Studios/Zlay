#include <renderer/canvas/zlay_canvas.h>

#include <string.h>

static ZLay_Id ZLay__CanvasNextId(ZLay_Canvas* canvas, ZLay_CanvasCommandType command_type) {
  uint64_t value;
  if (!canvas) return (ZLay_Id){0};
  value = ++canvas->next_command_id;
  value ^= ((uint64_t)command_type << 48u);
  value ^= 0x9E3779B97F4A7C15ull + (value << 6u) + (value >> 2u);
  if (value == 0u) value = 1u;
  return (ZLay_Id){value};
}

static ZLay_RenderCommand* ZLay__CanvasPushCommand(ZLay_Canvas* canvas, ZLay_RenderCommandType type, ZLay_CanvasCommandType canvas_type) {
  ZLay_RenderCommand* command;
  if (!canvas || !canvas->recording || !canvas->command_list.commands) return NULL;
  if (canvas->command_list.count >= canvas->command_capacity) return NULL;
  command = &canvas->command_list.commands[canvas->command_list.count++];
  *command = (ZLay_RenderCommand){0};
  command->type = type;
  command->id = ZLay__CanvasNextId(canvas, canvas_type);
  command->z_index = canvas->z_index;
  command->pointer_cursor = ZLAY_POINTER_CURSOR_DEFAULT;
  command->rounded = ZLay_RoundedCircular(0.0f);
  return command;
}

static ZLay_Rect ZLay__CanvasRect(float x, float y, float width, float height) {
  return (ZLay_Rect){x, y, width, height};
}

static ZLay_Rect ZLay__CanvasApplyRectTransform(const ZLay_Canvas* canvas, ZLay_Rect rect) {
  if (!canvas) return rect;
  return ZLay_CanvasTransformRect(canvas->transform, rect);
}

static ZLay_Vec2 ZLay__CanvasApplyPointTransform(const ZLay_Canvas* canvas, ZLay_Vec2 point) {
  if (!canvas) return point;
  return ZLay_CanvasTransformPoint(canvas->transform, point);
}

static bool ZLay__CanvasPaintReady(ZLay_CanvasPaint paint) {
  return ZLay_CanvasPaintIsVisible(paint);
}

void ZLay_CanvasInit(ZLay_Canvas* canvas, ZLay_RenderCommand* commands, uint32_t command_capacity) {
  if (!canvas) return;
  canvas->command_list.commands = commands;
  canvas->command_list.count = 0u;
  canvas->command_capacity = command_capacity;
  canvas->recording = false;
  canvas->clip_depth = 0u;
  canvas->z_index = 0;
  canvas->next_command_id = 0u;
  canvas->transform = ZLay_CanvasTransformIdentity();
  canvas->transform_stack_count = 0u;
}

void ZLay_CanvasReset(ZLay_Canvas* canvas) {
  if (!canvas) return;
  canvas->command_list.count = 0u;
  canvas->recording = false;
  canvas->clip_depth = 0u;
  canvas->z_index = 0;
  canvas->next_command_id = 0u;
  canvas->transform = ZLay_CanvasTransformIdentity();
  canvas->transform_stack_count = 0u;
}

void ZLay_CanvasBegin(ZLay_Canvas* canvas) {
  if (!canvas) return;
  canvas->command_list.count = 0u;
  canvas->recording = true;
  canvas->clip_depth = 0u;
  canvas->next_command_id = 0u;
  canvas->transform = ZLay_CanvasTransformIdentity();
  canvas->transform_stack_count = 0u;
}

void ZLay_CanvasEnd(ZLay_Canvas* canvas) {
  if (!canvas) return;
  while (canvas->clip_depth > 0u) {
    if (!ZLay_CanvasPopClip(canvas)) {
      canvas->clip_depth = 0u;
      break;
    }
  }
  canvas->recording = false;
}

bool ZLay_CanvasIsRecording(const ZLay_Canvas* canvas) {
  return canvas ? canvas->recording : false;
}

uint32_t ZLay_CanvasCommandCount(const ZLay_Canvas* canvas) {
  return canvas ? canvas->command_list.count : 0u;
}

uint32_t ZLay_CanvasCommandCapacity(const ZLay_Canvas* canvas) {
  return canvas ? canvas->command_capacity : 0u;
}

ZLay_RenderCommandList ZLay_CanvasGetRenderCommands(const ZLay_Canvas* canvas) {
  if (!canvas) return (ZLay_RenderCommandList){0};
  return canvas->command_list;
}

const ZLay_RenderCommandList* ZLay_CanvasGetRenderCommandList(const ZLay_Canvas* canvas) {
  return canvas ? &canvas->command_list : NULL;
}

void ZLay_CanvasSetZIndex(ZLay_Canvas* canvas, int32_t z_index) {
  if (!canvas) return;
  canvas->z_index = z_index;
}

int32_t ZLay_CanvasGetZIndex(const ZLay_Canvas* canvas) {
  return canvas ? canvas->z_index : 0;
}

bool ZLay_CanvasRect(ZLay_Canvas* canvas, float x, float y, float width, float height, ZLay_CanvasPaint paint) {
  ZLay_RenderCommand* command;
  if (width <= 0.0f || height <= 0.0f || !ZLay__CanvasPaintReady(paint)) return false;
  command = ZLay__CanvasPushCommand(canvas, ZLAY_CMD_RECT, ZLAY_CANVAS_COMMAND_RECT);
  if (!command) return false;
  command->bounds = ZLay__CanvasApplyRectTransform(canvas, ZLay__CanvasRect(x, y, width, height));
  command->color = paint.color;
  command->font_size = 0.0f;
  return true;
}

bool ZLay_CanvasRoundedRect(
  ZLay_Canvas* canvas,
  float x,
  float y,
  float width,
  float height,
  float radius,
  ZLay_CanvasPaint paint
) {
  ZLay_RenderCommand* command;
  if (width <= 0.0f || height <= 0.0f || !ZLay__CanvasPaintReady(paint)) return false;
  command = ZLay__CanvasPushCommand(canvas, ZLAY_CMD_RECT, ZLAY_CANVAS_COMMAND_ROUNDED_RECT);
  if (!command) return false;
  command->bounds = ZLay__CanvasApplyRectTransform(canvas, ZLay__CanvasRect(x, y, width, height));
  command->color = paint.color;
  command->rounded = ZLay_RoundedCircular(radius);
  command->radius = ZLay_RoundedEffectiveRadius(command->rounded, command->bounds.width, command->bounds.height);
  return true;
}

bool ZLay_CanvasLine(
  ZLay_Canvas* canvas,
  float x1,
  float y1,
  float x2,
  float y2,
  ZLay_CanvasStroke stroke
) {
  ZLay_RenderCommand* command;
  ZLay_Vec2 p0;
  ZLay_Vec2 p1;
  const float half_width = stroke.width > 0.0f ? stroke.width * 0.5f : 0.5f;
  if (!ZLay__CanvasPaintReady(stroke.paint) || stroke.width <= 0.0f) return false;
  command = ZLay__CanvasPushCommand(canvas, ZLAY_CMD_LINE, ZLAY_CANVAS_COMMAND_LINE);
  if (!command) return false;
  p0 = ZLay__CanvasApplyPointTransform(canvas, (ZLay_Vec2){x1, y1});
  p1 = ZLay__CanvasApplyPointTransform(canvas, (ZLay_Vec2){x2, y2});
  command->line_start = p0;
  command->line_end = p1;
  command->stroke_width = stroke.width;
  command->color = stroke.paint.color;
  command->bounds.x = ZLay_MinFloat(p0.x, p1.x) - half_width;
  command->bounds.y = ZLay_MinFloat(p0.y, p1.y) - half_width;
  command->bounds.width = ZLay_MaxFloat(p0.x, p1.x) - ZLay_MinFloat(p0.x, p1.x) + stroke.width;
  command->bounds.height = ZLay_MaxFloat(p0.y, p1.y) - ZLay_MinFloat(p0.y, p1.y) + stroke.width;
  return true;
}

bool ZLay_CanvasText(ZLay_Canvas* canvas, float x, float y, const char* text, ZLay_TextStyle style) {
  if (!text) return false;
  return ZLay_CanvasTextLen(canvas, x, y, text, (uint32_t)strlen(text), style);
}

bool ZLay_CanvasTextLen(
  ZLay_Canvas* canvas,
  float x,
  float y,
  const char* text,
  uint32_t length,
  ZLay_TextStyle style
) {
  ZLay_RenderCommand* command;
  const float size = style.size > 0.0f ? style.size : 16.0f;
  const float width = 0.5f * size * (float)length;
  if (!text || length == 0u || style.color.a == 0u) return false;
  command = ZLay__CanvasPushCommand(canvas, ZLAY_CMD_TEXT, ZLAY_CANVAS_COMMAND_TEXT);
  if (!command) return false;
  command->bounds = ZLay__CanvasApplyRectTransform(canvas, ZLay__CanvasRect(x, y, width, size));
  command->color = style.color;
  command->text = (ZLay_String){text, length, false};
  command->font_family = style.font_family;
  command->font_size = size;
  return true;
}

bool ZLay_CanvasImage(ZLay_Canvas* canvas, ZLay_ImageHandle image, float x, float y, float width, float height) {
  return ZLay_CanvasImageEx(canvas, image, x, y, width, height, ZLay_CanvasImageOptionsDefault());
}

bool ZLay_CanvasImageEx(
  ZLay_Canvas* canvas,
  ZLay_ImageHandle image,
  float x,
  float y,
  float width,
  float height,
  ZLay_CanvasImageOptions options
) {
  ZLay_RenderCommand* command;
  if (image.id == 0u || width <= 0.0f || height <= 0.0f || options.tint.a == 0u) return false;
  command = ZLay__CanvasPushCommand(canvas, ZLAY_CMD_IMAGE, ZLAY_CANVAS_COMMAND_IMAGE);
  if (!command) return false;
  command->bounds = ZLay__CanvasApplyRectTransform(canvas, ZLay__CanvasRect(x, y, width, height));
  command->source_rect = options.source_rect;
  command->image = image;
  command->color = options.tint;
  return true;
}

bool ZLay_CanvasPushClipRect(ZLay_Canvas* canvas, float x, float y, float width, float height) {
  ZLay_RenderCommand* command;
  if (width <= 0.0f || height <= 0.0f) return false;
  command = ZLay__CanvasPushCommand(canvas, ZLAY_CMD_CLIP_BEGIN, ZLAY_CANVAS_COMMAND_CLIP_BEGIN);
  if (!command) return false;
  command->bounds = ZLay__CanvasApplyRectTransform(canvas, ZLay__CanvasRect(x, y, width, height));
  canvas->clip_depth++;
  return true;
}

bool ZLay_CanvasPopClip(ZLay_Canvas* canvas) {
  ZLay_RenderCommand* command;
  if (!canvas || canvas->clip_depth == 0u) return false;
  command = ZLay__CanvasPushCommand(canvas, ZLAY_CMD_CLIP_END, ZLAY_CANVAS_COMMAND_CLIP_END);
  if (!command) return false;
  canvas->clip_depth--;
  return true;
}

bool ZLay_CanvasPushTransform(ZLay_Canvas* canvas) {
  if (!canvas || canvas->transform_stack_count >= ZLAY_CANVAS_MAX_TRANSFORM_STACK) return false;
  canvas->transform_stack[canvas->transform_stack_count++] = canvas->transform;
  return true;
}

bool ZLay_CanvasPopTransform(ZLay_Canvas* canvas) {
  if (!canvas || canvas->transform_stack_count == 0u) return false;
  canvas->transform = canvas->transform_stack[--canvas->transform_stack_count];
  return true;
}

void ZLay_CanvasSetTransform(ZLay_Canvas* canvas, ZLay_CanvasTransform transform) {
  if (!canvas) return;
  canvas->transform = transform;
}

ZLay_CanvasTransform ZLay_CanvasGetTransform(const ZLay_Canvas* canvas) {
  return canvas ? canvas->transform : ZLay_CanvasTransformIdentity();
}

void ZLay_CanvasTranslate(ZLay_Canvas* canvas, float x, float y) {
  if (!canvas) return;
  canvas->transform = ZLay_CanvasTransformMultiply(canvas->transform, ZLay_CanvasTransformTranslate(x, y));
}

void ZLay_CanvasScale(ZLay_Canvas* canvas, float sx, float sy) {
  if (!canvas) return;
  canvas->transform = ZLay_CanvasTransformMultiply(canvas->transform, ZLay_CanvasTransformScale(sx, sy));
}

void ZLay_CanvasRotate(ZLay_Canvas* canvas, float radians) {
  if (!canvas) return;
  canvas->transform = ZLay_CanvasTransformMultiply(canvas->transform, ZLay_CanvasTransformRotate(radians));
}
