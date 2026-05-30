#ifndef ZLAY_RENDERER_CANVAS_ZLAY_CANVAS_H
#define ZLAY_RENDERER_CANVAS_ZLAY_CANVAS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <zlay.h>
#include <renderer/canvas/zlay_canvas_command.h>
#include <renderer/canvas/zlay_canvas_gradient.h>
#include <renderer/canvas/zlay_canvas_image.h>
#include <renderer/canvas/zlay_canvas_paint.h>
#include <renderer/canvas/zlay_canvas_path.h>
#include <renderer/canvas/zlay_canvas_text.h>
#include <renderer/canvas/zlay_canvas_transform.h>

#ifndef ZLAY_CANVAS_MAX_TRANSFORM_STACK
#  define ZLAY_CANVAS_MAX_TRANSFORM_STACK 32u
#endif

typedef struct ZLay_Canvas {
  ZLay_RenderCommandList command_list;
  uint32_t command_capacity;
  bool recording;
  uint32_t clip_depth;
  int32_t z_index;
  uint64_t next_command_id;
  ZLay_CanvasTransform transform;
  ZLay_CanvasTransform transform_stack[ZLAY_CANVAS_MAX_TRANSFORM_STACK];
  uint32_t transform_stack_count;
} ZLay_Canvas;

ZLAY_API void ZLay_CanvasInit(ZLay_Canvas* canvas, ZLay_RenderCommand* commands, uint32_t command_capacity);
ZLAY_API void ZLay_CanvasReset(ZLay_Canvas* canvas);
ZLAY_API void ZLay_CanvasBegin(ZLay_Canvas* canvas);
ZLAY_API void ZLay_CanvasEnd(ZLay_Canvas* canvas);
ZLAY_API bool ZLay_CanvasIsRecording(const ZLay_Canvas* canvas);
ZLAY_API uint32_t ZLay_CanvasCommandCount(const ZLay_Canvas* canvas);
ZLAY_API uint32_t ZLay_CanvasCommandCapacity(const ZLay_Canvas* canvas);
ZLAY_API ZLay_RenderCommandList ZLay_CanvasGetRenderCommands(const ZLay_Canvas* canvas);
ZLAY_API const ZLay_RenderCommandList* ZLay_CanvasGetRenderCommandList(const ZLay_Canvas* canvas);
ZLAY_API void ZLay_CanvasSetZIndex(ZLay_Canvas* canvas, int32_t z_index);
ZLAY_API int32_t ZLay_CanvasGetZIndex(const ZLay_Canvas* canvas);

ZLAY_API bool ZLay_CanvasRect(ZLay_Canvas* canvas, float x, float y, float width, float height, ZLay_CanvasPaint paint);
ZLAY_API bool ZLay_CanvasRoundedRect(
  ZLay_Canvas* canvas,
  float x,
  float y,
  float width,
  float height,
  float radius,
  ZLay_CanvasPaint paint
);
ZLAY_API bool ZLay_CanvasLine(
  ZLay_Canvas* canvas,
  float x1,
  float y1,
  float x2,
  float y2,
  ZLay_CanvasStroke stroke
);
ZLAY_API bool ZLay_CanvasText(ZLay_Canvas* canvas, float x, float y, const char* text, ZLay_TextStyle style);
ZLAY_API bool ZLay_CanvasTextLen(
  ZLay_Canvas* canvas,
  float x,
  float y,
  const char* text,
  uint32_t length,
  ZLay_TextStyle style
);
ZLAY_API bool ZLay_CanvasImage(ZLay_Canvas* canvas, ZLay_ImageHandle image, float x, float y, float width, float height);
ZLAY_API bool ZLay_CanvasImageEx(
  ZLay_Canvas* canvas,
  ZLay_ImageHandle image,
  float x,
  float y,
  float width,
  float height,
  ZLay_CanvasImageOptions options
);

ZLAY_API bool ZLay_CanvasPushClipRect(ZLay_Canvas* canvas, float x, float y, float width, float height);
ZLAY_API bool ZLay_CanvasPopClip(ZLay_Canvas* canvas);

ZLAY_API bool ZLay_CanvasPushTransform(ZLay_Canvas* canvas);
ZLAY_API bool ZLay_CanvasPopTransform(ZLay_Canvas* canvas);
ZLAY_API void ZLay_CanvasSetTransform(ZLay_Canvas* canvas, ZLay_CanvasTransform transform);
ZLAY_API ZLay_CanvasTransform ZLay_CanvasGetTransform(const ZLay_Canvas* canvas);
ZLAY_API void ZLay_CanvasTranslate(ZLay_Canvas* canvas, float x, float y);
ZLAY_API void ZLay_CanvasScale(ZLay_Canvas* canvas, float sx, float sy);
ZLAY_API void ZLay_CanvasRotate(ZLay_Canvas* canvas, float radians);

#ifdef __cplusplus
}
#endif

#endif
