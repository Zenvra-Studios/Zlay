#include <zlay.h>

bool ZLay_RectContains(ZLay_Rect rect, float x, float y) {
  return x >= rect.x &&
         y >= rect.y &&
         x < rect.x + rect.width &&
         y < rect.y + rect.height;
}

bool ZLay_RenderCommandListFindBounds(ZLay_RenderCommandList commands, ZLay_Id id, ZLay_Rect* out_bounds) {
  uint32_t i;

  if (out_bounds == NULL) {
    return false;
  }

  for (i = 0; i < commands.count; ++i) {
    const ZLay_RenderCommand* command = &commands.commands[i];
    if (command->type == ZLAY_CMD_RECT && command->id.hash == id.hash) {
      *out_bounds = command->bounds;
      return true;
    }
  }

  return false;
}

ZLay_PointerCursor ZLay_RenderCommandListCursorAt(ZLay_RenderCommandList commands, float x, float y) {
  ZLay_PointerCursor cursor = ZLAY_POINTER_CURSOR_DEFAULT;
  uint32_t i;

  for (i = 0; i < commands.count; ++i) {
    const ZLay_RenderCommand* command = &commands.commands[i];
    if (command->pointer_cursor != ZLAY_POINTER_CURSOR_DEFAULT && ZLay_RectContains(command->bounds, x, y)) {
      cursor = command->pointer_cursor;
    }
  }

  return cursor;
}

uint32_t ZLay_ComponentStateFromPointer(const ZLay_PointerState* pointer, ZLay_Rect bounds, bool tracked) {
  uint32_t state = ZLAY_COMPONENT_STATE_NONE;

  if (pointer == NULL || !tracked || !pointer->inside || !ZLay_RectContains(bounds, pointer->x, pointer->y)) {
    return state;
  }

  state |= ZLAY_COMPONENT_STATE_HOVERED;
  if (pointer->left_down) {
    state |= ZLAY_COMPONENT_STATE_ACTIVE;
  }

  return state;
}
