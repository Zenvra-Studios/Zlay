#include <renderer/canvas/zlay_canvas_path.h>

static bool ZLay__CanvasPathPush(ZLay_CanvasPath* path, ZLay_CanvasPathCommand command) {
  if (!path || !path->commands || path->count >= path->capacity) return false;
  path->commands[path->count++] = command;
  return true;
}

void ZLay_CanvasPathInit(ZLay_CanvasPath* path, ZLay_CanvasPathCommand* commands, uint32_t capacity) {
  if (!path) return;
  path->commands = commands;
  path->count = 0u;
  path->capacity = capacity;
}

void ZLay_CanvasPathReset(ZLay_CanvasPath* path) {
  if (!path) return;
  path->count = 0u;
}

bool ZLay_CanvasPathMoveTo(ZLay_CanvasPath* path, float x, float y) {
  return ZLay__CanvasPathPush(path, (ZLay_CanvasPathCommand){ZLAY_CANVAS_PATH_MOVE_TO, {x, y}, {0.0f, 0.0f}, {0.0f, 0.0f}});
}

bool ZLay_CanvasPathLineTo(ZLay_CanvasPath* path, float x, float y) {
  return ZLay__CanvasPathPush(path, (ZLay_CanvasPathCommand){ZLAY_CANVAS_PATH_LINE_TO, {x, y}, {0.0f, 0.0f}, {0.0f, 0.0f}});
}

bool ZLay_CanvasPathQuadraticTo(ZLay_CanvasPath* path, float cx, float cy, float x, float y) {
  return ZLay__CanvasPathPush(path, (ZLay_CanvasPathCommand){ZLAY_CANVAS_PATH_QUADRATIC_TO, {cx, cy}, {x, y}, {0.0f, 0.0f}});
}

bool ZLay_CanvasPathCubicTo(ZLay_CanvasPath* path, float cx1, float cy1, float cx2, float cy2, float x, float y) {
  return ZLay__CanvasPathPush(path, (ZLay_CanvasPathCommand){ZLAY_CANVAS_PATH_CUBIC_TO, {cx1, cy1}, {cx2, cy2}, {x, y}});
}

bool ZLay_CanvasPathClose(ZLay_CanvasPath* path) {
  return ZLay__CanvasPathPush(path, (ZLay_CanvasPathCommand){ZLAY_CANVAS_PATH_CLOSE, {0.0f, 0.0f}, {0.0f, 0.0f}, {0.0f, 0.0f}});
}

const char* ZLay_CanvasPathVerbName(ZLay_CanvasPathVerb verb) {
  switch (verb) {
    case ZLAY_CANVAS_PATH_MOVE_TO: return "MoveTo";
    case ZLAY_CANVAS_PATH_LINE_TO: return "LineTo";
    case ZLAY_CANVAS_PATH_QUADRATIC_TO: return "QuadraticTo";
    case ZLAY_CANVAS_PATH_CUBIC_TO: return "CubicTo";
    case ZLAY_CANVAS_PATH_CLOSE: return "Close";
    default: return "Unknown";
  }
}
