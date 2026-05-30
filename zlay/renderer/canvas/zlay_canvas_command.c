#include <renderer/canvas/zlay_canvas_command.h>

const char* ZLay_CanvasCommandTypeName(ZLay_CanvasCommandType type) {
  switch (type) {
    case ZLAY_CANVAS_COMMAND_RECT: return "Rect";
    case ZLAY_CANVAS_COMMAND_ROUNDED_RECT: return "RoundedRect";
    case ZLAY_CANVAS_COMMAND_LINE: return "Line";
    case ZLAY_CANVAS_COMMAND_PATH: return "Path";
    case ZLAY_CANVAS_COMMAND_TEXT: return "Text";
    case ZLAY_CANVAS_COMMAND_IMAGE: return "Image";
    case ZLAY_CANVAS_COMMAND_CLIP_BEGIN: return "ClipBegin";
    case ZLAY_CANVAS_COMMAND_CLIP_END: return "ClipEnd";
    case ZLAY_CANVAS_COMMAND_PUSH_TRANSFORM: return "PushTransform";
    case ZLAY_CANVAS_COMMAND_POP_TRANSFORM: return "PopTransform";
    case ZLAY_CANVAS_COMMAND_CUSTOM: return "Custom";
    case ZLAY_CANVAS_COMMAND_NONE:
    default: return "None";
  }
}
