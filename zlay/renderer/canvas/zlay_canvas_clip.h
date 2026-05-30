#ifndef ZLAY_RENDERER_CANVAS_ZLAY_CANVAS_CLIP_H
#define ZLAY_RENDERER_CANVAS_ZLAY_CANVAS_CLIP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include <zlay.h>

typedef struct ZLay_Canvas ZLay_Canvas;

ZLAY_API bool ZLay_CanvasPushClipRect(ZLay_Canvas* canvas, float x, float y, float width, float height);
ZLAY_API bool ZLay_CanvasPopClip(ZLay_Canvas* canvas);

#ifdef __cplusplus
}
#endif

#endif
