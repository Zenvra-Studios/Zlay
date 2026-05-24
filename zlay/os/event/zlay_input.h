#ifndef ZLAY_OS_EVENT_ZLAY_INPUT_H
#define ZLAY_OS_EVENT_ZLAY_INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

#include <zlay.h>
#include <os/event/zlay_event.h>

typedef struct ZLay_InputState {
  float mouse_x;
  float mouse_y;
  float mouse_dx;
  float mouse_dy;
  float wheel_x;
  float wheel_y;
  bool mouse_down[8];
  bool mouse_pressed[8];
  bool mouse_released[8];
  bool key_down[512];
  bool key_pressed[512];
  bool key_released[512];
  uint32_t modifiers;
  uint32_t text_input_count;
  uint32_t text_input[32];
} ZLay_InputState;

ZLAY_API void ZLay_InputBeginFrame(ZLay_InputState* input);
ZLAY_API void ZLay_InputProcessEvent(ZLay_InputState* input, const ZLay_Event* event);
ZLAY_API void ZLay_InputEndFrame(ZLay_InputState* input);

#ifdef __cplusplus
}
#endif

#endif
