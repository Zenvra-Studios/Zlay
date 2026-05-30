#ifndef ZLAY_RENDERER_CANVAS_ZLAY_CANVAS_PATH_H
#define ZLAY_RENDERER_CANVAS_ZLAY_CANVAS_PATH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <zlay.h>

typedef enum ZLay_CanvasPathVerb {
  ZLAY_CANVAS_PATH_MOVE_TO = 0,
  ZLAY_CANVAS_PATH_LINE_TO,
  ZLAY_CANVAS_PATH_QUADRATIC_TO,
  ZLAY_CANVAS_PATH_CUBIC_TO,
  ZLAY_CANVAS_PATH_CLOSE
} ZLay_CanvasPathVerb;

typedef struct ZLay_CanvasPathCommand {
  ZLay_CanvasPathVerb verb;
  ZLay_Vec2 p0;
  ZLay_Vec2 p1;
  ZLay_Vec2 p2;
} ZLay_CanvasPathCommand;

typedef struct ZLay_CanvasPath {
  ZLay_CanvasPathCommand* commands;
  uint32_t count;
  uint32_t capacity;
} ZLay_CanvasPath;

ZLAY_API void ZLay_CanvasPathInit(ZLay_CanvasPath* path, ZLay_CanvasPathCommand* commands, uint32_t capacity);
ZLAY_API void ZLay_CanvasPathReset(ZLay_CanvasPath* path);
ZLAY_API bool ZLay_CanvasPathMoveTo(ZLay_CanvasPath* path, float x, float y);
ZLAY_API bool ZLay_CanvasPathLineTo(ZLay_CanvasPath* path, float x, float y);
ZLAY_API bool ZLay_CanvasPathQuadraticTo(ZLay_CanvasPath* path, float cx, float cy, float x, float y);
ZLAY_API bool ZLay_CanvasPathCubicTo(ZLay_CanvasPath* path, float cx1, float cy1, float cx2, float cy2, float x, float y);
ZLAY_API bool ZLay_CanvasPathClose(ZLay_CanvasPath* path);
ZLAY_API const char* ZLay_CanvasPathVerbName(ZLay_CanvasPathVerb verb);

#ifdef __cplusplus
}
#endif

#endif
